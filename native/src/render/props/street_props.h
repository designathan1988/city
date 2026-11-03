// Where street furniture stands (Roadcraft jobs/props.ts): signal masts at signalled junctions, concrete utility
// poles with lamps on the lamp stations the road's night pools follow, wires between consecutive poles, hydrants,
// bins and benches by the furniture level of the segment, and street trees on wide footways. Pure placement: the
// meshes are render/props/prop_kit.h, the trees render/trees.
#pragma once

#include "render/props/prop_kit.h"
#include "render/roads/road_look.h"
#include "sim/state/city_state.h"

#include <vector>

namespace city {

struct WireLook {
	float y[4] = {}, z[4] = {}; // strand heights and offsets across the pole (local z)
	int strands = 0, pieces = 1;
	float thick = 0, sag = 0, min_span = 0, max_span = 0;
	godot::Color color;
};

struct FurnitureLook {
	float min_walk = 0, kerb_u = 0, wall_u = 0, end_clear = 0, lamp_clear = 0;
	float hydrant_first = 0, hydrant_step = 1, hydrant_min_span = 0;
	float bin_first = 0, bin_min_span = 0, bench_min_span = 0, bench_min_walk = 0, bin_after_bench = 0;
	float signal_back = 0, signal_in = 0;
	int signal_min_priority = 0, signal_min_major = 0, signal_degree = 0, signal_cross_degree = 0;
	float tree_first = 0, tree_step = 1, tree_min_walk = 0, tree_kerb_u = 0, tree_clear = 0;
	WireLook wire;
	bool load(const godot::Dictionary &t, godot::PackedStringArray &errors);
};

struct TreeSpot {
	godot::Vector3 p;
	float scale = 1, yaw = 0;
	int species = 0;
};

// Props and wire strands of everything owned by chunk `key` (segments by midpoint, nodes by position).
void place_street_props(const CityState &s, const StreetLook &L, const FurnitureLook &F, int64_t key, std::vector<PropPlace> &out,
		MeshBuilder &wires);
// Street trees on the footways of the segments owned by chunk `key`.
void place_street_trees(const CityState &s, const StreetLook &L, const FurnitureLook &F, int64_t key, std::vector<TreeSpot> &out);

} // namespace city
