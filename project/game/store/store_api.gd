## StoreApi: the names of the C++ core's methods the script side may call. The store sends a command (dispatch) or asks a
## question (query) only through these names; the value is the method that runs it.
class_name StoreApi
extends RefCounted

## Commands the UI/tools may send, with the core method that executes each one.
const COMMANDS := {
	&"set_speed": "set_speed", &"set_day_hour": "set_day_hour", &"build_road": "build_road", &"demolish": "demolish", &"paint_zone": "paint_zone",
	&"preview_road": "preview_road", &"preview_road_path": "preview_road_path",
	&"build_road_path": "build_road_path", &"undo": "undo", &"redo": "redo", &"begin_stroke": "begin_stroke",
	&"end_stroke": "end_stroke", &"split_road": "split_road", &"set_road_end": "set_road_end",
	&"upgrade_road": "upgrade_road", &"road_variant": "road_variant", &"move_node": "move_node", &"set_node_control": "set_node_control",
	&"roundabout": "roundabout", &"terrain_brush": "terrain_brush", &"draw_building": "draw_building",
	&"set_building": "set_building", &"set_overlay": "set_overlay", &"set_signal": "set_signal", &"green_wave": "green_wave",
	&"set_weather": "set_weather",
	&"add_cloud": "add_cloud", &"edit_cloud": "edit_cloud", &"remove_cloud": "remove_cloud",
	&"scatter_clouds": "scatter_clouds", &"clear_clouds": "clear_clouds", &"fog_brush": "fog_brush",
	&"clear_fog": "clear_fog", &"paint_brush": "paint_brush", &"clear_gullies": "clear_gullies",
	&"tree_brush": "tree_brush", &"clear_trees": "clear_trees", &"element_brush": "element_brush", &"clear_elements": "clear_elements", &"set_biome": "set_biome",
	&"clear_relief": "clear_relief",
	&"body_storeys": "body_storeys",
	&"body_insert_storey": "body_insert_storey", &"body_resize": "body_resize", &"body_add_wing": "body_add_wing",
	&"body_add_setback": "body_add_setback", &"body_split": "body_split", &"body_remove_volume": "body_remove_volume",
	&"body_heights": "body_heights", &"body_set_bay": "body_set_bay", &"body_set_zone": "body_set_zone",
	&"body_set_pattern": "body_set_pattern", &"body_set_face": "body_set_face", &"body_set_relief": "body_set_relief",
	&"body_set_roof": "body_set_roof", &"body_roof_shape": "body_roof_shape", &"body_paint": "body_paint",
	&"body_add_gear": "body_add_gear", &"body_remove_gear": "body_remove_gear", &"body_turn_gear": "body_turn_gear",
	&"body_set_flag": "body_set_flag", &"body_flag_colour": "body_flag_colour",
	&"body_add_part": "body_add_part", &"body_add_part_at": "body_add_part_at", &"body_remove_part": "body_remove_part",
	&"body_turn_part": "body_turn_part", &"body_repeat_part": "body_repeat_part", &"place_model": "place_model",
	&"transit_stop": "transit_stop", &"transit_terminal": "transit_terminal",
	&"transit_station": "transit_station", &"transit_track": "transit_track", &"transit_line": "transit_line", &"transit_remove": "transit_remove",
	&"transit_drop_line": "transit_drop_line", &"transit_vehicles": "transit_vehicles",
}
const QUERIES := {
	&"raycast": "raycast", &"height_at": "height_at", &"inspect": "inspect", &"probe_building": "probe_building",
	&"list_buildings": "list_buildings", &"building_chunk": "building_chunk", &"job_pool_probe": "job_pool_probe", &"city_bounds": "city_bounds", &"film_grade": "film_grade", &"film_graded": "film_graded", &"render_pending": "render_pending",
	&"render_cost": "render_cost", &"road_mesh_counts": "road_mesh_counts", &"draw_verges": "draw_verges", &"wear_paint": "wear_paint", &"pose_stats": "pose_stats", &"agent_cost": "agent_cost", &"building_cost": "building_cost",
	&"traffic_probe": "traffic_probe", &"road_start": "road_start", &"road_end": "road_end",
	&"road_pieces": "road_pieces", &"road_profiles": "road_profiles", &"node_at": "node_at", &"junctions": "junctions", &"moved_roads": "moved_roads",
	&"segment_at": "segment_at", &"preview_roundabout": "preview_roundabout", &"preview_lot": "preview_lot",
	&"model_list": "model_list", &"model_bytes": "model_bytes", &"doc_body": "doc_body", &"building_bytes": "building_bytes", &"body_size": "body_size",
	&"place_fit": "place_fit", &"place_check": "place_check", &"stage_show": "stage_show", &"stage_move": "stage_move", &"stage_free": "stage_free", &"stage_triangles": "stage_triangles",
	&"overlays": "overlays", &"save_status": "save_status", &"park_sample": "park_sample", &"camera_clear": "camera_clear", &"kerb_busy": "kerb_busy", &"dock_count": "dock_count", &"lot_clearance": "lot_clearance", &"vehicles_near": "vehicles_near",
	&"landscape": "landscape", &"cloud_at": "cloud_at",
	&"sky_revision": "sky_revision", &"fog_field": "fog_field", &"fog_revision": "fog_revision",&"effects_near": "effects_near",
	&"body_info": "body_info", &"body_bay": "body_bay", &"body_pick": "body_pick", &"body_outline": "body_outline",
	&"transit_plan": "transit_plan", &"transit_footway": "transit_footway", &"transit_xray": "transit_xray", &"transit_preview": "transit_preview", &"transit_run": "transit_run", &"transit_pick": "transit_pick", &"transit_snap": "transit_snap",
}
