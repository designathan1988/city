// Pedestrians (Roadcraft src/sim/peds.ts): walkers come out (ped_spawn.cpp) with a walking route, walk their own track on
// the sidewalk, go round each corner and over the crosswalks on the way (ped_routes.cpp), wait at the kerb for the walk
// phase or a gap in the traffic (ped_crossing.cpp) and at the end of the route go in at a door of the street (ped_doors.cpp);
// among the others and the street furniture on every way as ped_crowd.cpp moves them (a stripe across each way, the pace the
// bodies ahead leave), at a kerb in a place of its own (ped_kerb.cpp).
#include "sim/traffic/kerb_stops.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

constexpr float MIN_LEG_M = 0.05f;

static void off_crosswalk(TrafficState &t, int id) {
	Pedestrian &w = t.peds[id];
	if (w.cross_seg < 0) return;
	auto &l = t.node_crossers[w.way_node];
	l.erase(std::remove(l.begin(), l.end(), id), l.end());
	w.cross_seg = -1;
}

// A straight line toward ways[wi] from where the walker stands.
void aim_line(Pedestrian &w) {
	const Waypoint &to = w.ways[w.wi];
	w.lx = w.cur.x; w.ly = w.cur.y; w.lz = w.cur.z;
	w.llen = std::max(MIN_LEG_M, std::hypot(to.x - w.lx, to.z - w.lz));
	w.ls = 0;
	onto_line(w);
}

// On the sidewalk at the same place across as where it stands.
void onto_sidewalk(CityState &s, Pedestrian &w) {
	w.mode = PedMode::SIDEWALK;
	Pose mid;
	sidewalk_pose(s, w.seg, w.side, w.s, 0.f, mid);
	const float half = walk_half(s, w.seg, w.side);
	w.u = clampf((w.cur.x - mid.x) * mid.fz - (w.cur.z - mid.z) * mid.fx, -half, half);
}

void despawn(TrafficState &t, int id, bool arrived) {
	off_crosswalk(t, id);
	free_kerb_place(t, id);
	const int mate = t.peds[id].mate, with = t.peds[id].with;
	t.ped_waste += size_t(t.peds[id].route.cap);
	t.peds[id] = Pedestrian();
	if (with >= 0) t.peds[size_t(with)].mate = -1;
	if (mate >= 0) {
		t.peds[size_t(mate)].with = -1;
		despawn(t, mate, arrived);
	}
	t.free_peds.push_back(id);
	t.ped_count--;
	t.ped_arrived += arrived ? 1 : 0;
}

