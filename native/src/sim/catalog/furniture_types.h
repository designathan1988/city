// Street furniture and crosswalk tunables (project/data/catalog/street_furniture.json, every value with its primary
// source there; api/furniture_loader.cpp fills them and rejects missing keys). The rules that place the pieces are
// sim/roads/street_furniture.h; no values here.
#pragma once

#include <cstdint>

namespace city {

// Scalar keys of the blocks (X-macros: declared here, required by the loader).
#define CITY_SIDEWALK_FIELDS(X) X(free_width_m) X(kerb_clear_m)
#define CITY_LAMP_FIELDS(X) X(span_m) X(reach_m)
#define CITY_SIGNAL_FIELDS(X) X(kerb_m)
#define CITY_HYDRANT_FIELDS(X) X(reach_m)
#define CITY_FURNITURE_FIELDS(X) X(corner_clear_m) X(bin_spacing_m)
#define CITY_TREE_FIELDS(X) \
	X(corner_clear_m) X(crossing_clear_m) X(spacing_m) X(pole_clear_m) X(hydrant_clear_m) X(furniture_clear_m) \
	X(kerb_clear_m) X(pit_m)
#define CITY_CROSSING_FIELDS(X) X(zebra_from_m) X(zebra_to_m) X(stop_from_m) X(stop_to_m)
#define CITY_POLE_FIELDS(X) \
	X(height_m) X(kerb_inset_m) X(min_spacing_m) X(max_spacing_m) X(min_gap_m) X(reuse_m) X(corner_tolerance_m) \
	X(line_catch_m) X(footway_catch_m) X(off_line_m) X(station_m) X(crossing_cost_m) X(route_margin_m) \
	X(route_margin_most_m) X(corner_deflection_deg) X(same_arm_sin) X(landing_clear_m) X(clear_search_m) X(corner_reach_m)
#define CITY_BARRIER_FIELDS(X) X(min_run_m) X(footway_reach_m) X(kerb_gap_m) X(back_gap_m)
#define CITY_SIGN_FIELDS(X) \
	X(approach_reach_m) X(speed_past_mouth_m) X(speed_min_length_m) X(stop_past_line_m) X(name_past_mouth_m) X(speed_round_kmh) \
	X(far_share)
#define CITY_ITEM_FIELDS(X) X(gap_m) X(stand_reach_m) X(meadow_apart_m) X(cut_reach_m) X(median_reach_m) X(demolish_reach_m)
#define CITY_FURNITURE_DECLARE(name) float name = 0;

// The kinds of street furniture, in the order of street_furniture.json "footprints".
enum class FurnitureKind : uint8_t { SIGNAL = 0, LAMP, HYDRANT, BIN, BENCH, TREE, COLUMN, POSTBOX, PHONE, DRAIN, SHRUB, POLE, SIGN, COUNT };
constexpr int FURNITURE_KINDS = int(FurnitureKind::COUNT);

// What the player builds (sim/street/street_works.h), in street_furniture.json order.
enum class BarrierKind : uint8_t { FENCE = 0, WALL, HEDGE, GUARDRAIL, RAILING, COUNT };
constexpr int BARRIER_KINDS = int(BarrierKind::COUNT);
enum class SignType : uint8_t { STOP = 0, YIELD, SPEED, NO_PARKING, NO_ENTRY, PEDESTRIAN, SCHOOL, DIRECTION, STREET, INFO, COUNT };
constexpr int SIGN_TYPES = int(SignType::COUNT);

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

// The utility poles the player strings and the run planner's distances.
struct PoleDef {
	CITY_POLE_FIELDS(CITY_FURNITURE_DECLARE)
};
struct BarrierDef {
	CITY_BARRIER_FIELDS(CITY_FURNITURE_DECLARE)
	float height_m[BARRIER_KINDS] = {}, thickness_m[BARRIER_KINDS] = {};
};
struct SignDef {
	CITY_SIGN_FIELDS(CITY_FURNITURE_DECLARE)
};
struct ItemDef {
	CITY_ITEM_FIELDS(CITY_FURNITURE_DECLARE)
};

struct StreetFurnitureDef {
	SidewalkDef sidewalk;
	PoleDef poles;
	BarrierDef barriers;
	SignDef signs;
	ItemDef items;
	LampDef lamps;
	SignalDef signals;
	HydrantDef hydrants;
	FurnitureRules furniture;
	TreeDef trees;
	CrossingDef crossing;
	Footprint footprint[FURNITURE_KINDS];
};

} // namespace city
