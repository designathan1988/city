// A node's paving in plan (node_outline.h).
#include "sim/roads/node_outline.h"

#include "sim/roads/centre_line.h"
#include "sim/roads/junction.h"
#include "sim/roads/road_network.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double PLATE_MIN_RETREAT = 0.05; // a node whose ends all stand nearer than this has no plate
constexpr double BULB_SLACK = 0.05; // a bulb's ring opens at least this far out along its road
constexpr double TAU_D = 2 * PI_D;
} // namespace

std::vector<NodeEnd> node_ends(const RoadNetwork &net, int node) {
	std::vector<NodeEnd> ends;
	for (int sid : net.nodes[size_t(node)].segs) {
		const RoadSegment &g = net.segs[size_t(sid)];
		NodeEnd en;
		en.seg = sid;
		en.at_a = g.a == node;
		en.at = en.at_a ? g.retreat_a : g.retreat_b;
		double fx, fz;
		centre_at(net, sid, en.at_a ? en.at : g.len - en.at, en.x, en.z, fx, fz);
		en.dir = en.at_a ? V2(float(fx), float(fz)) : V2(float(-fx), float(-fz));
		en.prof = &net.profile_of(sid);
		en.arm = arm_of(g.q, *en.prof, en.at_a ? 0 : 1);
		ends.push_back(en);
	}
	std::sort(ends.begin(), ends.end(), [](const NodeEnd &a, const NodeEnd &b) { return arm_angle(a.arm.dx, a.arm.dz) < arm_angle(b.arm.dx, b.arm.dz); });
	return ends;
}

bool node_plated(const std::vector<NodeEnd> &ends) {
	if (ends.size() < 2) return false;
	for (const NodeEnd &en : ends)
		if (en.at > PLATE_MIN_RETREAT) return true;
	return false;
}

CornerOutline node_corner(const RoadNetwork &net, double nx, double nz, const NodeEnd &a, const NodeEnd &b) {
	const auto side = [&](const NodeEnd &en, bool left) {
		CornerSide s;
		s.mouth = V2(float(en.x - nx), float(en.z - nz));
		s.dir = en.dir;
		s.section = side_section(*en.prof, left == en.at_a); // the arm's left is the profile's +u at its end a
		s.at = en.at;
		return s;
	};
	const Tuning &tn = net.cat->tune;
	return corner_outline(a.arm, b.arm, side(a, true), side(b, false), tn.road_corner_arc_step_rad, int(tn.road_corner_bevel_steps));
}

double bulb_setback(const NodeEnd &en) { return std::max(en.at, BULB_SLACK); }

std::vector<V2> bulb_ring(const RoadNetwork &net, const NodeEnd &en, double r) {
	const int steps = int(net.cat->tune.road_bulb_steps);
	const double a_in = std::acos(std::min(1.0, bulb_setback(en) / r));
	std::vector<V2> out;
	for (int i = 0; i <= steps; i++) {
		const double phi = a_in + double(i) / steps * (TAU_D - 2 * a_in);
		out.push_back(V2(float(r * std::cos(phi)), float(r * std::sin(phi))));
	}
	return out;
}

} // namespace city
