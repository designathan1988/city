// Pedestrians (Roadcraft src/sim/peds.ts): walkers appear on sidewalks weighted by the zones beside them (commerce
// pulls most) with a destination node 150-900 m away and a walking route to it; they walk their own track on the
// sidewalk, go round each corner and over the crosswalks on the way (ped_routes.cpp), wait at the kerb for the walk
// phase or a gap in the traffic (ped_crossing.cpp) and go indoors at the destination.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr int PICK_TRIES = 12, DEST_TRIES = 12;
constexpr float SCALE_MIN = 0.9f, SCALE_MAX = 1.1f, MIN_LEG_M = 0.05f;
constexpr float CROSS_RETRY_S = 0.2f; // a walker waiting at the kerb tries the crosswalk 5 times a second
constexpr float PED_WINDOW_OUT = 1.2f; // this many ped_window_m away it is gone (not counted lost)

bool has_side(const Profile &p, int side) { return side == 0 ? p.has_walk_right : p.has_walk_left; }
float arc_lo(const RoadSegment &g) { return float(g.retreat_a); }
float arc_hi(const RoadSegment &g) { return float(g.len - g.retreat_b); }
float track_u(const Pedestrian &p) { return p.dir == 0 ? -p.track : p.track; }
int toward(const CityState &s, const Pedestrian &w) { return w.dir == 0 ? s.roads.segs[w.seg].b : s.roads.segs[w.seg].a; }

// Only the walkable segments inside ped_window_m: walkers exist near the camera, and that caps the crowd.
int pick_seg(CityState &s) {
	TrafficState &t = s.traffic;
	const int n = int(t.ped_segs.size());
	for (int k = 0; k < PICK_TRIES && n > 0; k++) {
		const int seg = t.ped_segs[size_t(t.rng.next() % uint32_t(n))];
		if (!walkable(s, seg)) continue;
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		const float *w = s.catalog.traffic.zone_walk;
		float wt = std::max(w[s.catalog.zone(g.zl).base % BASE_ZONES], w[s.catalog.zone(g.zr).base % BASE_ZONES]);
		if (t.rng.f() < wt) return seg;
	}
	return -1;
}

// A destination node ped_trip_min_m..max_m from `from` and the walking route to it.
bool plan_trip(CityState &s, int from, std::vector<int> &route) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const RoadNode &a = s.roads.nodes[from];
	const int n = int(s.roads.segs.size());
	for (int k = 0; k < DEST_TRIES && n > 0; k++) {
		int seg = int(t.rng.next() % uint32_t(n));
		if (!walkable(s, seg)) continue;
		int goal = s.roads.segs[seg].a;
		const RoadNode &b = s.roads.nodes[goal];
		float d = float(std::hypot(b.x - a.x, b.z - a.z));
		if (d < p.ped_trip_min_m || d > p.ped_trip_max_m) continue;
		return find_walk_route(s, from, goal, route) && route.size() >= 2;
	}
	return false;
}

int alloc(TrafficState &t) {
	if (!t.free_peds.empty()) {
		int id = t.free_peds.back();
		t.free_peds.pop_back();
		return id;
	}
	t.peds.emplace_back();
	return int(t.peds.size()) - 1;
}

bool spawn_one(CityState &s) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	int seg = pick_seg(s);
	if (seg < 0) return false;
	const Profile &pf = s.roads.profile_of(seg);
	const RoadSegment &g = s.roads.segs[seg];
	Pedestrian w;
	w.alive = true;
	w.seg = seg;
	w.dir = int(t.rng.next() & 1);
	std::vector<int> route;
	if (!plan_trip(s, toward(s, w), route)) return false;
	route_set(t.ped_pool, w.route, t.ped_waste, route.data(), int(route.size()));
	w.look = t.rng.next();
	w.side = has_side(pf, 0) && (!has_side(pf, 1) || (t.rng.next() & 1)) ? 0 : 1;
	w.s = t.rng.range(arc_lo(g), arc_hi(g));
	w.speed = t.rng.range(p.walk_speed_min, p.walk_speed_max);
	w.track = t.rng.range(p.walk_track_min, p.walk_track_max);
	w.scale = t.rng.range(SCALE_MIN, SCALE_MAX);
	w.walked = t.rng.range(0.f, w.speed);
	sidewalk_pose(s, seg, w.side, w.s, track_u(w), w.cur);
	w.prev = w.cur;
	int id = alloc(t);
	t.peds[id] = std::move(w);
	t.ped_count++;
	return true;
}

void off_crosswalk(TrafficState &t, int id) {
	Pedestrian &w = t.peds[id];
	if (w.cross_seg < 0) return;
	auto &l = t.node_crossers[w.way_node];
	l.erase(std::remove(l.begin(), l.end(), id), l.end());
	w.cross_seg = -1;
}

void despawn(TrafficState &t, int id, bool arrived) {
	off_crosswalk(t, id);
	t.ped_waste += size_t(t.peds[id].route.cap);
	t.peds[id] = Pedestrian();
	t.free_peds.push_back(id);
	t.ped_count--;
	t.ped_arrived += arrived ? 1 : 0;
}

// Next straight line toward ways[wi] from where the walker stands; a crosswalk waits for the walk phase and a gap.
void begin_line(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Pedestrian &w = t.peds[id];
	const Waypoint &to = w.ways[w.wi];
	if (to.cross >= 0 && (!walk_ok(s, w.way_node, to.cross) || cars_on_crossing(s, w.way_node, to.cross))) {
		w.mode = PedMode::WAIT;
		return;
	}
	w.mode = PedMode::WAY;
	w.lx = w.cur.x; w.ly = w.cur.y; w.lz = w.cur.z;
	w.llen = std::max(MIN_LEG_M, std::hypot(to.x - w.lx, to.z - w.lz));
	w.ls = 0;
	if (to.cross >= 0) {
		w.cross_seg = to.cross;
		t.node_crossers[w.way_node].push_back(id);
	}
}

