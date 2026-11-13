// Script bindings of CityCore (names are the GDScript API used by game/store/city_store.gd).
#include "api/city_core.h"

#include <godot_cpp/core/class_db.hpp>

#include <limits>

using namespace godot;

namespace {
constexpr float HEIGHT_IN_HAND = std::numeric_limits<float>::quiet_NaN(); // a road begun at the height the tool draws at now
}

namespace city {

void CityCore::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load_catalog", "tables"), &CityCore::load_catalog);
	ClassDB::bind_method(D_METHOD("setup_render", "render_tables", "materials"), &CityCore::setup_render);
	ClassDB::bind_method(D_METHOD("new_map", "map", "seed", "with_city", "paused"), &CityCore::new_map, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("mark_saved"), &CityCore::mark_saved);
	ClassDB::bind_method(D_METHOD("set_speed", "speed"), &CityCore::set_speed);
	ClassDB::bind_method(D_METHOD("set_levels", "traffic", "people", "demand"), &CityCore::set_levels);
	ClassDB::bind_method(D_METHOD("set_sim_switch", "name", "on"), &CityCore::set_sim_switch);
	ClassDB::bind_method(D_METHOD("set_detail_scale", "scale"), &CityCore::set_detail_scale);
	ClassDB::bind_method(D_METHOD("set_lamp_lights", "count", "casting", "heads"), &CityCore::set_lamp_lights);
	ClassDB::bind_method(D_METHOD("set_lamp_night", "night"), &CityCore::set_lamp_night);
	ClassDB::bind_method(D_METHOD("set_tree_share", "share"), &CityCore::set_tree_share);
	ClassDB::bind_method(D_METHOD("set_agent_detail", "cabin_m", "cabin_cap", "people_floor"), &CityCore::set_agent_detail);
	ClassDB::bind_method(D_METHOD("set_population", "share"), &CityCore::set_population);
	ClassDB::bind_method(D_METHOD("set_upload_every", "frames"), &CityCore::set_upload_every);
	ClassDB::bind_method(D_METHOD("set_lots_drawn", "on"), &CityCore::set_lots_drawn);
	ClassDB::bind_method(D_METHOD("traffic_metrics"), &CityCore::traffic_metrics);
	ClassDB::bind_method(D_METHOD("set_day_hour", "hour"), &CityCore::set_day_hour);
	bind_damage();
	ClassDB::bind_method(D_METHOD("area", "name"), &CityCore::area);
	ClassDB::bind_method(D_METHOD("get_speed"), &CityCore::get_speed);
	ClassDB::bind_method(D_METHOD("set_terrain_path", "path"), &CityCore::set_terrain_path);
	ClassDB::bind_method(D_METHOD("get_terrain_path"), &CityCore::get_terrain_path);
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "terrain_path"), "set_terrain_path", "get_terrain_path");
	ClassDB::bind_method(D_METHOD("step_ticks", "ticks"), &CityCore::step_ticks);
	bind_edits();
	bind_queries();
}

