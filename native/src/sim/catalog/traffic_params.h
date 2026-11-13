// Traffic and pedestrian tunables (Roadcraft src/sim/**, spec sim.md §5-§6). Values live ONLY in
// project/data/catalog/traffic.json; api/traffic_loader.cpp fills this struct and rejects missing keys.
#pragma once

#include <cstdint>
#include <vector>

namespace city {

// Scalar keys of traffic.json "params" (X-macro: declared here, required by the loader).
#define CITY_TRAFFIC_FIELDS(X) \
	X(density_per_lane_m) X(traffic_share) X(people_share) X(fill_share) X(fleet_tau_s) \
	X(spawn_attempts_per_step) X(spawn_fill_attempts) X(route_budget_per_step) X(route_max_expanded) \
	X(trip_min_m) X(trip_max_m) X(trip_decay_m) X(slot_m) X(car_share) X(bus_share) \
	X(turn_cost_right_s) X(turn_cost_left_s) X(turn_cost_uturn_s) X(signal_cost_s) X(edge_extra_s) X(guess_speed_kmh) \
	X(straight_dot) X(control_max_m) X(uturn_push_m) \
	X(body_small_length_m) X(body_small_width_m) X(body_car_length_m) X(body_car_width_m) X(body_heavy_length_m) X(body_heavy_width_m) \
	X(sweep_step_m) X(sweep_margin_m) X(sweep_queue_back_m) \
	X(signal_safety_green_s) X(signal_lost_start_s) X(signal_queue_spacing_m) \
	X(signal_sat_flow_lane_h) X(signal_cycle_max_s) X(signal_reaction_s) X(signal_decel) X(signal_vehicle_m) X(amber_low_kmh) X(amber_mid_kmh) \
	X(amber_min_s) X(amber_min_mid_s) X(amber_min_high_s) X(amber_max_s) X(signal_ped_red_s) X(ped_reaction_s) \
	X(ped_clear_speed) X(ped_all_red_s) X(ped_safety_green_s) X(stage_opposite_dot) \
	X(through_pair_dot) X(signal_arms) X(signal_lanes) X(yield_look_s) X(yield_near_m) X(stop_sign_ms) X(claim_after_s) X(exit_room_pad_m) X(stop_margin_m) \
	X(look_ahead_m) X(turn_speed_kmh) X(jerk_up) X(speed_drift) X(stuck_despawn_s) \
	X(lc_safe_brake) X(lc_threshold) X(lc_bias_right) X(lc_mandatory_bias) X(lc_free_m) X(lc_gap_front_m) \
	X(lc_gap_back_m) X(mouth_clear_m) X(lc_duration_s) X(lc_cooldown_s) X(lc_every_ticks) \
	X(brake_light_accel) X(brake_light_speed) X(body_lift_m) X(heading_chord_m) \
	X(walk_speed_min) X(walk_speed_max) X(walk_track_min) X(walk_track_max) X(ped_trip_min_m) X(ped_trip_max_m) \
	X(ped_child_share) X(ped_elder_share) X(ped_child_pace) X(ped_elder_pace) X(ped_rest_fade_s) \
	X(bus_passenger_s) X(bus_step_aboard_s) X(bus_fetch_m) X(bus_fetch_share) X(bus_front_door_m) X(ped_shot_fright_s) X(ped_shock_life_s) X(ped_body_shock_m) X(ped_blast_shock_m) X(ped_shock_look_s) X(ped_shocks_most) X(ped_react_near_m) X(ped_react_crouch_roll) X(ped_react_film_roll) X(ped_react_trip_roll) X(ped_react_crouch_s) X(ped_react_film_s) \
	X(ped_react_glance_s) X(ped_react_look_s) X(ped_trip_after_s) X(ped_trip_s) X(ped_stampede_trip_roll) X(ped_stampede_trip_m) X(ped_stampede_trip_span_s) \
	X(ped_stampede_crouch_roll) X(ped_stampede_crouch_s) X(ped_stampede_fall_roll) X(ped_stampede_fall_m) X(ped_stampede_fall_s) X(ped_stampede_fall_after_s) \
	X(ped_stampede_film_roll) X(ped_stampede_film_m) X(ped_mourn_m) X(ped_mourn_s) X(ped_turn_rad_s) \
	X(lorry_kerb_over_m) X(ped_crossing_share) X(ped_act_fade_s) X(ped_look_after_s) X(ped_look_hold_s) X(ped_phone_after_s) X(ped_hands_after_s) X(ped_talk_after_s) X(ped_talk_turn_s) \
	X(ped_talk_stagger_s) \
	X(ped_company_share) X(ped_company_child_share) X(ped_company_catch) X(ped_company_slow) X(ped_company_reach_m) X(ped_company_level_m) X(ped_company_wait_m) X(ped_door_off_m) X(ped_gate_out_m) \
	X(ped_door_reach_m) X(ped_door_budget) X(work_stay_h) X(errand_stay_h) \
	X(curb_m) X(ped_spawn_per_step) X(ped_fill_per_step) \
	X(day_base) X(day_morning_h) X(day_morning_w) X(day_morning_amp) X(day_evening_h) X(day_evening_w) \
	X(day_evening_amp) X(day_midday_h) X(day_midday_w) X(day_midday_amp) X(day_night_cut) X(day_night_from_h) \
	X(day_night_to_h) X(morning_from_h) X(morning_to_h) X(evening_from_h) X(evening_to_h) \
	X(jam_seconds) X(release_every_s) X(starve_seconds) X(detour_after_s) X(reconsider_after_s) X(reconsider_full_share) \
	X(rejoin_ds) X(detour_max_expanded) X(bpr_gain) X(tail_credit_s) X(tail_credit_cap_ms) X(short_exit_m) \
	X(tail_moving_ms) X(commit_min_m) X(at_line_m) X(gov_every_s) X(gov_min_fleet) X(gov_jammed) X(gov_lo) X(gov_hi) \
	X(gov_floor) X(gov_rate) \
	X(ped_route_max_expanded) X(kerb_stand_m) X(ped_courtesy_s) \
	X(ped_critical_gap_s) X(ped_span_reach_m) X(walk_cost_extra_s) X(ped_routes_per_step) \
	X(ped_stripe_m) X(ped_body_m) X(ped_margin_m) X(ped_look_m) X(ped_gap_m) X(ped_headway_s) X(ped_lateral_share) \
	X(ped_lateral_cost) X(ped_keep_right_cost) X(ped_stripe_gain_m) X(ped_stripe_hold_s) X(ped_accel) \
	X(kerb_wait_half_m) \
	X(ped_jam_s) X(ped_jam_crossing_s) X(ped_jam_narrow_s) X(ped_jam_share) X(ped_reserve_share) X(ped_reserve_max_m) X(ped_moving_ms) \
	X(claim_hold_s) X(claim_brake) X(publish_radius_m) \
	X(kerb_bay_pitch_m) X(kerb_corner_clear_m) X(kerb_crossing_clear_m) X(kerb_dead_end_m) X(kerb_gate_margin_m) X(kerb_gate_reach_m) \
	X(kerb_parked_share) X(park_window_m) X(park_window_out_m) X(park_window_step_m) X(park_reads_per_step) X(park_kerb_reads) X(parker_look_s) \
	X(parker_coming_min) X(parker_coming_s) X(parker_trip_s) X(parker_depart_per_look) X(parker_stay_min_s) X(parker_stay_max_s) X(parker_call_reach_m) \
	X(parker_call_timeout_s) X(parker_join_ms) X(parker_ask_way_s) X(parker_stop_ms) \
	X(parker_way_room_m) X(parker_way_wide_m) X(kerb_drop_share) X(kerb_pick_share) X(kerb_delivery_share) X(kerb_delivery_s) X(kerb_end_clear_m) X(kerb_plan_margin_m) X(kerb_stretch_m) X(kerb_stop_share) X(kerb_queue_m) X(kerb_door_along) X(kerb_door_out_m) X(kerb_door_clear_m) X(kerb_foot_clear_m) X(kerb_door_open_s) X(kerb_door_shut_s) X(kerb_seat_s) X(kerb_pace_ms) X(kerb_give_up_s) X(kerb_halt_s) X(kerb_find_reach_m) X(kerb_pick_fetch_m) X(parker_gate_margin_m) X(parker_gate_lead_m) X(parker_stop_slack_m) X(parker_past_gate_m) X(parker_give_up_s) X(parker_closure_margin_m) X(gate_lane_reach_m) X(gate_lane_min_m) X(gate_lane_end_m) X(gate_stop_before_m) X(gate_join_past_m) \
	X(gate_span_min_m) X(man_forward_ms) X(man_backward_ms) X(man_ramp_m) X(man_creep_ms) X(man_tangent) X(man_across_m) \
	X(man_least_m) X(man_lead_m) X(man_disc_pad_m) X(man_disc_step_m) X(stay_work_h) X(stay_errand_h) \
	X(lorry_most) X(lorry_radius_m) X(lorry_rear_m) X(lorry_margin_m) X(lorry_cell_m) X(lorry_street_reach_m) X(lorry_street_window_m) X(lorry_mouth_flare_m) X(lorry_gate_in_m) X(lorry_dock_gap_m) X(lorry_stop_before_m) X(lorry_join_past_m) X(lorry_pre_dock_m) X(lorry_pre_step_m) X(lorry_reach_m) X(lorry_turn_rad) X(lorry_settle_step_m) X(lorry_fit_margin_m) \
	X(lorry_clear_h_m) X(lorry_call_reach_m) X(lorry_call_timeout_s) X(lorry_unload_min_s) X(lorry_unload_max_s) X(lorry_closure_margin_m) X(lorry_clear_margin_m) X(lorry_join_ms) X(lorry_ask_way_s) X(lorry_walker_clear_m) X(lorry_look_m) X(lorry_forward_ms) X(lorry_backward_ms) X(lorry_ramp_m) X(lorry_creep_ms) X(lorry_stop_slack_m) X(lorry_plan_work) \
	X(parker_look_ticks) X(bike_share) X(bike_speed_kmh) X(bike_edge_m) X(cycle_ease_m) X(cycle_clear_share) \
	X(transit_pick_m) X(transit_stop_reach_m) X(transit_join_m) X(transit_along_road_m) X(transit_entrance_reach_m) X(transit_most_vehicles) X(transit_bus_vehicles) X(transit_rail_vehicles) X(bus_dwell_s) X(bus_terminal_s) X(bus_spacing_s) X(bus_approach_m) X(bus_at_stop_m) X(bus_stopped_ms) X(bus_detour) X(bus_pace) X(bus_away_margin_m) X(train_dwell_s) X(train_top_ms) X(metro_top_ms) X(train_accel) X(train_brake) X(train_gap_m) X(train_cars) X(metro_cars) X(train_car_m) X(crossing_stop_m) X(crossing_warn_m) X(crossing_keep_clear_m) X(train_clear_ahead_m) X(train_clear_time_s) X(train_clear_from_m) X(train_clear_step_m) X(train_clear_disc_m) X(train_car_disc_m) X(train_car_step_m) X(train_car_margin_m) X(train_idle_ms) X(train_walker_look_m) \
	X(meso_alpha) X(meso_crowd) X(meso_s0_m) X(meso_head_s) X(meso_dwell_s) \
	X(meso_window_m) X(meso_window_out_m) X(micro_spawn_m) X(ped_window_m) X(ped_despawn_window_share)

#define CITY_TRAFFIC_DECLARE(name) float name = 0;

// Vehicle classes (spec sim.md §5.1): car styles 0-5, then bus and truck.
enum class VehicleType : uint8_t { SEDAN = 0, HATCH, SUV, VAN, PICKUP, MOTO, BUS, TRUCK, BICYCLE, COUNT };
constexpr int VEHICLE_TYPES = int(VehicleType::COUNT);
// A rider sits in the open: a shot at the machine hits the rider (sim/commands/shoot_*.cpp, render/effects).
constexpr bool two_wheeler(VehicleType t) { return t == VehicleType::MOTO || t == VehicleType::BICYCLE; }
// What kind of place sends people out on foot (traffic.json people_kind_m, zone_kinds).
enum class PlaceKind : uint8_t { HOME = 0, SHOP, WORK, CIVIC, COUNT };
constexpr int PLACE_KINDS = int(PlaceKind::COUNT);
constexpr int HOURS = 24;
constexpr int CAR_STYLES = 6;
constexpr int BASE_ZONES = 5; // none, residential, commercial, industrial, mixed
constexpr int EXTENSION_LANES = 5; // traffic.json signal_extension_s: approaches of 1 to 5 lanes
constexpr int PAINTS = 16; // Roadcraft shared/vehiclePaints.ts

// Driver archetype of a class (drivers.ts ARCH) plus the body length and half width the simulation uses.
struct VehicleClass {
	float a = 0, b = 0, b_emergency = 0, headway = 0, min_gap = 0, speed_share = 0, length = 0, half_width = 0;
};

// The regional style read until a city carries its own (session-rules.md, section 10): the one name of it.
constexpr const char *REGIONAL_STYLE = "roadcraft";

struct TrafficParams {
	std::vector<uint32_t> line_colours; // RGB, one per new line in turn
	CITY_TRAFFIC_FIELDS(CITY_TRAFFIC_DECLARE)
	VehicleClass classes[VEHICLE_TYPES];
	float signal_extension_s[EXTENSION_LANES] = {};
	float style_share[CAR_STYLES] = {};
	float zone_home[BASE_ZONES] = {}, zone_work[BASE_ZONES] = {}, zone_mid[BASE_ZONES] = {}, zone_truck[BASE_ZONES] = {};
	float people_kind_m[PLACE_KINDS] = {}; // people per 100 m of street at full bustle
	float people_busy[PLACE_KINDS * HOURS] = {}; // how busy each kind is at each hour, 0..1
	float people_working[HOURS] = {}, people_purchasing[HOURS] = {}; // percent of everybody, at each hour
	float zone_kinds[BASE_ZONES * PLACE_KINDS] = {}; // what each base zone's land is
	int drive_side = 0; // the side of the road traffic keeps to, in the road's side numbers (0 right): the style's
};

} // namespace city
