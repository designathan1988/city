// Script bindings of CityCore (names are the GDScript API used by game/store/city_store.gd).
#include "api/city_core.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

namespace city {

void CityCore::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load_catalog", "tables"), &CityCore::load_catalog);
	ClassDB::bind_method(D_METHOD("setup_render", "render_tables", "materials"), &CityCore::setup_render);
	ClassDB::bind_method(D_METHOD("new_map", "map", "seed", "with_city", "paused"), &CityCore::new_map, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("mark_saved"), &CityCore::mark_saved);
	ClassDB::bind_method(D_METHOD("set_speed", "speed"), &CityCore::set_speed);
	ClassDB::bind_method(D_METHOD("set_day_hour", "hour"), &CityCore::set_day_hour);
	bind_models();
	bind_bodies();
	bind_nature();
	ClassDB::bind_method(D_METHOD("get_speed"), &CityCore::get_speed);
	ClassDB::bind_method(D_METHOD("set_terrain_path", "path"), &CityCore::set_terrain_path);
	ClassDB::bind_method(D_METHOD("get_terrain_path"), &CityCore::get_terrain_path);
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "terrain_path"), "set_terrain_path", "get_terrain_path");
	ClassDB::bind_method(D_METHOD("step_ticks", "ticks"), &CityCore::step_ticks);

	ClassDB::bind_method(D_METHOD("road_start", "position", "px", "on", "grid"), &CityCore::road_start);
	ClassDB::bind_method(D_METHOD("road_end", "start", "position", "px", "on", "angles", "grid"), &CityCore::road_end);
	ClassDB::bind_method(D_METHOD("road_pieces", "start", "end", "alignment", "has_control", "control", "samples", "profile", "px"), &CityCore::road_pieces);
	ClassDB::bind_method(D_METHOD("preview_road_path", "pieces", "profile", "px"), &CityCore::preview_road_path);
	ClassDB::bind_method(D_METHOD("build_road_path", "pieces", "profile", "px"), &CityCore::build_road_path);
	ClassDB::bind_method(D_METHOD("preview_road", "start", "end", "has_control", "control", "profile", "px"), &CityCore::preview_road);
	ClassDB::bind_method(D_METHOD("build_road", "start", "end", "has_control", "control", "profile", "px"), &CityCore::build_road);
	ClassDB::bind_method(D_METHOD("demolish", "position"), &CityCore::demolish);
	ClassDB::bind_method(D_METHOD("paint_zone", "position", "radius", "zone"), &CityCore::paint_zone);
	ClassDB::bind_method(D_METHOD("transit_stop", "position"), &CityCore::transit_stop);
	ClassDB::bind_method(D_METHOD("transit_terminal", "position"), &CityCore::transit_terminal);
	ClassDB::bind_method(D_METHOD("transit_station", "position", "mode"), &CityCore::transit_station);
	ClassDB::bind_method(D_METHOD("transit_track", "points", "mode"), &CityCore::transit_track);
	ClassDB::bind_method(D_METHOD("transit_line", "stops"), &CityCore::transit_line);
	ClassDB::bind_method(D_METHOD("transit_remove", "position"), &CityCore::transit_remove);
	ClassDB::bind_method(D_METHOD("transit_drop_line", "id"), &CityCore::transit_drop_line);
	ClassDB::bind_method(D_METHOD("transit_vehicles", "id", "vehicles"), &CityCore::transit_vehicles);
	ClassDB::bind_method(D_METHOD("transit_plan"), &CityCore::transit_plan);
	ClassDB::bind_method(D_METHOD("transit_run"), &CityCore::transit_run);
	ClassDB::bind_method(D_METHOD("transit_footway", "position", "reach"), &CityCore::transit_footway);
	ClassDB::bind_method(D_METHOD("transit_xray", "on"), &CityCore::transit_xray);
	ClassDB::bind_method(D_METHOD("transit_preview", "points", "mode"), &CityCore::transit_preview);
	ClassDB::bind_method(D_METHOD("transit_pick", "position", "mode", "any"), &CityCore::transit_pick);
	ClassDB::bind_method(D_METHOD("transit_snap", "position", "mode"), &CityCore::transit_snap);
	ClassDB::bind_method(D_METHOD("road_profiles"), &CityCore::road_profiles);
	ClassDB::bind_method(D_METHOD("road_variant", "base", "lanes", "width", "parking"), &CityCore::road_variant);
	ClassDB::bind_method(D_METHOD("undo"), &CityCore::undo);
	ClassDB::bind_method(D_METHOD("redo"), &CityCore::redo);
	ClassDB::bind_method(D_METHOD("begin_stroke", "label"), &CityCore::begin_stroke);
	ClassDB::bind_method(D_METHOD("end_stroke"), &CityCore::end_stroke);
	ClassDB::bind_method(D_METHOD("split_road", "position"), &CityCore::split_road);
	ClassDB::bind_method(D_METHOD("set_road_end", "node", "bulb"), &CityCore::set_road_end);
	ClassDB::bind_method(D_METHOD("upgrade_road", "position", "profile", "whole_street"), &CityCore::upgrade_road);
	ClassDB::bind_method(D_METHOD("roundabout", "position", "radius", "cut"), &CityCore::roundabout);
	ClassDB::bind_method(D_METHOD("preview_roundabout", "position", "radius", "cut"), &CityCore::preview_roundabout);
	ClassDB::bind_method(D_METHOD("segment_at", "position"), &CityCore::segment_at);
	ClassDB::bind_method(D_METHOD("move_node", "node", "position"), &CityCore::move_node);
	ClassDB::bind_method(D_METHOD("set_node_control", "node", "control"), &CityCore::set_node_control);
	ClassDB::bind_method(D_METHOD("junctions"), &CityCore::junctions);
	ClassDB::bind_method(D_METHOD("node_at", "position", "px"), &CityCore::node_at);
	ClassDB::bind_method(D_METHOD("moved_roads", "node", "position"), &CityCore::moved_roads);
	ClassDB::bind_method(D_METHOD("terrain_brush", "position", "radius", "strength", "dt", "mode", "hardness"), &CityCore::terrain_brush);
	ClassDB::bind_method(D_METHOD("draw_building", "corner_a", "corner_b", "function", "zone"), &CityCore::draw_building);
	ClassDB::bind_method(D_METHOD("preview_lot", "corner_a", "corner_b"), &CityCore::preview_lot);
	ClassDB::bind_method(D_METHOD("set_building", "id", "function", "floors"), &CityCore::set_building);
	ClassDB::bind_method(D_METHOD("save_city", "path"), &CityCore::save_city);
	ClassDB::bind_method(D_METHOD("save_status"), &CityCore::save_status);
	ClassDB::bind_method(D_METHOD("load_city", "path"), &CityCore::load_city);

	ClassDB::bind_method(D_METHOD("snapshot"), &CityCore::snapshot);
	ClassDB::bind_method(D_METHOD("constants"), &CityCore::constants);
	ClassDB::bind_method(D_METHOD("raycast", "origin", "direction"), &CityCore::raycast);
	ClassDB::bind_method(D_METHOD("height_at", "x", "z"), &CityCore::height_at);
	ClassDB::bind_method(D_METHOD("inspect", "position"), &CityCore::inspect);
	ClassDB::bind_method(D_METHOD("probe_building", "id"), &CityCore::probe_building);
	ClassDB::bind_method(D_METHOD("list_buildings"), &CityCore::list_buildings);
	ClassDB::bind_method(D_METHOD("city_bounds"), &CityCore::city_bounds);
	ClassDB::bind_method(D_METHOD("film_grade", "grade"), &CityCore::film_grade);
	ClassDB::bind_method(D_METHOD("film_graded", "grade", "colour"), &CityCore::film_graded);
	ClassDB::bind_method(D_METHOD("traffic_probe", "sample"), &CityCore::traffic_probe);
	ClassDB::bind_method(D_METHOD("set_signal", "node", "fixed", "greens", "offset"), &CityCore::set_signal);
	ClassDB::bind_method(D_METHOD("green_wave", "node"), &CityCore::green_wave);
	ClassDB::bind_method(D_METHOD("park_sample"), &CityCore::park_sample);
	ClassDB::bind_method(D_METHOD("camera_clear", "at", "radius", "vehicles"), &CityCore::camera_clear, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("kerb_busy"), &CityCore::kerb_busy);
	ClassDB::bind_method(D_METHOD("dock_count"), &CityCore::dock_count);
	ClassDB::bind_method(D_METHOD("lot_clearance"), &CityCore::lot_clearance);
	ClassDB::bind_method(D_METHOD("vehicles_near", "at", "radius"), &CityCore::vehicles_near);
	ClassDB::bind_method(D_METHOD("amber_stops", "v", "stop_m", "top", "left", "type"), &CityCore::amber_stops);
	ClassDB::bind_method(D_METHOD("amber_wanted", "kmh"), &CityCore::amber_wanted);
	ClassDB::bind_method(D_METHOD("set_overlay", "name", "on"), &CityCore::set_overlay);
	ClassDB::bind_method(D_METHOD("overlays"), &CityCore::overlays);
	ClassDB::bind_method(D_METHOD("render_pending"), &CityCore::render_pending);
	ClassDB::bind_method(D_METHOD("building_chunk", "x", "z"), &CityCore::building_chunk);
	ClassDB::bind_method(D_METHOD("job_pool_probe"), &CityCore::job_pool_probe);
	ClassDB::bind_method(D_METHOD("render_cost"), &CityCore::render_cost);
	ClassDB::bind_method(D_METHOD("road_mesh_counts"), &CityCore::road_mesh_counts);
	ClassDB::bind_method(D_METHOD("draw_verges", "on"), &CityCore::draw_verges);
	ClassDB::bind_method(D_METHOD("wear_paint", "x0", "z0", "x1", "z1", "value"), &CityCore::wear_paint);
	ClassDB::bind_method(D_METHOD("agent_cost"), &CityCore::agent_cost);
	ClassDB::bind_method(D_METHOD("building_cost"), &CityCore::building_cost);
	ClassDB::bind_method(D_METHOD("pose_stats"), &CityCore::pose_stats);
}

