// The walker a kerb stop or a bus's call holds (PedMode::KERB) or calls: who may be called from the footway, the call (its own walk
// along its sidewalk to the door's arc, then its way in across to its foot and on to the door, clear of the street's pieces), the moves
// between a door and the footway, where somebody getting out first shows, and the letting go. The errands themselves are kerb_stops.cpp
// and bus_calls.cpp.
#include "sim/traffic/kerb_stops.h"

#include "sim/roads/parking_layout.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {

constexpr float MOVED_MIN_M = 0.05f; // shorter than this a move has no direction

// Who a car or a bus may call from the footway: somebody walking it alone (one of a party would leave the other on the street),
// no child, doing nothing else, not running from a bang (the reference game's rule) and not called by another already.
bool may_be_hailed(const TrafficState &t, const Pedestrian &w) {
	return w.alive && w.mode == PedMode::SIDEWALK && w.with < 0 && w.mate < 0 && !w.company_due && w.age != PedAge::CHILD &&
			w.act == PedAct::NONE && t.time >= w.panic_until && w.call_s < 0;
}

} // namespace

int kerb_hailable(CityState &s, const DirSeg &d, float a0, float a1, int *on_foot) {
	const TrafficState &t = s.traffic;
	const size_t key = size_t(d.seg) * 2 + size_t(kerb_side(d));
	if (key >= t.crowd.size()) return -1;
	const float lo = std::min(a0, a1), hi = std::max(a0, a1);
	int found = -1, there = 0;
	for (const CrowdEntry &e : t.crowd[key]) {
		// somebody lying (a dead body, no walker at all: ped_dead.cpp, or the living down) is neither on foot nor called
		if (e.s < lo || e.s > hi || e.lie > 0.f) continue;
		there++;
		if (found < 0 && may_be_hailed(t, t.peds[size_t(e.id)])) found = e.id;
	}
	if (on_foot) *on_foot = there;
	return found;
}

bool kerb_call(CityState &s, int ped, const V2 &door, float at, float reach) {
	TrafficState &t = s.traffic;
	if (ped < 0 || size_t(ped) >= t.peds.size()) return false;
	Pedestrian &w = t.peds[size_t(ped)];
	if (!may_be_hailed(t, w) || (V2(w.cur.x, w.cur.z) - door).len() > reach) return false;
	const RoadSegment &g = s.roads.segs[size_t(w.seg)];
	w.call_s = clampf(at, arc_lo(g), arc_hi(g)); // on its sidewalk: it never walks off its end for it
	w.call_turned = w.dir == 0 ? w.call_s < w.s : w.call_s > w.s;
	if (w.call_turned) turn_round(w);
	return true;
}

// It stands at the arc it was called to, where its place across puts it (its body still turning as it turns), held by the stop.
void kerb_came(const CityState &s, Pedestrian &w) {
	const V2 faced(w.cur.fx, w.cur.fz);
	w.s = w.call_s;
	stand_on_sidewalk(s, w);
	w.cur.fx = faced.x;
	w.cur.fz = faced.y;
	enter_mode(w, PedMode::KERB);
}

void kerb_forget_call(Pedestrian &w, bool back) {
	if (w.call_s < 0) return;
	if (back && w.call_turned) turn_round(w);
	w.call_s = -1;
	w.call_turned = false;
}

KerbHold kerb_hold(const TrafficState &t, int ped) {
	const Pedestrian &w = t.peds[size_t(ped)];
	if (!w.alive) return KerbHold::LOST;
	if (w.mode == PedMode::KERB) return KerbHold::HELD;
	return w.mode == PedMode::SIDEWALK && w.call_s >= 0 ? KerbHold::COMING : KerbHold::LOST;
}

bool kerb_way_in(CityState &s, int ped, const V2 &door, V2 &foot) {
	const Pedestrian &w = s.traffic.peds[size_t(ped)];
	const V2 here(w.cur.x, w.cur.z);
	float arc = w.s, u = w.u;
	if (!clear_foot(s, w.seg, w.side, arc, u, door, true, &here)) return false;
	Pose on;
	sidewalk_pose(s, w.seg, w.side, arc, u, on);
	foot = V2(on.x, on.z);
	return true;
}

float kerb_way_len(const V2 &from, const V2 &foot, const V2 &door) { return (foot - from).len() + (door - foot).len(); }

V2 kerb_way_at(const V2 &from, const V2 &foot, const V2 &door, float metres, V2 &heading) {
	const float across = (foot - from).len(), on = (door - foot).len();
	if (metres < across) {
		heading = foot - from;
		return from + heading * (metres / across);
	}
	heading = door - foot;
	return on > 0 ? foot + heading * std::min(1.f, (metres - across) / on) : foot;
}

bool kerb_foot(CityState &s, int ds, const KerbDoors &at, V2 &foot) {
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	float arc = at.arc, u = 0;
	if (!clear_foot(s, d.seg, kerb_side(d), arc, u, at.door, true)) return false;
	Pose on;
	sidewalk_pose(s, d.seg, kerb_side(d), arc, u, on);
	foot = V2(on.x, on.z);
	return true;
}

// The walker moves to `at`, heading the way it moves: its way's heading, which its body turns toward as any walker's does
// (pedestrians.cpp turn_body), never a facing written at once. Its arc and place across follow it: the crowd files it there, a body
// the others keep off (ped_crowd.cpp entry_of).
void kerb_place(CityState &s, int ped, const V2 &at, const V2 &facing) {
	Pedestrian &w = s.traffic.peds[size_t(ped)];
	w.walked += (V2(w.cur.x, w.cur.z) - at).len();
	w.cur.x = at.x;
	w.cur.z = at.y;
	where_on_sidewalk(s, w);
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
// born or hailed, walking its own way again; one called and still on its way to the door's arc walks on its own way from there.
void kerb_let_go(CityState &s, int ped, bool on_foot) {
	Pedestrian &w = s.traffic.peds[size_t(ped)];
	if (!w.alive) return;
	kerb_forget_call(w, true);
	if (w.mode != PedMode::KERB) return;
	if (on_foot && w.route.len > 0) onto_sidewalk(s, w);
	else ped_gone(s, ped);
}

// A walker made on the footway beside `foot`, on the kerb's sidewalk of `ds`, held by the stop; -1 with no room there.
int kerb_walker_out(CityState &s, int ds, const V2 &foot) {
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	const RoadSegment &g = s.roads.segs[size_t(d.seg)];
	double at, dist;
	closest_t(g.q, foot.x, foot.y, at, dist);
	return ped_on_footway(s, d.seg, kerb_side(d), float(arc_at(g.table, at)));
}

} // namespace traffic
} // namespace city
