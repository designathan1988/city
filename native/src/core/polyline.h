// Open lines and closed rings of points in the XZ plane: the distance to a line and how far along it the nearest point
// lies, whether a ring holds a point, a line's length and a line moved sideways. Pure C++, usable from any thread.
#pragma once

#include "core/math.h"

#include <vector>

namespace city {

using Polyline = std::vector<V2>;

// Distance from p to the line, and in `along` the length of line before its nearest point. A single point is a line too.
inline float polyline_distance(const Polyline &line, V2 p, float *along = nullptr) {
	if (line.empty()) return BIG;
	if (line.size() == 1) {
		if (along) *along = 0;
		return (p - line[0]).len();
	}
	float best = BIG, walked = 0, at = 0;
	for (size_t i = 0; i + 1 < line.size(); i++) {
		float t = 0;
		const float d = dist_point_segment(p, line[i], line[i + 1], &t), seg = (line[i + 1] - line[i]).len();
		if (d < best) {
			best = d;
			at = walked + seg * t;
		}
		walked += seg;
	}
	if (along) *along = at;
	return best;
}

inline float polyline_length(const Polyline &line) {
	float l = 0;
	for (size_t i = 0; i + 1 < line.size(); i++) l += (line[i + 1] - line[i]).len();
	return l;
}

// Even-odd test: true when the closed ring (last point joined to the first) holds p.
inline bool ring_holds(const Polyline &ring, V2 p) {
	bool in = false;
	for (size_t i = 0, j = ring.size() - 1; i < ring.size(); j = i++) {
		const V2 &a = ring[i], &b = ring[j];
		if ((a.y > p.y) != (b.y > p.y) && p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x) in = !in;
	}
	return in;
}

// Distance from p to the ring's edge (closing side included).
inline float ring_edge_distance(const Polyline &ring, V2 p) {
	if (ring.size() < 2) return BIG;
	float best = dist_point_segment(p, ring.back(), ring.front());
	for (size_t i = 0; i + 1 < ring.size(); i++) best = std::min(best, dist_point_segment(p, ring[i], ring[i + 1]));
	return best;
}

// The line moved `offset` to its right (V2::perp of its heading; negative: to its left), mitred at the bends.
inline Polyline polyline_offset(const Polyline &line, float offset) {
	Polyline out;
	const size_t n = line.size();
	if (n < 2) return line;
	for (size_t i = 0; i < n; i++) {
		const V2 in = (line[i > 0 ? i : 1] - line[i > 0 ? i - 1 : 0]).norm(), on = (line[i + 1 < n ? i + 1 : n - 1] - line[i + 1 < n ? i : n - 2]).norm();
		const V2 side = (in.perp() + on.perp()).norm();
		const float mitre = std::max(0.5f, side.dot(on.perp()));
		out.push_back(line[i] + side * (offset / mitre));
	}
	return out;
}

} // namespace city
