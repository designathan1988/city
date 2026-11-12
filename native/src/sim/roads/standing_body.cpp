// A body standing on the ground of the world (standing_body.h): a piece's rectangle from its segment's frame, and the distance
// between two rectangles (apart on an axis of either: the nearest of their corners and sides; else how deep one stands in the other).
#include "sim/roads/standing_body.h"

#include "sim/roads/centre_line.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {

constexpr int CORNERS = 4;

V2 across_of(const StandingBody &b) { return V2(b.along.y, -b.along.x); } // the left of its length

// How far the body reaches either side of its middle along the unit direction `n`.
float half_extent(const StandingBody &b, const V2 &n) {
	return b.half_along * std::fabs(b.along.dot(n)) + b.half_across * std::fabs(across_of(b).dot(n));
}

void fill_corners(const StandingBody &b, V2 out[CORNERS]) {
	const V2 l = b.along * b.half_along, w = across_of(b) * b.half_across;
	out[0] = b.at + l + w;
	out[1] = b.at - l + w;
	out[2] = b.at - l - w;
	out[3] = b.at + l - w;
}

float point_to_side(const V2 &p, const V2 &a, const V2 &b) {
	const V2 d = b - a;
	const float len2 = d.len2();
	const float t = len2 > 0 ? clampf((p - a).dot(d) / len2, 0.f, 1.f) : 0.f;
	return (p - (a + d * t)).len();
}

// The least distance from the corners of `a` to the sides of `b`.
float corners_to_sides(const V2 a[CORNERS], const V2 b[CORNERS]) {
	float least = BIG;
	for (int i = 0; i < CORNERS; i++)
		for (int k = 0; k < CORNERS; k++) least = std::min(least, point_to_side(a[i], b[k], b[(k + 1) % CORNERS]));
	return least;
}

} // namespace

StandingBody body_of(const RoadNetwork &net, const FurnitureSpot &s) {
	double cx, cz, fx, fz;
	centre_at(net, s.seg, s.v, cx, cz, fx, fz);
	StandingBody b;
	b.along = V2(float(fx), float(fz));
	b.at = V2(float(cx + fz * s.u), float(cz - fx * s.u)); // the left of (fx, fz) is (fz, -fx)
	b.half_along = s.half_v;
	b.half_across = s.half_u;
	return b;
}

float bodies_apart(const StandingBody &a, const StandingBody &b) {
	const V2 between = b.at - a.at;
	float widest = -BIG; // the widest gap between the two along an axis of either: none over zero, and they stand in one another
	for (const V2 &n : { a.along, across_of(a), b.along, across_of(b) })
		widest = std::max(widest, std::fabs(between.dot(n)) - half_extent(a, n) - half_extent(b, n));
	if (widest <= 0) return widest;
	V2 ca[CORNERS], cb[CORNERS];
	fill_corners(a, ca);
	fill_corners(b, cb);
	return std::min(corners_to_sides(ca, cb), corners_to_sides(cb, ca));
}

} // namespace city
