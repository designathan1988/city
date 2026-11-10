// The stones of a river, a lake or the sea's coast (data/render/cover.json shore): a lattice round the water, each stone kept by its height over
// the level of the water beside it, thickest on the line and thinning up the bank and into the deep. Read by the cover's jobs.
#pragma once

#include "render/data_reader.h"
#include "sim/terrain/ground_patch.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <vector>

namespace city {

struct ShoreBand {
	float under_m = 0, odds = 0; // the odds of a stone lower than this over the water, the bands from the deep up
};

struct ShoreLook {
	float spacing_m = 1, search_m = 0, size0 = 0, size1 = 0, size_power = 1, reach_m = 0;
	std::vector<ShoreBand> bands;
	bool load(const DataValue &table, godot::PackedStringArray &errors); // cover.json shore, through the strict reader's values
	float odds(float above_m) const;
};

// The level of the water within `search_m` of a point (a lake, a river or the sea), or TerrainGen::NO_WATER.
float shore_level(const GroundRead &t, float x, float z, float search_m);
// Whether any water stands on a square of the map, `pad_m` round it.
bool shore_near(const GroundRead &t, float x0, float z0, float side_m, float pad_m);

} // namespace city
