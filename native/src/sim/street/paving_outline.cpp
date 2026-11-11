// The paving as it is drawn, as polygons (paving_outline.h).
#include "sim/street/paving_outline.h"

#include "sim/roads/junction.h"
#include "sim/roads/node_outline.h"
#include "sim/street/footway.h"

#include <algorithm>
#include <cmath>
#include <set>

namespace city {

using Clipper2Lib::FillRule;
using Clipper2Lib::Path64;
using Clipper2Lib::Paths64;
using Clipper2Lib::Point64;

namespace {
constexpr double ALONG_PACE_M = 4; // a curve's outline in chords of this length
constexpr double PLATE_REACH_M = 40; // how far a junction's plate may stand from the roads that meet it

// A segment's band between its mouths, out to its footways' outer edges.
void add_ribbon(const RoadNetwork &net, int seg, Paths64 &out) {
	const RoadSegment &g = net.segs[size_t(seg)];
	const Profile &p = net.profile_of(seg);
	const double from = g.retreat_a, to = g.len - g.retreat_b;
	if (p.rail || p.edges.empty() || to <= from) return;
	const int steps = std::max(1, int(std::ceil((to - from) / ALONG_PACE_M)));
	Path64 left, right;
	for (int k = 0; k <= steps; k++) {
		const double v = from + (to - from) * k / steps;
		double x, z, y;
		footway_point(net, seg, v, p.edges.back().u1, x, z, y);
		left.push_back(paving_point(x, z));
		footway_point(net, seg, v, p.edges.front().u0, x, z, y);
		right.push_back(paving_point(x, z));
	}
	left.insert(left.end(), right.rbegin(), right.rend());
	add_positive(out, std::move(left));
}

// A plated node's ring: each corner's footway back in turn, from one arm's left outer edge round to the next arm's right one;
// between two corners it crosses an arm, whose band lies over that line.
void add_plate(const RoadNetwork &net, const RoadNode &nd, const std::vector<NodeEnd> &ends, Paths64 &out) {
	Path64 ring;
	for (size_t i = 0; i < ends.size(); i++)
		for (const V2 &p : node_corner(net, nd.x, nd.z, ends[i], ends[(i + 1) % ends.size()]).back) ring.push_back(paving_point(nd.x + p.x, nd.z + p.y));
	add_positive(out, std::move(ring));
}

// A turning bulb's ring at its footway's back, open where its road comes in (the band lies over the opening).
void add_bulb(const RoadNetwork &net, const RoadNode &nd, const NodeEnd &en, Paths64 &out) {
	const double ax = en.arm.dx, az = en.arm.dz; // along the arm; its left is (az, -ax)
	Path64 ring;
	for (const V2 &s : bulb_ring(net, en, BULB_R + BULB_WALK)) ring.push_back(paving_point(nd.x + ax * s.x + az * s.y, nd.z + az * s.x - ax * s.y));
	add_positive(out, std::move(ring));
}

// What a node lays besides its segments' bands, as the road surfaces draw it: a plate with its corners, a bulb, or nothing (a cut
// end, a joint whose bands meet).
void add_node(const RoadNetwork &net, int node, Paths64 &out) {
	const RoadNode &nd = net.nodes[size_t(node)];
	if (nd.segs.empty()) return;
	const std::vector<NodeEnd> ends = node_ends(net, node);
	if (ends.size() == 1) {
		if (!ends[0].prof->rail && nd.bulb) add_bulb(net, nd, ends[0], out);
		return;
	}
	if (node_plated(ends)) add_plate(net, nd, ends, out);
}
} // namespace

Point64 paving_point(double x, double z) { return Point64(std::llround(x * PAVING_UNITS_PER_M), std::llround(z * PAVING_UNITS_PER_M)); }

void add_positive(Paths64 &out, Path64 path) {
	if (path.size() < 3) return;
	if (Clipper2Lib::Area(path) < 0) std::reverse(path.begin(), path.end());
	out.push_back(std::move(path));
}

Paths64 paving_near(const RoadNetwork &net, double x0, double z0, double x1, double z1) {
	Paths64 pieces;
	std::set<int> nodes;
	net.for_segments_near(x0 - PLATE_REACH_M, z0 - PLATE_REACH_M, x1 + PLATE_REACH_M, z1 + PLATE_REACH_M, [&](int seg) {
		add_ribbon(net, seg, pieces);
		nodes.insert(net.segs[size_t(seg)].a);
		nodes.insert(net.segs[size_t(seg)].b);
	});
	for (int n : nodes) add_node(net, n, pieces);
	return pieces.empty() ? pieces : Clipper2Lib::Union(pieces, FillRule::NonZero);
}

} // namespace city
