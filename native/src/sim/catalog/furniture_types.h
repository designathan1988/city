// Street furniture and crosswalk tunables (project/data/catalog/street_furniture.json; api/furniture_loader.cpp fills
// them and rejects missing keys). The rules that place the pieces are sim/roads/street_furniture.h; no values here.
#pragma once

#include <cstdint>

namespace city {

// Scalar keys of the "lamps", "furniture" and "crossing" blocks (X-macros: declared here, required by the loader).
#define CITY_LAMP_FIELDS(X) X(first_m) X(step_m) X(end_clear_m) X(inset_m) X(reach_m) X(median_min_m)
#define CITY_FURNITURE_FIELDS(X) \
	X(min_walk_m) X(kerb_u_m) X(wall_u_m) X(end_clear_m) X(lamp_clear_m) X(hydrant_first_m) X(hydrant_step_m) \
	X(hydrant_min_span_m) X(bin_first_m) X(bin_min_span_m) X(bench_min_span_m) X(bench_min_walk_m) X(bin_after_bench_m) \
	X(signal_back_m) X(signal_in_m) X(signal_max_share) X(tree_first_m) X(tree_step_m) X(tree_min_walk_m) X(tree_kerb_u_m) \
	X(tree_clear_m)
#define CITY_CROSSING_FIELDS(X) X(zebra_from_m) X(zebra_to_m) X(stop_from_m) X(stop_to_m) X(stop_clear_m) X(zebra_clear_m)
#define CITY_FURNITURE_DECLARE(name) float name = 0;

// The kinds of street furniture, in the order of street_furniture.json "footprints".
enum class FurnitureKind : uint8_t { SIGNAL = 0, LAMP, HYDRANT, BIN, BENCH, TREE, COUNT };
constexpr int FURNITURE_KINDS = int(FurnitureKind::COUNT);

struct LampDef {
	CITY_LAMP_FIELDS(CITY_FURNITURE_DECLARE)
};

struct FurnitureRules {
	CITY_FURNITURE_FIELDS(CITY_FURNITURE_DECLARE)
};

// A crosswalk at a junction mouth, metres from the mouth: the zebra, the stop line and what follows from them.
struct CrossingDef {
	CITY_CROSSING_FIELDS(CITY_FURNITURE_DECLARE)
	float middle() const { return (zebra_from_m + zebra_to_m) * 0.5f; } // the line the walkers cross along
	float half() const { return (zebra_to_m - zebra_from_m) * 0.5f; } // half the zebra's width along the kerb
	float stop_back() const { return stop_to_m + stop_clear_m; } // where the vehicles stop, behind the line
	float cleared() const { return zebra_to_m + zebra_clear_m; } // a leaving vehicle's rear past this is off it
};

// Ground a kind covers at walking height, in its own frame (x along the sidewalk, z toward the road or the traffic).
struct Footprint {
	float x_m = 0, z_m = 0;
};

struct StreetFurnitureDef {
	LampDef lamps;
	FurnitureRules furniture;
	CrossingDef crossing;
	Footprint footprint[FURNITURE_KINDS];
};

} // namespace city
