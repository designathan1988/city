// The ground shaped to the roads (road_shape.h): Roadcraft 4.0 world/elevation.ts roadShapeAt for every height sample a
// road reaches. Each piece between two stations walks the samples within reach of it, so a sample learns its distance to
// the road and the surface there from the nearest piece; then its weight, kept where it beats the best so far.
#include "sim/terrain/heightfield.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float SHAPE_INNER_M = 1.2f; // SHAPE_INNER: past the casing, the ground held at road level
constexpr float SHAPE_SHOULDER_M = 18.f; // SHAPE_SHOULDER: the least batter back to the natural ground
constexpr float SHAPE_SHOULDER_MAX_M = 36.f; // SHAPE_SHOULDER_MAX: one deep cut never reshapes a quarter of the map
constexpr float BATTER = 2.5f; // BATTER: run per unit of rise of a cut or fill slope (1:2.5)
constexpr float CUT_SHOULDER_M = 6.4f; // CUT_SHOULDER: a tunnel's approach cutting, or one held by retaining walls
constexpr float SHAPE_DROP_M = 0.6f; // SHAPE_DROP: under the road the ground is pulled this far below its surface
constexpr float BESIDE_DROP_M = 0.1f; // BESIDE_DROP: beside it, this far
constexpr float TUNNEL_CLOSE_PAD_M = 0.4f; // structures.ts TUNNEL_CLOSE: headroom, arch and this: the mouth of the bore
constexpr float TUNNEL_BAND_M = 0.16f; // TUNNEL_ROOF / TUNNEL_BORE: the band either side of it the bore closes over
constexpr float CASING_BAND_SHAPE_M = 1.f; // roadTypes.ts CASING_BAND: past the footway, inside the casing (sim/terrain does not see sim/roads)

constexpr float LEAST_SPAN = 1e-6f; // a step's span never quite nothing

float smoothstep(float a, float b, float v) {
	const float u = std::min(1.f, std::max(0.f, (v - a) / std::max(LEAST_SPAN, b - a)));
	return u * u * (3.f - 2.f * u);
}
} // namespace

float road_shape_reach() { return SHAPE_INNER_M + SHAPE_SHOULDER_MAX_M; }

float cut_wall_reach() { return SHAPE_INNER_M + CUT_SHOULDER_M; }

void Heightfield::apply_road_shape(const RoadShape &r, int x0, int z0, int x1, int z1, std::vector<float> &best_w, std::vector<float> &best_d) {
	const int ax0 = std::max(x0, ix_of(r.minx)), ax1 = std::min(x1, ix_of(r.maxx) + 1);
	const int az0 = std::max(z0, ix_of(r.minz)), az1 = std::min(z1, ix_of(r.maxz) + 1);
	if (ax0 > ax1 || az0 > az1) return;
	const int W = ax1 - ax0 + 1;
	const std::vector<NearSample> &near = nearest(r.pts, r.half + road_shape_reach(), ax0, az0, ax1, az1);
	const float close = tune->grade_tunnel_headroom_m + tune->grade_tunnel_arch_m + TUNNEL_CLOSE_PAD_M;
	const float roof = close - TUNNEL_BAND_M, bore = close + TUNNEL_BAND_M;
	const float lift_on = tune->grade_lift_on_m, lift_off = tune->economy_on_piers_from_m;
	const float inner = r.half + SHAPE_INNER_M;
	const int BW = x1 - x0 + 1;
	for (int z = az0; z <= az1; z++)
		for (int x = ax0; x <= ax1; x++) {
			const NearSample &n = near[size_t(z - az0) * size_t(W) + size_t(x - ax0)];
			if (n.d >= inner + SHAPE_SHOULDER_MAX_M) continue;
			const size_t gi = size_t(z) * size_t(N) + size_t(x);
			const float natural = dequant(base_q[gi]), surface = n.surface;
			const bool manual = (r.kind & SHAPE_MANUAL) != 0;
			const bool sunken = (r.kind & SHAPE_SUNKEN) != 0 || (manual && natural - surface > 0);
			// Under the road (carriageway and footway) the ground is pulled well below the deck; beside it, just under.
			const bool beside = !sunken && n.d > r.half - CASING_BAND_SHAPE_M;
			const float height = surface - (beside ? BESIDE_DROP_M : SHAPE_DROP_M);
			// The batter is sized from the earthwork it carries away; a cutting held by walls, or a bore's, is narrow.
			const float shoulder = sunken || ((r.kind & SHAPE_WALLS) != 0 && natural > height) ? CUT_SHOULDER_M
					: std::min(SHAPE_SHOULDER_MAX_M, std::max(SHAPE_SHOULDER_M, std::fabs(natural - height) * BATTER));
			if (n.d >= inner + shoulder) continue;
			float w = 1.f - smoothstep(inner, inner + shoulder, n.d);
			if ((r.kind & SHAPE_RAISED) != 0 || (manual && surface > natural)) w *= 1.f - smoothstep(lift_on, lift_off, surface - natural);
			else if (sunken) w *= 1.f - smoothstep(roof, bore, natural - surface);
			const size_t li = size_t(z - z0) * size_t(BW) + size_t(x - x0);
			// two pieces of a road both weigh whole round the cut between them: the nearest one says, or a sample past the cut keeps the height of the piece before it
			if (w < best_w[li] || (w == best_w[li] && n.d >= best_d[li])) continue;
			best_w[li] = w;
			best_d[li] = n.d;
			final_h.set(x, z, lerpf(natural, height, w));
		}
}

} // namespace city
