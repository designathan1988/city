// A block's box (internal to sim/citygen block_lots.cpp): the least-area rectangle round a piece of land, its long side first,
// which the block's rows and columns are cut along.
#pragma once

#include "core/math.h"
#include "core/plane_geometry.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace city {

constexpr double EPS_LEN = 1e-9, EPS_AREA = 1e-6; // a hull edge this short has no direction; an area this much smaller is no smaller

inline std::vector<XZ> convex_hull(std::vector<XZ> s) {
	std::sort(s.begin(), s.end(), [](const XZ &a, const XZ &b) { return a.x < b.x || (!(b.x < a.x) && a.z < b.z); }); // as a pair sorts
	auto cross = [](const XZ &o, const XZ &a, const XZ &b) { return (a.x - o.x) * (b.z - o.z) - (a.z - o.z) * (b.x - o.x); };
	return monotone_hull(s, cross, 0.0);
}

struct Box {
	double cx, cz, ux, uz, vx, vz, L, D;
	// the point s along its length from its middle and t across it
	void at(double s, double t, double &x, double &z) const { x = cx + ux * s + vx * t; z = cz + uz * s + vz * t; }
};

inline bool min_area_box(const std::vector<XZ> &h, Box &best) {
	if (h.size() < 3) return false;
	double best_a = NO_ROOM;
	bool found = false;
	for (size_t e = 0; e < h.size(); e++) {
		const XZ &a = h[e], &b = h[(e + 1) % h.size()];
		double l = std::hypot(b.x - a.x, b.z - a.z);
		if (l < EPS_LEN) continue;
		double ux = (b.x - a.x) / l, uz = (b.z - a.z) / l, vx = -uz, vz = ux;
		double s0 = NO_ROOM, s1 = -NO_ROOM, t0 = NO_ROOM, t1 = -NO_ROOM;
		for (const XZ &q : h) {
			double s = q.x * ux + q.z * uz, t = q.x * vx + q.z * vz;
			s0 = std::min(s0, s); s1 = std::max(s1, s); t0 = std::min(t0, t); t1 = std::max(t1, t);
		}
		double area = (s1 - s0) * (t1 - t0);
		if (area >= best_a - EPS_AREA) continue;
		best_a = area;
		double sm = (s0 + s1) / 2, tm = (t0 + t1) / 2, ls = s1 - s0, dt = t1 - t0;
		double cx = ux * sm + vx * tm, cz = uz * sm + vz * tm;
		best = ls >= dt ? Box{ cx, cz, ux, uz, vx, vz, ls, dt } : Box{ cx, cz, vx, vz, -ux, -uz, dt, ls };
		found = true;
	}
	return found;
}

} // namespace city
