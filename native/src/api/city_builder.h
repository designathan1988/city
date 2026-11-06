// CityBuilder: the Build drawer's script API, an area of the core (api/city_core.h). The body a player edits (city_core_body.cpp)
// and the models: a body in hand travels as bytes, placed, drawn apart in the stage (city_core_models.cpp). Made and freed by CityCore;
// game/store/city_store.gd reaches it through CityCore.area("builder").
#pragma once

#include <godot_cpp/classes/object.hpp>

namespace city {

struct CityState;
class RenderWorld;

class CityBuilder : public godot::Object {
	GDCLASS(CityBuilder, godot::Object)

public:
	void attach(CityState *state, RenderWorld *render) {
		state_ = state;
		render_ = render;
	}
	godot::Dictionary body_info(int id) const;
	godot::Dictionary body_pick(const godot::Vector3 &origin, const godot::Vector3 &dir) const;
	// Build > Models (city_core_models.cpp): a body in hand travels as bytes.
	godot::Array model_list() const;
	godot::PackedByteArray model_bytes(const godot::String &id) const;
	godot::Dictionary doc_body(const godot::Dictionary &doc) const;
	godot::Dictionary building_bytes(int id) const;
	godot::Vector3 body_size(const godot::PackedByteArray &bytes) const; // width, depth, height
	godot::Dictionary place_fit(const godot::Vector3 &at, const godot::Vector2 &size, double rot) const;
	godot::String place_check(const godot::PackedByteArray &bytes, double x, double z, double rot) const;
	godot::Dictionary place_model(const godot::PackedByteArray &bytes, double x, double z, double rot, int fn, int zone);
	int stage_show(const godot::PackedByteArray &bytes, const godot::RID &scenario, bool ghost);
	void stage_move(int handle, const godot::Transform3D &at);
	void stage_free(int handle);
	int stage_triangles(int handle) const;
	godot::PackedVector3Array body_outline(int id, int vol) const;
	godot::Dictionary body_storeys(int id, int vol, int count);
	godot::Dictionary body_insert_storey(int id, int vol, int at, bool above);
	godot::Dictionary body_resize(int id, int vol, int side, double delta, bool snap);
	godot::Dictionary body_add_wing(int id, int vol, int side, double depth, double length);
	godot::Dictionary body_add_setback(int id, int vol, double inset, int storeys);
	godot::Dictionary body_set_plan(int id, int vol, int shape);
	godot::Dictionary body_add_mass(int id, int vol, const godot::Vector3 &from, const godot::Vector3 &to, int action);
	godot::Dictionary body_set_block_mode(int id, int vol, int mode);
	godot::Dictionary body_extrude_face(int id, int vol, int side, double distance);
	godot::Dictionary body_extrude_block(int id, int vol, int side, double depth);
	godot::Dictionary body_inset_face(int id, int vol, int side, double depth);
	godot::Dictionary body_offset(int id, int vol, double distance);
	godot::Dictionary body_bevel(int id, int vol, double distance, int corner);
	godot::Dictionary body_move_point(int id, int vol, int index, const godot::Vector3 &at, bool snap);
	godot::Dictionary body_change_point(int id, int vol, int index, bool remove);
	godot::Dictionary body_copy_block(int id, int vol);
	godot::Dictionary body_center_block(int id, int vol);
	godot::Dictionary body_turn_block(int id, int vol, double degrees);
	godot::Dictionary body_slide_block(int id, int vol, const godot::Vector3 &from, const godot::Vector3 &to, bool snap);
	godot::Dictionary body_detach(int id, int vol);
	godot::Dictionary body_add_primitive(int id, int vol, int side, int storey, const godot::Vector3 &at, const godot::Dictionary &spec);
	godot::Dictionary drop_primitive(const godot::Vector3 &at, const godot::Dictionary &spec);
	godot::Dictionary body_add_run(int id, int vol, int storey, const godot::PackedVector3Array &points, const godot::Dictionary &spec);
	godot::Dictionary body_paint_at(int id, int vol, int side, int storey) const;
	godot::Dictionary body_add_core(int id, const godot::Vector3 &at, int kind);
	godot::Dictionary body_remove_core(int id, const godot::Vector3 &at);
	godot::Dictionary body_put_piece(int id, int level, const godot::String &kind, const godot::Vector3 &at, double angle);
	godot::Dictionary body_move_piece(int id, int level, int index, const godot::Vector3 &at);
	godot::Dictionary body_turn_piece(int id, int level, int index);
	godot::Dictionary body_remove_piece(int id, int level, const godot::Vector3 &at);
	int body_piece_at(int id, int level, const godot::Vector3 &at) const;
	double body_floor_y(int id, int level) const;
	godot::Dictionary body_mass_rect(int id, const godot::Vector3 &from, const godot::Vector3 &to) const;
	godot::Dictionary body_split(int id, int vol, int level);
	godot::Dictionary body_remove_volume(int id, int vol);
	godot::Dictionary body_heights(int id, double ground_h, double storey_h);
	int body_bay(int id, int vol, int storey, int side, int bay) const;
	godot::Dictionary body_set_bay(int id, int vol, int storey, int side, int bay, int what, int scope);
	godot::Dictionary body_set_zone(int id, int vol, int side, int storey0, int storey1, int bay0, int bay1, int what);
	godot::Dictionary body_set_pattern(int id, int vol, int storey, int side, int pattern);
	godot::Dictionary body_set_face(int id, int vol, int side, const godot::Dictionary &patch);
	godot::Dictionary body_set_relief(int id, int vol, const godot::Dictionary &region, bool snap);
	godot::Dictionary body_set_roof(int id, int vol, int roof);
	godot::Dictionary body_roof_shape(int id, int vol, double pitch, int ridge, int fall);
	godot::Dictionary body_paint(int id, int scope, int vol, int side, int storey, int finish, const godot::String &colour);
	godot::Dictionary body_add_gear(int id, int vol, const godot::Vector3 &at, const godot::Dictionary &spec);
	godot::Dictionary body_remove_gear(int id, int vol, int gear), body_turn_gear(int id, int vol, int gear);
	godot::Dictionary body_set_flag(int id, int vol, const godot::Vector3 &at, int pattern);
	godot::Dictionary body_flag_colour(int id, int vol, const godot::Vector3 &at, int slot, const godot::String &colour);
	godot::Dictionary body_add_part(int id, int vol, int storey, int side, int bay, const godot::Dictionary &spec);
	godot::Dictionary body_add_part_at(int id, const godot::Vector3 &at, const godot::Dictionary &spec);
	godot::Dictionary body_remove_part(int id, int part), body_turn_part(int id, int part);
	godot::Dictionary body_repeat_part(int id, int part, double spacing);

protected:
	static void _bind_methods();

private:
	CityState *state_ = nullptr; // the core's: it outlives this area (CityCore frees its areas first)
	RenderWorld *render_ = nullptr;
};

} // namespace city
