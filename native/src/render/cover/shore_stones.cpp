// The stones of the water (see shore_stones.h).
#include "render/cover/shore_stones.h"

#include "render/mesh_builder.h"
#include "sim/terrain/terrain_gen.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
// The water samples of a square of the map, each with the level of the lake or river on it and whether the sea stands
// there, walked until `stop` says so.
template <typename F>
void each_sample(const Heightfield &t, float x0, float z0, float x1, float z1, F stop) {
	if (t.water_n == 0) return;
	const float half = t.half_size(), step = t.water_spacing_m;
	int i0 = std::max(0, int(std::floor((x0 + half) / step))), i1 = std::min(t.water_n - 1, int(std::ceil((x1 + half) / step)));
	int j0 = std::max(0, int(std::floor((z0 + half) / step))), j1 = std::min(t.water_n - 1, int(std::ceil((z1 + half) / step)));
	for (int j = j0; j <= j1; j++)
		for (int i = i0; i <= i1; i++) {
			const float wx = float(i) * step - half, wz = float(j) * step - half;
			const bool sea = t.in_bounds(wx, wz) && t.height_at(wx, wz) < t.water_level;
			if (stop(t.water[size_t(j) * t.water_n + i], sea)) return;
		}
}
} // namespace

void ShoreKind::load(const DataValue &d) {
	size_power = d["size_power"].f();
	const DataValue size = d["size_m"], rows = d["bands"];
	size.list(2);
	size0 = size[0].f();
	size1 = size[1].f();
	tint = rgba8_to_color(d["tint"].colour());
	bands.clear();
	for (int i = 0; i < rows.size(); i++) {
		const DataValue row = rows[i];
		row.list(2); // [under_m, odds]
		bands.push_back({ row[0].f(), row[1].f() });
	}
}

float ShoreKind::odds(float above_m) const {
	for (const ShoreBand &b : bands)
		if (above_m < b.under_m) return b.odds;
	return 0.f;
}

bool ShoreLook::load(const DataValue &d, PackedStringArray &errors) {
	const int64_t before = errors.size();
	spacing_m = d["spacing_m"].f();
	search_m = d["search_m"].f();
	reach_m = d["reach_m"].f();
	banks.load(d);
	coast.load(d["coast"]);
	return errors.size() == before;
}

ShoreWater shore_water(const Heightfield &t, float x, float z, float search_m) {
	ShoreWater out;
	float lake = TerrainGen::NO_WATER;
	bool sea = false;
	each_sample(t, x - search_m, z - search_m, x + search_m, z + search_m, [&](float w, bool under_sea) {
		lake = std::max(lake, w);
		sea = sea || under_sea;
		return false;
	});
	out.sea = sea && t.water_level >= lake; // the higher water is the one the bank stands by
	out.level = out.sea ? t.water_level : lake;
	out.wet = out.sea || lake > TerrainGen::NO_WATER;
	return out;
}

bool shore_near(const Heightfield &t, float x0, float z0, float side_m, float pad_m) {
	bool any = false;
	each_sample(t, x0 - pad_m, z0 - pad_m, x0 + side_m + pad_m, z0 + side_m + pad_m, [&](float w, bool sea) {
		any = sea || w > TerrainGen::NO_WATER;
		return any;
	});
	return any;
}

} // namespace city