namespace {

constexpr float CROSS_RETRY_S = 0.2f; // a walker waiting at the kerb tries the crosswalk 5 times a second

int toward(const CityState &s, const Pedestrian &w) { return w.dir == 0 ? s.roads.segs[w.seg].b : s.roads.segs[w.seg].a; }

// Heading for the kerb of a crosswalk with every place there taken: it asks again, and aims at the place it gets.
void retry_kerb_place(CityState &s, int id) {
	Pedestrian &w = s.traffic.peds[size_t(id)];
	if (w.slot >= 0 || w.ways[w.wi].cross >= 0 || w.wi + 1 >= w.wn || w.ways[w.wi + 1].cross < 0) return;
	take_kerb_place(s, id);
	if (w.slot >= 0) aim_line(w);
}

void begin_line(CityState &s, int id);

// Off the end of a line (its lateral offset kept): onto the next line, or onto the sidewalk at the same place across.
void end_line(CityState &s, int id) {
	Pedestrian &w = s.traffic.peds[size_t(id)];
	const Waypoint &done = w.ways[w.wi];
	const V2 d = V2(done.x - w.lx, done.z - w.lz).norm();
	w.cur.x = done.x + d.y * w.u;
	w.cur.z = done.z - d.x * w.u;
	w.cur.y = done.y;
	if (++w.wi < w.wn) {
		begin_line(s, id);
		return;
	}
	onto_sidewalk(s, w);
}

// Next straight line toward ways[wi] from where the walker stands; a crosswalk waits for the walk phase and a gap.
void begin_line(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Pedestrian &w = t.peds[id];
	take_kerb_place(s, id); // toward the kerb of a crosswalk: to its own place there
	const Waypoint &to = w.ways[w.wi];
	if (to.cross >= 0 && (!walk_ok(s, w.way_node, to.cross) || cars_on_crossing(s, w.way_node, to.cross))) {
		w.mode = PedMode::WAIT;
		return;
	}
	w.mode = PedMode::WAY;
	aim_line(w);
	if (to.cross >= 0) {
		free_kerb_place(t, id); // onto the crosswalk: the place at the kerb is free again
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
	const bool last = w.ri + 2 >= w.route.len;
	int k = next < 0 ? -1 : plan_corner(s, n, w.seg, w.side, next, last ? w.goal_side : -1, w.ways, side_out);
	if (k < 0) { despawn(t, id, false); return; }
	const RoadSegment &ng = s.roads.segs[next];
	w.ri++;
	w.seg = next;
	w.dir = ng.a == n ? 0 : 1;
	w.side = side_out;
	w.s = w.dir == 0 ? arc_lo(ng) : arc_hi(ng);
	on_sidewalk(w);
	Pose start;
	sidewalk_pose(s, next, w.side, w.s, w.u, start);
	w.ways[k++] = Waypoint{ start.x, start.y, start.z, -1 };
	w.wn = k;
	w.wi = 0;
	w.way_node = n;
	if (w.ri + 1 >= w.route.len) aim_at_door(s, w);
	begin_line(s, id);
}

void walk(CityState &s, int id, float dt) {
	TrafficState &t = s.traffic;
	Pedestrian &w = t.peds[id];
	if (w.mode == PedMode::KERB) return; // a kerb stop moves it
	if (!walkable(s, w.seg)) { despawn(t, id, false); return; }
	if (t.time < w.down_until || t.time < w.flinch_until) return; // knocked down by a blow (the render shows the body) or bent over a wound
	if (w.mode == PedMode::LOT) {
		walk_lot(s, id, dt);
		return;
	}
	if (w.mode == PedMode::WAIT) {
		w.wait += dt;
		t.ped_waiting++;
		if (w.cross_cd > 0) { // 5 Hz, not once per tick
			w.cross_cd -= dt;
			return;
		}
		w.cross_cd = CROSS_RETRY_S;
		begin_line(s, id);
		// still waiting: a place nearer the kerb that freed is taken (the queue moves up)
		if (w.mode == PedMode::WAIT && step_to_nearer_place(s, id)) begin_line(s, id);
		if (w.mode == PedMode::WAIT) return;
	}
	if (transit::walker_held(s, V2(w.cur.x, w.cur.z), V2(w.cur.fx, w.cur.fz))) return; // a train on the ground ahead
	const float run = (t.time < w.panic_until ? s.catalog.blast.panic_gain : 1.f) * traffic::wounded_pace(s.catalog.blast, w); // frightened by a bang: runs
	const float step = w.pace * run * dt; // the crowd sets the pace on every way (ped_crowd.cpp)
	w.walked += step;
	if (w.mode == PedMode::WAY) {
		if (w.slot < 0 && (w.cross_cd -= dt) <= 0) { // no place at the kerb ahead yet: asked again at 5 Hz
			w.cross_cd = CROSS_RETRY_S;
			retry_kerb_place(s, id);
		}
		w.ls += step;
		if (w.ls < w.llen) return;
		off_crosswalk(t, id);
		end_line(s, id);
		return;
	}
	const RoadSegment &g = s.roads.segs[w.seg];
	w.s += w.dir == 0 ? step : -step;
	if (w.door.building >= 0 && (w.dir == 0 ? w.s >= w.door.s : w.s <= w.door.s)) {
		go_in(s, id);
		return;
	}
	bool end = w.dir == 0 ? w.s >= arc_hi(g) : w.s <= arc_lo(g);
	if (!end) return;
	w.s = clampf(w.s, arc_lo(g), arc_hi(g));
	at_node(s, id);
}

void pose(const CityState &s, Pedestrian &w) {
	if (w.mode == PedMode::KERB) return;
	if (w.mode == PedMode::SIDEWALK) {
		sidewalk_pose(s, w.seg, w.side, w.s, w.u, w.cur);
		if (w.dir == 1) { w.cur.fx = -w.cur.fx; w.cur.fz = -w.cur.fz; }
		return;
	}
	const Waypoint &to = w.ways[w.wi];
	V2 d = V2(to.x - (w.mode != PedMode::WAIT ? w.lx : w.cur.x), to.z - (w.mode != PedMode::WAIT ? w.lz : w.cur.z)).norm();
	if (w.mode == PedMode::WAIT) { w.cur.fx = d.x; w.cur.fz = d.y; return; } // standing at the kerb, facing the crossing
	const float f = clampf(w.ls / w.llen, 0.f, 1.f);
	w.cur = { lerpf(w.lx, to.x, f) + d.y * w.u, lerpf(w.ly, to.y, f), lerpf(w.lz, to.z, f) - d.x * w.u, d.x, d.y }; // u across its line
}

} // namespace

int ped_on_footway(CityState &s, int seg, int side, float arc) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	Pedestrian w;
	w.alive = true;
	w.seg = seg;
	w.side = side;
	w.hp = s.catalog.blast.walker_hp;
	w.dir = int(t.rng.next() & 1);
	w.look = t.rng.next();
	w.s = clampf(arc, arc_lo(g), arc_hi(g));
	w.speed = t.rng.range(p.walk_speed_min, p.walk_speed_max);
	w.track = t.rng.range(p.walk_track_min, p.walk_track_max);
	w.scale = t.rng.range(SCALE_MIN, SCALE_MAX);
	on_sidewalk(w);
	sidewalk_pose(s, seg, side, w.s, w.u, w.cur);
	std::vector<int> route;
	if (crowded(t, p, w.cur) || !plan_trip(s, toward(s, w), route)) return -1;
	route_set(t.ped_pool, w.route, t.ped_waste, route.data(), int(route.size()));
	w.mode = PedMode::KERB;
	const int id = alloc(t);
	t.peds[size_t(id)] = std::move(w);
	t.ped_count++;
	return id;
}

