// The ground graded to the lots (lot_shape.h): every height sample a lot's shape reaches, the lot's own rectangle at its level, an
// apron round it at that level, then a straight bank to the natural ground (fill under the level, cut over it). Between two lots
// the stronger weight, then the nearer, grades a sample. What a lot grades is laid under the roads' ground by the roads' weight
// (compose.cpp compose_rect): under a road, its footways and the band beside them, the ground is the road's, never the lot's.
#include "sim/terrain/heightfield.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float HALF = 0.5f;
} // namespace

void Heightfield::apply_lot_shape(const LotShape &r, int x0, int z0, int x1, int z1, LotGrades &grades) const {
	const SampleRect a = samples_over(r.minx, r.minz, r.maxx, r.maxz, x0, z0, x1, z1);
	if (a.empty()) return;
	const float apron = tune->lot_apron_m, batter = tune->lot_bank_batter, reach = tune->lot_bank_reach_m;
	const int BW = x1 - x0 + 1;
	for (int z = a.z0; z <= a.z1; z++)
		for (int x = a.x0; x <= a.x1; x++) {
			const float px = x_of(x) - r.ox, pz = x_of(z) - r.oz;
			const float lx = px * r.ax + pz * r.az, ly = px * r.bx + pz * r.bz;
			// how far outside the lot's own rectangle, on every side (its front too: the street in front is the road's)
			const float out_x = std::max(0.f, std::fabs(lx) - r.w * HALF), out_y = std::max({ 0.f, -ly, ly - r.d });
			const float dist = std::sqrt(out_x * out_x + out_y * out_y);
			const size_t gi = size_t(z) * size_t(N) + size_t(x);
			const float natural = dequant(base_q[gi]), level = r.level_at(lx);
			float w = 1.f;
			if (dist > apron) {
				const float run = std::min(reach, std::fabs(natural - level) * batter);
				if (dist - apron >= run) continue; // the bank met the land
				w = 1.f - (dist - apron) / run;
			}
			const size_t li = size_t(z - z0) * size_t(BW) + size_t(x - x0);
			// the stronger weight wins, between equals the nearer lot: a lot's own ground beats a neighbour's apron
			if (w < grades.weight[li] || (w == grades.weight[li] && grades.height[li] != NO_LOT && dist >= grades.dist[li])) continue;
			grades.weight[li] = w;
			grades.dist[li] = dist;
			grades.height[li] = lerpf(natural, level, w);
		}
}

} // namespace city