// The player's commands and previews: roads, zones, undo, the drawers' edits, the buildings drawn and the save.
void CityCore::bind_edits() {
	ClassDB::bind_method(D_METHOD("road_start", "position", "px", "on", "grid", "cell", "offset"), &CityCore::road_start, DEFVAL(0.0), DEFVAL(0.0));
	ClassDB::bind_method(D_METHOD("road_end", "start", "position", "px", "on", "angles", "grid", "cell", "offset"), &CityCore::road_end, DEFVAL(0.0), DEFVAL(0.0));
	ClassDB::bind_method(D_METHOD("road_pieces", "start", "end", "alignment", "has_control", "control", "samples", "profile", "px"), &CityCore::road_pieces);
	ClassDB::bind_method(D_METHOD("preview_road_path", "pieces", "profile", "px", "from_lift"), &CityCore::preview_road_path, DEFVAL(HEIGHT_IN_HAND));
	ClassDB::bind_method(D_METHOD("build_road_path", "pieces", "profile", "px", "from_lift"), &CityCore::build_road_path, DEFVAL(HEIGHT_IN_HAND));
	ClassDB::bind_method(D_METHOD("preview_road", "start", "end", "has_control", "control", "profile", "px", "from_lift"), &CityCore::preview_road, DEFVAL(HEIGHT_IN_HAND));
	ClassDB::bind_method(D_METHOD("build_road", "start", "end", "has_control", "control", "profile", "px", "from_lift"), &CityCore::build_road, DEFVAL(HEIGHT_IN_HAND));
	ClassDB::bind_method(D_METHOD("demolish", "position"), &CityCore::demolish);
	ClassDB::bind_method(D_METHOD("paint_zone", "position", "radius", "zone"), &CityCore::paint_zone);
	ClassDB::bind_method(D_METHOD("paint_zone_path", "points", "radius", "zone"), &CityCore::paint_zone_path);
	ClassDB::bind_method(D_METHOD("road_profiles"), &CityCore::road_profiles);
	ClassDB::bind_method(D_METHOD("road_variant", "base", "lanes", "width", "parking"), &CityCore::road_variant);
	ClassDB::bind_method(D_METHOD("undo"), &CityCore::undo);
	ClassDB::bind_method(D_METHOD("redo"), &CityCore::redo);
	ClassDB::bind_method(D_METHOD("begin_stroke", "label"), &CityCore::begin_stroke);
	ClassDB::bind_method(D_METHOD("end_stroke"), &CityCore::end_stroke);
	ClassDB::bind_method(D_METHOD("split_road", "position"), &CityCore::split_road);
	ClassDB::bind_method(D_METHOD("body_census", "kind"), &CityCore::body_census);
	ClassDB::bind_method(D_METHOD("road_census"), &CityCore::road_census);
	ClassDB::bind_method(D_METHOD("set_node_height", "node", "metres"), &CityCore::set_node_height);
	ClassDB::bind_method(D_METHOD("bend_road", "segment", "height", "at"), &CityCore::bend_road);
	ClassDB::bind_method(D_METHOD("join_roads", "node"), &CityCore::join_roads);
	ClassDB::bind_method(D_METHOD("add_crossing", "position", "kind"), &CityCore::add_crossing);
	ClassDB::bind_method(D_METHOD("remove_crossing", "node"), &CityCore::remove_crossing);
	ClassDB::bind_method(D_METHOD("add_uturn", "position", "toward_b"), &CityCore::add_uturn);
	ClassDB::bind_method(D_METHOD("set_movement_banned", "node", "from", "to", "banned"), &CityCore::set_movement_banned);
	ClassDB::bind_method(D_METHOD("remove_node", "node"), &CityCore::remove_node);
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
	ClassDB::bind_method(D_METHOD("draw_building", "corner_a", "corner_b", "function", "zone", "shape"), &CityCore::draw_building);
	ClassDB::bind_method(D_METHOD("preview_lot", "corner_a", "corner_b"), &CityCore::preview_lot);
	ClassDB::bind_method(D_METHOD("set_building", "id", "function", "floors"), &CityCore::set_building);
	ClassDB::bind_method(D_METHOD("save_city", "path"), &CityCore::save_city);
	ClassDB::bind_method(D_METHOD("save_status"), &CityCore::save_status);
	ClassDB::bind_method(D_METHOD("load_city", "path", "paused"), &CityCore::load_city, DEFVAL(false));
}

