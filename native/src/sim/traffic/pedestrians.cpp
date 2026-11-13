// Pedestrians (Roadcraft src/sim/peds.ts): walkers come out (ped_spawn.cpp) with a walking route, walk their own track on
// the sidewalk, go round each corner and over the crosswalks on the way (ped_routes.cpp, ped_ways.cpp), wait at the kerb for the walk
// phase or a gap in the traffic (ped_crossing.cpp) and at the end of the route go in at a door of the street (ped_doors.cpp);
// among the others and the street furniture on every way as ped_crowd.cpp moves them (a stripe across each way, the pace the
// bodies ahead leave), before a kerb in its queue (ped_kerb.cpp); with somebody, as a party (ped_party.cpp).
#include "sim/roads/parking_layout.h"
#include "sim/traffic/kerb_stops.h"
#include "sim/traffic/swaths.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

constexpr float MIN_LEG_M = 0.05f;

// A straight line toward ways[wi] from where the walker stands.
void aim_line(Pedestrian &w) {
	const Waypoint &to = w.ways[w.wi];
	w.lx = w.cur.x; w.ly = w.cur.y; w.lz = w.cur.z;
	w.llen = std::max(MIN_LEG_M, std::hypot(to.x - w.lx, to.z - w.lz));
	w.ls = 0;
	onto_line(w);
}

void where_on_sidewalk(const CityState &s, Pedestrian &w) {
	const RoadSegment &g = s.roads.segs[size_t(w.seg)];
	double t, dist;
	closest_t(g.q, w.cur.x, w.cur.z, t, dist);
	w.s = float(arc_of_t(g, t));
	Pose mid;
	sidewalk_pose(s, w.seg, w.side, w.s, 0.f, mid);
	w.u = (w.cur.x - mid.x) * mid.fz - (w.cur.z - mid.z) * mid.fx;
}

// On the sidewalk where it stands (where_on_sidewalk), whatever its room: a walker that ends its way past its room's edge is stepped
// back in by the stripes at its lateral pace (ped_stripes.cpp side_limits), never moved there at once. From there it aims at its own side
// of the way again (the line it came by aimed it at the line).
void onto_sidewalk(const CityState &s, Pedestrian &w) {
	enter_mode(w, PedMode::SIDEWALK);
	where_on_sidewalk(s, w);
	w.u_goal = w.dir == 0 ? -w.track : w.track;
	w.stripe_held = 0;
}

void turn_round(Pedestrian &w) {
	w.dir = 1 - w.dir;
	w.u_goal = w.dir == 0 ? -w.track : w.track;
	w.stripe_held = 0;
}

// Where it stands on its sidewalk, its body and its way heading the way it walks.
void stand_on_sidewalk(const CityState &s, Pedestrian &w) {
	sidewalk_pose(s, w.seg, w.side, w.s, w.u, w.cur);
	if (w.dir == 1) { w.cur.fx = -w.cur.fx; w.cur.fz = -w.cur.fz; }
	w.way_x = w.cur.fx;
	w.way_z = w.cur.fz;
}

float run_share(const CityState &s, const Pedestrian &w) {
	return (s.traffic.time < w.panic_until ? s.catalog.blast.panic_gain : 1.f) * wounded_pace(s.catalog.blast, w); // frightened by a bang: runs
}

// The one it walked with walks on by itself (ped_party.cpp).
void despawn(TrafficState &t, int id, bool arrived) {
	off_crosswalk(t, id);
	const int mate = t.peds[id].mate, with = t.peds[id].with;
	t.ped_waste += size_t(t.peds[id].route.cap);
	t.peds[id] = Pedestrian();
	if (with >= 0) t.peds[size_t(with)].mate = -1;
	if (mate >= 0) t.peds[size_t(mate)].with = -1;
	t.free_peds.push_back(id);
	t.ped_count--;
	t.ped_arrived += arrived ? 1 : 0;
}

