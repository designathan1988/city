// The stones of the water (see shore_stones.h).
#include "render/cover/shore_stones.h"

#include "sim/terrain/terrain_gen.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
// The water samples of a square of the map, walked until `stop` says so.
template <typename F>
void each_sample(const Heightfield &t, float x0, float z0, float x1, float z1, F stop) {
	if (t.water_n == 0) return;
	const float half = t.half_size(), step = t.water_spacing_m;
	int i0 = std::max(0, int(std::floor((x0 + half) / step))), i1 = std::min(t.water_n - 1, int(std::ceil((x1 + half) / step)));
	int j0 = std::max(0, int(std::floor((z0 + half) / step))), j1 = std::min(t.water_n - 1, int(std::ceil((z1 + half) / step)));
	for (int j = j0; j <= j1; j++)
		for (int i = i0; i <= i1; i++)
			if (stop(t.water[size_t(j) * t.water_n + i])) return;
}
} // namespace

bool ShoreLook::load(const DataValue &d, PackedStringArray &errors) {
	const int64_t before = errors.size();
	spacing_m = d["spacing_m"].f();
	search_m = d["search_m"].f();
	size_power = d["size_power"].f();
	reach_m = d["reach_m"].f();
	const DataValue size = d["size_m"], rows = d["bands"];
	size.list(2);
	size0 = size[0].f();
	size1 = size[1].f();
	bands.clear();
	for (int i = 0; i < rows.size(); i++) {
		const DataValue row = rows[i];
		row.list(2); // [under_m, odds]
		bands.push_back({ row[0].f(), row[1].f() });
	}
	return errors.size() == before;
}

float ShoreLook::odds(float above_m) const {
	for (const ShoreBand &b : bands)
		if (above_m < b.under_m) return b.odds;
	return 0.f;
}

float shore_level(const Heightfield &t, float x, float z, float search_m) {
	float level = TerrainGen::NO_WATER;
	each_sample(t, x - search_m, z - search_m, x + search_m, z + search_m, [&](float w) {
		level = std::max(level, w);
		return false;
	});
	return level;
}

bool shore_near(const Heightfield &t, float x0, float z0, float side_m, float pad_m) {
	bool any = false;
	each_sample(t, x0 - pad_m, z0 - pad_m, x0 + side_m + pad_m, z0 + side_m + pad_m, [&](float w) {
		any = w > TerrainGen::NO_WATER;
		return any;
	});
	return any;
}

} // namespace city
