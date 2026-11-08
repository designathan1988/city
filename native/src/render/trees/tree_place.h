// Where trees grow (data/render/streets.json "trees.placement"): street trees where the simulation stands them (read through render/props/
// street_props.h), groves in the park and square lots, and the woods of the open land. The woods follow a density: patches of
// smooth noise at three scales, raised on steep ground, by the ecosystem and by painted forest. It cuts the land into
// bands (inner wood, its edge, the open land with its scattered and lone trees), each with its chance per cell of a lattice
// and its sizes; one scale over all the chances holds the land to `woods_per_ha`. A tree among many stands taller and narrower,
// one at a wood's edge leans to the open side. Nothing grows on roads, lots, water or cliffs. Near the camera every tree is
// its model; far away the same trees are one card each.
#pragma once

#include "render/data_reader.h"
#include "render/props/street_props.h"
#include "render/trees/tree_kit.h"

#include <array>
#include <atomic>
#include <vector>

namespace city {

enum class WoodBand : int { INNER = 0, EDGE = 1, OPEN = 2, COUNT = 3 };
constexpr int WOOD_BANDS = int(WoodBand::COUNT);

// What a chunk's plants keep clear of and read: its roads and lots, with a margin, and its ecosystem (none on a map with neither a
// biome nor paint). The roads and lots are filed by cell of a grid over the chunk and its margin, each in every cell its reach can
// touch, so near_road and in_lot test only those that can answer: the same answer as testing them all. Only read once made, so the
// jobs of several threads may read one (ground_kept.h).
struct ChunkGround {
	std::vector<int> segs, lots;
	EcologyField eco;
	struct Grid {
		double x0 = 0, z0 = 0;
		int n = 0, count = 0; // cells a side; indices filed
		std::vector<std::vector<int>> cells; // indices into segs (or lots)
		void file(int index, double minx, double minz, double maxx, double maxz);
		template <typename F>
		bool any(double x, double z, double reach, F &&test) const; // test(index) on each filed index the square x, z +- reach meets
	} seg_grid, lot_grid;
	size_t bytes() const; // what it holds, for the memory it is kept by
};
ChunkGround chunk_ground(const CityState &s, int64_t key);
bool near_road(const CityState &s, const ChunkGround &l, float x, float z, float r);
bool in_lot(const CityState &s, const ChunkGround &l, float margin, float x, float z);

struct SpeciesMix {
	std::vector<int> species;
	std::vector<float> weight; // cumulative, ending at 1
	int pick(float r) const;
	bool load(DataReader r, const godot::Dictionary &mix, const TreeKit &kit); // name -> weight; an unknown name is an error on `r`
};

struct BandLook {
	float from = 0; // density above which the land is of this band
	float keep0 = 0, keep1 = 0; // chance a cell holds a tree, at the band's lower density and at the next band's
	float lone = 0; // the open band's chance below `from`
	float h0 = 0, h1 = 0; // size range, metres
	SpeciesMix mix;
};

constexpr int WOOD_PATCHES = 3;
constexpr int FLOWERING_KINDS = 3;

struct TreeLook {
	float near_m = 0, fade_m = 0, far_m = 0, grid_m = 1, jitter = 0, plate_m = 1;
	std::array<float, WOOD_PATCHES> patch_m{}, patch_weight{};
	float mass_from = 0, mass_span = 1, mass_gain = 0, density_floor = 0;
	float slope_from = 0, slope_to = 1, slope_gain = 0, slope_probe_m = 1;
	float woods_per_ha = 0, crowd_reach_m = 0, crowd_full = 1;
	float crowd_tall = 0, crowd_narrow = 0, edge_lean = 0, lean_rise = 0, width_vary = 0, tint_vary = 0;
	float open_tall = 1, open_wide = 1; // a tree standing alone against its size
	float min_y = 0, max_grade = 0, road_margin = 0, lot_margin = 0;
	float park_m2_per_tree = 1, park_edge = 0, park_h0 = 0, park_h1 = 0, street_height_m = 1;
	float eco_gain = 0, eco_mean = 0, forest_gain = 0; // how the ecosystem and painted forest raise the density
	std::vector<int> planted_species; // the species of each model slot of a planted tree (sim/nature/planted_trees.h)
	int shrub_species = 0, hedge_species = 0; // the gardens' plants that are no tree (garden_place.h)
	std::array<int, FLOWERING_KINDS> flowering_species{}, bed_species{}; // a flowering bush and a flower bed, in each tone
	int park_max = 0, far_shadows = 0;
	std::array<BandLook, WOOD_BANDS> band;
	SpeciesMix park, street;
	std::atomic<float> share{ 1.f }; // the graphics tier's share of the woods and lone trees; jobs read it
	float scale = 1, most = 1; // worked out by load(): the chances' scale and the highest chance
	float over_planned = 1; // the land's budget of trees over what its plan would grow
	float tier_share(float tier) const; // what is left of the plan on a tier of that share of the budget
	float density_of(double x, double y) const; // the patches' share of the density, on the map's own axes
	WoodBand band_at(float density) const;
	float keep_at(float density, WoodBand b) const; // the chance of a cell, scaled
	bool load(const godot::Dictionary &trees, const TreeKit &kit, godot::PackedStringArray &errors);
};

// One tree as drawn near: its foot, height and width (metres), turn, lean (shear of its up axis per unit of height),
// species and tint.
struct TreeInstance {
	godot::Vector3 p;
	float height = 1, width = 1, yaw = 0, lean_x = 0, lean_z = 0;
	int species = 0;
	godot::Color tint;
	float depth = 0; // across the model's x, when it differs from `width` (a hedge's clipped piece)
};

// One far card: a tree.
struct FarCard {
	godot::Vector3 p;
	float height = 1, width = 1;
	godot::Color tone; // linear albedo multiplier (the species' crown tone times the tint)
	int species = 0;
};

// The trees of 256 m chunk `key` on its ground `l` (chunk_ground), near models.
void place_trees(const CityState &s, const StreetLook &L, const TreeLook &T, const TreeKit &kit, const ChunkGround &l, int64_t key,
		std::vector<TreeInstance> &out);
// The far cards of chunk `key` on its ground `l`: the same trees, one card each.
void place_far(const CityState &s, const StreetLook &L, const TreeLook &T, const TreeKit &kit, const ChunkGround &l, int64_t key,
		std::vector<FarCard> &out);

} // namespace city
