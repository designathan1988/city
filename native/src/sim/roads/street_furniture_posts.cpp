// The posts of the street furniture, see street_furniture.h: the lamp rows (distribution poles evenly along the block,
// on one face or both by the street's width) and the signal post of each signalled approach (across the junction, or on
// its own corner). Every distance comes from data/catalog/street_furniture.json and its sources.
#include "sim/roads/street_furniture_internal.h"

#include "sim/roads/junction.h"

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

// The arm going on from segment `from` through node `n` when only the two meet there, else -1.
int going_on(const RoadNetwork &net, int n, int from) {
	const std::vector<int> &arms = net.nodes[size_t(n)].segs;
	if (int(arms.size()) != STREET_JOINT_ARMS) return -1;
	const int next = arms[0] == from ? arms[1] : arms[0];
	const RoadSegment &g = net.segs[size_t(next)];
	return next != from && g.alive && g.a != g.b ? next : -1;
}

} // namespace

int row_face(const RoadNetwork &net, int sid) {
	int lowest = sid;
	bool flipped = false; // the lowest-numbered segment runs the other way from `sid`
	for (const bool toward_b : { true, false }) {
		int at = sid, n = toward_b ? net.segs[size_t(sid)].b : net.segs[size_t(sid)].a;
		bool flip = false;
		for (int next = going_on(net, n, at); next >= 0 && next != sid; next = going_on(net, n, at)) {
			const RoadSegment &here = net.segs[size_t(at)], &on = net.segs[size_t(next)];
			flip = flip != ((here.b == n) != (on.a == n)); // the same way: it leaves the node the other arrives at
			if (next < lowest) {
				lowest = next;
				flipped = flip;
			}
			n = on.a == n ? on.b : on.a;
			at = next;
		}
	}
	const RoadSegment &g = net.segs[size_t(lowest)];
	const RoadNode &a = net.nodes[size_t(g.a)], &b = net.nodes[size_t(g.b)];
	const int face = (b.x - a.x) - (b.z - a.z) >= 0 ? 0 : 1; // the right face looks to (-dz, dx): east is +x, south +z
	return flipped ? 1 - face : face;
}

LampRow lamp_row(const RoadNetwork &net, int sid) {
	LampRow r;
	const RoadSegment &g = net.segs[size_t(sid)];
	const Profile &p = net.profile_of(sid);
	if (p.edges.empty() || p.rail || p.ring || !g.alive) return r;
	const bool right = walk_on(p, 0), left = walk_on(p, 1);
	if (!right && !left) return r;
	const StreetFurnitureDef &d = defs(net);
	r.any = true;
	// The power line alone, its poles carrying the lamps; or lighting columns, on both faces when both have a sidewalk.
	r.line = (g.flags & SEG_LIT_BY_COLUMNS) == 0;
	r.twin = !r.line && right && left;
	r.side = right && left ? row_face(net, sid) : (right ? 0 : 1);
	const float pole = d.sidewalk.kerb_clear_m + d.footprint[int(FurnitureKind::LAMP)].z_m * 0.5f;
	r.pole_u = kerb_in(p, r.side, pole);
	r.head_u = r.pole_u + toward_road(r.side) * d.lamps.reach_m;
	// The light falls over the carriageway and a little past it: wider with the road, centred on it.
	const StripEdge &re = p.edges.front(), &le = p.edges.back();
	const float lo = re.strip.kind == StripKind::SIDEWALK ? re.u1 : re.u0, hi = le.strip.kind == StripKind::SIDEWALK ? le.u0 : le.u1;
	r.across = std::max(d.lamps.pool_least_m, (hi - lo) * d.lamps.pool_share + d.lamps.pool_margin_m);
	r.centre = r.pole_u + toward_road(r.side) * std::max(d.lamps.reach_m, r.across - d.lamps.pool_back_m);
	if (r.twin) {
		r.pole_u2 = kerb_in(p, 1 - r.side, pole);
		r.head_u2 = r.pole_u2 + toward_road(1 - r.side) * d.lamps.reach_m;
		r.centre = (r.head_u + r.head_u2) * 0.5f;
	}
	// a pole just past each crosswalk, after the place a signal post may take, and evenly at most span_m apart between
	const double past = d.crossing.zebra_to_m + d.footprint[int(FurnitureKind::SIGNAL)].z_m + d.footprint[int(FurnitureKind::LAMP)].x_m * 0.5;
	const double v0 = g.retreat_a, v1 = g.len - g.retreat_b, from = v0 + past, to = v1 - past;
	if (v1 <= v0) return r;
	r.spacing = r.span = d.lamps.span_m;
	if (to <= from) { // a short block: one pole in its middle
		r.origin = (v0 + v1) / 2;
		r.stations = 1;
		return r;
	}
	const int spans = std::max(1, int(std::ceil((to - from) / d.lamps.span_m)));
	r.origin = from;
	r.spacing = (to - from) / spans;
	r.stations = spans + 1;
	return r;
}

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
