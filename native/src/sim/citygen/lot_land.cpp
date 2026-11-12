// The paving as polygons in whole millimetres (Clipper2 works in integers, so a cut is the same on every machine): each road's ribbon
// between its mouths, each junction's plate out to the point where two footways meet, each turning bulb. Read only round the box
// asked: the whole map's outline is never made.
#include "sim/citygen/lot_land.h"

#include <iterator> // before Clipper2: its header needs it and does not say so

#include <clipper.h>
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
constexpr double UNITS_PER_M = 1000;
constexpr double ALONG_PACE_M = 4; // a curve's outline in chords of this length
constexpr int ROUND_STEPS = 24;
constexpr double PAVING_MARGIN_M = 2; // past the box, so the cut inside it is the whole map's
constexpr double PLATE_REACH_M = 40; // how far a junction's plate may stand from the roads that meet it
constexpr double CROSSED_AREA_M2 = 1; // less off a lot is the paving's own outline, not a road through it
constexpr double STRAIGHT_BACK_M = 2; // an outline edge this long is a straight back of a footway; shorter, a corner's curve
constexpr double SQUARE_SHARE = 0.3; // sine under which two backs are one street, not two
constexpr double SQUARE_REACH_M = 8; // how far a squared corner may stand from the outline
constexpr double FRONT_SLACK_M = 1.5; // sides this much further from the street than the nearest still face it
constexpr double FRONT_SEARCH_M = 60;
constexpr double LOT_SNAP_SHARE = 0.7; // of the reach, for the corners of other lots
constexpr double BACK_SPAN = 1.5; // of the reach, for the backs that make a corner
constexpr double APART_M = 0.8;
constexpr double PARALLEL = 1e-6;
constexpr double OUTLINE_SLACK_UNITS = 20; // the chords of a straight back are one side: 2 cm off a line is on it

Point64 unit_point(double x, double z) { return Point64(std::llround(x * UNITS_PER_M), std::llround(z * UNITS_PER_M)); }

Path64 path_of(const std::vector<LotPoint> &ring) {
	Path64 out;
	out.reserve(ring.size());
	for (const LotPoint &p : ring) out.push_back(unit_point(p.x, p.z));
	return out;
}

std::vector<LotPoint> ring_of(const Path64 &path) {
	std::vector<LotPoint> out;
	out.reserve(path.size());
	for (const Point64 &p : path) out.push_back({ double(p.x) / UNITS_PER_M, double(p.y) / UNITS_PER_M });
	return out;
}

// One winding for every piece: under the non-zero rule two pieces wound against each other cancel where they overlap.
void add_positive(Paths64 &out, Path64 path) {
	if (path.size() < 3) return;
	if (Clipper2Lib::Area(path) < 0) std::reverse(path.begin(), path.end());
	out.push_back(std::move(path));
}

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
		left.push_back(unit_point(x, z));
		footway_point(net, seg, v, p.edges.front().u0, x, z, y);
		right.push_back(unit_point(x, z));
	}
	left.insert(left.end(), right.rbegin(), right.rend());
	add_positive(out, std::move(left));
}

