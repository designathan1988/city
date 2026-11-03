// The Relief brush (Roadcraft world/brush.ts BrushStroke.dab): raise, lower, smooth and level with a soft cosine
// falloff, and the ready-made landforms (mesa, canyon, escarpment, sugarloaf, gully) built from the first dab's height.
#include "sim/terrain/heightfield.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float HALF = 0.5f;
// Landform height H = LANDFORM_BASE_M + LANDFORM_PER_STRENGTH_M x strength (0.4 -> 45 m).
constexpr float LANDFORM_BASE_M = 5.f, LANDFORM_PER_STRENGTH_M = 100.f;
constexpr float MESA_EDGE = 0.18f; // share of the radius the mesa's cliff takes
constexpr float ESC_EDGE = 0.08f, ESC_LOWER = 0.55f, ESC_UPPER = 0.45f, ESC_STEP = 0.6f; // two steps of basalt
constexpr float CANYON_EDGE = 0.2f, CANYON_FLOOR_ABOVE_SEA_M = 2.f;
constexpr float SUGARLOAF_TALL = 1.6f, SUGARLOAF_POWER = 0.7f;
constexpr float GULLY_FLOOR_ABOVE_SEA_M = 1.5f, GULLY_DEPTH_SHARE = 0.25f, GULLY_MAX_M = 3.f, GULLY_RATE = 4.f;
constexpr float NEIGHBOURS = 4.f;

float smoothstep01(float t) { return smooth01(t); }
} // namespace

SampleRect Heightfield::brush_rect(float cx, float cz, float radius) const {
	SampleRect r;
	r.x0 = clampi(ix_of(cx - radius), 0, N - 1);
	r.x1 = clampi(ix_of(cx + radius) + 1, 0, N - 1);
	r.z0 = clampi(ix_of(cz - radius), 0, N - 1);
	r.z1 = clampi(ix_of(cz + radius) + 1, 0, N - 1);
	return r;
}

float Heightfield::dab_value(const BrushDab &d, size_t i, float dist, int x, int z) const {
	float v = dequant(base_q[i]);
	float f = HALF + HALF * std::cos(dist * PI_F);
	float k = std::min(1.f, d.strength * d.dt);
	float H = LANDFORM_BASE_M + d.strength * LANDFORM_PER_STRENGTH_M, base = d.h0;
	const float sea = water_level; // the sea (terrain_gen.h OCEAN_Y)
	auto at = [&](int ax, int az) { return dequant(base_q[size_t(clampi(az, 0, N - 1)) * N + clampi(ax, 0, N - 1)]); };
	switch (d.mode) {
		case BrushMode::RAISE: return v + d.strength * d.dt * f;
		case BrushMode::LOWER: return v - d.strength * d.dt * f;
		case BrushMode::FLATTEN: return v + (d.h0 - v) * k * f;
		case BrushMode::SMOOTH: {
			float mean = (at(x - 1, z) + at(x + 1, z) + at(x, z - 1) + at(x, z + 1)) / NEIGHBOURS;
			return v + (mean - v) * k * f;
		}
		case BrushMode::MESA: return std::max(v, base + H * smoothstep01((1 - dist) / MESA_EDGE));
		case BrushMode::ESCARPMENT:
			return std::max(v, base + H * (ESC_LOWER * smoothstep01((1 - dist) / ESC_EDGE) + ESC_UPPER * smoothstep01((ESC_STEP - dist) / ESC_EDGE)));
		case BrushMode::CANYON: {
			float bot = std::max(sea + CANYON_FLOOR_ABOVE_SEA_M, base - H);
			return std::min(v, bot + (base - bot) * (1 - smoothstep01((1 - dist) / CANYON_EDGE)));
		}
		case BrushMode::SUGARLOAF: return std::max(v, base + H * SUGARLOAF_TALL * std::pow(std::max(0.f, 1 - dist * dist), SUGARLOAF_POWER));
		case BrushMode::GULLY:
			return std::max(std::min(v, sea + GULLY_FLOOR_ABOVE_SEA_M), v - std::min(H * GULLY_DEPTH_SHARE, GULLY_MAX_M) * d.dt * GULLY_RATE * (1 - dist) * (1 - dist));
		default: return v;
	}
}

bool Heightfield::brush(const BrushDab &d) {
	SampleRect r = brush_rect(d.x, d.z, d.radius);
	if (r.empty() || d.radius <= 0) return false;
	bool changed = false;
	for (int z = r.z0; z <= r.z1; z++)
		for (int x = r.x0; x <= r.x1; x++) {
			float wx = x_of(x) - d.x, wz = x_of(z) - d.z;
			float dist = std::sqrt(wx * wx + wz * wz) / d.radius;
			if (dist >= 1.f) continue;
			size_t i = size_t(z) * N + x;
			float nh = clampf(dab_value(d, i, dist, x, z), tune->terrain_min_height_m, tune->terrain_max_height_m);
			uint16_t q = uint16_t(clampf(std::round((nh + HEIGHT_Q_OFFSET) * HEIGHT_Q_SCALE), 0.f, QUANT_LIMIT));
			if (q == base_q[i]) continue;
			base_q[i] = q;
			mark_tile(i);
			changed = true;
		}
	if (changed) recompose(r.x0 - 1, r.z0 - 1, r.x1 + 1, r.z1 + 1);
	return changed;
}

} // namespace city
