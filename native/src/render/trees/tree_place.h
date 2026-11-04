// Where trees grow (data/render/streets.json "trees.placement"): street trees where the simulation stands them (read through render/props/
// street_props.h), groves in the park and square lots, and the woods of the open land. The woods follow a field of
// patches (fractal value noise) whose thresholds are set from the land share the data asks to be inner wood and wood
// edge (measured cover), raised on steep slopes and along water (the Forest Code's permanent protection areas); inside
// a wood the trees stand denser, taller and narrower, at its edge lower and leaning to the open side, out in the
// open few and short. Nothing grows on roads, lots, water or cliffs. Near the camera every tree is its model; far
// away the woods are drawn as one card per cell of `far_cell_m` and the lone trees as one card each.
#pragma once

#include "render/props/street_props.h"
#include "render/trees/tree_kit.h"

#include <array>
#include <vector>

namespace city {

enum class WoodBand : int { INNER = 0, EDGE = 1, OPEN = 2, COUNT = 3 };
constexpr int WOOD_BANDS = int(WoodBand::COUNT);

// What a chunk's plants keep clear of and read, gathered once: its roads and lots, with a margin, and its ecosystem (none
// on a map with neither a biome nor paint).
struct ChunkGround {
	std::vector<int> segs, lots;
	EcologyField eco;
};
ChunkGround chunk_ground(const CityState &s, int64_t key);
bool near_road(const CityState &s, const ChunkGround &l, float x, float z, float r);
bool in_lot(const CityState &s, const ChunkGround &l, float margin, float x, float z);

struct SpeciesMix {
	std::vector<int> species;
	std::vector<float> weight; // cumulative, ending at 1
	int pick(float r) const;
	bool load(const godot::Dictionary &mix, const TreeKit &kit, const godot::String &where, godot::PackedStringArray &errors);
};

struct BandLook {
	float cover = 0; // share of the ground under crowns
	float h0 = 0, h1 = 0; // height range, metres
	float keep = 0; // chance a candidate cell holds a tree (from the cover, the grid and the mean crown)
	SpeciesMix mix;
};

struct TreeLook {
	float near_m = 0, fade_m = 0, pop_m = 0, far_m = 0, far_cell_m = 1, far_spread = 1, far_jitter = 0, grid_m = 1, jitter = 0, mass_m = 1;
	int octaves = 1;
	float inner_share = 0, edge_share = 0, open_per_ha = 0;
	float slope_from = 0, slope_to = 1, slope_gain = 0, water_m = 0, water_gain = 0;
	float crowd_tall = 0, crowd_narrow = 0, edge_lean = 0, width_vary = 0, tint_vary = 0;
	float min_y = 0, max_grade = 0, road_margin = 0, lot_margin = 0;
	float park_m2_per_tree = 1, park_edge = 0, park_h0 = 0, park_h1 = 0, street_height_m = 1;
	float eco_gain = 0, eco_mean = 0, forest_gain = 0; // how the ecosystem and painted forest raise the woods' field
	std::vector<int> planted_species; // the species of each model slot of a planted tree (sim/nature/planted_trees.h)
	int park_max = 0, far_shadows = 0;
	std::array<BandLook, WOOD_BANDS> band;
	SpeciesMix park, street;
	float t_inner = 0, t_edge = 0; // field thresholds, calibrated by load()
	bool load(const godot::Dictionary &trees, const TreeKit &kit, godot::PackedStringArray &errors);
};

// One tree as drawn near: its foot, height and width (metres), turn, lean (shear of its up axis per unit of height),
// species and tint.
struct TreeInstance {
	godot::Vector3 p;
	float height = 1, width = 1, yaw = 0, lean_x = 0, lean_z = 0;
	int species = 0;
	godot::Color tint;
};

// One far card: a lone tree, or the trees of a cell of a wood.
struct FarCard {
	godot::Vector3 p;
	float height = 1, width = 1;
	godot::Color tone; // linear albedo multiplier (the species' crown tone times the tint)
	int species = 0;
};

// The trees of 256 m chunk `key`, near models.
void place_trees(const CityState &s, const StreetLook &L, const TreeLook &T, const TreeKit &kit, int64_t key,
		std::vector<TreeInstance> &out);
// The far cards of chunk `key`: its woods by cell, its lone, park and street trees one by one.
void place_far(const CityState &s, const StreetLook &L, const TreeLook &T, const TreeKit &kit, int64_t key,
		std::vector<FarCard> &out);

} // namespace city
