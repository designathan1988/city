// Names of every scalar tunable (X-macro). Values live ONLY in project/data/catalog/tuning.json (same groups);
// tools/gates/gate_tuning.py checks that this list and the JSON have exactly the same keys and that every
// field is read somewhere. api/catalog_loader.cpp refuses to start when a key is missing.
#pragma once

#define CITY_TUNING_FIELDS(X) \
	X(sim, tick_hz) X(sim, minutes_per_second) X(sim, speed_fast) X(sim, speed_fastest) X(sim, day_start_min) \
	X(sim, growth_per_s) X(sim, growth_tries) \
	X(road, min_len_m) X(road, uturn_opening_m) X(road, uturn_radius_m) X(road, snap_m) X(road, snap_end_m) X(road, max_seg_m) X(road, fixed_gap_m) X(road, merge_m) \
	X(road, sharp_cos) X(road, level_dy_m) X(road, max_degree) X(road, pick_m) X(road, overlap_share) \
	X(road, overlap_run_m) X(road, parallel_cos) \
	X(road, snap_node_px) X(road, snap_pick_px) X(road, snap_land_px) X(road, snap_step_deg) X(road, grid_m) X(road, inner_radius_m) X(road, bend_limit) X(road, bend_gain) \
	X(road, free_tolerance_m) X(road, free_max_controls) X(road, click_px) X(road, variant_lane_lo_m) X(road, variant_lane_hi_m) X(road, variant_walk_lo_m) X(road, variant_walk_hi_m) \
	X(road, bay_m) X(road, max_variants) \
	X(grade, ground) X(grade, authored) X(grade, join_m) X(grade, ramp) X(grade, deck) X(grade, deck_tie) X(grade, tunnel) X(grade, clearance_m) \
	X(grade, clear_elevated_m) X(grade, clear_bridge_m) X(grade, deck_ground_m) X(grade, deck_elevated_m) X(grade, deck_bridge_m) X(grade, deck_tunnel_m) \
	X(flow, stop_major) X(flow, stop_minor) X(flow, signal_major) X(flow, signal_minor) X(flow, signal_major_b) X(flow, signal_minor_b) \
	X(flow, step_down) X(flow, trend_window) \
	X(grade, tunnel_headroom_m) X(grade, tunnel_arch_m) X(grade, tunnel_floor_m) X(grade, lift_on_m) \
	X(structure, raised_m) X(structure, buried_m) X(structure, wall_thick_m) X(structure, wall_coping_m) X(structure, wall_foot_m) X(structure, wall_step_m) X(structure, bore_over_m) X(structure, pier_spacing_elevated_m) X(structure, pier_spacing_bridge_m) \
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
	X(terrain, rough_reach_min) X(terrain, min_hard_rim) X(terrain, min_hard_wall_m) X(terrain, river_depth_m) \
	X(terrain, river_freeboard_m) X(terrain, river_floor_share) X(terrain, shore_reach_m) 	X(render, occluder_min_storeys) X(render, road_end_m) X(render, props_end_m) X(render, pool_handover_m) X(render, building_detail_end_m) X(render, building_bay_end_m) X(render, building_mid_end_m) X(render, building_far_end_m) 	X(render, max_chunk_rebuilds_per_frame) X(render, commit_budget_ms) X(render, upload_budget_mb) X(render, pose_delay_ticks) \
	X(history, max_entries) X(history, max_mb) \
	X(roundabout, radius_min_m) X(roundabout, radius_max_m) X(roundabout, approach_m) X(roundabout, entry_merge_m) \
	X(roundabout, max_arc_deg) X(roundabout, disc_pad_m) \
	X(edit, split_min_m) X(edit, street_cos) X(edit, max_floors) X(edit, duplicate_min_m) X(edit, duplicate_gap_m) \
	X(inspect, lane_vehicle_m) X(inspect, min_leg_deg) \
	X(build, grid_m) X(build, module_m) X(build, wing_modules) X(build, primitive_m) X(build, primitive_depth_m) X(build, primitive_storeys) X(build, primitive_ground_m) X(build, primitive_storey_m) X(build, roof_mass_storeys) X(build, min_side_m) X(build, max_side_m) X(build, min_storey_m) \
	X(build, max_storey_m) X(build, max_ground_m) X(build, max_volumes) X(build, min_bearing) \
	X(build, least_opening) X(build, most_opening) X(build, max_projection_m) X(build, max_recess_m) X(build, relief_step_m) \
	X(build, max_bays) X(build, max_pier_every) X(build, min_pitch_deg) X(build, max_pitch_deg) X(build, pick_step_m) \
	X(build, max_parts) X(build, dial_share) X(build, step_run_m) X(build, ramp_run_per_rise) X(build, max_gear) X(build, gear_turn_deg) X(build, road_probe_m) X(build, world_margin_m) X(build, max_plinth_m) \
	X(build, road_clearance_m) X(build, snap_road_reach_m) X(build, snap_building_reach_m) X(build, snap_edge)

#define CITY_TUNING_DECLARE(group, name) float group##_##name = 0;
