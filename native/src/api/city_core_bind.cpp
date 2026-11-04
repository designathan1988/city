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
	ClassDB::bind_method(D_METHOD("set_day_hour", "hour"), &CityCore::set_day_hour);
	ClassDB::bind_method(D_METHOD("body_info", "id"), &CityCore::body_info);
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
	ClassDB::bind_method(D_METHOD("get_speed"), &CityCore::get_speed);
	ClassDB::bind_method(D_METHOD("set_terrain_path", "path"), &CityCore::set_terrain_path);
	ClassDB::bind_method(D_METHOD("get_terrain_path"), &CityCore::get_terrain_path);
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "terrain_path"), "set_terrain_path", "get_terrain_path");
	ClassDB::bind_method(D_METHOD("step_ticks", "ticks"), &CityCore::step_ticks);

	ClassDB::bind_method(D_METHOD("snap_road", "position", "profile"), &CityCore::snap_road);
	ClassDB::bind_method(D_METHOD("preview_road", "start", "end", "has_control", "control", "profile"), &CityCore::preview_road);
	ClassDB::bind_method(D_METHOD("build_road", "start", "end", "has_control", "control", "profile"), &CityCore::build_road);
	ClassDB::bind_method(D_METHOD("demolish", "position"), &CityCore::demolish);
	ClassDB::bind_method(D_METHOD("paint_zone", "position", "radius", "zone"), &CityCore::paint_zone);
	ClassDB::bind_method(D_METHOD("road_profiles"), &CityCore::road_profiles);
	ClassDB::bind_method(D_METHOD("undo"), &CityCore::undo);
	ClassDB::bind_method(D_METHOD("redo"), &CityCore::redo);
	ClassDB::bind_method(D_METHOD("begin_stroke", "label"), &CityCore::begin_stroke);
	ClassDB::bind_method(D_METHOD("end_stroke"), &CityCore::end_stroke);
	ClassDB::bind_method(D_METHOD("split_road", "position"), &CityCore::split_road);
	ClassDB::bind_method(D_METHOD("upgrade_road", "position", "profile", "whole_street"), &CityCore::upgrade_road);
	ClassDB::bind_method(D_METHOD("roundabout", "position", "radius", "cut"), &CityCore::roundabout);
	ClassDB::bind_method(D_METHOD("preview_roundabout", "position", "radius", "cut"), &CityCore::preview_roundabout);
	ClassDB::bind_method(D_METHOD("segment_at", "position"), &CityCore::segment_at);
	ClassDB::bind_method(D_METHOD("terrain_brush", "position", "radius", "strength", "dt", "mode"), &CityCore::terrain_brush);
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