// What the UI and the tests read: the snapshot, the ground and its inspector, the traffic, the layers and the render's costs.
void CityCore::bind_queries() {
	ClassDB::bind_method(D_METHOD("snapshot"), &CityCore::snapshot);
	ClassDB::bind_method(D_METHOD("map_revision"), &CityCore::map_revision);
	ClassDB::bind_method(D_METHOD("constants"), &CityCore::constants);
	ClassDB::bind_method(D_METHOD("raycast", "origin", "direction"), &CityCore::raycast);
	ClassDB::bind_method(D_METHOD("height_at", "x", "z"), &CityCore::height_at);
	ClassDB::bind_method(D_METHOD("heights_at", "points"), &CityCore::heights_at);
	ClassDB::bind_method(D_METHOD("inspect", "position"), &CityCore::inspect);
	ClassDB::bind_method(D_METHOD("segment_traffic", "segment"), &CityCore::segment_traffic);
	ClassDB::bind_method(D_METHOD("node_traffic", "node"), &CityCore::node_traffic);
	ClassDB::bind_method(D_METHOD("impossible_nodes"), &CityCore::impossible_nodes);
	ClassDB::bind_method(D_METHOD("clear_reach", "from", "to", "samples"), &CityCore::clear_reach);
	ClassDB::bind_method(D_METHOD("sight_pull", "focus", "eye", "samples", "soft"), &CityCore::sight_pull);
	ClassDB::bind_method(D_METHOD("eye_free", "focus", "eye", "lift", "room"), &CityCore::eye_free);
	ClassDB::bind_method(D_METHOD("eye_floor", "x", "z", "room"), &CityCore::eye_floor);
	ClassDB::bind_method(D_METHOD("highest_roof", "x", "z", "radius"), &CityCore::highest_roof);
	ClassDB::bind_method(D_METHOD("recent_edits", "count"), &CityCore::recent_edits);
	ClassDB::bind_method(D_METHOD("reverse_road", "segment"), &CityCore::reverse_road);
	ClassDB::bind_method(D_METHOD("duplicate_road", "segment"), &CityCore::duplicate_road);
	ClassDB::bind_method(D_METHOD("node_lanes", "node"), &CityCore::node_lanes);
	ClassDB::bind_method(D_METHOD("node_legs", "node"), &CityCore::node_legs);
	ClassDB::bind_method(D_METHOD("set_approach_rule", "node", "seg", "rule"), &CityCore::set_approach_rule);
	ClassDB::bind_method(D_METHOD("toggle_lane_link", "node", "from", "from_lane", "to", "to_lane"), &CityCore::toggle_lane_link);
	ClassDB::bind_method(D_METHOD("road_spec", "profile"), &CityCore::road_spec);
	ClassDB::bind_method(D_METHOD("road_spec_limits"), &CityCore::road_spec_limits);
	ClassDB::bind_method(D_METHOD("road_spec_issues", "base", "spec"), &CityCore::road_spec_issues);
	ClassDB::bind_method(D_METHOD("road_spec_make", "base", "spec"), &CityCore::road_spec_make);
	ClassDB::bind_method(D_METHOD("lot_add_rect", "a", "b", "angle"), &CityCore::lot_add_rect);
	ClassDB::bind_method(D_METHOD("lot_add_polygon", "points"), &CityCore::lot_add_polygon);
	ClassDB::bind_method(D_METHOD("lot_land", "a", "b", "angle"), &CityCore::lot_land);
	ClassDB::bind_method(D_METHOD("lot_proposals", "point"), &CityCore::lot_proposals);
	ClassDB::bind_method(D_METHOD("lot_snap_point", "point", "reach", "apart"), &CityCore::lot_snap_point);
	ClassDB::bind_method(D_METHOD("lot_move_corner", "from", "to"), &CityCore::lot_move_corner);
	ClassDB::bind_method(D_METHOD("lot_curve_side", "a", "b", "through"), &CityCore::lot_curve_side);
	ClassDB::bind_method(D_METHOD("lot_set_front", "id", "side"), &CityCore::lot_set_front);
	ClassDB::bind_method(D_METHOD("lot_split", "id", "kind", "parts", "a", "b"), &CityCore::lot_split);
	ClassDB::bind_method(D_METHOD("lot_join", "a", "b"), &CityCore::lot_join);
	ClassDB::bind_method(D_METHOD("lot_delete", "id"), &CityCore::lot_delete);
	ClassDB::bind_method(D_METHOD("lot_zone", "id", "zone"), &CityCore::lot_zone);
	ClassDB::bind_method(D_METHOD("lot_at", "point"), &CityCore::lot_at);
	ClassDB::bind_method(D_METHOD("lot_angle_at", "point"), &CityCore::lot_angle_at);
	ClassDB::bind_method(D_METHOD("lots"), &CityCore::lots);
	ClassDB::bind_method(D_METHOD("lot_cut_lines", "id", "kind", "parts", "a", "b"), &CityCore::lot_cut_lines);
	ClassDB::bind_method(D_METHOD("build_blocks", "at", "cols", "rows", "block_m", "angle", "profile"), &CityCore::build_blocks);
	ClassDB::bind_method(D_METHOD("block_lines", "at", "cols", "rows", "block_m", "angle", "step_m"), &CityCore::block_lines);
	ClassDB::bind_method(D_METHOD("set_marking_style", "style", "undoable"), &CityCore::set_marking_style);
	ClassDB::bind_method(D_METHOD("segment_path", "segment"), &CityCore::segment_path);
	ClassDB::bind_method(D_METHOD("probe_building", "id"), &CityCore::probe_building);
	ClassDB::bind_method(D_METHOD("list_buildings"), &CityCore::list_buildings);
	ClassDB::bind_method(D_METHOD("city_bounds"), &CityCore::city_bounds);
	ClassDB::bind_method(D_METHOD("film_grade", "grade"), &CityCore::film_grade);
	ClassDB::bind_method(D_METHOD("film_graded", "grade", "colour"), &CityCore::film_graded);
	ClassDB::bind_method(D_METHOD("traffic_probe", "sample"), &CityCore::traffic_probe);
	ClassDB::bind_method(D_METHOD("set_signal", "node", "fixed", "greens", "offset"), &CityCore::set_signal);
	ClassDB::bind_method(D_METHOD("green_wave", "node"), &CityCore::green_wave);
	ClassDB::bind_method(D_METHOD("set_overlay", "name", "on"), &CityCore::set_overlay);
	ClassDB::bind_method(D_METHOD("show_every_zone", "on"), &CityCore::show_every_zone);
	ClassDB::bind_method(D_METHOD("set_style", "id"), &CityCore::set_style);
	ClassDB::bind_method(D_METHOD("overlays"), &CityCore::overlays);
	ClassDB::bind_method(D_METHOD("set_interior", "on", "target", "level", "dir"), &CityCore::set_interior);
	ClassDB::bind_method(D_METHOD("interior"), &CityCore::interior);
	ClassDB::bind_method(D_METHOD("interior_revision"), &CityCore::interior_revision);
	ClassDB::bind_method(D_METHOD("interior_cost", "id", "level", "dir"), &CityCore::interior_cost);
	ClassDB::bind_method(D_METHOD("render_pending"), &CityCore::render_pending);
	ClassDB::bind_method(D_METHOD("building_chunk", "x", "z"), &CityCore::building_chunk);
	ClassDB::bind_method(D_METHOD("render_cost"), &CityCore::render_cost);
	ClassDB::bind_method(D_METHOD("wear_probe"), &CityCore::wear_probe);
	ClassDB::bind_method(D_METHOD("road_mesh_counts"), &CityCore::road_mesh_counts);
	ClassDB::bind_method(D_METHOD("draw_verges", "on"), &CityCore::draw_verges);
	ClassDB::bind_method(D_METHOD("wear_paint", "x0", "z0", "x1", "z1", "value"), &CityCore::wear_paint);
	ClassDB::bind_method(D_METHOD("agent_cost"), &CityCore::agent_cost);
	ClassDB::bind_method(D_METHOD("building_cost"), &CityCore::building_cost);
	ClassDB::bind_method(D_METHOD("pose_stats"), &CityCore::pose_stats);
}