void ped_gone(CityState &s, int id) { despawn(s.traffic, id, false); }

void remove_pedestrian(CityState &s, int id) {
	if (id >= 0 && id < int(s.traffic.peds.size()) && s.traffic.peds[size_t(id)].alive) despawn(s.traffic, id, false);
}

void down_pedestrian(CityState &s, int id, float seconds) {
	Pedestrian &w = s.traffic.peds[size_t(id)];
	w.down_until = float(s.traffic.time) + seconds;
}

void frighten_pedestrian(CityState &s, int id, float seconds) {
	Pedestrian &w = s.traffic.peds[size_t(id)];
	w.panic_until = float(s.traffic.time) + seconds;
}

void reset_pedestrians(CityState &s) {
	TrafficState &t = s.traffic;
	t.peds.clear();
	t.free_peds.clear();
	t.ped_pool.clear();
	t.ped_waste = 0;
	t.ped_count = 0;
	t.node_crossers.assign(s.roads.nodes.size(), {});
	t.kerb_slots.clear();
	t.crowd.clear();
	t.crowd_keys.clear();
	t.node_walkers.clear();
	t.node_keys.clear();
	t.furniture.clear();
	t.doors.clear();
	t.door_seen.clear();
	t.door_todo.clear();
	t.door_sides.clear();
	t.doors_in = t.doors_out = 0;
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
	const PeopleNow now = people_now(p, float(s.clock.hour()));
	const double target = people_wanted(s, now) * t.levels.people * t.levels.population;
	const bool filling = t.ped_count < p.fill_share * target; // a street just opened: the crowd is on it already
	int spawns = filling ? int(p.ped_fill_per_step) : (t.ped_count < target ? int(p.ped_spawn_per_step) : 0);
	spawns = std::min(spawns, int(p.ped_routes_per_step));
	read_doors(s);
	const float cx = s.camera_x.load(), cz = s.camera_z.load();
	const float out = p.ped_window_m * p.ped_despawn_window_share, out2 = out * out; // beyond it: gone, not lost
	t.ped_waiting = 0;
	crowd_step(s, dt);
	for (int id = 0; id < int(t.peds.size()); id++) {
		Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive) continue;
		const float dx = w.cur.x - cx, dz = w.cur.z - cz;
		if (dx * dx + dz * dz > out2 && w.mode != PedMode::KERB) { // out of the walking window: gone, not lost
			despawn(t, id, false);
			continue;
		}
		if (w.with >= 0) continue; // its leader's step moves it
		walk(s, id, dt);
		if (!t.peds[size_t(id)].alive) continue;
		Pedestrian &after = t.peds[size_t(id)];
		const bool standing = after.mode == PedMode::WAIT || after.pace <= p.ped_moving_ms;
		after.rest = clampf(after.rest + (standing ? dt : -dt) / p.ped_rest_fade_s, 0.f, 1.f);
		pose(s, after);
	}
	follow_leaders(s, dt);
	for (int k = 0; k < spawns; k++) spawn_walker(s, now, !filling);
}

} // namespace traffic
} // namespace city
