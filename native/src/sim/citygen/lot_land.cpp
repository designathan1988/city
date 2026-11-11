// A lot against the paving as it is drawn (sim/street/paving_outline.h: the bands, the junctions' plates through their corners'
// backs, the bulbs), read only round the box asked: what of a lot stands off it, which side faces the street, where a point lands.
#include "sim/citygen/lot_land.h"

#include "sim/roads/road_network.h"
#include "sim/street/footway.h"
#include "sim/street/paving_outline.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

using Clipper2Lib::FillRule;
using Clipper2Lib::Path64;
using Clipper2Lib::Paths64;
using Clipper2Lib::Point64;

namespace {
constexpr double PAVING_MARGIN_M = 2; // past the box, so the cut inside it is the whole map's
constexpr double CROSSED_AREA_M2 = 1; // less off a lot is the paving's own outline, not a road through it
constexpr double STRAIGHT_BACK_M = 2; // an outline edge this long is a straight back of a footway; shorter, a corner's curve
constexpr double SQUARE_SHARE = 0.3; // sine under which two backs are one street, not two
constexpr double SQUARE_REACH_M = 8; // how far a squared corner may stand from the outline
constexpr double FRONT_SLACK_M = 1.5; // sides this much further from the street than the nearest still face it
constexpr double FRONT_SEARCH_M = 60;
constexpr double LOT_SNAP_SHARE = 0.7; // of the reach, for the corners of other lots
constexpr double BACK_SPAN = 1.5; // of the reach, for the backs that make a corner
constexpr double APART_M = 0.8;
constexpr double OUTLINE_SLACK_UNITS = 20; // the chords of a straight back are one side: 2 cm off a line is on it

Path64 path_of(const std::vector<LotPoint> &ring) {
	Path64 out;
	out.reserve(ring.size());
	for (const LotPoint &p : ring) out.push_back(paving_point(p.x, p.z));
	return out;
}

std::vector<LotPoint> ring_of(const Path64 &path) {
	std::vector<LotPoint> out;
	out.reserve(path.size());
	for (const Point64 &p : path) out.push_back({ double(p.x) / PAVING_UNITS_PER_M, double(p.y) / PAVING_UNITS_PER_M });
	return out;
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
	for (const PlayerLot &l : book.lots) {
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
			const double x0 = double(a.x) / PAVING_UNITS_PER_M, z0 = double(a.y) / PAVING_UNITS_PER_M, dx = double(b.x - a.x) / PAVING_UNITS_PER_M,
						 dz = double(b.y - a.y) / PAVING_UNITS_PER_M;
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