// The edited body's script API: reading, the shape tools, the facade tools, the roof, the gear and the parts.
void CityCore::bind_bodies() {
	ClassDB::bind_method(D_METHOD("body_info", "id"), &CityCore::body_info);
	ClassDB::bind_method(D_METHOD("body_pick", "origin", "dir"), &CityCore::body_pick);
	ClassDB::bind_method(D_METHOD("body_outline", "id", "vol"), &CityCore::body_outline);
	ClassDB::bind_method(D_METHOD("body_storeys", "id", "vol", "count"), &CityCore::body_storeys);
	ClassDB::bind_method(D_METHOD("body_insert_storey", "id", "vol", "at", "above"), &CityCore::body_insert_storey);
	ClassDB::bind_method(D_METHOD("body_resize", "id", "vol", "side", "delta", "snap"), &CityCore::body_resize);
	ClassDB::bind_method(D_METHOD("body_add_wing", "id", "vol", "side", "depth", "length"), &CityCore::body_add_wing);
	ClassDB::bind_method(D_METHOD("body_add_setback", "id", "vol", "inset", "storeys"), &CityCore::body_add_setback);
	ClassDB::bind_method(D_METHOD("body_split", "id", "vol", "level"), &CityCore::body_split);
	ClassDB::bind_method(D_METHOD("body_remove_volume", "id", "vol"), &CityCore::body_remove_volume);
	ClassDB::bind_method(D_METHOD("body_heights", "id", "ground_h", "storey_h"), &CityCore::body_heights);
	ClassDB::bind_method(D_METHOD("body_bay", "id", "vol", "storey", "side", "bay"), &CityCore::body_bay);
	ClassDB::bind_method(D_METHOD("body_set_bay", "id", "vol", "storey", "side", "bay", "what", "scope"), &CityCore::body_set_bay);
	ClassDB::bind_method(D_METHOD("body_set_zone", "id", "vol", "side", "storey0", "storey1", "bay0", "bay1", "what"), &CityCore::body_set_zone);
	ClassDB::bind_method(D_METHOD("body_set_pattern", "id", "vol", "storey", "side", "pattern"), &CityCore::body_set_pattern);
	ClassDB::bind_method(D_METHOD("body_set_face", "id", "vol", "side", "patch"), &CityCore::body_set_face);
	ClassDB::bind_method(D_METHOD("body_set_relief", "id", "vol", "region", "snap"), &CityCore::body_set_relief);
	ClassDB::bind_method(D_METHOD("body_set_roof", "id", "vol", "roof"), &CityCore::body_set_roof);
	ClassDB::bind_method(D_METHOD("body_roof_shape", "id", "vol", "pitch", "ridge", "fall"), &CityCore::body_roof_shape);
	ClassDB::bind_method(D_METHOD("body_paint", "id", "scope", "vol", "side", "storey", "finish", "colour"), &CityCore::body_paint);
	ClassDB::bind_method(D_METHOD("body_add_gear", "id", "vol", "at", "spec"), &CityCore::body_add_gear);
	ClassDB::bind_method(D_METHOD("body_remove_gear", "id", "vol", "gear"), &CityCore::body_remove_gear);
	ClassDB::bind_method(D_METHOD("body_turn_gear", "id", "vol", "gear"), &CityCore::body_turn_gear);
	ClassDB::bind_method(D_METHOD("body_add_part", "id", "vol", "storey", "side", "bay", "spec"), &CityCore::body_add_part);
	ClassDB::bind_method(D_METHOD("body_add_part_at", "id", "at", "spec"), &CityCore::body_add_part_at);
	ClassDB::bind_method(D_METHOD("body_remove_part", "id", "part"), &CityCore::body_remove_part);
	ClassDB::bind_method(D_METHOD("body_turn_part", "id", "part"), &CityCore::body_turn_part);
	ClassDB::bind_method(D_METHOD("body_repeat_part", "id", "part", "spacing"), &CityCore::body_repeat_part);
}