// The bomb and the pistol's script API: blows, shots, boxes knocked down and what the tests read of the damage.
void CityCore::bind_damage() {
	ClassDB::bind_method(D_METHOD("strike", "position", "strength"), &CityCore::strike);
	ClassDB::bind_method(D_METHOD("shoot", "origin", "direction"), &CityCore::shoot);
	ClassDB::bind_method(D_METHOD("demolish_box", "a", "b"), &CityCore::demolish_box);
	ClassDB::bind_method(D_METHOD("damage_probe"), &CityCore::damage_probe);
	ClassDB::bind_method(D_METHOD("shot_targets", "limit"), &CityCore::shot_targets);
	ClassDB::bind_method(D_METHOD("effects_shake"), &CityCore::effects_shake);
	ClassDB::bind_method(D_METHOD("effects_since_blast"), &CityCore::effects_since_blast);
	ClassDB::bind_method(D_METHOD("effects_ruin_pieces"), &CityCore::effects_ruin_pieces);
	ClassDB::bind_method(D_METHOD("ruin_seeds"), &CityCore::ruin_seeds);
	ClassDB::bind_method(D_METHOD("effects_ruin_flying"), &CityCore::effects_ruin_flying);
	ClassDB::bind_method(D_METHOD("effects_ruin_digest", "seed"), &CityCore::effects_ruin_digest);
	ClassDB::bind_method(D_METHOD("effects_bodies"), &CityCore::effects_bodies);
	ClassDB::bind_method(D_METHOD("effects_bodies_moving"), &CityCore::effects_bodies_moving);
	ClassDB::bind_method(D_METHOD("effects_bodies_kept"), &CityCore::effects_bodies_kept);
	ClassDB::bind_method(D_METHOD("effects_body_places"), &CityCore::effects_body_places);
	ClassDB::bind_method(D_METHOD("effects_bodies_from_drawn"), &CityCore::effects_bodies_from_drawn);
	ClassDB::bind_method(D_METHOD("effects_body_pieces"), &CityCore::effects_body_pieces);
	ClassDB::bind_method(D_METHOD("effects_bodies_deepest"), &CityCore::effects_bodies_deepest);
	ClassDB::bind_method(D_METHOD("blast_probe", "strength", "distance"), &CityCore::blast_probe);
}

} // namespace city
