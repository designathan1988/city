// Public transport tunables (Roadcraft 4.0 src/sim/transit/transit.ts, src/world/transit.ts, src/editor/transitTools.ts).
// Values live ONLY in project/data/catalog/transit.json; api/transit_loader.cpp fills this struct and rejects missing keys.
#pragma once

#include <cstdint>
#include <vector>

namespace city {

// Scalar keys of transit.json "params" (X-macro: declared here, required by the loader).
#define CITY_TRANSIT_FIELDS(X) \
	X(bus_dwell_s) X(terminal_dwell_s) X(train_dwell_s) X(train_top_ms) X(metro_top_ms) X(train_accel) X(train_brake) \
	X(train_brake_share) X(train_gap_m) X(headway_pad_m) X(headway_share) X(arrive_m) X(arrive_speed) X(creep_speed) \
	X(approach_m) X(bus_spacing_s) X(at_stop_m) X(bus_stopped_ms) X(train_ahead_m) X(ahead_time_s) X(crossing_stop_m) \
	X(keep_clear_m) X(crossing_warn_m) X(crossing_tail_m) X(train_cars) X(metro_cars) X(car_length_m) X(away_margin_m) \
	X(detour) X(bus_pace) X(min_pace_ms) X(zone_still_ms) X(zone_margin_m) X(zone_ahead_from_m) X(zone_ahead_step_m) \
	X(zone_ahead_r_m) X(zone_car_inset_m) X(zone_car_step_m) X(zone_car_r_m) X(join_m) X(along_road_m) X(road_probe_step_m) \
	X(vehicles_min) X(vehicles_max) X(bus_vehicles) X(rail_vehicles) X(pick_m) X(stop_reach_m) X(station_pick_share) \
	X(metro_way_reach_m) X(track_dedup_m) X(built_pad_m) X(reject_step_m)

#define CITY_TRANSIT_DECLARE(name) float name = 0;

struct TransitParams {
	CITY_TRANSIT_FIELDS(CITY_TRANSIT_DECLARE)
	std::vector<uint32_t> line_colours; // RGBA8, in the order new lines take them
};

} // namespace city
