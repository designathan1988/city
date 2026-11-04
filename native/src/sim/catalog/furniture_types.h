// Street furniture and crosswalk tunables (project/data/catalog/street_furniture.json, every value with its primary
// source there; api/furniture_loader.cpp fills them and rejects missing keys). The rules that place the pieces are
// sim/roads/street_furniture.h; no values here.
#pragma once

#include <cstdint>

namespace city {

// Scalar keys of the blocks (X-macros: declared here, required by the loader).
#define CITY_SIDEWALK_FIELDS(X) X(free_width_m) X(kerb_clear_m)
#define CITY_LAMP_FIELDS(X) X(span_m) X(mount_height_m) X(unilateral_ratio) X(reach_m)
#define CITY_SIGNAL_FIELDS(X) X(kerb_m)
#define CITY_HYDRANT_FIELDS(X) X(reach_m)
#define CITY_FURNITURE_FIELDS(X) X(corner_clear_m) X(bin_spacing_m)
#define CITY_TREE_FIELDS(X) \
	X(corner_clear_m) X(crossing_clear_m) X(spacing_m) X(pole_clear_m) X(hydrant_clear_m) X(furniture_clear_m) \
	X(kerb_clear_m) X(pit_m)
#define CITY_CROSSING_FIELDS(X) X(zebra_from_m) X(zebra_to_m) X(stop_from_m) X(stop_to_m)
#define CITY_FURNITURE_DECLARE(name) float name = 0;

// The kinds of street furniture, in the order of street_furniture.json "footprints".
enum class FurnitureKind : uint8_t { SIGNAL = 0, LAMP, HYDRANT, BIN, BENCH, TREE, COUNT };
constexpr int FURNITURE_KINDS = int(FurnitureKind::COUNT);

// The sidewalk's own zones: the free width kept for walking, and how far from the kerb's face a piece's face stands.
struct SidewalkDef {
	CITY_SIDEWALK_FIELDS(CITY_FURNITURE_DECLARE)
};
struct LampDef {
	CITY_LAMP_FIELDS(CITY_FURNITURE_DECLARE)
};
struct SignalDef {
	CITY_SIGNAL_FIELDS(CITY_FURNITURE_DECLARE)
};
struct HydrantDef {
	CITY_HYDRANT_FIELDS(CITY_FURNITURE_DECLARE)
};
struct FurnitureRules { // bins and benches
	CITY_FURNITURE_FIELDS(CITY_FURNITURE_DECLARE)
};
struct TreeDef {
	CITY_TREE_FIELDS(CITY_FURNITURE_DECLARE)
};

// A crosswalk at a junction mouth, metres from the mouth: the zebra and the stop line, and what follows from them.
struct CrossingDef {
	CITY_CROSSING_FIELDS(CITY_FURNITURE_DECLARE)
	float middle() const { return (zebra_from_m + zebra_to_m) * 0.5f; } // the line the walkers cross along
	float half() const { return (zebra_to_m - zebra_from_m) * 0.5f; } // half the zebra's width along the kerb
	float stop_back() const { return stop_to_m; } // the vehicles stop with their front at the stop line's near edge
	float cleared() const { return zebra_to_m; } // a leaving vehicle's rear past the zebra's edge is off it
};

// Ground a kind covers at walking height, in its own frame (x along the sidewalk, z toward the road or the traffic).
struct Footprint {
	float x_m = 0, z_m = 0;
};

struct StreetFurnitureDef {
	SidewalkDef sidewalk;
	LampDef lamps;
	SignalDef signals;
	HydrantDef hydrants;
	FurnitureRules furniture;
	TreeDef trees;
	CrossingDef crossing;
	Footprint footprint[FURNITURE_KINDS];
};

} // namespace city
