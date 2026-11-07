// Walking with somebody. Most groups on a street are pairs, walking abreast at one pace (Moussaid et al. 2010, PLoS ONE
// 5(4) e10047), so a walker's company is a body at its side: it asks the crowd nothing, it goes where its leader goes, abreast
// where the sidewalk has room for both and the way ahead is clear of the furniture and of those coming up, else in its leader's
// footsteps; on a line and at a kerb it keeps a step from its leader, closing on it, so it comes round a corner as the leader did.
// The leader goes at the pace of the slower of the two; a child always has a grown-up with it, an adult brings a child now and
// then, and an elder walks with an elder.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float ARRIVED_M = 0.02f; // this near its place it stands
constexpr float MIN_STEP_S = 1e-3f;
constexpr float LOOK_STEPS = 3; // it drops behind this many gaps before a piece of furniture, and comes back as far after

PedAge partner_age(TrafficState &t, const TrafficParams &p, PedAge leader) {
	if (leader == PedAge::CHILD) return PedAge::ADULT;
	return leader == PedAge::ADULT && t.rng.f() < p.ped_company_child_share ? PedAge::CHILD : leader;
}

float company_gap(const TrafficParams &p) { return 2 * p.ped_body_m + p.ped_gap_m; } // the room walkers keep

// Whether a body between `u0` and `u1` across the sidewalk is clear of its furniture from arc `from` to `to`.
bool strip_clear(CityState &s, int seg, int side, float from, float to, float u0, float u1) {
	const SidewalkFurniture &sf = sidewalk_furniture(s, seg);
	const float body = s.catalog.traffic.ped_body_m;
	for (const StreetObstacle &o : sf.side[side]) {
		if (o.s0 > to + body) break; // by s0
		if (o.s1 >= from - body && o.u1 >= u0 - body && o.u0 <= u1 + body) return false;
	}
	return true;
}

// Whether nobody else stands in the strip beside its leader up to `to` (the sidewalk's crowd, by arc, from this step's start).
bool strip_free(const CityState &s, int seg, int side, float from, float to, float u0, float u1, int party) {
	const TrafficState &t = s.traffic;
	const size_t key = size_t(seg * 2 + side);
	if (key >= t.crowd.size()) return true;
	const auto &list = t.crowd[key];
	const float touch = 2 * s.catalog.traffic.ped_body_m;
	auto it = std::lower_bound(list.begin(), list.end(), from, [](const CrowdEntry &e, float v) { return e.s < v; });
	for (; it != list.end() && it->s <= to; ++it)
		if (it->party != party && it->u >= u0 - touch && it->u <= u1 + touch) return false;
	return true;
}

// Where the company of `l` wants to be, and the sidewalk that is on when it is on one.
Pose place_by(CityState &s, Pedestrian &w, const Pedestrian &l) {
	const TrafficParams &p = s.catalog.traffic;
	const float gap = company_gap(p);
	w.way_node = l.way_node;
	w.seg = l.seg;
	w.on_way = false;
	Pose at;
	if (l.mode != PedMode::SIDEWALK) { // a step from its leader, on the line between them
		const V2 back = V2(w.cur.x - l.cur.x, w.cur.z - l.cur.z).norm();
		at = l.cur;
		at.x += back.x * gap;
		at.z += back.y * gap;
		return at;
	}
	const RoadSegment &g = s.roads.segs[size_t(l.seg)];
	const float half = walk_half(s, l.seg, l.side), lo = float(g.retreat_a), hi = float(g.len - g.retreat_b);
	float u = l.u + w.flank * gap;
	float arc = l.s;
	const float look = LOOK_STEPS * gap, u0 = std::min(u, l.u), u1 = std::max(u, l.u);
	const float ahead = l.dir == 0 ? l.s : l.s - look, past = l.dir == 0 ? l.s + look : l.s;
	if (std::fabs(u) > half || !strip_clear(s, l.seg, l.side, l.s - look, l.s + look, u0, u1) ||
			!strip_free(s, l.seg, l.side, ahead, past, w.flank > 0 ? l.u + gap / 2 : u0, w.flank > 0 ? u1 : l.u - gap / 2, w.with)) {
		u = l.u; // a step behind, in the leader's own footsteps
		arc = clampf(l.s + (l.dir == 0 ? -gap : gap), lo, hi);
	}
	w.side = l.side;
	w.dir = l.dir;
	w.s = arc;
	w.u = u;
	w.on_way = true;
	sidewalk_pose(s, l.seg, l.side, arc, u, at);
	return at;
}

} // namespace

void add_company(CityState &s, int leader) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Pedestrian w;
	dress_walker(t, p, w, partner_age(t, p, t.peds[size_t(leader)].age), s.catalog.blast.walker_hp);
	const int id = alloc_walker(t);
	Pedestrian &l = t.peds[size_t(leader)];
	l.speed = std::min(l.speed, w.speed);
	l.pace = std::min(l.pace, l.speed);
	w.with = leader;
	w.mode = PedMode::FOLLOW;
	w.flank = l.u > 0 ? -1.f : 1.f; // toward the sidewalk's middle
	w.speed = l.speed;
	w.pace = l.pace;
	w.cur = place_by(s, w, l);
	w.cur.fx = l.cur.fx;
	w.cur.fz = l.cur.fz;
	l.mate = id;
	t.peds[size_t(id)] = std::move(w);
	walker_born(t, id);
	t.ped_count++;
}

void follow_leaders(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	for (Pedestrian &w : t.peds) {
		if (!w.alive || w.with < 0) continue;
		const Pedestrian &l = t.peds[size_t(w.with)];
		const bool held = held_by_fright(t, w);
		const Pose want = place_by(s, w, l);
		const V2 to(want.x - w.cur.x, want.z - w.cur.z);
		const float far = to.len(), step = held ? 0.f : std::min(far, std::max(l.pace, p.ped_moving_ms) * p.ped_company_catch * dt);
		if (far > ARRIVED_M) {
			const V2 d = to.norm();
			w.cur.x += d.x * step;
			w.cur.z += d.y * step;
		}
		w.cur.y = want.y;
		// walking it faces its way; level with its place it faces as its leader does
		const V2 face = far > company_gap(p) * p.ped_company_catch - company_gap(p) ? to.norm() : V2(l.cur.fx, l.cur.fz);
		w.cur.fx = face.x;
		w.cur.fz = face.y;
		w.walked += step;
		w.pace = step / std::max(dt, MIN_STEP_S);
		const bool standing = l.mode == PedMode::WAIT || w.pace <= p.ped_moving_ms || held;
		w.rest = clampf(w.rest + (standing ? dt : -dt) / p.ped_rest_fade_s, 0.f, 1.f);
		step_act(s.traffic, p, w, dt);
		ease_facing(s.traffic, p, w, dt);
	}
}

} // namespace traffic
} // namespace city
