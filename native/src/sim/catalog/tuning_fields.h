// Names of every scalar tunable (X-macro). Values live ONLY in project/data/catalog/tuning.json (same groups);
// tools/gates/gate_tuning.py checks that this list and the JSON have exactly the same keys and that every
// field is read somewhere. api/catalog_loader.cpp refuses to start when a key is missing.
#pragma once

#define CITY_TUNING_FIELDS(X) \
	X(sim, tick_hz) X(sim, minutes_per_second) X(sim, speed_fast) X(sim, speed_fastest) X(sim, day_start_min) \
	X(sim, day_light_hour) X(sim, night_light_hour) X(sim, growth_per_s) X(sim, growth_tries) \
	X(road, min_len_m) X(road, snap_m) X(road, snap_end_m) X(road, max_seg_m) X(road, fixed_gap_m) X(road, merge_m) \
	X(road, sharp_cos) X(road, level_dy_m) X(road, max_degree) X(road, pick_m) X(road, overlap_share) \
	X(road, overlap_run_m) X(road, parallel_cos) X(road, overlap_pad_m) \
	X(zone, side_offset_m) X(zone, sample_m) \
	X(economy, start_money) X(economy, refund_share) \
	X(world, tile_m) X(world, height_spacing_m) X(world, water_spacing_m) \
	X(terrain, min_height_m) X(terrain, max_height_m) \
	X(terrain, embankment_ratio) X(terrain, min_run_m) X(terrain, road_sink_m) \
	X(render, road_end_m) X(render, props_end_m) X(render, building_detail_end_m) X(render, building_mid_end_m) X(render, building_far_end_m) \
	X(render, max_chunk_rebuilds_per_frame) X(render, commit_budget_ms) \
	X(history, max_entries) X(history, max_mb) \
	X(roundabout, radius_min_m) X(roundabout, radius_max_m) X(roundabout, approach_m) X(roundabout, entry_merge_m) \
	X(roundabout, max_arc_deg) X(roundabout, disc_pad_m) \
	X(edit, split_min_m) X(edit, street_cos) X(edit, max_floors)

#define CITY_TUNING_DECLARE(group, name) float group##_##name = 0;
