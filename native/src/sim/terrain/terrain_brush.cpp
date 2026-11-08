// The Relief brush (Roadcraft world/brush.ts BrushStroke.dab): raise, lower, smooth and level with a soft cosine
// falloff, and the ready-made landforms (mesa, canyon, escarpment, sugarloaf) built from the first dab's height.
// Raise, lower and river reach out unevenly by a noise scaled to the brush, so a stroke has the outline of country and
// not of a compass; `hardness` squeezes their falloff into the outer rim (a cliff instead of a dome); a river carves a
// flat bed under its water with ramped banks that wander along its length.
#include "sim/terrain/heightfield.h"

#include "core/random.h"

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
constexpr float NEIGHBOURS = 4.f;

constexpr float RAISE_LOBE = 0.45f, RIVER_LOBE = 0.6f; // the outline's noise cell, as a share of the radius
constexpr float RAISE_SALT_X = 41.3f, RAISE_SALT_Z = -17.9f, RIVER_SALT_X = 23.9f, RIVER_SALT_Z = 31.4f;
constexpr uint32_t OUTLINE_SEED = 0x6c616e64u;

float smoothstep01(float t) { return smooth01(t); }
} // namespace

float Heightfield::natural_reach(const BrushDab &d, float x, float z, float lobe_share, float salt_x, float salt_z) const {
	float lobe = d.radius * lobe_share, least = float(tune->terrain_rough_reach_min);
	return least + (1.f - least) * (HALF + HALF * vnoise(x / lobe + salt_x, z / lobe + salt_z, OUTLINE_SEED));
}

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
		case BrushMode::RAISE:
		case BrushMode::LOWER: {
			float reach = natural_reach(d, x_of(x), x_of(z), RAISE_LOBE, RAISE_SALT_X, RAISE_SALT_Z);
			if (dist >= reach) return v;
			float unit = 1.f - dist / reach;
			float rim = std::min(1.f, std::max({ tune->terrain_min_hard_rim, 1.f - d.hardness, tune->terrain_min_hard_wall_m / (d.radius * reach) }));
			float g = d.hardness > 0 ? smoothstep01(unit / rim) : HALF + HALF * std::cos((dist / reach) * PI_F);
			return v + (d.mode == BrushMode::RAISE ? 1.f : -1.f) * d.strength * d.dt * g;
		}
		case BrushMode::RIVER: {
			float reach = natural_reach(d, x_of(x), x_of(z), RIVER_LOBE, RIVER_SALT_X, RIVER_SALT_Z);
			if (dist >= reach) return v;
			float bed = river_level(d.h0) - tune->terrain_river_depth_m;
			return std::min(v, v + (bed - v) * smoothstep01((1.f - dist / reach) / float(tune->terrain_river_floor_share)));
		}
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
	if (!changed) return false;
	ground_moved.insert(ground_moved.end(), { x_of(r.x0), x_of(r.z0), x_of(r.x1), x_of(r.z1) }); // the roads there are read again
	recompose(r.x0 - 1, r.z0 - 1, r.x1 + 1, r.z1 + 1);
	return true;
}

void Heightfield::flood(float x, float z, float radius, float level, std::vector<uint32_t> &index, std::vector<float> &before) {
	if (water.empty()) return;
	float shift = water_n * HALF;
	int x0 = clampi(int(std::floor((x - radius) / water_spacing_m + shift)), 0, water_n - 1), x1 = clampi(int(std::ceil((x + radius) / water_spacing_m + shift)), 0, water_n - 1);
	int z0 = clampi(int(std::floor((z - radius) / water_spacing_m + shift)), 0, water_n - 1), z1 = clampi(int(std::ceil((z + radius) / water_spacing_m + shift)), 0, water_n - 1);
	for (int gz = z0; gz <= z1; gz++)
		for (int gx = x0; gx <= x1; gx++) {
			float wx = (gx - shift) * water_spacing_m, wz = (gz - shift) * water_spacing_m;
			size_t i = size_t(gz) * water_n + gx;
			if (std::hypot(wx - x, wz - z) >= radius || water[i] >= level || base_height_at(wx, wz) >= level) continue;
			index.push_back(uint32_t(i));
			before.push_back(water[i]);
			set_water(uint32_t(i), level);
		}
	if (!index.empty()) ground_moved.insert(ground_moved.end(), { x - radius, z - radius, x + radius, z + radius }); // water is a road's floor
}

} // namespace city
