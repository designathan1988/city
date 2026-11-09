// Who comes out on foot: a walker of an age and pace, on the streets the land use has people on at this hour, with a
// destination 150-900 m away and a walking route to it. The crowd of a street just opened stands on its sidewalks; after
// that they come out of the doors (ped_doors.cpp), or onto a footway out of the player's sight (sight.h). Some come with company
// (ped_party.cpp).
#include "sim/traffic/sight.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

bool has_side(const Profile &p, int side) { return side == 0 ? p.has_walk_right : p.has_walk_left; }

// Only the walkable segments inside ped_window_m, by the people their zones have on the street now.
int pick_seg(CityState &s, const PeopleNow &now) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const int n = int(t.ped_segs.size());
	for (int k = 0; k < p.ped_spawn_pick_tries && n > 0 && now.peak > 0; k++) {
		const int seg = t.ped_segs[size_t(t.rng.next() % uint32_t(n))];
		if (!walkable(s, seg)) continue;
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		const float here = std::max(zone_people(p, now, s.catalog.zone(g.zl).base % BASE_ZONES), zone_people(p, now, s.catalog.zone(g.zr).base % BASE_ZONES));
		if (t.rng.f() * now.peak < here) return seg;
	}
	return -1;
}

// The walking route to `goal` when that lies ped_trip_min_m..max_m from `from`.
bool route_to(CityState &s, int from, int goal, std::vector<int> &route) {
	const TrafficParams &p = s.catalog.traffic;
	const RoadNode &a = s.roads.nodes[from], &b = s.roads.nodes[goal];
	const float d = float(std::hypot(b.x - a.x, b.z - a.z));
	return d >= p.ped_trip_min_m && d <= p.ped_trip_max_m && find_walk_route(s, from, goal, route) && route.size() >= 2;
}

// Whether a walker standing at `at` would be closer to another than the room walkers keep (two bodies and the gap):
// against every walker, those born this step too (a few are born a step).
bool crowded(const TrafficState &t, const TrafficParams &p, const Pose &at) {
	return walker_near(t, at, 2 * p.ped_body_m + p.ped_gap_m);
}

// Standing at the door, its way out to the sidewalk at `d`.
void at_door(const CityState &s, Pedestrian &w, const PedDoor &d) {
	Pose foot;
	sidewalk_pose(s, w.seg, w.side, d.s, d.u, foot);
	w.s = d.s;
	w.cur.x = d.x;
	w.cur.y = d.y;
	w.cur.z = d.z;
	w.ways[0] = Waypoint{ foot.x, foot.y, foot.z, -1 };
	w.wn = 1;
	w.wi = 0;
	w.mode = PedMode::LOT;
	w.lx = d.x;
	w.ly = d.y;
	w.lz = d.z;
	w.llen = std::hypot(foot.x - d.x, foot.z - d.z);
	w.ls = 0;
	const V2 way = V2(foot.x - d.x, foot.z - d.z).norm();
	w.cur.fx = way.x;
	w.cur.fz = way.y;
	onto_line(w);
}

} // namespace

bool plan_trip(CityState &s, int from, std::vector<int> &route, int &side) {
	const TrafficParams &p = s.catalog.traffic;
	TrafficState &t = s.traffic;
	const int n = int(s.roads.segs.size());
	for (int k = 0; k < p.ped_spawn_dest_tries; k++) {
		const int key = door_sidewalk(s), seg = key / 2;
		if (key < 0 || !walkable(s, seg)) continue;
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		if (!route_to(s, from, g.a, route)) continue;
		if (route.size() < 2 || route[route.size() - 2] != g.b) route.push_back(g.b); // else it comes down the street already
		side = key % 2;
		return true;
	}
	for (int k = 0; k < p.ped_spawn_dest_tries && n > 0; k++) {
		const int seg = int(t.rng.next() % uint32_t(n));
		if (walkable(s, seg) && route_to(s, from, s.roads.segs[size_t(seg)].a, route)) return true;
	}
	return false;
}

PedAge roll_age(TrafficState &t, const TrafficParams &p) {
	const float r = t.rng.f();
	if (r < p.ped_child_share) return PedAge::CHILD;
	return r < p.ped_child_share + p.ped_elder_share ? PedAge::ELDER : PedAge::ADULT;
}

// The age of somebody who goes on foot with no one: a child never does (it walks with a grown-up, the party), so the share of
// children is left out and the others keep their proportion.
PedAge roll_age_alone(TrafficState &t, const TrafficParams &p) {
	const float r = t.rng.f() * (1.f - p.ped_child_share);
	return r < p.ped_elder_share ? PedAge::ELDER : PedAge::ADULT;
}