struct NodeEnd {
	LotPoint left, right; // the outer edges of the footways at the mouth, seen going out along the arm
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

void add_junction(const RoadNetwork &net, int node, Paths64 &out) {
	const RoadNode &nd = net.nodes[size_t(node)];
	std::vector<NodeEnd> ends;
	for (int seg : nd.segs)
		if (!net.profile_of(seg).rail && !net.profile_of(seg).edges.empty()) ends.push_back(end_at(net, node, seg));
	if (ends.empty()) return;
	if (ends.size() == 1) {
		if (!nd.bulb) return;
		Path64 circle;
		const double r = BULB_R + BULB_WALK;
		for (int i = 0; i < ROUND_STEPS; i++) {
			const double a = 2 * PI_D * i / ROUND_STEPS;
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

void box_of(const std::vector<LotPoint> &ring, double &x0, double &z0, double &x1, double &z1) {
	x0 = z0 = std::numeric_limits<double>::max();
	x1 = z1 = std::numeric_limits<double>::lowest();
	for (const LotPoint &p : ring) {
		x0 = std::min(x0, p.x);
		z0 = std::min(z0, p.z);
		x1 = std::max(x1, p.x);
		z1 = std::max(z1, p.z);
	}
}

// The paving round a box, whole: its outline and, wound the other way, the blocks it closes.
Paths64 paving_near(const RoadNetwork &net, double x0, double z0, double x1, double z1) {
	Paths64 pieces;
	std::set<int> nodes;
	net.for_segments_near(x0 - PLATE_REACH_M, z0 - PLATE_REACH_M, x1 + PLATE_REACH_M, z1 + PLATE_REACH_M, [&](int seg) {
		add_ribbon(net, seg, pieces);
		nodes.insert(net.segs[size_t(seg)].a);
		nodes.insert(net.segs[size_t(seg)].b);
	});
	for (int n : nodes) add_junction(net, n, pieces);
	return pieces.empty() ? pieces : Clipper2Lib::Union(pieces, FillRule::NonZero);
}

Paths64 paving_round(const RoadNetwork &net, const std::vector<LotPoint> &ring) {
	double x0, z0, x1, z1;
	box_of(ring, x0, z0, x1, z1);
	return paving_near(net, x0 - PAVING_MARGIN_M, z0 - PAVING_MARGIN_M, x1 + PAVING_MARGIN_M, z1 + PAVING_MARGIN_M);
}

// What of a ring stands off the paving: the outlines, holes left out as a lot has none.
Paths64 land_of(const std::vector<LotPoint> &ring, const Paths64 &paving) {
	Paths64 subject;
	add_positive(subject, path_of(ring));
	Paths64 left = Clipper2Lib::Difference(subject, paving, FillRule::NonZero), out;
	for (Path64 &p : left)
		if (Clipper2Lib::Area(p) > 0) out.push_back(std::move(p));
	return Clipper2Lib::SimplifyPaths(out, OUTLINE_SLACK_UNITS);
}

double side_length(const LotPoint &p, const LotPoint &q) { return std::hypot(q.x - p.x, q.z - p.z); }

// A lot cut back to the footways, the corners between its long sides made square: the curve of a street's corner goes.
std::vector<LotPoint> square_corners(const std::vector<LotPoint> &ring) {
	const size_t n = ring.size();
	std::vector<size_t> long_sides;
	for (size_t i = 0; i < n; i++)
		if (side_length(ring[i], ring[(i + 1) % n]) >= STRAIGHT_BACK_M) long_sides.push_back(i);
	if (long_sides.size() < 2) return ring;
	std::vector<LotPoint> out;
	for (size_t k = 0; k < long_sides.size(); k++) {
		const size_t i = long_sides[k], j = long_sides[(k + 1) % long_sides.size()];
		const LotPoint &a = ring[i], &b = ring[(i + 1) % n], &c = ring[j], &d = ring[(j + 1) % n];
		if ((i + 1) % n == j) {
			out.push_back(b);
			continue;
		}
		const double ux = b.x - a.x, uz = b.z - a.z, vx = d.x - c.x, vz = d.z - c.z, den = ux * vz - uz * vx;
		bool met = std::fabs(den) > SQUARE_SHARE * std::hypot(ux, uz) * std::hypot(vx, vz);
		LotPoint x;
		if (met) {
			const double t = ((c.x - a.x) * vz - (c.z - a.z) * vx) / den;
			x = { a.x + ux * t, a.z + uz * t };
			met = side_length(x, b) < SQUARE_REACH_M;
		}
		if (met) out.push_back(x);
		else // not a corner of two streets: the short sides kept as they are
			for (size_t r = (i + 1) % n; r != (j + 1) % n; r = (r + 1) % n) out.push_back(ring[r]);
	}
	return out.size() >= 3 ? out : ring;
}

double street_distance(const RoadNetwork &net, double x, double z) {
	double best = std::numeric_limits<double>::infinity();
	net.for_segments_near(x - FRONT_SEARCH_M, z - FRONT_SEARCH_M, x + FRONT_SEARCH_M, z + FRONT_SEARCH_M, [&](int seg) {
		if (net.profile_of(seg).rail) return;
		double v;
		float u, past;
		segment_place(net, seg, x, z, v, u, past);
		best = std::min(best, std::hypot(double(u), double(past)));
	});
	return best;
}

bool lot_corner_near(const LotBook &book, const LotPoint &p, double reach, int skip, const LotPoint *apart, LotPoint &out) {
	double best = reach;
	bool found = false;
	for (int id : book.overlapping({ p.x - reach, p.z - reach, p.x + reach, p.z + reach })) {
		const PlayerLot &l = *book.find(id);
		if (l.id == skip) continue;
		for (const LotPoint &q : l.corners) {
			if (apart && side_length(q, *apart) <= APART_M) continue;
			const double d = side_length(q, p);
			if (d < best) {
				best = d;
				out = q;
				found = true;
			}
		}
	}
	return found;
}

struct Back {
	double d, x0, z0, dx, dz;
};
} // namespace

std::vector<LotPoint> lot_on_land(const RoadNetwork &net, const std::vector<LotPoint> &ring) {
	if (ring.size() < 3) return {};
	const Paths64 paving = paving_round(net, ring);
	if (paving.empty()) return ring;
	const Path64 *best = nullptr;
	double most = 0;
	const Paths64 left = land_of(ring, paving);
	for (const Path64 &p : left)
		if (Clipper2Lib::Area(p) > most) {
			most = Clipper2Lib::Area(p);
			best = &p;
		}
	if (!best) return {};
	const std::vector<LotPoint> land = ring_of(*best);
	return big_enough(land) ? square_corners(land) : std::vector<LotPoint>();
}

std::vector<std::vector<LotPoint>> lot_pieces(const RoadNetwork &net, const std::vector<LotPoint> &ring, bool &crossed) {
	crossed = false;
	std::vector<std::vector<LotPoint>> pieces;
	const Paths64 paving = paving_round(net, ring);
	if (paving.empty()) return pieces;
	double left = 0;
	for (const Path64 &p : land_of(ring, paving)) {
		std::vector<LotPoint> piece = ring_of(p);
		left += std::abs(lot_area(piece));
		if (big_enough(piece)) pieces.push_back(std::move(piece));
	}
	crossed = std::abs(lot_area(ring)) - left >= CROSSED_AREA_M2;
	return pieces;
}

int lot_front_side(const RoadNetwork &net, const std::vector<LotPoint> &ring) {
	const size_t n = ring.size();
	std::vector<double> away(n);
	double nearest = std::numeric_limits<double>::infinity();
	for (size_t i = 0; i < n; i++) {
		const LotPoint &p = ring[i], &q = ring[(i + 1) % n];
		away[i] = street_distance(net, (p.x + q.x) / 2, (p.z + q.z) / 2);
		nearest = std::min(nearest, away[i]);
	}
	int front = 0;
	double longest = -1;
	for (size_t i = 0; i < n; i++) {
		const double length = side_length(ring[i], ring[(i + 1) % n]);
		if (away[i] < nearest + FRONT_SLACK_M && length > longest) {
			longest = length;
			front = int(i);
		}
	}
	return front;
}

LotSnap lot_snap(const LotBook &book, const RoadNetwork &net, const LotPoint &p, double reach, int skip, const LotPoint *apart) {
	LotSnap out;
	out.p = p;
	LotPoint corner;
	if (lot_corner_near(book, p, reach * LOT_SNAP_SHARE, skip, apart, corner)) {
		out.p = corner;
		out.kind = LotSnapKind::LOT;
		return out;
	}
	const double span = reach * BACK_SPAN, around = span + STRAIGHT_BACK_M + PAVING_MARGIN_M;
	const Paths64 paving = paving_near(net, p.x - around, p.z - around, p.x + around, p.z + around);
	std::vector<Back> backs;
	double best = reach;
	bool street = false;
	LotPoint on_street;
	for (const Path64 &ring : paving)
		for (size_t k = 0; k < ring.size(); k++) {
			const Point64 &a = ring[k], &b = ring[(k + 1) % ring.size()];
			const double x0 = double(a.x) / UNITS_PER_M, z0 = double(a.y) / UNITS_PER_M, dx = double(b.x - a.x) / UNITS_PER_M, dz = double(b.y - a.y) / UNITS_PER_M;
			const double len2 = dx * dx + dz * dz, t = len2 > 0 ? std::clamp(((p.x - x0) * dx + (p.z - z0) * dz) / len2, 0.0, 1.0) : 0;
			const double qx = x0 + dx * t, qz = z0 + dz * t, d = std::hypot(qx - p.x, qz - p.z);
			if (d < best) {
				best = d;
				on_street = { qx, qz };
				street = true;
			}
			if (len2 >= STRAIGHT_BACK_M * STRAIGHT_BACK_M && d < span) backs.push_back({ d, x0, z0, dx, dz });
		}
	if (!backs.empty()) {
		const Back first = *std::min_element(backs.begin(), backs.end(), [](const Back &u, const Back &v) { return u.d < v.d; });
		std::stable_sort(backs.begin(), backs.end(), [](const Back &u, const Back &v) { return u.d < v.d; });
		for (const Back &other : backs) {
			const double den = first.dx * other.dz - first.dz * other.dx;
			if (std::fabs(den) < SQUARE_SHARE * std::hypot(first.dx, first.dz) * std::hypot(other.dx, other.dz)) continue; // one back cut in two
			const double k = ((other.x0 - first.x0) * other.dz - (other.z0 - first.z0) * other.dx) / den;
			const LotPoint x = { first.x0 + first.dx * k, first.z0 + first.dz * k };
			if (side_length(x, p) < span) {
				out.p = x;
				out.kind = LotSnapKind::CORNER;
				return out;
			}
			break;
		}
	}
	if (street) {
		out.p = on_street;
		out.kind = LotSnapKind::STREET;
	}
	return out;
}

} // namespace city
