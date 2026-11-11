// The ground graded to the lots (lot_shape.h): every height sample a lot's shape reaches, after the road shapes; the lot itself
// stands on its own level (LotShape::holds), the grid kept under it. Inside the lot and
// over the sidewalk in front of it the ground stands at the lot's level, with the strongest authority and no distance, so it wins
// over the ground a road holds beside itself; it never reaches the kerb or the carriageway in front. Round the lot the apron keeps
// that level; past it a straight bank meets the natural ground (fill under the level, cut over it).
#include "sim/terrain/heightfield.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float HALF = 0.5f;
} // namespace

void Heightfield::apply_lot_shape(const LotShape &r, int x0, int z0, int x1, int z1, std::vector<float> &best_w, std::vector<float> &best_d) {
	const SampleRect a = samples_over(r.minx, r.minz, r.maxx, r.maxz, x0, z0, x1, z1);
	if (a.empty()) return;
	const float apron = tune->lot_apron_m, batter = tune->lot_bank_batter, reach = tune->lot_bank_reach_m;
	const int BW = x1 - x0 + 1;
	for (int z = a.z0; z <= a.z1; z++)
		for (int x = a.x0; x <= a.x1; x++) {
			const float px = x_of(x) - r.ox, pz = x_of(z) - r.oz;
			const float lx = px * r.ax + pz * r.az, ly = px * r.bx + pz * r.bz;
			if (ly < -r.front_in) continue; // the kerb and the carriageway in front are the road's
			const float out_x = std::max(0.f, std::fabs(lx) - r.w * HALF), out_y = std::max(0.f, ly - r.d);
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
			// the stronger authority wins; between equals the nearer lot (a lot's own ground beats a neighbour's apron and the ground a
			// road holds beside itself, whose distance is from its centre line)
			if (w < best_w[li] || (w == best_w[li] && best_w[li] > 0.f && dist >= best_d[li])) continue;
			best_w[li] = w;
			best_d[li] = dist;
			set_final(x, z, lerpf(natural, level, w));
		}
}

} // namespace city
