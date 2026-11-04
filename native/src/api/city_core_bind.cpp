// Script bindings of CityCore (names are the GDScript API used by game/store/city_store.gd).
#include "api/city_core.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

namespace city {

void CityCore::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load_catalog", "tables"), &CityCore::load_catalog);
	ClassDB::bind_method(D_METHOD("setup_render", "render_tables", "materials"), &CityCore::setup_render);
	ClassDB::bind_method(D_METHOD("new_map", "map", "seed", "with_city"), &CityCore::new_map);
	ClassDB::bind_method(D_METHOD("mark_saved"), &CityCore::mark_saved);
	ClassDB::bind_method(D_METHOD("set_speed", "speed"), &CityCore::set_speed);
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
	ClassDB::bind_method(D_METHOD("amber_stops", "v", "stop_m", "top", "left", "type"), &CityCore::amber_stops);
	ClassDB::bind_method(D_METHOD("amber_wanted", "kmh"), &CityCore::amber_wanted);
	ClassDB::bind_method(D_METHOD("set_overlay", "name", "on"), &CityCore::set_overlay);
	ClassDB::bind_method(D_METHOD("overlays"), &CityCore::overlays);
	ClassDB::bind_method(D_METHOD("render_pending"), &CityCore::render_pending);
	ClassDB::bind_method(D_METHOD("render_cost"), &CityCore::render_cost);
	ClassDB::bind_method(D_METHOD("agent_cost"), &CityCore::agent_cost);
	ClassDB::bind_method(D_METHOD("pose_stats"), &CityCore::pose_stats);
}

} // namespace city