// A party leaves the street together (ped_party.cpp): the one it walks with goes too when near.
void despawn_party(CityState &s, int id, bool arrived) {
	TrafficState &t = s.traffic;
	const int other = company_near(t, s.catalog.traffic, t.peds[size_t(id)]);
	despawn(t, id, arrived);
	if (other >= 0) despawn(t, other, arrived);
}

// A casualty belongs to what happened, not to the crowd round the camera: whoever lies down (a trip, a blow) or bleeds is not taken
// away by the walking window, by distance alone, nor by the one they walked with (ped_party.cpp company_near), but stays until they
// rise (then the window takes them as anybody) or die (wounds.cpp lays them dead); a body lying there with nobody to die in it never
// comes about (Pwalkers-14). Bounded: who bleeds always dies.
bool casualty(const TrafficState &t, const Pedestrian &w) { return t.time < w.down_until || w.bleed > 0.f; }

namespace {

constexpr float CROSS_RETRY_S = 0.2f; // a walker waiting at the kerb tries the crosswalk 5 times a second

void walk(CityState &s, int id, float dt) {
	TrafficState &t = s.traffic;
	Pedestrian &w = t.peds[id];
	if (w.mode == PedMode::KERB) return; // a kerb stop moves it
	if (!walkable(s, w.seg)) { despawn(t, id, false); return; }
	if (t.time < w.down_until || t.time < w.flinch_until) return; // down (a trip, trips.cpp, or a blow: the render shows the body) or bent over a wound
	if (held_by_fright(t, w)) return;
	turn_from_bang(s, w);
	if (w.mode == PedMode::LOT) {
		walk_lot(s, id, dt);
		return;
	}
	if (w.mode == PedMode::WAIT) {
		step_off_swath(s, w, dt); // off the way a car drives through where it waits
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
	if (transit::walker_held(s, V2(w.cur.x, w.cur.z), V2(w.cur.fx, w.cur.fz))) return; // a train on the ground ahead
	const float step = w.pace * run_share(s, w) * dt; // the crowd sets the pace on every way (ped_crowd.cpp), held by the same step (hold_apart)
	w.walked += step;
	if (w.mode == PedMode::WAY) {
		w.ls += step;
		if (w.ls < w.llen) return;
		off_crosswalk(t, id);
		end_line(s, id);
		return;
	}
	if (w.company_due) add_company(s, id); // its company still to come out of the door behind it (ped_party.cpp)
	Pedestrian &on = t.peds[size_t(id)]; // the company may have grown the list
	const RoadSegment &g = s.roads.segs[on.seg];
	on.s += on.dir == 0 ? step : -step;
	if (on.call_s >= 0 && (on.dir == 0 ? on.s >= on.call_s : on.s <= on.call_s)) { // across from the door it was called to: the stop takes it
		kerb_came(s, on);
		return;
	}
	if (on.call_s < 0 && on.door.building >= 0 && (on.dir == 0 ? on.s >= on.door.s : on.s <= on.door.s)) {
		go_in(s, id);
		return;
	}
	const bool end = on.dir == 0 ? on.s >= arc_hi(g) : on.s <= arc_lo(g);
	if (!end) return;
	on.s = clampf(on.s, arc_lo(g), arc_hi(g));
	at_node(s, id);
}

void pose(const CityState &s, Pedestrian &w) {
	if (w.mode == PedMode::KERB) return;
	if (w.mode == PedMode::SIDEWALK) {
		stand_on_sidewalk(s, w);
		return;
	}
	const Waypoint &to = w.ways[w.wi];
	V2 d = V2(to.x - (w.mode != PedMode::WAIT ? w.lx : w.cur.x), to.z - (w.mode != PedMode::WAIT ? w.lz : w.cur.z)).norm();
	if (w.mode == PedMode::WAIT) { w.cur.fx = d.x; w.cur.fz = d.y; return; } // standing at the kerb, facing the crossing
	const float f = clampf(w.ls / w.llen, 0.f, 1.f);
	w.cur = { lerpf(w.lx, to.x, f) + d.y * w.u, lerpf(w.ly, to.y, f), lerpf(w.lz, to.z, f) - d.x * w.u, d.x, d.y }; // u across its line
}

// After pose(): what it left in `cur` is its way's heading there, kept for the crowd (under a kerb stop's hand the stop has set
// that heading: kerb_stops.cpp kerb_place); the body, which faced `was`, turns toward it no faster than a body turns (round to
// a bang and back: ease_facing).
void turn_body(const TrafficState &t, const TrafficParams &p, Pedestrian &w, const V2 &was, float dt) {
	if (w.mode != PedMode::KERB) {
		w.way_x = w.cur.fx;
		w.way_z = w.cur.fz;
	}
	if (w.turning) {
		ease_facing(t, p, w, dt);
		return;
	}
	const float most = p.ped_turn_rad_s * dt;
	const float by = clampf(std::atan2(was.x * w.way_z - was.y * w.way_x, was.x * w.way_x + was.y * w.way_z), -most, most);
	const float c = std::cos(by), n = std::sin(by);
	w.cur.fx = was.x * c - was.y * n;
	w.cur.fz = was.x * n + was.y * c;
}

} // namespace

void ped_gone(CityState &s, int id) { despawn(s.traffic, id, false); }

void remove_pedestrian(CityState &s, int id) {
	if (id >= 0 && id < int(s.traffic.peds.size()) && s.traffic.peds[size_t(id)].alive) despawn(s.traffic, id, false);
}

void down_pedestrian(CityState &s, int id, float seconds) {
	Pedestrian &w = s.traffic.peds[size_t(id)];
	w.down_until = float(s.traffic.time) + seconds;
}

void reset_pedestrians(CityState &s) {
	TrafficState &t = s.traffic;
	t.peds.clear();
	t.free_peds.clear();
	t.ped_pool.clear();
	t.ped_waste = 0;
	t.ped_count = 0;
	t.node_crossers.assign(s.roads.nodes.size(), {});
	t.shocks.clear();
	t.shock_look = 0;
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
	const double target = people_wanted(s, now) * s.traffic_levels.people * s.traffic_levels.population;
	const bool filling = t.ped_count < p.fill_share * target; // a street just opened: the crowd is on it already
	int spawns = filling ? int(p.ped_fill_per_step) : (t.ped_count < target ? int(p.ped_spawn_per_step) : 0);
	spawns = std::min(spawns, int(p.ped_routes_per_step));
	read_doors(s);
	look_at_shocks(s);
	const float cx = s.camera_x.load(), cz = s.camera_z.load();
	const float out = p.ped_window_m * p.ped_despawn_window_share, out2 = out * out; // beyond it: gone, not lost
	t.ped_waiting = 0;
	file_swaths(s); // the vehicles off the lanes have moved (step_lots): where their swaths are this tick
	crowd_step(s, dt);
	for (int id = 0; id < int(t.peds.size()); id++) {
		Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive) continue;
		const float dx = w.cur.x - cx, dz = w.cur.z - cz;
		if (dx * dx + dz * dz > out2 && w.mode != PedMode::KERB && !casualty(t, w)) { // out of the walking window: gone, not lost
			despawn_party(s, id, false);
			continue;
		}
		const V2 faced(w.cur.fx, w.cur.fz);
		walk(s, id, dt);
		if (!t.peds[size_t(id)].alive) continue;
		Pedestrian &after = t.peds[size_t(id)]; // its company may have been born: the list may have grown
		const bool standing = after.mode == PedMode::WAIT || after.pace <= p.ped_moving_ms || held_by_fright(t, after);
		after.rest = clampf(after.rest + (standing ? dt : -dt) / p.ped_rest_fade_s, 0.f, 1.f);
		step_act(t, p, after, dt);
		pose(s, after);
		turn_body(t, p, after, faced, dt);
	}
	if (spawns <= 0) return;
	file_spawn_cells(t); // everybody has moved: the births look at where they stand now, and at each other
	for (int k = 0; k < spawns; k++) spawn_walker(s, now, !filling);
	t.spawn_cells.live = false;
}

} // namespace traffic
} // namespace city
