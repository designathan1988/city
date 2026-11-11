// A bang heard (Roadcraft walk.ts frighten, after GTA V: everybody near a gunshot flees, some film it): first the head round to it; near it some
// crouch where they are, far off some hold a phone up at it, and then they run, away from it by whatever street leads there, back on their way when
// the fright is over. A blast is a stampede: some trip and go down, some break down crouched, some film it from far enough off.
#include "core/random.h"
#include "sim/traffic/kerb_stops.h"
#include "sim/traffic/ped_system.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace city {
namespace traffic {

namespace {

constexpr uint32_t ROLL_SALT = 0x7a11, TRIP_SALT = 0x51, RUN_SALT = 0x2d1, MOURN_SALT = 0x3e1;
constexpr uint32_t BYTE = 255; // a roll is one byte of a person's hash, what the data's *_roll are out of
constexpr float RUN_SHORTEST = 0.7f, RUN_SPREAD = 0.6f; // of the fright's seconds, by the person
// How the same roll spreads the seconds (walk.ts: `roll % n`)
constexpr int CROUCH_SPREAD = 4, FILM_SPREAD = 5, STAMPEDE_CROUCH_SPREAD = 9, FALL_SPREAD = 7, MOURN_SPREAD = 4, TRIP_SPREAD = 4, TRIP_SHIFT = 3;
constexpr int LOOK_STEPS = 6, TRIP_STEPS = 10;
constexpr float LOOK_STEP_S = 0.1f, TRIP_STEP_S = 0.2f;
constexpr float ALIGNED_RAD = 1e-3f;

V2 node_at(const CityState &s, int n) { return V2(float(s.roads.nodes[size_t(n)].x), float(s.roads.nodes[size_t(n)].z)); }

uint32_t roll_of(const Pedestrian &w, uint32_t salt) { return hash_combine(w.look, salt) & BYTE; }

void hold(Pedestrian &w, PedAct act, float now, float seconds, float x, float z) {
	w.held_act = act;
	w.held_until = now + seconds;
	w.face_x = x;
	w.face_z = z;
	w.turn_x = w.cur.fx;
	w.turn_z = w.cur.fz;
	w.turning = true;
}

// The hold's seconds are added to the run: it runs as long as it would have.
void stop_for(Pedestrian &w, PedAct act, float now, float seconds, float x, float z) {
	hold(w, act, now, seconds, x, z);
	w.panic_until += seconds;
}

// A shot: what the person does before running.
void at_a_shot(const TrafficParams &p, Pedestrian &w, float now, float d, float x, float z) {
	const uint32_t roll = roll_of(w, ROLL_SALT);
	const bool near = d < p.ped_react_near_m;
	if (near && float(roll) < p.ped_react_crouch_roll) {
		stop_for(w, PedAct::CROUCH, now, p.ped_react_crouch_s + float(roll % CROUCH_SPREAD), x, z);
	} else if (!near && float(roll) >= p.ped_react_crouch_roll && float(roll) < p.ped_react_film_roll) {
		stop_for(w, PedAct::PHOTO, now, p.ped_react_film_s + float(roll % FILM_SPREAD), x, z);
	} else {
		const float look = near ? p.ped_react_glance_s : p.ped_react_look_s + float(roll % LOOK_STEPS) * LOOK_STEP_S;
		stop_for(w, PedAct::LOOK, now, look, x, z);
		if (near && float(roll_of(w, TRIP_SALT)) < p.ped_react_trip_roll) {
			w.trip_at = now + look + p.ped_trip_after_s + float(roll % TRIP_STEPS) * TRIP_STEP_S;
			w.trip_s = p.ped_trip_s + float((hash_u32(w.look) >> TRIP_SHIFT) % TRIP_SPREAD);
		}
	}
}

// A blast: `after` is what it is still busy with, a fall or a wound.
void in_a_stampede(const TrafficParams &p, Pedestrian &w, float now, float after, float d, float x, float z) {
	const uint32_t roll = roll_of(w, ROLL_SALT);
	if (float(roll) < p.ped_stampede_trip_roll && d < p.ped_stampede_trip_m) {
		w.trip_at = now + after + p.ped_trip_after_s + float(roll_of(w, TRIP_SALT)) / float(BYTE) * p.ped_stampede_trip_span_s;
		w.trip_s = p.ped_trip_s + float((hash_u32(w.look) >> TRIP_SHIFT) % TRIP_SPREAD);
		return;
	}
	if (after > 0) return;
	if (float(roll) < p.ped_stampede_crouch_roll) {
		hold(w, PedAct::CROUCH, now, p.ped_stampede_crouch_s + float(roll % STAMPEDE_CROUCH_SPREAD), x, z);
	} else if (float(roll) < p.ped_stampede_fall_roll && d < p.ped_stampede_fall_m) {
		w.trip_at = now + p.ped_stampede_fall_after_s;
		w.trip_s = p.ped_stampede_fall_s + float(roll % FALL_SPREAD);
	} else if (float(roll) < p.ped_stampede_film_roll && d > p.ped_stampede_film_m) {
		stop_for(w, PedAct::PHOTO, now, p.ped_react_film_s + float(roll % FILM_SPREAD), x, z);
	}
}

float off_the_bang(const CityState &s, const Pedestrian &w, int node) { return (node_at(s, node) - V2(w.bang_x, w.bang_z)).len(); }

// Its route from `first` on (`start` before it when there is one), to where it was going all along.
void route_from(CityState &s, Pedestrian &w, int start, int first) {
	static thread_local std::vector<int> way;
	TrafficState &t = s.traffic;
	const int goal = route_at(t.ped_pool, w.route, w.route.len - 1);
	way.clear();
	if (!find_walk_route(s, first, goal, way) || way.empty()) way.assign(1, first);
	if (start >= 0) way.insert(way.begin(), start);
	route_set(t.ped_pool, w.route, t.ped_waste, way.data(), int(way.size()));
	w.ri = 0;
	w.door = PedDoor();
	w.goal_side = -1;
}

} // namespace

void frighten_pedestrian(CityState &s, int id, float seconds, float x, float z, bool stampede) {
	const TrafficParams &p = s.catalog.traffic;
	Pedestrian &w = s.traffic.peds[size_t(id)];
	const float now = float(s.traffic.time);
	// Busy with its own fall, wound or grief: the fright waits till that is over.
	const float after = std::max({ w.down_until - now, w.flinch_until - now, w.mourning ? w.held_until - now : 0.f, 0.f });
	w.panic_until = now + after + seconds * (RUN_SHORTEST + RUN_SPREAD * float(roll_of(w, RUN_SALT)) / float(BYTE));
	w.bang_x = x;
	w.bang_z = z;
	w.flee_due = true;
	const float d = std::hypot(w.cur.x - x, w.cur.z - z);
	if (stampede) in_a_stampede(p, w, now, after, d, x, z);
	else if (after <= 0) at_a_shot(p, w, now, d, x, z);
}

void startle_pedestrians(CityState &s, float x, float z, float reach, float seconds) {
	TrafficState &t = s.traffic;
	for (int id = 0; id < int(t.peds.size()); id++) {
		const Pedestrian &w = t.peds[size_t(id)];
		if (w.alive && std::hypot(w.cur.x - x, w.cur.z - z) <= reach) frighten_pedestrian(s, id, seconds, x, z, false);
	}
}

void add_shock(CityState &s, float x, float z, float reach) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	if (t.shocks.size() >= size_t(p.ped_shocks_most)) t.shocks.erase(t.shocks.begin());
	t.shocks.push_back({ x, z, reach, float(t.time) + p.ped_shock_life_s });
}

