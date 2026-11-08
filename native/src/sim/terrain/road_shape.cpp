// The ground shaped to the roads (road_shape.h) for every height sample a road reaches. Each piece between two stations walks
// the samples within reach of it, so a sample learns its distance to the road and the surface there from the nearest piece;
// then its weight, kept where it beats the best so far.
#include "sim/terrain/heightfield.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float TUNNEL_CLOSE_PAD_M = 0.4f; // headroom, arch and this: the mouth of the bore
constexpr float TUNNEL_BAND_M = 0.16f; // the band either side of it the bore closes over

constexpr float LEAST_SPAN = 1e-6f; // a step's span never quite nothing

float smoothstep(float a, float b, float v) {
	const float u = std::min(1.f, std::max(0.f, (v - a) / std::max(LEAST_SPAN, b - a)));
	return u * u * (3.f - 2.f * u);
}
} // namespace

float road_shape_reach(const Tuning &t) { return t.road_shape_hold_m + t.road_shape_shoulder_most_m; }

float cut_wall_reach(const Tuning &t) { return t.road_shape_hold_m + t.road_shape_cut_shoulder_m; }

float ground_carries(uint8_t kind, float lift, const Tuning &t) {
	if ((kind & SHAPE_RAISED) != 0) return 1.f - smoothstep(t.grade_lift_on_m, t.economy_on_piers_from_m, lift);
	if ((kind & SHAPE_MANUAL) != 0 && lift > 0.f) return 1.f - smoothstep(t.structure_raised_m, t.economy_on_piers_from_m, lift);
	return 1.f;
}

void Heightfield::apply_road_shape(const RoadShape &r, int x0, int z0, int x1, int z1, std::vector<float> &best_w, std::vector<float> &best_d) {
	const int ax0 = std::max(x0, ix_of(r.minx)), ax1 = std::min(x1, ix_of(r.maxx) + 1);
	const int az0 = std::max(z0, ix_of(r.minz)), az1 = std::min(z1, ix_of(r.maxz) + 1);
	if (ax0 > ax1 || az0 > az1) return;
	const Tuning &t = *tune;
	const int W = ax1 - ax0 + 1;
	const std::vector<NearSample> &near = nearest(r.pts, r.half + road_shape_reach(t), ax0, az0, ax1, az1);
	const float close = t.grade_tunnel_headroom_m + t.grade_tunnel_arch_m + TUNNEL_CLOSE_PAD_M;
	const float roof = close - TUNNEL_BAND_M, bore = close + TUNNEL_BAND_M;
	const float inner = r.half + t.road_shape_hold_m, most = t.road_shape_shoulder_most_m;
	const int BW = x1 - x0 + 1;
	for (int z = az0; z <= az1; z++)
		for (int x = ax0; x <= ax1; x++) {
			const NearSample &n = near[size_t(z - az0) * size_t(W) + size_t(x - ax0)];
			if (n.d >= inner + most) continue;
			const size_t gi = size_t(z) * size_t(N) + size_t(x);
			const float natural = dequant(base_q[gi]), surface = n.surface;
			const bool manual = (r.kind & SHAPE_MANUAL) != 0;
			const bool sunken = (r.kind & SHAPE_SUNKEN) != 0 || (manual && natural - surface > 0);
			// Under the paving the ground is pulled well below it; past its outer edge it stands level with that edge's top, the
			// ground meeting the road with nothing drawn between them.
			const bool beside = !sunken && n.d > r.paving;
			const float height = beside ? surface + r.rise : surface - t.road_shape_under_m;
			// The batter is sized from the earthwork it carries away; a cutting held by walls, or a bore's, is narrow.
			const float shoulder = sunken || ((r.kind & SHAPE_WALLS) != 0 && natural > height) ? t.road_shape_cut_shoulder_m
					: std::min(most, std::max(t.road_shape_shoulder_least_m, std::fabs(natural - height) * t.road_shape_batter));
			if (n.d >= inner + shoulder) continue;
			float w = 1.f - smoothstep(inner, inner + shoulder, n.d);
			if (sunken && (r.kind & SHAPE_RAISED) == 0) w *= 1.f - smoothstep(roof, bore, natural - surface);
			else w *= ground_carries(r.kind, surface - natural, t);
			const size_t li = size_t(z - z0) * size_t(BW) + size_t(x - x0);
			// two pieces of a road both weigh whole round the cut between them: the nearest one says, or a sample past the cut keeps the height of the piece before it
			if (w < best_w[li] || (w == best_w[li] && n.d >= best_d[li])) continue;
			best_w[li] = w;
			best_d[li] = n.d;
			final_h[gi] = lerpf(natural, height, w);
		}
}

} // namespace city