// At the end of a sidewalk: arrived, or plan the way round the node onto the next street's sidewalk.
void at_node(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Pedestrian &w = t.peds[id];
	const int n = toward(s, w);
	if (w.ri + 1 >= w.route.len) { despawn(t, id, true); return; }
	int next = seg_between(s, n, route_at(t.ped_pool, w.route, w.ri + 1)), side_out = 0;
	int k = next < 0 ? -1 : plan_corner(s, n, w.seg, w.side, next, w.ways, side_out);
	if (k < 0) { despawn(t, id, false); return; }
	const RoadSegment &ng = s.roads.segs[next];
	w.ri++;
	w.seg = next;
	w.dir = ng.a == n ? 0 : 1;
	w.side = side_out;
	w.s = w.dir == 0 ? arc_lo(ng) : arc_hi(ng);
	Pose start;
	sidewalk_pose(s, next, w.side, w.s, track_u(w), start);
	w.ways[k++] = Waypoint{ start.x, start.y, start.z, -1 };
	w.wn = k;
	w.wi = 0;
	w.way_node = n;
	begin_line(s, id);
}

void walk(CityState &s, int id, float dt) {
	TrafficState &t = s.traffic;
	Pedestrian &w = t.peds[id];
	if (!walkable(s, w.seg)) { despawn(t, id, false); return; }
	if (w.mode == PedMode::WAIT) {
		w.wait += dt;
		t.ped_waiting++;
		if (w.cross_cd > 0) { // 5 Hz, not once per tick
			w.cross_cd -= dt;
			return;
		}
		w.cross_cd = CROSS_RETRY_S;
		begin_line(s, id);
		if (w.mode == PedMode::WAIT) return;
	}
	float step = w.speed * dt;
	w.walked += step;
	if (w.mode == PedMode::WAY) {
		w.ls += step;
		if (w.ls < w.llen) return;
		off_crosswalk(t, id);
		if (++w.wi >= w.wn) { w.mode = PedMode::SIDEWALK; return; }
		Waypoint &done = w.ways[w.wi - 1];
		w.cur.x = done.x; w.cur.y = done.y; w.cur.z = done.z;
		begin_line(s, id);
		return;
	}
	const RoadSegment &g = s.roads.segs[w.seg];
	w.s += w.dir == 0 ? step : -step;
	bool end = w.dir == 0 ? w.s >= arc_hi(g) : w.s <= arc_lo(g);
	if (!end) return;
	w.s = clampf(w.s, arc_lo(g), arc_hi(g));
	at_node(s, id);
}

void pose(const CityState &s, Pedestrian &w) {
	w.prev = w.cur;
	if (w.mode == PedMode::SIDEWALK) {
		sidewalk_pose(s, w.seg, w.side, w.s, track_u(w), w.cur);
		if (w.dir == 1) { w.cur.fx = -w.cur.fx; w.cur.fz = -w.cur.fz; }
		return;
	}
	const Waypoint &to = w.ways[w.wi];
	V2 d = V2(to.x - (w.mode == PedMode::WAY ? w.lx : w.cur.x), to.z - (w.mode == PedMode::WAY ? w.lz : w.cur.z)).norm();
	if (w.mode == PedMode::WAIT) { w.cur.fx = d.x; w.cur.fz = d.y; return; } // standing at the kerb, facing the crossing
	float f = clampf(w.ls / w.llen, 0.f, 1.f);
	w.cur = { lerpf(w.lx, to.x, f), lerpf(w.ly, to.y, f), lerpf(w.lz, to.z, f), d.x, d.y };
}

} // namespace

void reset_pedestrians(CityState &s) {
	TrafficState &t = s.traffic;
	t.peds.clear();
	t.free_peds.clear();
	t.ped_pool.clear();
	t.ped_waste = 0;
	t.ped_count = 0;
	t.node_crossers.assign(s.roads.nodes.size(), {});
}

void drop_pedestrians_on_seg(CityState &s, int seg) {
	TrafficState &t = s.traffic;
	for (int id = 0; id < int(t.peds.size()); id++) {
		const Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive || (w.seg != seg && w.cross_seg != seg)) continue;
		despawn(t, id, false);
	}
}

void step_pedestrians(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	// the crowd follows the sidewalk metres inside ped_window_m, so it has a natural ceiling
	const double target = t.ped_metres * p.people_per_lane_m * p.people_share;
	int spawns = t.ped_count < p.fill_share * target ? int(p.ped_fill_per_step) : (t.ped_count < target ? int(p.ped_spawn_per_step) : 0);
	spawns = std::min(spawns, int(p.ped_routes_per_step));
	const float cx = s.camera_x.load(), cz = s.camera_z.load();
	const float out = p.ped_window_m * PED_WINDOW_OUT, out2 = out * out;
	t.ped_waiting = 0;
	for (int id = 0; id < int(t.peds.size()); id++) {
		Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive) continue;
		const float dx = w.cur.x - cx, dz = w.cur.z - cz;
		if (dx * dx + dz * dz > out2) { // out of the walking window: gone, not lost
			despawn(t, id, false);
			continue;
		}
		walk(s, id, dt);
		if (t.peds[size_t(id)].alive) pose(s, t.peds[size_t(id)]);
	}
	for (int k = 0; k < spawns; k++) spawn_one(s);
}

} // namespace traffic
} // namespace city