void look_at_shocks(CityState &s) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const float now = float(t.time);
	if (t.shocks.empty() || now < t.shock_look) return;
	t.shock_look = now + p.ped_shock_look_s;
	t.shocks.erase(std::remove_if(t.shocks.begin(), t.shocks.end(), [&](const PedShock &k) { return k.until <= now; }), t.shocks.end());
	for (int id = 0; id < int(t.peds.size()); id++) { // each walker once, against the few there are: never a shock against every walker of the map
		const Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive || now < w.panic_until || now < w.held_until) continue;
		for (const PedShock &k : t.shocks) {
			if (std::hypot(w.cur.x - k.x, w.cur.z - k.z) > k.reach) continue;
			frighten_pedestrian(s, id, p.ped_shot_fright_s, k.x, k.z, false);
			break;
		}
	}
}

void mourn_pedestrian(CityState &s, int id) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const Pedestrian &shot = t.peds[size_t(id)];
	int first = id;
	while (t.peds[size_t(first)].with >= 0) first = t.peds[size_t(first)].with;
	const float now = float(t.time);
	for (int k = first; k >= 0; k = t.peds[size_t(k)].mate) {
		Pedestrian &q = t.peds[size_t(k)];
		if (k == id || !q.alive || now < q.down_until) continue;
		if (std::hypot(q.cur.x - shot.cur.x, q.cur.z - shot.cur.z) > p.ped_mourn_m || q.hits > 0) continue;
		hold(q, PedAct::CROUCH, now, p.ped_mourn_s + float(hash_combine(q.look, MOURN_SALT) % MOURN_SPREAD), shot.cur.x, shot.cur.z);
		q.mourning = true;
	}
}

