// A body standing on the ground of the world (standing_body.h): a piece's rectangle from its segment's frame, and the distance
// between two rectangles (apart on an axis of either: the nearest of their corners and sides; else how deep one stands in the other);
// a piece in volume: which of its kind's parts it shows, each part's box turned as the piece faces, and two pieces' boxes apart.
#include "sim/roads/standing_body.h"

#include "core/random.h"
#include "sim/roads/centre_line.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {

constexpr int CORNERS = 4;
constexpr int CAN_PERIOD = 5, CAN_PHASE = 3; // a transformer on one pole in five
constexpr int AXIS_X = 0, AXIS_Y = 1, AXIS_Z = 2; // a part's box: along the sidewalk, up, toward the road
constexpr float HALF = 0.5f;

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

int spot_parts(const FurnitureSpot &s) {
	if (s.kind != FurnitureKind::LAMP && s.kind != FurnitureKind::COLUMN) return 0;
	if (s.row != 0 || s.kind == FurnitureKind::COLUMN) return int(PoleSet::COLUMN);
	int mask = int(PoleSet::MAST) | int(PoleSet::ARM) | (s.lit ? int(PoleSet::LAMP) : 0);
	if (hash_combine(uint32_t(s.seg), uint32_t(s.station)) % uint32_t(CAN_PERIOD) == uint32_t(CAN_PHASE)) mask |= int(PoleSet::CAN);
	return mask;
}

void body_boxes(const RoadNetwork &net, const StreetFurnitureDef &def, const FurnitureSpot &s, std::vector<BodyBox> &out) {
	const std::vector<PartBox> &parts = def.parts[int(s.kind)];
	if (parts.empty()) return;
	double cx, cz, fx, fz;
	centre_at(net, s.seg, s.v, cx, cz, fx, fz);
	const float ax = float(fx), az = float(fz);
	const V2 fwd = V2(ax, az), left = V2(az, -ax); // the left of (fx, fz) is (fz, -fx)
	const V2 at = V2(float(cx + fz * s.u), float(cz - fx * s.u));
	const V2 turned = left * s.face_left + fwd * s.face_fwd;
	const V2 face = turned.len() > 0 ? turned.norm() : fwd; // the piece's +z
	const V2 side(face.y, -face.x); // its +x: up cross its facing
	const int shown = spot_parts(s);
	for (const PartBox &p : parts) {
		if (p.part != 0 && (p.part & shown) == 0) continue;
		BodyBox b;
		b.plan.at = at + side * ((p.lo[AXIS_X] + p.hi[AXIS_X]) * HALF) + face * ((p.lo[AXIS_Z] + p.hi[AXIS_Z]) * HALF);
		b.plan.along = side;
		b.plan.half_along = (p.hi[AXIS_X] - p.lo[AXIS_X]) * HALF;
		b.plan.half_across = (p.hi[AXIS_Z] - p.lo[AXIS_Z]) * HALF;
		b.y0 = p.lo[AXIS_Y];
		b.y1 = p.hi[AXIS_Y];
		out.push_back(b);
	}
}

float volumes_apart(const std::vector<BodyBox> &a, const std::vector<BodyBox> &b) {
	float least = BIG;
	for (const BodyBox &p : a)
		for (const BodyBox &q : b)
			if (p.y0 < q.y1 && q.y0 < p.y1) least = std::min(least, bodies_apart(p.plan, q.plan));
	return least;
}

float boxes_reach(const std::vector<BodyBox> &boxes, const V2 &at) {
	float reach = 0;
	for (const BodyBox &b : boxes) reach = std::max(reach, (b.plan.at - at).len() + std::hypot(b.plan.half_along, b.plan.half_across));
	return reach;
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
