// Walking with somebody. Most groups on a street are pairs (Moussaid et al. 2010, PLoS ONE 5(4) e10047). Who comes out with
// somebody is a walker like any other: the same trip (its own copy of the route), the same pace, its own side of the way two bodies
// and the gap beside its partner's, so the crowd step (ped_crowd.cpp) walks it among the others and the street furniture, round the
// corners and over the zebras by itself, abreast where there is room and one behind the other where there is not. What makes the two
// a party is only this, all of it stated here: each bends its pace to stay level with the other while they are near (the reference
// game's rule) and, once parted by more than a few steps, the one ahead on their route stands and waits while the other catches up
// (this game's rule: a pair is never far apart); they take the same mid-block crossing and the same door; one goes over a crosswalk
// with the other; they go in at a door and leave the street together when near each other; and they talk at the kerb (ped_acts.cpp).
// A child always has a grown-up with it, an adult brings a child now and then, and an elder walks with an elder.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace city {
namespace traffic {

namespace {

PedAge partner_age(TrafficState &t, const TrafficParams &p, PedAge leader) {
	if (leader == PedAge::CHILD) return PedAge::ADULT;
	return leader == PedAge::ADULT && t.rng.f() < p.ped_company_child_share ? PedAge::CHILD : leader;
}

float company_gap(const TrafficParams &p) { return 2 * p.ped_body_m + p.ped_gap_m; } // the room walkers keep

// The one a walker came out with, or the one that came out with it; nullptr alone.
const Pedestrian *partner_of(const TrafficState &t, const Pedestrian &w) {
	const int id = w.with >= 0 ? w.with : w.mate;
	return id >= 0 ? &t.peds[size_t(id)] : nullptr;
}

// On its sidewalk at `arc`, `u` across: whether it stands there on the footway, in no piece of furniture, in nobody and on no swath.
bool stands_clear(CityState &s, Pedestrian &w, float arc, float u) {
	const RoadSegment &g = s.roads.segs[size_t(w.seg)];
	const float body = s.catalog.traffic.ped_body_m;
	if (arc < arc_lo(g) || arc > arc_hi(g) || std::fabs(u) > walk_half(s, w.seg, w.side)) return false;
	w.s = arc;
	w.u = u;
	stand_on_sidewalk(s, w);
	return !on_furniture(s, w.seg, w.side, w.cur.x, w.cur.z, body) && !walker_near(s.traffic, w.cur, 2 * body) &&
			!on_a_swath(s.traffic, w.cur.x, w.cur.z, body);
}

// Beside its leader on the sidewalk: abreast on its own side of the way, else a step behind in the leader's footsteps.
bool beside(CityState &s, Pedestrian &w, const Pedestrian &l) {
	on_sidewalk(w);
	const float back = l.dir == 0 ? -company_gap(s.catalog.traffic) : company_gap(s.catalog.traffic);
	return stands_clear(s, w, l.s, w.u) || stands_clear(s, w, l.s + back, l.u);
}

// Out of the door its leader came out of, by the leader's way to the sidewalk (which the leader keeps until its first corner), once
// nobody stands at the door: the leader has walked the room walkers keep, on that way or on along the sidewalk.
bool out_of_door(const CityState &s, Pedestrian &w, const Pedestrian &l) {
	for (int k = 0; k < l.wn; k++) w.ways[k] = l.ways[k];
	w.wn = l.wn;
	w.cur.x = w.out_x = l.out_x;
	w.cur.y = w.out_y = l.out_y;
	w.cur.z = w.out_z = l.out_z;
	leave_door(w);
	return !walker_near(s.traffic, w.cur, company_gap(s.catalog.traffic));
}

} // namespace

bool add_company(CityState &s, int leader) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Pedestrian w;
	std::vector<int> route;
	{
		const Pedestrian &l = t.peds[size_t(leader)];
		dress_walker(t, p, w, partner_age(t, p, l.age), s.catalog.blast.walker_hp);
		w.speed = l.speed; // its leader's pace
		w.track = l.track - company_gap(p); // its own side of the way: beside its leader's, toward the sidewalk's middle
		w.seg = l.seg;
		w.dir = l.dir;
		w.side = l.side;
		w.s = l.s;
		w.goal_side = l.goal_side;
		w.door = l.door;
		w.ri = l.ri;
		if (!(l.company_due ? out_of_door(s, w, l) : beside(s, w, l))) return false;
		route.resize(size_t(l.route.len));
		for (int k = 0; k < l.route.len; k++) route[size_t(k)] = route_at(t.ped_pool, l.route, k);
	}
	route_set(t.ped_pool, w.route, t.ped_waste, route.data(), int(route.size()));
	w.with = leader;
	const int id = alloc_walker(t);
	t.peds[size_t(leader)].mate = id;
	t.peds[size_t(leader)].company_due = false;
	t.peds[size_t(id)] = std::move(w);
	walker_born(t, id);
	t.ped_count++;
	return true;
}

