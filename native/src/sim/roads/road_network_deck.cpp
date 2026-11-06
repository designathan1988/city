// Where a ray lands on the deck of a road: what the pointer meets before the ground.
#include "sim/roads/road_network.h"
#include "sim/roads/junction.h"
#include <algorithm>
#include <cmath>

namespace city {

bool RoadNetwork::ray_deck(double ox, double oy, double oz, double dx, double dy, double dz, double max_t, double &t_hit) const {
	if (dy >= 0) return false; // a ray going up never comes down onto a deck
	const double ex = ox + dx * max_t, ez = oz + dz * max_t, run = std::hypot(dx, dz);
	bool found = false;
	t_hit = max_t;
	for_segments_near(std::min(ox, ex), std::min(oz, ez), std::max(ox, ex), std::max(oz, ez), [&](int s) {
		const RoadSegment &g = segs[size_t(s)];
		if (is_sunken(structure_of(g.flags))) return;
		const double half = profile_of(s).half();
		// The stretch of the ray over the segment's box grown by its section, and between its lowest and highest deck.
		double t0 = 0, t1 = t_hit;
		auto slab = [&](double o, double d, double lo, double hi) {
			if (std::fabs(d) < RAY_PARALLEL_EPS) return o >= lo && o <= hi;
			double a = (lo - o) / d, b = (hi - o) / d;
			if (a > b) std::swap(a, b);
			t0 = std::max(t0, a);
			t1 = std::min(t1, b);
			return t0 <= t1;
		};
		double ylo = std::min(nodes[size_t(g.a)].y, nodes[size_t(g.b)].y), yhi = std::max(nodes[size_t(g.a)].y, nodes[size_t(g.b)].y);
		if (grade.has(s))
			for (float h : grade.prof[size_t(s)].h) { ylo = std::min(ylo, double(h)); yhi = std::max(yhi, double(h)); }
		if (!slab(ox, dx, g.minx - half, g.maxx + half) || !slab(oz, dz, g.minz - half, g.maxz + half) || !slab(oy, dy, ylo, yhi)) return;
		// Above the deck at t, where the section holds the ray's point (outside it: no deck there).
		auto above = [&](double t, bool &on) {
			double ct, dist;
			closest_t(g.q, ox + dx * t, oz + dz * t, ct, dist);
			on = dist <= half;
			return oy + dy * t - seg_y(s, arc_at(g.table, ct));
		};
		const double step = DECK_RAY_STEP_M / std::max(RAY_RUN_EPS, run);
		double prev = t0;
		for (double t = t0; t <= t1 + step; t += step) {
			const double at = std::min(t, t1);
			bool on;
			if (above(at, on) <= 0 && on) {
				double lo = prev, hi = at;
				for (int i = 0; i < DECK_RAY_REFINE; i++) {
					const double m = (lo + hi) * 0.5;
					bool mid_on;
					if (above(m, mid_on) > 0 || !mid_on) lo = m; else hi = m;
				}
				if (hi < t_hit) { t_hit = hi; found = true; }
				return;
			}
			prev = at;
			if (at >= t1) break;
		}
	});
	return found;
}

} // namespace city
