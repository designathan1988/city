// Traffic and pedestrian tunables (Roadcraft src/sim/**, spec sim.md §5-§6). Values live ONLY in
// project/data/catalog/traffic.json; api/traffic_loader.cpp fills this struct and rejects missing keys.
#pragma once

#include <cstdint>

namespace city {

// Scalar keys of traffic.json "params" (X-macro: declared here, required by the loader).
#define CITY_TRAFFIC_FIELDS(X) \
	X(density_per_lane_m) X(people_per_lane_m) X(traffic_share) X(people_share) X(fill_share) X(fleet_tau_s) \
	X(spawn_attempts_per_step) X(spawn_fill_attempts) X(route_budget_per_step) X(route_max_expanded) \
	X(trip_min_m) X(trip_max_m) X(trip_decay_m) X(slot_m) X(car_share) X(bus_share) \
	X(turn_cost_right_s) X(turn_cost_left_s) X(turn_cost_uturn_s) X(signal_cost_s) X(edge_extra_s) X(guess_speed_kmh) \
	X(straight_dot) X(control_max_m) X(uturn_push_m) X(conflict_samples) X(conflict_clear_m) \
	X(signal_safety_green_s) X(signal_lost_start_s) X(signal_queue_spacing_m) \
	X(signal_sat_flow_lane_h) X(signal_cycle_max_s) X(signal_reaction_s) X(signal_decel) X(signal_vehicle_m) X(amber_low_kmh) X(amber_mid_kmh) \
	X(amber_min_s) X(amber_min_mid_s) X(amber_min_high_s) X(amber_max_s) X(signal_ped_red_s) X(ped_reaction_s) \
	X(ped_clear_speed) X(ped_all_red_s) X(ped_safety_green_s) X(stage_opposite_dot) \
	X(through_pair_dot) X(yield_look_s) X(yield_near_m) X(claim_after_s) X(exit_room_pad_m) X(stop_margin_m) \
	X(look_ahead_m) X(turn_speed_kmh) X(jerk_up) X(speed_drift) X(stuck_despawn_s) \
	X(lc_safe_brake) X(lc_threshold) X(lc_bias_right) X(lc_mandatory_bias) X(lc_free_m) X(lc_gap_front_m) \
	X(lc_gap_back_m) X(mouth_clear_m) X(lc_duration_s) X(lc_cooldown_s) X(lc_every_ticks) \
	X(brake_light_accel) X(brake_light_speed) X(body_lift_m) \
	X(walk_speed_min) X(walk_speed_max) X(walk_track_min) X(walk_track_max) X(ped_trip_min_m) X(ped_trip_max_m) \
	X(curb_m) X(ped_spawn_per_step) X(ped_fill_per_step) \
	X(day_base) X(day_morning_h) X(day_morning_w) X(day_morning_amp) X(day_evening_h) X(day_evening_w) \
	X(day_evening_amp) X(day_midday_h) X(day_midday_w) X(day_midday_amp) X(day_night_cut) X(day_night_from_h) \
	X(day_night_to_h) X(morning_from_h) X(morning_to_h) X(evening_from_h) X(evening_to_h) \
	X(jam_seconds) X(release_every_s) X(starve_seconds) X(detour_after_s) X(reconsider_after_s) X(reconsider_full_share) \
	X(rejoin_ds) X(detour_max_expanded) X(bpr_gain) X(tail_credit_s) X(tail_credit_cap_ms) X(short_exit_m) \
	X(tail_moving_ms) X(commit_min_m) X(at_line_m) X(gov_every_s) X(gov_min_fleet) X(gov_jammed) X(gov_lo) X(gov_hi) \
	X(gov_floor) X(gov_rate) X(reroute_budget_per_step) \
	X(ped_route_max_expanded) X(kerb_stand_m) X(ped_courtesy_s) \
	X(ped_critical_gap_s) X(ped_span_reach_m) X(walk_cost_extra_s) X(ped_routes_per_step) \
	X(ped_stripe_m) X(ped_body_m) X(ped_margin_m) X(ped_look_m) X(ped_gap_m) X(ped_headway_s) X(ped_lateral_share) \
	X(ped_lateral_cost) X(ped_keep_right_cost) X(ped_stripe_gain_m) X(ped_stripe_hold_s) X(ped_accel) \
	X(kerb_wait_half_m) X(kerb_wait_gap_m) X(kerb_wait_rows) \
	X(ped_jam_s) X(ped_jam_share) X(ped_reserve_share) X(ped_reserve_max_m) X(ped_moving_ms) \
	X(claim_hold_s) X(claim_brake) X(publish_radius_m) \
	X(meso_alpha) X(meso_crowd) X(meso_s0_m) X(meso_head_s) X(meso_dwell_s) \
	X(meso_window_m) X(meso_window_out_m) X(micro_spawn_m) X(ped_window_m) X(ped_despawn_window_share)

#define CITY_TRAFFIC_DECLARE(name) float name = 0;

// Vehicle classes (spec sim.md §5.1): car styles 0-5, then bus and truck.
enum class VehicleType : uint8_t { SEDAN = 0, HATCH, SUV, VAN, PICKUP, MOTO, BUS, TRUCK, COUNT };
constexpr int VEHICLE_TYPES = int(VehicleType::COUNT);
constexpr int CAR_STYLES = 6;
constexpr int BASE_ZONES = 5; // none, residential, commercial, industrial, mixed
constexpr int EXTENSION_LANES = 5; // traffic.json signal_extension_s: approaches of 1 to 5 lanes
constexpr int PAINTS = 16; // Roadcraft shared/vehiclePaints.ts

// Driver archetype of a class (drivers.ts ARCH) plus the body length and half width the simulation uses.
struct VehicleClass {
	float a = 0, b = 0, b_emergency = 0, headway = 0, min_gap = 0, speed_share = 0, length = 0, half_width = 0;
};

struct TrafficParams {
	CITY_TRAFFIC_FIELDS(CITY_TRAFFIC_DECLARE)
	VehicleClass classes[VEHICLE_TYPES];
	float signal_extension_s[EXTENSION_LANES] = {};
	float style_share[CAR_STYLES] = {};
	float zone_home[BASE_ZONES] = {}, zone_work[BASE_ZONES] = {}, zone_mid[BASE_ZONES] = {}, zone_truck[BASE_ZONES] = {};
	float zone_walk[BASE_ZONES] = {};
};

} // namespace city
