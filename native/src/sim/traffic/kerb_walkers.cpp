// The walker a kerb stop or a bus's call holds (PedMode::KERB): who may be called from the footway, the calling, the moves
// between a door and the footway, where somebody getting out first shows, and the letting go. The errands themselves are
// kerb_stops.cpp and bus_calls.cpp.
#include "sim/traffic/kerb_stops.h"

#include "sim/roads/parking_layout.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float MOVED_MIN_M = 0.05f; // shorter than this a move has no direction

// Who a car or a bus may call from the footway: somebody walking it alone (one of a party would leave the other on the street),
// no child, doing nothing else and not running from a bang (the reference game's rule).
bool may_be_hailed(const TrafficState &t, const Pedestrian &w) {
	return w.alive && w.mode == PedMode::SIDEWALK && w.with < 0 && w.mate < 0 && !w.company_due && w.age != PedAge::CHILD &&
			w.act == PedAct::NONE && t.time >= w.panic_until;
}

} // namespace

// The first walker on the kerb's footway between two arcs who may be hailed, or -1.
int kerb_hailable(const CityState &s, const DirSeg &d, float a0, float a1) {
	const TrafficState &t = s.traffic;
	const size_t key = size_t(d.seg) * 2 + size_t(kerb_side(d));
	if (key >= t.crowd.size()) return -1;
	const float lo = std::min(a0, a1), hi = std::max(a0, a1);
	for (const CrowdEntry &e : t.crowd[key])
		if (e.s >= lo && e.s <= hi && may_be_hailed(t, t.peds[size_t(e.id)])) return e.id;
	return -1;
}

// Whether the straight way from where the walker stands to `door` passes no street piece of its sidewalk (a bench, a bin, a post):
// the stop walks it there on that line.
bool way_to_door_clear(CityState &s, const Pedestrian &w, const V2 &door) {
	const float body = s.catalog.traffic.ped_body_m;
	const V2 from(w.cur.x, w.cur.z), way = door - from;
	const int steps = int(std::ceil(way.len() / body));
	for (int k = 0; k <= steps; k++) {
		const V2 at = from + way * (steps > 0 ? float(k) / float(steps) : 0.f);
		if (on_furniture(s, w.seg, w.side, at.x, at.y, body)) return false;
	}
	return true;
}

// Called to a door within `reach` whose way there is clear: from now on the stop moves it.
bool kerb_hail(CityState &s, int ped, const V2 &door, float reach) {
	TrafficState &t = s.traffic;
	if (ped < 0 || size_t(ped) >= t.peds.size()) return false;
	Pedestrian &w = t.peds[size_t(ped)];
	if (!w.alive || w.mode != PedMode::SIDEWALK || (V2(w.cur.x, w.cur.z) - door).len() > reach || !way_to_door_clear(s, w, door)) return false;
	w.mode = PedMode::KERB;
	return true;
}

// The walker moves to `at`, heading the way it moves: its way's heading, which its body turns toward as any walker's does
// (pedestrians.cpp turn_body), never a facing written at once.
void kerb_place(CityState &s, int ped, const V2 &at, const V2 &facing) {
	Pedestrian &w = s.traffic.peds[size_t(ped)];
	w.walked += (V2(w.cur.x, w.cur.z) - at).len();
	w.cur.x = at.x;
	w.cur.z = at.y;
	if (facing.len() <= MOVED_MIN_M) return;
	const V2 f = facing.norm();
	w.way_x = f.x;
	w.way_z = f.y;
}

// Somebody getting out of a vehicle first shows at its door: the walker made for it on the footway (which has room for it
// there, and its trip) stands at `at` from the tick it is made, its body facing the way it will go.
void kerb_show(CityState &s, int ped, const V2 &at, const V2 &facing) {
	kerb_place(s, ped, at, facing);
	Pedestrian &w = s.traffic.peds[size_t(ped)];
	w.cur.fx = w.way_x;
	w.cur.fz = w.way_z;
}

// Let go on foot it is a walker on the sidewalk where it stands (its arc and place across from its body), not where it was
// born or hailed.
void kerb_let_go(CityState &s, int ped, bool on_foot) {
	Pedestrian &w = s.traffic.peds[size_t(ped)];
	if (!w.alive || w.mode != PedMode::KERB) return;
	if (on_foot && w.route.len > 0) onto_sidewalk(s, w);
	else ped_gone(s, ped);
}

// A walker made on the footway beside `foot`, on the kerb's sidewalk of `ds`, held by the stop; -1 with no room there.
int kerb_walker_out(CityState &s, int ds, const V2 &foot) {
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	const RoadSegment &g = s.roads.segs[size_t(d.seg)];
	double at, dist;
	closest_t(g.q, foot.x, foot.y, at, dist);
	return ped_on_footway(s, d.seg, kerb_side(d), float(arc_of_t(g, at)));
}

} // namespace traffic
} // namespace city