bool held_by_fright(const TrafficState &t, Pedestrian &w) {
	const float now = float(t.time);
	if (w.trip_at > 0 && now >= w.trip_at) {
		w.trip_at = 0;
		hold(w, PedAct::CROUCH, now, w.trip_s, w.cur.x + w.cur.fx, w.cur.z + w.cur.fz); // down where it ran, not turned round
	}
	if (now >= w.held_until) w.mourning = false;
	return now < w.held_until || w.act == PedAct::CROUCH; // up again before it walks: the crouch's clip is the whole of the pose
}

void turn_from_bang(CityState &s, Pedestrian &w) {
	if (!w.flee_due || w.mode != PedMode::SIDEWALK) return;
	w.flee_due = false;
	const RoadSegment &g = s.roads.segs[size_t(w.seg)];
	const int ahead = w.dir == 0 ? g.b : g.a, behind = w.dir == 0 ? g.a : g.b;
	const V2 way = node_at(s, ahead) - V2(w.cur.x, w.cur.z), away(w.cur.x - w.bang_x, w.cur.z - w.bang_z);
	if (way.x * away.x + way.y * away.y >= 0) return; // already going away from it
	route_from(s, w, -1, behind);
	kerb_forget_call(w, false); // a vehicle it was called to is left behind: it runs its own way (the stop sees it lost)
	turn_round(w);
}

void flee_on(CityState &s, Pedestrian &w, int n) {
	TrafficState &t = s.traffic;
	const int next = w.ri + 1 < w.route.len ? route_at(t.ped_pool, w.route, w.ri + 1) : -1;
	int best = -1;
	float furthest = next >= 0 ? off_the_bang(s, w, next) : 0.f;
	for (int seg : s.roads.nodes[size_t(n)].segs) {
		if (seg == w.seg || !walkable(s, seg)) continue;
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		const int other = g.a == n ? g.b : g.a;
		const float off = off_the_bang(s, w, other);
		if (off > furthest) {
			furthest = off;
			best = other;
		}
	}
	if (best >= 0) route_from(s, w, n, best);
}

void ease_facing(const TrafficState &t, const TrafficParams &p, Pedestrian &w, float dt) {
	if (!w.turning) return;
	const bool held = t.time < w.held_until;
	const V2 want = held ? V2(w.face_x - w.cur.x, w.face_z - w.cur.z).norm() : V2(w.way_x, w.way_z);
	const float err = std::atan2(w.turn_x * want.y - w.turn_z * want.x, w.turn_x * want.x + w.turn_z * want.y);
	const float most = p.ped_turn_rad_s * dt, by = clampf(err, -most, most);
	const float c = std::cos(by), n = std::sin(by);
	const float x = w.turn_x * c - w.turn_z * n, z = w.turn_x * n + w.turn_z * c;
	w.turn_x = x;
	w.turn_z = z;
	w.cur.fx = x;
	w.cur.fz = z;
	if (!held && std::fabs(err) <= std::max(most, ALIGNED_RAD)) w.turning = false;
}

} // namespace traffic
} // namespace city
