// The posts of the street furniture, see street_furniture.h: the lamp rows (distribution poles evenly along the block,
// on one face or both by the street's width), the ground each lit lamp lights (lamp_pool) and the signal post of each signalled
// approach (across the junction, or on its own corner). Every distance comes from data/catalog/street_furniture.json and its sources.
#include "sim/roads/street_furniture_internal.h"

#include "sim/roads/junction.h"
#include "sim/roads/street_lines.h"

#include <algorithm>
#include <cmath>

namespace city {

using namespace furniture;

namespace {

// The arm out of node `n` that traffic arriving on `approach` goes straight on into (with lanes leaving `n`), or -1.
int straight_on(const RoadNetwork &net, int n, int approach) {
	const RoadSegment &ga = net.segs[size_t(approach)];
	const Arm in = arm_of(ga.q, net.profile_of(approach), ga.a == n ? 0 : 1);
	int best = -1;
	double best_dot = net.cat->traffic.straight_dot;
	for (int sid : net.nodes[size_t(n)].segs) {
		const RoadSegment &g = net.segs[size_t(sid)];
		const Profile &p = net.profile_of(sid);
		if (sid == approach || !g.alive || g.a == g.b || p.edges.empty() || (g.a == n ? p.fwd : p.back).empty()) continue;
		const Arm out = arm_of(g.q, p, g.a == n ? 0 : 1);
		const double dot = -(in.dx * out.dx + in.dz * out.dz); // the approach's arm points out of the node
		if (dot > best_dot) {
			best_dot = dot;
			best = sid;
		}
	}
	return best;
}

} // namespace

LampRow lamp_row(const RoadNetwork &net, int sid) {
	LampRow r;
	const RoadSegment &g = net.segs[size_t(sid)];
	const Profile &p = net.profile_of(sid);
	if (p.edges.empty() || p.rail || p.ring || !g.alive) return r;
	const bool right = walk_on(p, 0), left = walk_on(p, 1);
	if (!right && !left) return r;
	const StreetFurnitureDef &d = defs(net);
	r.any = true;
	// the wires on one face, a lighting column on the other when there is one (the Roadcraft 4.0 street); the line on the east or south face
	r.twin = right && left;
	r.side = street_face(net, sid); // one face through a street's joints, the lines of crossing streets meeting on corners
	const float pole = kerbside_reach(net, FurnitureKind::LAMP);
	r.pole_u = kerb_in(p, r.side, pole);
	if (r.twin) r.pole_u2 = kerb_in(p, 1 - r.side, pole); // the lighting columns: on the face across from the line, never beside its poles
	// The line's ends (street_lines.h): on a junction's corner pole or through a joint's one pole, else a pole of its own just past
	// the crosswalk and past the place a signal post may take, a hand's gap clear of it (items.gap_m); evenly at most span_m apart
	// between its ends.
	const double past = line_end_setback(net);
	const LineEndAt ea = line_end_at(net, sid, 0), eb = line_end_at(net, sid, 1);
	const bool carried_a = ea.corner >= 0 || ea.joint, carried_b = eb.corner >= 0 || eb.joint;
	const double v0 = g.retreat_a, v1 = g.len - g.retreat_b, from = carried_a ? ea.v : v0 + past, to = carried_b ? eb.v : v1 - past;
	r.own_first = !carried_a || ea.own;
	r.own_last = !carried_b || eb.own;
	r.first_v = v0 + past;
	r.last_v = v1 - past;
	if (v1 <= v0) return r;
	r.spacing = d.lamps.span_m;
	// How far apart the line's two end poles stand, straight (through a bend at a joint the arc is longer): two poles never stand
	// nearer than poles.min_gap_m.
	double ax, az, bx, bz;
	const double apart = line_end_point(net, sid, 0, ax, az) && line_end_point(net, sid, 1, bx, bz) ? std::hypot(bx - ax, bz - az) : to - from;
	if (apart < d.poles.min_gap_m && (carried_a || carried_b)) { // too short for a pole between: strung from the pole it is carried on
		r.origin = from;
		r.spacing = std::max(to - from, 0.0);
		r.stations = 2;
		r.own_first = r.own_last = false;
		return r;
	}
	if (apart < d.poles.min_gap_m) { // too short for a line (two of its poles never stand closer than that): its light on a column at its middle
		r.origin = (v0 + v1) / 2;
		r.stations = 1;
		r.lone = true;
		return r;
	}
	const int spans = std::max(1, int(std::ceil((to - from) / d.lamps.span_m)));
	r.origin = from;
	r.spacing = (to - from) / spans;
	r.stations = spans + 1;
	return r;
}

LampPool lamp_pool(const RoadNetwork &net, int sid, bool column) {
	const LampDef &d = defs(net).lamps;
	LampPool pool{ d.pool_least_m, d.pool_least_m, d.reach_m }; // a round pool under the head
	if (!column || sid < 0 || net.profile_of(sid).edges.empty()) return pool;
	// The light falls over the carriageway and a little past it, back over the footway behind the column: wider with the road.
	const Profile &p = net.profile_of(sid);
	const StripEdge &re = p.edges.front(), &le = p.edges.back();
	const float lo = re.strip.kind == StripKind::SIDEWALK ? re.u1 : re.u0, hi = le.strip.kind == StripKind::SIDEWALK ? le.u0 : le.u1;
	pool.across = std::max(d.pool_least_m, (hi - lo) * d.pool_share + d.pool_margin_m);
	pool.along = d.pool_along_m;
	pool.out = std::max(d.reach_m, pool.across - d.pool_back_m);
	return pool;
}

bool faces_road(const FurnitureSpot &s) { return s.side < 0 || s.face_left * toward_road(s.side) > 0.f; }

bool signal_post(const RoadNetwork &net, int n, int approach, FurnitureSpot &out) {
	const RoadSegment &ga = net.segs[size_t(approach)];
	if (!ga.alive || ga.a == ga.b) return false;
	const Profile &pa = net.profile_of(approach);
	if ((ga.a == n ? pa.back : pa.fwd).empty() || pa.edges.empty()) return false; // arriving at a: travelling b to a
	const StreetFurnitureDef &d = defs(net);
	const float in = d.signals.kerb_m, half = d.footprint[int(FurnitureKind::SIGNAL)].z_m * 0.5f;
	const int far = straight_on(net, n, approach);
	if (far >= 0) { // across the junction, at the right of the traffic going on, just clear of that arm's crosswalk
		const RoadSegment &g = net.segs[size_t(far)];
		const Profile &p = net.profile_of(far);
		const bool at_a = g.a == n; // leaving through a: travelling a to b, its right is the right sidewalk
		const double v = at_a ? g.retreat_a + d.crossing.zebra_to_m + half : g.len - g.retreat_b - d.crossing.zebra_to_m - half;
		out = spot(net, FurnitureKind::SIGNAL, far, p, std::clamp(v, g.retreat_a, g.len - g.retreat_b), kerb_in(p, at_a ? 0 : 1, in), 0.f,
				at_a ? -1.f : 1.f);
	} else { // no straight continuation: on its own corner, between the mouth and the crosswalk
		const bool at_a = ga.a == n; // arriving at a: its right is the left sidewalk
		const double gap = d.crossing.zebra_from_m * 0.5;
		out = spot(net, FurnitureKind::SIGNAL, approach, pa, at_a ? ga.retreat_a + gap : ga.len - ga.retreat_b - gap,
				kerb_in(pa, at_a ? 1 : 0, in), 0.f, at_a ? 1.f : -1.f);
	}
	out.node = n;
	out.approach = approach;
	return true;
}

} // namespace city
