// Where a footprint goes under the pointer (model_ops.h): its front on the back of the pavement of the nearest road, slid onto the
// crossing road at a corner; else at a near neighbour's bearing; else on the grid; and flush with the lots beside it. It runs at every
// pointer move, so it only asks by neighbourhood.
#include "sim/buildings/b40/record.h"
#include "sim/commands/model_ops.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

using namespace b40;

namespace {
constexpr double QUARTER = 1.5707963267948966, TURN = 2 * QUARTER * 2;
constexpr double ALONG_SIDE_COS = 0.9; // a road this close to the footprint's depth runs along its side
constexpr double SAME_BEARING_RAD = 0.02;
constexpr double FLUSH_PAD_M = 8;
constexpr double BEYOND = 1e30, TINY = 1e-9;

struct Vec {
	double x = 0, z = 0;
	Vec operator+(Vec o) const { return { x + o.x, z + o.z }; }
	Vec operator-(Vec o) const { return { x - o.x, z - o.z }; }
	Vec operator*(double k) const { return { x * k, z * k }; }
	double dot(Vec o) const { return x * o.x + z * o.z; }
};
// The lot's axes at a turn: along the front, and from the street into the lot (b40::lot_frame).
Vec along_of(double rot) { return { std::cos(rot), -std::sin(rot) }; }
Vec into_of(double rot) { return { -std::sin(rot), -std::cos(rot) }; }

double normal_angle(double a) {
	a = std::fmod(a, TURN);
	if (a > TURN / 2) a -= TURN;
	if (a < -TURN / 2) a += TURN;
	return a;
}

struct Size {
	double w, d, module;
};

// Stands for the point and direction of a road at t.
struct RoadAt {
	Vec p, t;
};
RoadAt road_at(const Quad &q, double t) {
	RoadAt r;
	q.at(t, r.p.x, r.p.z);
	q.deriv(t, r.t.x, r.t.z);
	r.t = r.t * (1 / std::max(std::hypot(r.t.x, r.t.z), TINY));
	return r;
}

bool counts(const Building &b) { return b.floors > 0 || b.body != 0; }

Vec corner_flush(const CityState &s, Vec anchor, double rot, const Size &size, int snapped, Vec cursor) {
	const Vec u = along_of(rot), v = into_of(rot);
	const Vec mid = anchor + v * (size.d / 2);
	const double reach = size.w / 2 + size.w + size.module, clearance = s.catalog.tune.build_road_clearance_m;
	double shift = 0, best = BEYOND;
	s.roads.for_segments_near(mid.x - reach, mid.z - reach, mid.x + reach, mid.z + reach, [&](int sg) {
		if (sg == snapped) return;
		const double back = s.roads.profile_of(sg).half() + clearance;
		for (double side : { -1.0, 1.0 }) {
			const Vec p = anchor + u * (side * size.w / 2) + v * (size.d / 2);
			double t, dist;
			closest_t(s.roads.segs[sg].q, p.x, p.z, t, dist);
			if (dist > back + size.w + size.module) continue;
			const RoadAt road = road_at(s.roads.segs[sg].q, t);
			if (std::fabs(road.t.dot(v)) < ALONG_SIDE_COS) continue;
			const double beyond = (road.p - p).dot(u) * side;
			if (beyond <= 0) continue;
			const double need = beyond - back;
			const Vec rel = cursor - (anchor + u * (need * side));
			const double out_along = std::max(0.0, std::fabs(rel.dot(u)) - size.w / 2), behind = rel.dot(v);
			const double out_depth = std::max({ 0.0, -behind, behind - size.d });
			if (std::hypot(out_along, out_depth) > size.module) continue;
			if (std::fabs(need) < std::fabs(best)) {
				best = need;
				shift = need * side;
			}
		}
	});
	return anchor + u * shift;
}

bool snap_road(const CityState &s, const Size &size, Vec at, PlaceSnap &out) {
	const Tuning &tune = s.catalog.tune;
	const double reach = tune.build_road_clearance_m + size.d + tune.build_snap_road_reach_m, grid = tune.build_grid_m;
	double best = BEYOND;
	int road = -1;
	Vec anchor;
	double rot = 0;
	s.roads.for_segments_near(at.x - reach, at.z - reach, at.x + reach, at.z + reach, [&](int sg) {
		const Quad &q = s.roads.segs[sg].q;
		const double half = s.roads.profile_of(sg).half() + tune.build_road_clearance_m;
		double t, dist;
		closest_t(q, at.x, at.z, t, dist);
		if (dist > half + size.d + tune.build_snap_road_reach_m || dist >= best) return;
		const RoadAt hit = road_at(q, t);
		const Vec normal{ -hit.t.z, hit.t.x };
		const Vec out_of_road = normal * ((at - hit.p).dot(normal) >= 0 ? 1.0 : -1.0);
		const std::vector<double> table = arc_table(q, ARC_STEPS_DEFAULT);
		const double length = table.back(), on = std::round(t * length / grid) * grid;
		const RoadAt stop = road_at(q, t_at_length(table, std::clamp(on, 0.0, length)));
		best = dist;
		road = sg;
		anchor = stop.p + out_of_road * half;
		rot = std::atan2(-out_of_road.x, -out_of_road.z); // the lot's y runs out of the road
	});
	if (road < 0) return false;
	const Vec flush = corner_flush(s, anchor, rot, size, road, at);
	out = PlaceSnap{ flush.x, flush.z, normal_angle(rot), SNAP_ROAD };
	return true;
}

const Building *nearest_building(const CityState &s, Vec at) {
	const double reach = s.catalog.tune.build_snap_building_reach_m, pad = reach + s.catalog.tune.build_max_side_m;
	const Building *best = nullptr;
	double closest = reach;
	s.buildings.query(float(at.x - pad), float(at.z - pad), float(at.x + pad), float(at.z + pad), [&](int id) {
		const Building &b = s.buildings.items[size_t(id)];
		if (!counts(b)) return;
		const Vec rel = at - Vec{ b.x, b.z };
		const double u = rel.dot(along_of(b.rot)), v = rel.dot(into_of(b.rot));
		const double d = std::hypot(std::max(std::fabs(u) - b.w / 2, 0.0), std::max(std::fabs(v) - b.d / 2, 0.0));
		if (d < closest) {
			closest = d;
			best = &b;
		}
	});
	return best;
}

// The footprint's edges made flush with lots of the same bearing: along the front always, in depth when `both`.
void flush(const CityState &s, PlaceSnap &snap, const Size &size, bool both) {
	const Vec u = along_of(snap.rot), v = into_of(snap.rot), anchor{ snap.x, snap.z };
	const double near_m = s.catalog.tune.build_snap_edge * size.module, half = size.w / 2, reach = std::max(size.w, size.d) * 2 + FLUSH_PAD_M;
	double best_u = near_m, best_v = near_m, shift_u = 0, shift_v = 0;
	s.buildings.query(float(snap.x - reach), float(snap.z - reach), float(snap.x + reach), float(snap.z + reach), [&](int id) {
		const Building &b = s.buildings.items[size_t(id)];
		if (!counts(b) || std::fabs(normal_angle((b.rot - snap.rot) * 4)) / 4 > SAME_BEARING_RAD) return;
		double lo_u = BEYOND, hi_u = -BEYOND, lo_v = BEYOND, hi_v = -BEYOND;
		const Vec centre{ b.x, b.z }, bu = along_of(b.rot), bv = into_of(b.rot);
		for (double i : { -1.0, 1.0 })
			for (double k : { -1.0, 1.0 }) {
				const Vec rel = centre + bu * (i * b.w / 2) + bv * (k * b.d / 2) - anchor;
				lo_u = std::min(lo_u, rel.dot(u));
				hi_u = std::max(hi_u, rel.dot(u));
				lo_v = std::min(lo_v, rel.dot(v));
				hi_v = std::max(hi_v, rel.dot(v));
			}
		if (hi_v > 0 && lo_v < size.d)
			for (double shift : { hi_u + half, lo_u - half, lo_u + half, hi_u - half })
				if (std::fabs(shift) < best_u) {
					best_u = std::fabs(shift);
					shift_u = shift;
				}
		if (both && hi_u > -half && lo_u < half)
			for (double shift : { hi_v, lo_v - size.d, lo_v, hi_v - size.d })
				if (std::fabs(shift) < best_v) {
					best_v = std::fabs(shift);
					shift_v = shift;
				}
	});
	const Vec moved = anchor + u * shift_u + v * shift_v;
	snap.x = moved.x;
	snap.z = moved.z;
}

Vec front_of(Vec centre, const Size &size, double rot) { return centre - into_of(rot) * (size.d / 2); }
} // namespace

PlaceSnap snap_place(const CityState &s, float w, float d, double x, double z, double rot) {
	const Tuning &t = s.catalog.tune;
	const Size size{ w, d, t.build_module_m };
	const Vec at{ x, z };
	PlaceSnap out;
	if (snap_road(s, size, at, out)) {
		flush(s, out, size, false);
		return out;
	}
	if (const Building *near_one = nearest_building(s, at)) {
		const double turn = near_one->rot + std::round(normal_angle(rot - near_one->rot) / QUARTER) * QUARTER;
		const Vec front = front_of(at, size, turn);
		out = PlaceSnap{ front.x, front.z, normal_angle(turn), SNAP_BUILDING };
		flush(s, out, size, true);
		return out;
	}
	const double grid = t.build_grid_m;
	const Vec front = front_of(Vec{ std::round(x / grid) * grid, std::round(z / grid) * grid }, size, rot);
	return PlaceSnap{ front.x, front.z, normal_angle(rot), SNAP_GRID };
}

} // namespace city
