// A scenic city (data/catalog/scenic.json): a city laid out by hand instead of drawn from a seed. Its land (a plateau with
// ridges, rivers in their channels, lakes), its districts (each a street grid of its own turn and block size; the city's
// square is split between them as the Voronoi cells of their seeds), its named roads, parks and works, and the heat that
// says how dense and how commercial each place is. Pure data: sim/terrain/scenic_land.h raises the land and
// sim/citygen/scenic_*.h lays the streets, zones and parks.
#pragma once

#include "core/polyline.h"

#include <string>
#include <vector>

namespace city {

// A broad rise along a line: `height_m` on it, fading smoothly to nothing `half_width_m` away.
struct ScenicRidge {
	Polyline line;
	float height_m = 0, half_width_m = 0;
};

// A river in its channel, drawn downstream: water at `level_m` at its first point and `level_end_m` at its last; a bed
// `depth_m` under the water `half_width_m` to each side, walls `wall_m` wide up to banks `freeboard_m` over the water, and
// the land easing down to the banks over `valley_m` (the floodplain).
struct ScenicRiver {
	std::string id;
	Polyline line;
	float half_width_m = 0, depth_m = 0, wall_m = 0, freeboard_m = 0, valley_m = 0, level_m = 0, level_end_m = 0;
};

// A lake: its shore, its water `drop_m` under the lowest land of its shore, its bed `depth_m` under the water, shelving
// over `shore_m` on both sides of the shore.
struct ScenicLake {
	Polyline shore;
	float drop_m = 0, depth_m = 0, shore_m = 0;
};

// The plateau at `base_m`, rolling by `relief_m` over cells of `relief_cell_m`, then the ridges, the rivers and the lakes.
struct ScenicLand {
	float base_m = 0, relief_m = 0, relief_cell_m = 1;
	std::vector<ScenicRidge> ridges;
	std::vector<ScenicRiver> rivers;
	std::vector<ScenicLake> lakes;
};

// A district: its seed, the turn of its grid (degrees from +x towards +z), its blocks, its own heat and trade (zoning),
// the profiles of its streets and how many lines apart its collectors run (0: none).
struct ScenicDistrict {
	std::string id;
	V2 seed;
	float turn_deg = 0, block_along_m = 0, block_across_m = 0, heat = 0, trade = 0;
	int local = -1, collector = -1, collector_every = 0;
};

// A road of the plan: its own line, or the course of river `river` moved `offset_m` to the side (positive: the right bank,
// looking downstream); `name` the player reads on its plates (translated from data/i18n; empty: a name from the style's list).
struct ScenicRoad {
	std::string name;
	int profile = -1, river = -1;
	float offset_m = 0;
	Polyline line;
};

// Heat along a line (or round a single point): `heat` (density) and `trade` (commerce) on it, fading to nothing at `reach_m`.
struct ScenicHeat {
	Polyline line;
	float heat = 0, trade = 0, reach_m = 1;
};

// From heat to zone: dense over `high_heat`, medium over `medium_heat`; commercial when the trade is over `commerce_trade`, or
// over `main_trade` beside a main road; value noise of `noise_heat` over cells of `noise_cell_m` breaks the bands.
struct ScenicZoning {
	float high_heat = 0, medium_heat = 0, commerce_trade = 0, main_trade = 0, noise_cell_m = 1, noise_heat = 0;
};

// The trees of the parks: a lattice `spacing_m` apart shaken by `jitter` of it, in groves where the value noise over cells
// of `grove_cell_m` passes `grove_cut`, `low_m` to `high_m` tall, kept `road_clear_m` off a road's edge and off water.
struct ScenicTrees {
	float spacing_m = 1, jitter = 0, grove_cell_m = 1, grove_cut = 0, low_m = 0, high_m = 0, road_clear_m = 0;
};

// How the streets are laid: the seams between districts take profile `seam`; a district's lines run `seam_overshoot_m` past its
// cell so they meet the seam; no district street comes within `highway_clear_m` of a highway's line; a named main road
// bridges water up to `bridge_max_m`; the lines are walked in steps of `march_step_m`.
struct ScenicStreets {
	int seam = -1;
	float seam_overshoot_m = 0, highway_clear_m = 0, bridge_max_m = 0, march_step_m = 1;
};

struct ScenicCity {
	std::string id;
	float half_m = 0; // the laid-out square, round the map's middle
	int biome = 0; // the ecosystem slot of its land (data/catalog/nature.json)
	ScenicLand land;
	ScenicStreets streets;
	std::vector<ScenicDistrict> districts;
	std::vector<ScenicRoad> roads;
	std::vector<Polyline> parks, works;
	std::vector<ScenicHeat> heat;
	ScenicZoning zoning;
	ScenicTrees trees;
	// Whether p lies in one of its parks: the one owner of that test, for the zoning (no zone, no street) and the lots (none stands there).
	bool in_park(V2 p) const {
		for (const Polyline &ring : parks)
			if (ring_holds(ring, p)) return true;
		return false;
	}
};

} // namespace city
