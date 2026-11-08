// The stones of the water's edge (data/render/cover.json shore): a lattice round the water, each stone kept by its height over
// the level of the water beside it, thickest on the line and thinning up the bank and into the deep. The banks of a river or
// a lake and the sea's coast are two looks, each with its sizes, its bands and its tint. Read by the cover's jobs.
#pragma once

#include "render/data_reader.h"
#include "sim/terrain/heightfield.h"

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <vector>

namespace city {

struct ShoreBand {
	float under_m = 0, odds = 0; // the odds of a stone lower than this over the water, the bands from the deep up
};

// One edge's stones: how big, how likely at each height over the water, and the tint over the rocks' own tones.
struct ShoreKind {
	float size0 = 0, size1 = 0, size_power = 1;
	godot::Color tint = godot::Color(1, 1, 1, 1); // a linear multiplier of the rock's colour
	std::vector<ShoreBand> bands;
	void load(const DataValue &table);
	float odds(float above_m) const;
};

struct ShoreLook {
	float spacing_m = 1, search_m = 0, reach_m = 0;
	ShoreKind banks, coast; // a river's or a lake's, the sea's
	bool load(const DataValue &table, godot::PackedStringArray &errors); // cover.json shore, through the strict reader's values
};

// The water within `search_m` of a point: the highest level there, and whether it is the sea's (a water sample whose ground
// lies under the sea, the rule render/water/shore_field.cpp lays the beach by).
struct ShoreWater {
	float level = 0;
	bool wet = false, sea = false;
};
ShoreWater shore_water(const Heightfield &t, float x, float z, float search_m);
// Whether any water stands on a square of the map, `pad_m` round it: a lake's, a river's or the sea's.
bool shore_near(const Heightfield &t, float x0, float z0, float side_m, float pad_m);

} // namespace city
