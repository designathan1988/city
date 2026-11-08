// Names of every scalar tunable (X-macro), each with its type: X a measure or a share (float), XI a count (int, refused
// when the table gives a fraction), XD a value the code works on in double precision (the generated landscape). Values
// live ONLY in project/data/catalog/tuning.json (same groups); tools/gates/gate_tuning.py checks that this list and the
// JSON have exactly the same keys and that every field is read somewhere. api/catalog_loader.cpp refuses to start when a
// key is missing.
#pragma once

#define CITY_TUNING_FIELDS(X, XI, XD) \
	X(sim, tick_hz) X(sim, minutes_per_second) XI(sim, speed_fast) XI(sim, speed_fastest) X(sim, day_start_min) \
	X(sim, growth_per_s) XI(sim, growth_tries) XI(sim, body_cache_mb) \
	X(road, min_len_m) X(road, uturn_opening_m) X(road, uturn_radius_m) X(road, snap_m) X(road, snap_end_m) X(road, max_seg_m) X(road, fixed_gap_m) X(road, merge_m) \
	X(road, sharp_cos) X(road, level_dy_m) XI(road, max_degree) X(road, pick_m) X(road, overlap_share) \
	X(road, overlap_run_m) X(road, parallel_cos) \
	X(road, snap_node_px) X(road, snap_pick_px) X(road, snap_land_px) X(road, snap_step_deg) X(road, grid_m) X(road, inner_radius_m) X(road, bend_limit) X(road, bend_gain) \
	X(road, free_tolerance_m) XI(road, free_max_controls) X(road, click_px) X(road, variant_lane_lo_m) X(road, variant_lane_hi_m) X(road, variant_walk_lo_m) X(road, variant_walk_hi_m) \
	X(road, bay_m) XI(road, max_variants) \
	X(grade, ground) X(grade, authored) X(grade, join_m) X(grade, ramp) X(grade, deck) X(grade, deck_tie) X(grade, tunnel) X(grade, clearance_m) \
	X(grade, clear_elevated_m) X(grade, clear_bridge_m) X(grade, deck_ground_m) X(grade, deck_elevated_m) X(grade, deck_bridge_m) X(grade, deck_tunnel_m) \
	X(flow, stop_major) X(flow, stop_minor) X(flow, signal_major) X(flow, signal_minor) X(flow, signal_major_b) X(flow, signal_minor_b) \
	X(flow, step_down) X(flow, trend_window) \
	X(grade, tunnel_headroom_m) X(grade, tunnel_arch_m) X(grade, tunnel_floor_m) X(grade, lift_on_m) \
	X(structure, span_from_m) X(structure, span_every_m) X(structure, cable_step_m) X(structure, cable_first_m) X(structure, tower_over_m) X(structure, tower_leg_m) X(structure, raised_m) X(structure, buried_m) X(structure, wall_thick_m) X(structure, wall_coping_m) X(structure, wall_foot_m) X(structure, wall_step_m) X(structure, bore_over_m) X(structure, pier_spacing_elevated_m) X(structure, pier_spacing_bridge_m) \
	X(structure, pier_radius_elevated_m) X(structure, pier_radius_bridge_m) X(structure, bearing_m) X(structure, pier_slenderness) \
	X(structure, pier_max_width) X(structure, min_support_m) X(structure, bent_spread) X(structure, cap_depth) X(structure, cap_height_m) \
	X(structure, cap_overhang_m) X(structure, parapet_height_m) X(structure, parapet_foot_m) X(structure, parapet_top_m) \
	X(structure, parapet_step_m) X(structure, parapet_from_m) X(structure, parapet_full_m) X(structure, abutment_lift_m) \
	X(structure, abutment_depth_m) X(structure, pier_abutment_gap_m) X(structure, portal_wing_m) X(structure, portal_thickness_m) \
	X(structure, portal_slice_m) X(structure, portal_coping_m) X(structure, portal_cover_m) \
	X(zone, side_offset_m) X(zone, sample_m) \
	X(economy, start_money) X(economy, refund_share) X(economy, fill_from_m) X(economy, on_piers_from_m) \
	X(world, tile_m) X(world, height_spacing_m) X(world, water_spacing_m) \
	X(terrain, min_height_m) X(terrain, max_height_m) \
	X(terrain, embankment_ratio) X(terrain, min_run_m) X(terrain, road_sink_m) \
	XD(terrain, rough_reach_min) X(terrain, min_hard_rim) X(terrain, min_hard_wall_m) X(terrain, river_depth_m) \
	X(terrain, river_freeboard_m) XD(terrain, river_floor_share) X(terrain, shore_reach_m) \
	XD(land, plain_m) XD(land, river_radius_m) XD(land, river_strength_m) XD(land, canal_from_edge_m) XD(land, river_carve) XD(land, river_bed_depth) XD(land, water_fill) XD(land, water_margin_m) XD(land, river_min_depth_m) XD(land, flood_slack_m) XD(land, river_min_fall) XD(land, valley_floor_m) XD(land, valley_width_m) XD(land, open_share) XD(land, lake_grid_m) XD(land, lake_chance) XD(land, lake_edge_max) XD(land, lake_river_gap_m) XD(land, basin_radius_m) XD(land, basin_depth_m) XD(land, basin_reach) XD(land, basin_wobble) XD(land, basin_noise_m) XD(land, basin_noise_step) XD(land, hills_base_m) XD(land, hills_amp_m) XD(land, hills_scale_m) XI(land, hills_octaves) XD(land, bumps_amp_m) XD(land, bumps_scale_m) XI(land, bumps_octaves) XD(land, edge_start) XD(land, edge_ramp) XD(land, range_base_m) XD(land, range_amp_m) XD(land, range_scale_m) XI(land, range_octaves) XI(land, bank_probes) XI(land, smooth_passes) XI(land, descend_steps) \
		XI(render, occluder_min_storeys) X(render, road_end_m) X(render, props_end_m) X(render, light_unit_lx) X(render, lamp_flux_lm) X(render, lamp_lens_m2) X(render, lamp_range_m) X(render, lamp_cone_deg) X(render, lamp_cone_fall) X(render, lamp_decay) X(render, lamp_specular) X(render, lamp_reach_m) X(render, lamp_gives_way_from) X(render, lamp_cast_reach_m) X(render, head_intensity_cd) X(render, head_range_m) X(render, head_cone_deg) X(render, head_cone_fall) X(render, head_decay) X(render, head_specular) X(render, head_reach_m) X(render, head_gives_way_from) X(render, head_height_m) X(render, head_dip_deg) X(render, building_detail_end_m) X(render, building_bay_end_m) X(render, building_mid_end_m) X(render, building_far_end_m) X(render, detail_error_px) 	XI(render, max_chunk_rebuilds_per_frame) X(render, commit_budget_ms) XI(render, upload_budget_mb) XI(render, ground_cache_mb) XI(render, pose_delay_ticks) \
	XI(history, max_entries) XI(history, max_mb) \
	X(roundabout, radius_min_m) X(roundabout, radius_max_m) X(roundabout, approach_m) X(roundabout, entry_merge_m) \
	X(roundabout, max_arc_deg) X(roundabout, disc_pad_m) \
	X(edit, split_min_m) X(edit, street_cos) XI(edit, max_floors) X(edit, duplicate_min_m) X(edit, duplicate_gap_m) \
	X(inspect, lane_vehicle_m) X(inspect, min_leg_deg) \
	X(build, grid_m) X(build, module_m) X(build, wing_modules) X(build, primitive_m) X(build, primitive_depth_m) X(build, max_lift_m) X(build, hung_reach_m) XI(build, primitive_storeys) X(build, primitive_ground_m) X(build, primitive_storey_m) XI(build, roof_mass_storeys) X(build, min_side_m) X(build, max_side_m) X(build, min_storey_m) \
	X(build, max_storey_m) X(build, max_ground_m) XI(build, max_volumes) X(build, min_bearing) \
	X(build, least_opening) X(build, most_opening) X(build, max_projection_m) X(build, max_recess_m) X(build, relief_step_m) \
	XI(build, max_bays) XI(build, max_pier_every) X(build, min_pitch_deg) X(build, max_pitch_deg) X(build, pick_step_m) \
	XI(build, max_parts) X(build, dial_share) X(build, step_run_m) X(build, ramp_run_per_rise) XI(build, max_gear) X(build, gear_turn_deg) X(build, road_probe_m) X(build, world_margin_m) X(build, max_plinth_m) \
	X(strike, force_start) X(strike, force_fine) X(strike, force_coarse) XI(strike, frame_floors) X(strike, frame_collapse_kpa) X(strike, standing_least) X(strike, storey_m) X(strike, scar_radius_m) X(strike, scar_radius_per_force) X(strike, scar_near_share) X(strike, scar_force_gain) X(strike, scar_force_least) X(strike, scar_lift_m) XI(strike, scars_most) \
	X(build, road_clearance_m) X(build, snap_road_reach_m) X(build, snap_building_reach_m) X(build, snap_edge) \
	X(build, free_relief_step_m) X(build, wing_share) \
	X(build, shaped_inset_share) X(build, click_in_m) X(build, shortest_leg_m) X(build, lowest_flight_m) \
	X(build, lowest_step_m) X(build, flight_width_m) X(build, band_width_m) X(build, thinnest_m) XD(build, along_side_cos) \
	XD(build, flush_pad_m) \
	XD(lot, corner_take_m) XD(lot, side_reach_m) \
	XI(roundabout, refuse_samples) XI(roundabout, water_samples) XI(roundabout, cross_samples) XI(roundabout, bisect_steps) \
	XI(roundabout, empty_ring_nodes) XI(roundabout, vacant_arms) XD(roundabout, water_clear_m) \
	X(shoot, rider_back_m) X(shoot, rider_push) X(shoot, rider_hit_m) \
	X(strike, furniture_reach_m) X(strike, splinter_share) X(strike, splinter_chance) \
	XD(junction, max_retreat) XD(junction, far_hit) XD(junction, opposite_dot) XD(junction, taper_step) \
	XD(junction, taper_min) XD(junction, taper_slope) XD(junction, width_jump) XD(junction, width_jump_run) \
	XD(junction, max_bend) XD(junction, kerb_radius_min) XD(junction, corner_miter) XD(junction, bulb_half_max) \
	XD(junction, bulb_len_min) XD(junction, bent_radius) XD(junction, bevel_pad) XD(junction, taper_min_step) \
	XD(junction, fillet_share) XD(junction, taper_turn_deg) XD(junction, taper_angle_deg) XD(junction, acute_deg) \
	X(likeness, form_apart) X(likeness, roof_apart) X(likeness, storeys_apart) X(likeness, storeys_span) \
	X(likeness, colour_span) X(likeness, distance_weight) X(likeness, front_row_m) X(likeness, whole_share) \
	X(likeness, columns) \
	X(inspect, radius_m) \
	XD(lot, angle_look_m) \
	X(build, handle_side_out_m) X(build, handle_corner_out_m) X(build, handle_scale_out_m) X(build, handle_above_roof_m) \
	X(build, handle_off_floor_m) X(build, handle_lift_out_m) X(build, handle_relief_out_m) \
	X(structure, backfill_in_m) X(structure, backfill_over_m)

#define CITY_TUNING_DECLARE(group, name) float group##_##name = 0;
#define CITY_TUNING_DECLARE_COUNT(group, name) int group##_##name = 0;
#define CITY_TUNING_DECLARE_DOUBLE(group, name) double group##_##name = 0;