// The Landscape drawer's script API: weather, clouds, fog, painting, trees, elements, biome and the landscape reads.
void CityCore::bind_nature() {
	ClassDB::bind_method(D_METHOD("set_weather", "change"), &CityCore::set_weather);
	ClassDB::bind_method(D_METHOD("add_cloud", "cloud"), &CityCore::add_cloud);
	ClassDB::bind_method(D_METHOD("edit_cloud", "id", "cloud"), &CityCore::edit_cloud);
	ClassDB::bind_method(D_METHOD("remove_cloud", "id"), &CityCore::remove_cloud);
	ClassDB::bind_method(D_METHOD("scatter_clouds", "count", "like", "variation"), &CityCore::scatter_clouds);
	ClassDB::bind_method(D_METHOD("clear_clouds"), &CityCore::clear_clouds);
	ClassDB::bind_method(D_METHOD("cloud_at", "origin", "direction", "drift_x", "drift_z"), &CityCore::cloud_at);
	ClassDB::bind_method(D_METHOD("fog_brush", "position", "radius", "strength", "height", "speed", "erase"), &CityCore::fog_brush);
	ClassDB::bind_method(D_METHOD("clear_fog"), &CityCore::clear_fog);
	ClassDB::bind_method(D_METHOD("paint_brush", "position", "radius", "strength", "hardness", "kind"), &CityCore::paint_brush);
	ClassDB::bind_method(D_METHOD("clear_gullies"), &CityCore::clear_gullies);
	ClassDB::bind_method(D_METHOD("tree_brush", "position", "radius", "mode", "kind", "brush"), &CityCore::tree_brush);
	ClassDB::bind_method(D_METHOD("clear_trees"), &CityCore::clear_trees);
	ClassDB::bind_method(D_METHOD("element_brush", "position", "radius", "kind", "brush", "erase"), &CityCore::element_brush);
	ClassDB::bind_method(D_METHOD("clear_elements"), &CityCore::clear_elements);
	ClassDB::bind_method(D_METHOD("effects_near", "position", "reach", "first_effect"), &CityCore::effects_near);
	ClassDB::bind_method(D_METHOD("set_biome", "region"), &CityCore::set_biome);
	ClassDB::bind_method(D_METHOD("clear_relief"), &CityCore::clear_relief);
	ClassDB::bind_method(D_METHOD("landscape"), &CityCore::landscape);
	ClassDB::bind_method(D_METHOD("sky_revision"), &CityCore::sky_revision);
	ClassDB::bind_method(D_METHOD("fog_field", "camera"), &CityCore::fog_field);
}

} // namespace city