// Whether `o` is further on than `w` along the route the two share: by the node walked toward, then the way round a node
// before the sidewalk after it, then how far along that way or sidewalk.
static bool further_on(const Pedestrian &o, const Pedestrian &w) {
	if (o.ri != w.ri) return o.ri > w.ri;
	const bool o_walks = o.mode == PedMode::SIDEWALK, w_walks = w.mode == PedMode::SIDEWALK;
	if (o_walks != w_walks) return o_walks;
	if (o_walks) return w.dir == 0 ? o.s > w.s : o.s < w.s;
	return o.wi != w.wi ? o.wi > w.wi : o.ls > w.ls;
}

float company_pace(const TrafficState &t, const TrafficParams &p, const Pedestrian &w) {
	const Pedestrian *o = partner_of(t, w);
	if (!o || t.time < w.panic_until || t.time < o->held_until) return 1.f; // alone, running, or the other stopped by something
	const V2 to(o->cur.x - w.cur.x, o->cur.z - w.cur.z);
	if (to.len() > p.ped_company_wait_m) { // parted: the one behind catches up; the one ahead stands and waits, or on a crosswalk goes slower
		if (further_on(*o, w)) return p.ped_company_catch;
		return w.cross_seg >= 0 ? p.ped_company_slow : 0.f;
	}
	const float ahead = to.x * w.way_x + to.y * w.way_z; // near: how far the other is ahead of it along its own way
	if (ahead > p.ped_company_level_m) return p.ped_company_catch;
	return ahead < -p.ped_company_level_m ? p.ped_company_slow : 1.f;
}

// Nobody stands on the straight way from `from` to `to` at their junction: no walker filed with it (but the two) within two bodies of that
// way between them, whom the one behind would have to pass to come up to the other.
static bool way_between_clear(const TrafficState &t, const TrafficParams &p, int node, int from, int to) {
	const Pedestrian &a = t.peds[size_t(from)], &b = t.peds[size_t(to)];
	const V2 start(a.cur.x, a.cur.z), d = V2(b.cur.x, b.cur.z) - start;
	const float len2 = d.dot(d), touch = 2 * p.ped_body_m;
	if (len2 <= 0) return true;
	for (const CrowdEntry &c : t.node_walkers[size_t(node)]) {
		if (c.id == from || c.id == to) continue;
		const V2 r = V2(c.x, c.z) - start;
		const float along = r.dot(d) / len2;
		if (along > 0 && along < 1 && (r - d * along).len() < touch) return false;
	}
	return true;
}

bool company_ready(const TrafficState &t, const TrafficParams &p, const Pedestrian &w) {
	const Pedestrian *o = partner_of(t, w);
	if (!o || further_on(*o, w)) return true; // alone, or the other is over already
	if (o->mode == PedMode::SIDEWALK || o->way_node != w.way_node || o->ri != w.ri) return false;
	// at the same kerb: its queue leg ended, waiting there too; or on its queue leg with nobody between the two, so it steps up as this one
	// steps off (one held behind others who wait, for a car or the next green, would be left behind: no body passes through another)
	if (o->wi >= w.wi) return true;
	const int self = int(&w - t.peds.data()), other = int(o - t.peds.data());
	return o->wi + 1 == w.wi && way_between_clear(t, p, w.way_node, other, self);
}

int company_steps_off(const TrafficState &t, const TrafficParams &p, const Pedestrian &w) {
	const Pedestrian *o = partner_of(t, w);
	if (!o || w.cross_seg < 0 || o->cross_seg >= 0 || o->way_node != w.way_node || o->ri != w.ri) return -1;
	const int self = int(&w - t.peds.data()), other = int(o - t.peds.data());
	if (o->mode == PedMode::WAIT && o->wi == w.wi && o->ways[o->wi].cross == w.cross_seg) return other; // waiting at the same kerb
	const bool queued = o->mode == PedMode::WAY && o->wi + 1 == w.wi && toward_kerb(*o) && o->ways[o->wi + 1].cross == w.cross_seg;
	return queued && way_between_clear(t, p, w.way_node, other, self) ? other : -1; // right behind it in the queue
}

bool crossing_with(const TrafficState &t, const Pedestrian &w, int arm) {
	const Pedestrian *o = partner_of(t, w);
	return o && o->way_node == w.way_node && o->cross_seg == arm; // the pair's leave, however long the crosswalk
}

// Who lies down or bleeds is left where they are (pedestrians.cpp casualty): the other leaves the street or goes in at a door alone.
int company_near(const TrafficState &t, const TrafficParams &p, const Pedestrian &w) {
	const Pedestrian *o = partner_of(t, w);
	if (!o || casualty(t, *o) || V2(o->cur.x - w.cur.x, o->cur.z - w.cur.z).len() > p.ped_company_reach_m) return -1;
	return int(o - t.peds.data());
}

uint32_t party_look(const TrafficState &t, const Pedestrian &w) {
	return w.with >= 0 ? t.peds[size_t(w.with)].look : w.look;
}

bool company_door(const TrafficState &t, Pedestrian &w) {
	const Pedestrian *o = partner_of(t, w);
	const bool aims = o && o->door.building >= 0 && o->seg == w.seg && o->side == w.side; // the other has a door on this sidewalk
	if (!aims) return false;
	if (w.dir == 0 ? o->door.s < w.s : o->door.s > w.s) return false; // behind it already
	w.door = o->door;
	return true;
}

} // namespace traffic
} // namespace city
