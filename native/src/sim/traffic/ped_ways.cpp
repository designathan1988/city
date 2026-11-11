// A walker's ways off the sidewalk (pedestrians.cpp walks them): at a sidewalk's end the way round the node onto the next street's sidewalk
// (ped_routes.cpp plan_corner), each straight line of it begun and ended, a crosswalk waited for (the walk phase and a gap, ped_crossing.cpp)
// and gone over, the one it walks with taken over on the same leave (ped_party.cpp).
#include "core/random.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {

constexpr uint32_t CROSS_MASK = 0xFFFF;

int toward(const CityState &s, const Pedestrian &w) { return w.dir == 0 ? s.roads.segs[w.seg].b : s.roads.segs[w.seg].a; }

// The one it walks with steps off with it, on the same leave (ped_party.cpp company_steps_off), the same tick's answers of the signal and
// the cars: from the kerb it waits at, or from its queue leg into the other's footsteps.
void company_over(CityState &s, int id) {
	TrafficState &t = s.traffic;
	const Pedestrian &w = t.peds[size_t(id)];
	const int other = company_steps_off(t, s.catalog.traffic, w);
	if (other < 0) return;
	Pedestrian &o = t.peds[size_t(other)];
	if (o.mode == PedMode::WAIT) { // beside it at the kerb: over from where it stands
		begin_line(s, other);
		return;
	}
	// from its queue leg: into the other's footsteps (the way between the two is clear) and over behind it, the two legs one crossing
	o.ways[o.wi] = Waypoint{ w.lx, w.ly, w.lz, w.cross_seg, w.ways[w.wi - 1].side };
	o.ways[o.wi + 1] = w.ways[w.wi];
	o.line_off = w.line_off;
	o.mode = PedMode::WAY;
	aim_line(o);
	o.cross_seg = w.cross_seg;
	t.node_crossers[size_t(o.way_node)].push_back(other);
}

} // namespace

void off_crosswalk(TrafficState &t, int id) {
	Pedestrian &w = t.peds[id];
	if (w.cross_seg < 0) return;
	auto &l = t.node_crossers[w.way_node];
	l.erase(std::remove(l.begin(), l.end(), id), l.end());
	w.cross_seg = -1;
}

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

void begin_line(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Pedestrian &w = t.peds[id];
	const Waypoint &to = w.ways[w.wi];
	const bool frightened = t.time < w.panic_until; // no signal holds somebody running: only the cars on the crossing do
	// the second leg of a crossing taken in the footsteps of the one it walks with (company_over): it is on the crosswalk already
	const bool asks = to.cross >= 0 && !(w.wi > 0 && w.ways[w.wi - 1].cross == to.cross);
	// a party goes over on one leave: one goes with the other already on the crosswalk (whatever the signal shows by then, never in front of
	// a car), and starts only when the other is at the kerb too
	const bool with = asks && crossing_with(t, w, to.cross);
	const bool held = asks && ((!frightened && !with && !walk_ok(s, w.way_node, to.cross)) || cars_on_crossing(s, w.way_node, to.cross));
	const bool waits = asks && !frightened && !company_ready(t, s.catalog.traffic, w);
	if (waits || held) {
		w.mode = PedMode::WAIT;
		return;
	}
	if (to.cross >= 0) cross_from_here(w); // over from where the queue left it at the kerb
	else if (toward_queue(w)) onto_approach(s, w); // to the back of a kerb's queue, at its place along the kerb
	else if (toward_kerb(w)) onto_queue(s, w); // the queue before the kerb, along the way across from where it stands
	else if (w.wi + 1 == w.wn) aim_onto_sidewalk(s, w); // onto the next sidewalk, at its place across (ped_routes.cpp)
	w.mode = PedMode::WAY;
	aim_line(w);
	if (to.cross >= 0) {
		w.cross_seg = to.cross;
		t.node_crossers[w.way_node].push_back(id);
		company_over(s, id);
	}
}

void at_node(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Pedestrian &w = t.peds[id];
	const int n = toward(s, w);
	if (w.ri + 1 >= w.route.len) { despawn_party(s, id, true); return; }
	if (t.time < w.panic_until) flee_on(s, w, n);
	int next = seg_between(s, n, route_at(t.ped_pool, w.route, w.ri + 1)), side_out = 0;
	const bool last = w.ri + 2 >= w.route.len;
	int want = last ? w.goal_side : -1;
	// a party goes over together: the choice is drawn by its first walker's hash
	if (next >= 0 && want < 0 && s.roads.mid_block(n) && float(hash_combine(party_look(t, w), uint32_t(n)) & CROSS_MASK) < s.catalog.traffic.ped_crossing_share * float(CROSS_MASK)) {
		int stays = 0; // going over to the other sidewalk: what a crossing between junctions is there for
		if (plan_corner(s, n, w.seg, w.side, next, -1, w.ways, stays) >= 0) want = 1 - stays;
	}
	int k = next < 0 ? -1 : plan_corner(s, n, w.seg, w.side, next, want, w.ways, side_out);
	if (k < 0) { despawn(t, id, false); return; }
	const RoadSegment &ng = s.roads.segs[next];
	w.company_due = false; // the line it came out of a door by is gone with the lines of this node
	w.ri++;
	w.seg = next;
	w.dir = ng.a == n ? 0 : 1;
	w.side = side_out;
	w.s = w.dir == 0 ? arc_lo(ng) : arc_hi(ng);
	on_sidewalk(w);
	w.ways[k++] = Waypoint(); // the next sidewalk's start: aimed at when its line begins, from where the walker stands then
	w.wn = k;
	w.wi = 0;
	w.way_node = n;
	if (w.ri + 1 >= w.route.len && !company_door(t, w)) aim_at_door(s, w);
	begin_line(s, id);
}

} // namespace traffic
} // namespace city
