// Where trees grow: street trees from the
// footways (render/props/street_props.h), a grove in every park or square lot, and wild trees on a jittered grid
// outside roads, lots and water with a noise-driven density. Numbers from data/render/streets.json "trees".
#pragma once

#include "render/props/street_props.h"
#include "render/trees/tree_kit.h"

namespace city {

struct TreeLook {
	float view_m = 0, near_m = 0, grid_m = 1, jitter_m = 0, keep_base = 0, keep_noise = 0, keep_scale = 0, noise_m = 1, noise_bias = 0;
	float min_y = 0, max_grade = 0, road_margin = 0, lot_margin = 0, lot_reach = 0, scale_min = 0, scale_span = 0;
	float park_m2_per_tree = 1, park_edge = 0;
	int park_max = 0, build_per_frame = 1, broadleaf = 0, conifer = 0, shrub = 0;
	float shrub_share = 0, conifer_share = 0;
	std::vector<godot::Color> tints;
	bool load(const godot::Dictionary &trees, godot::PackedStringArray &errors);
};

void place_trees(const CityState &s, const StreetLook &L, const TreeLook &T, const TreeKit &kit, int64_t key,
		std::vector<TreeSpot> &out);

} // namespace city