void dress_walker(TrafficState &t, const TrafficParams &p, Pedestrian &w, PedAge age, float hp) {
	w.alive = true;
	w.hp = hp;
	w.age = age;
	w.look = t.rng.next();
	const float pace = age == PedAge::CHILD ? p.ped_child_pace : age == PedAge::ELDER ? p.ped_elder_pace : 1.f;
	w.speed = t.rng.range(p.walk_speed_min, p.walk_speed_max) * pace;
	w.track = t.rng.range(p.walk_track_min, p.walk_track_max);
	w.scale = t.rng.range(p.ped_spawn_scale_min, p.ped_spawn_scale_max);
	w.walked = t.rng.range(0.f, w.speed);
}

int alloc_walker(TrafficState &t) {
	if (!t.free_peds.empty()) {
		int id = t.free_peds.back();
		t.free_peds.pop_back();
		return id;
	}
	t.peds.emplace_back();
	return int(t.peds.size()) - 1;
}

bool spawn_walker(CityState &s, const PeopleNow &now, bool from_door) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	int seg = pick_seg(s, now);
	if (seg < 0) return false;
	const Profile &pf = s.roads.profile_of(seg);
	const RoadSegment &g = s.roads.segs[seg];
	Pedestrian w;
	dress_walker(t, p, w, roll_age(t, p), s.catalog.blast.walker_hp);
	w.seg = seg;
	w.dir = int(t.rng.next() & 1);
	w.side = has_side(pf, 0) && (!has_side(pf, 1) || (t.rng.next() & 1)) ? 0 : 1;
	w.s = t.rng.range(arc_lo(g), arc_hi(g));
	on_sidewalk(w);
	Pose foot;
	sidewalk_pose(s, seg, w.side, w.s, w.u, foot);
	// where the player may see the footway, somebody comes out of a door of that stretch or nobody comes (the lab's edgePeople, sight.h)
	const bool seen = !unseen_birth(s, foot.x, foot.z);
	PedDoor door;
	const bool out = (from_door || seen) && pick_door(s, now, seg, w.side, true, arc_lo(g), arc_hi(g), door);
	if (out) {
		at_door(s, w, door);
	} else {
		if (seen) return false;
		w.s = clear_arc(s, seg, w.side, w.s, w.u); // born clear of the street furniture and of the others
		if (w.s > arc_hi(g)) return false;
		sidewalk_pose(s, seg, w.side, w.s, w.u, w.cur);
		if (!unseen_birth(s, w.cur.x, w.cur.z)) return false; // nobody appears on a footway the player may see (sight.h)
	}
	if (crowded(t, p, w.cur)) return false;
	std::vector<int> route; // planned last: a walker that is not born takes no room in the route pool
	const int ahead = w.dir == 0 ? g.b : g.a;
	if (!plan_trip(s, ahead, route, w.goal_side)) return false;
	route_set(t.ped_pool, w.route, t.ped_waste, route.data(), int(route.size()));
	const bool company = w.age == PedAge::CHILD || t.rng.f() < p.ped_company_share;
	w.company_due = company && out; // out of the same door, after the first
	int id = alloc_walker(t);
	t.peds[id] = std::move(w);
	walker_born(t, id);
	t.ped_count++;
	t.doors_out += out ? 1 : 0;
	if (company && !out) add_company(s, id);
	return true;
}

// A walker on the footway at `arc`, set there for a kerb stop's errand (kerb_stops.cpp): its body and trip as any walker's, held by
// the stop (PedMode::KERB); -1 with no room there.
int ped_on_footway(CityState &s, int seg, int side, float arc) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	Pedestrian w;
	dress_walker(t, p, w, roll_age_alone(t, p), s.catalog.blast.walker_hp);
	w.seg = seg;
	w.side = side;
	w.dir = int(t.rng.next() & 1);
	w.s = clampf(arc, arc_lo(g), arc_hi(g));
	on_sidewalk(w);
	sidewalk_pose(s, seg, side, w.s, w.u, w.cur);
	std::vector<int> route;
	if (crowded(t, p, w.cur) || !plan_trip(s, w.dir == 0 ? g.b : g.a, route, w.goal_side)) return -1;
	route_set(t.ped_pool, w.route, t.ped_waste, route.data(), int(route.size()));
	w.mode = PedMode::KERB;
	const int id = alloc_walker(t);
	t.peds[size_t(id)] = std::move(w);
	walker_born(t, id);
	t.ped_count++;
	return id;
}

} // namespace traffic
} // namespace city
