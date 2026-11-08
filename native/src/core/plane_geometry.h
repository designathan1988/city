// The plane's few primitives every point type of the core shares, written once over whatever type holds the points: where two
// lines meet, whether a point lies inside a ring, the convex hull. Each caller keeps its own tolerance and its own range.
#pragma once

#include <cmath>
#include <cstddef>
#include <vector>

namespace city {

// Where line a + t (b - a) meets line c + u (d - c), as the shares t and u; false when the lines are parallel, their cross
// product under `eps`.
template <class T>
bool line_shares(T ax, T ay, T bx, T by, T cx, T cy, T dx, T dy, T eps, T &t, T &u) {
	const T rx = bx - ax, ry = by - ay, sx = dx - cx, sy = dy - cy;
	const T den = rx * sy - ry * sx;
	if (std::fabs(den) < eps) return false;
	const T qx = cx - ax, qy = cy - ay;
	t = (qx * sy - qy * sx) / den;
	u = (qx * ry - qy * rx) / den;
	return true;
}

// Whether (x, y) lies inside `ring` by the crossings of a ray towards +x (even-odd); `xy(point)` gives a corner as {x, y}.
template <class Ring, class T, class XY>
bool inside_ring(const Ring &ring, T x, T y, XY xy) {
	bool in = false;
	for (size_t i = 0, j = ring.size() - 1; i < ring.size(); j = i++) {
		const auto a = xy(ring[i]), b = xy(ring[j]);
		if ((a.second > y) != (b.second > y) && x < (b.first - a.first) * (y - a.second) / (b.second - a.second) + a.first) in = !in;
	}
	return in;
}

// Andrew's monotone chain over points sorted by (x, y): the convex hull from the lowest-leftmost point round, a turn of `eps` or
// less (straight on, or nearly) dropped. `cross(o, a, b)` is the turn from a to b about o.
template <class P, class Cross>
std::vector<P> monotone_hull(const std::vector<P> &sorted, Cross cross, double eps) {
	if (sorted.size() < 3) return sorted;
	auto chain = [&](auto first, auto last) {
		std::vector<P> out;
		for (auto it = first; it != last; ++it) {
			while (out.size() >= 2 && cross(out[out.size() - 2], out.back(), *it) <= eps) out.pop_back();
			out.push_back(*it);
		}
		out.pop_back();
		return out;
	};
	std::vector<P> hull = chain(sorted.begin(), sorted.end());
	const std::vector<P> upper = chain(sorted.rbegin(), sorted.rend());
	hull.insert(hull.end(), upper.begin(), upper.end());
	return hull;
}

} // namespace city
