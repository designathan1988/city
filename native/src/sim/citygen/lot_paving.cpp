// The paving as polygons (lot_paving.h).
#include "sim/citygen/lot_paving.h"

#include "sim/roads/junction.h"
#include "sim/roads/road_network.h"
#include "sim/street/footway.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>

namespace city {

using Clipper2Lib::FillRule;
using Clipper2Lib::Path64;
using Clipper2Lib::Paths64;
using Clipper2Lib::Point64;

namespace {
constexpr double PARALLEL = 1e-6;

Point64 unit_point(double x, double z) { return Point64(std::llround(x * PAVING_UNITS_PER_M), std::llround(z * PAVING_UNITS_PER_M)); }

Path64 path_of(const std::vector<XZ> &ring) {
	Path64 out;
	out.reserve(ring.size());
	for (const XZ &p : ring) out.push_back(unit_point(p.x, p.z));
	return out;
}

} // namespace

std::vector<XZ> paving_ring(const Path64 &path) {
	std::vector<XZ> out;
	out.reserve(path.size());
	for (const Point64 &p : path) out.push_back({ double(p.x) / PAVING_UNITS_PER_M, double(p.y) / PAVING_UNITS_PER_M });
	return out;
}

namespace {
// One winding for every piece: under the non-zero rule two pieces wound against each other cancel where they overlap.
void add_positive(Paths64 &out, Path64 path) {
	if (path.size() < 3) return;
	if (Clipper2Lib::Area(path) < 0) std::reverse(path.begin(), path.end());
	out.push_back(std::move(path));
}

void add_ribbon(const GenShapes &S, const RoadNetwork &net, int seg, Paths64 &out) {
	const PavingShape &K = S.paving;
	const RoadSegment &g = net.segs[size_t(seg)];
	const Profile &p = net.profile_of(seg);
	const double from = g.retreat_a, to = g.len - g.retreat_b;
	if (p.rail || p.edges.empty() || to <= from) return;
	const int steps = std::max(1, int(std::ceil((to - from) / K.along_pace_m)));
	Path64 left, right;
	for (int k = 0; k <= steps; k++) {
		const double v = from + (to - from) * k / steps;
		double x, z, y;
		footway_point(net, seg, v, p.edges.back().u1, x, z, y);
		left.push_back(unit_point(x, z));
		footway_point(net, seg, v, p.edges.front().u0, x, z, y);
		right.push_back(unit_point(x, z));
	}
	left.insert(left.end(), right.rbegin(), right.rend());
	add_positive(out, std::move(left));
}

struct NodeEnd {
	XZ left, right; // the outer edges of the footways at the mouth, seen going out along the arm
	double dx = 0, dz = 0;
	Arm arm;
};

NodeEnd end_at(const RoadNetwork &net, int node, int seg) {
	const RoadSegment &g = net.segs[size_t(seg)];
	const Profile &p = net.profile_of(seg);
	const bool at_a = g.a == node;
	const double v = at_a ? g.retreat_a : g.len - g.retreat_b;
	NodeEnd e;
	e.arm = arm_of(g.q, p, at_a ? 0 : 1);
	double y, fx, fz, lx, lz;
	footway_point(net, seg, v, at_a ? p.edges.back().u1 : p.edges.front().u0, e.left.x, e.left.z, y);
	footway_point(net, seg, v, at_a ? p.edges.front().u0 : p.edges.back().u1, e.right.x, e.right.z, y);
	segment_frame(net, seg, v, fx, fz, lx, lz);
	e.dx = at_a ? fx : -fx;
	e.dz = at_a ? fz : -fz;
	return e;
}

void add_junction(const GenShapes &S, const RoadNetwork &net, int node, Paths64 &out) {
	const PavingShape &K = S.paving;
	const RoadNode &nd = net.nodes[size_t(node)];
	std::vector<NodeEnd> ends;
	for (int seg : nd.segs)
		if (!net.profile_of(seg).rail && !net.profile_of(seg).edges.empty()) ends.push_back(end_at(net, node, seg));
	if (ends.empty()) return;
	if (ends.size() == 1) {
		if (!nd.bulb) return;
		Path64 circle;
		const double r = BULB_R + BULB_WALK;
		for (int i = 0; i < K.round_steps; i++) {
			const double a = 2 * PI_D * i / K.round_steps;
			circle.push_back(unit_point(nd.x + r * std::cos(a), nd.z + r * std::sin(a)));
		}
		add_positive(out, std::move(circle));
		return;
	}
	std::sort(ends.begin(), ends.end(), [](const NodeEnd &a, const NodeEnd &b) { return arm_angle(a.arm.dx, a.arm.dz) < arm_angle(b.arm.dx, b.arm.dz); });
	Path64 ring;
	for (size_t i = 0; i < ends.size(); i++) {
		const NodeEnd &a = ends[i], &b = ends[(i + 1) % ends.size()];
		ring.push_back(unit_point(a.right.x, a.right.z));
		ring.push_back(unit_point(a.left.x, a.left.z));
		// The two footways run on until they meet: the lot on that corner ends in a point, however round the kerb.
		const double den = a.dx * b.dz - a.dz * b.dx;
		if (corner_fillet(a.arm, b.arm).mode != CornerMode::FILLET || std::fabs(den) < PARALLEL) continue;
		const double wx = b.right.x - a.left.x, wz = b.right.z - a.left.z, along = (wx * b.dz - wz * b.dx) / den;
		ring.push_back(unit_point(a.left.x + a.dx * along, a.left.z + a.dz * along));
	}
	add_positive(out, std::move(ring));
}

void box_of(const std::vector<XZ> &ring, double &x0, double &z0, double &x1, double &z1) {
	x0 = z0 = std::numeric_limits<double>::max();
	x1 = z1 = std::numeric_limits<double>::lowest();
	for (const XZ &p : ring) {
		x0 = std::min(x0, p.x);
		z0 = std::min(z0, p.z);
		x1 = std::max(x1, p.x);
		z1 = std::max(z1, p.z);
	}
}

} // namespace

Paths64 paving_near(const GenShapes &S, const RoadNetwork &net, double x0, double z0, double x1, double z1) {
	const PavingShape &K = S.paving;
	Paths64 pieces;
	std::set<int> nodes;
	net.for_segments_near(x0 - K.plate_reach_m, z0 - K.plate_reach_m, x1 + K.plate_reach_m, z1 + K.plate_reach_m, [&](int seg) {
		add_ribbon(S, net, seg, pieces);
		nodes.insert(net.segs[size_t(seg)].a);
		nodes.insert(net.segs[size_t(seg)].b);
	});
	for (int n : nodes) add_junction(S, net, n, pieces);
	return pieces.empty() ? pieces : Clipper2Lib::Union(pieces, FillRule::NonZero);
}

Paths64 paving_round(const GenShapes &S, const RoadNetwork &net, const std::vector<XZ> &ring) {
	double x0, z0, x1, z1;
	box_of(ring, x0, z0, x1, z1);
	return paving_near(S, net, x0 - PAVING_MARGIN_M, z0 - PAVING_MARGIN_M, x1 + PAVING_MARGIN_M, z1 + PAVING_MARGIN_M);
}

// What of a ring stands off the paving: the outlines, holes left out as a lot has none.
Paths64 land_of(const GenShapes &S, const std::vector<XZ> &ring, const Paths64 &paving) {
	const PavingShape &K = S.paving;
	Paths64 subject;
	add_positive(subject, path_of(ring));
	Paths64 left = Clipper2Lib::Difference(subject, paving, FillRule::NonZero), out;
	for (Path64 &p : left)
		if (Clipper2Lib::Area(p) > 0) out.push_back(std::move(p));
	return Clipper2Lib::SimplifyPaths(out, K.outline_slack_units);
}

} // namespace city
