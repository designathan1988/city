// Modelling a block as script commands: faces, plans, points, and a block turned, copied, slid or taken out as a building of its own (sim/commands/body_model.cpp).
#include "api/body_api.h"
#include "api/city_builder.h"

#include "sim/buildings/b40/plan_area.h"

using namespace godot;

namespace city {

namespace {
constexpr const char *DETACH_LABEL = "cmd.place.editBuilding", *DROP_LABEL = "cmd.build.lotDrawn";

} // namespace

Dictionary CityBuilder::body_extrude_face(int id, int vol, int side, double distance) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_extrude_face(b, t, vol, side, float(distance)); });
}

Dictionary CityBuilder::body_extrude_block(int id, int vol, int side, double depth) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_extrude_block(b, t, vol, side, float(depth)); });
}

Dictionary CityBuilder::body_inset_face(int id, int vol, int side, double depth) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_inset_face(b, t, vol, side, float(depth)); });
}

Dictionary CityBuilder::body_offset(int id, int vol, double distance) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_offset_block(b, vol, float(distance)); });
}

Dictionary CityBuilder::body_bevel(int id, int vol, double distance, int corner) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_bevel(b, vol, float(distance), corner); });
}

Dictionary CityBuilder::body_move_point(int id, int vol, int index, const Vector3 &at, bool snap) {
	const V2 p = lot_point(*state_, id, at);
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_move_point(b, t, vol, index, p.x, p.y, snap); });
}

Dictionary CityBuilder::body_change_point(int id, int vol, int index, bool remove) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_change_point(b, vol, index, remove); });
}

Dictionary CityBuilder::body_copy_block(int id, int vol) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_copy_block(b, t, vol); });
}

Dictionary CityBuilder::body_center_block(int id, int vol) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_center_block(b, vol); });
}

Dictionary CityBuilder::body_turn_block(int id, int vol, double degrees) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_turn_block(b, vol, float(degrees)); });
}

Dictionary CityBuilder::body_slide_block(int id, int vol, const Vector3 &from, const Vector3 &to, bool snap) {
	const V2 a = lot_point(*state_, id, from), c = lot_point(*state_, id, to);
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_slide_block(b, t, vol, c.x - a.x, c.y - a.y, snap); });
}

Dictionary CityBuilder::body_detach(int id, int vol) {
	return run_edit(*state_, DETACH_LABEL, [&](CityState &s) { return detach_block(s, id, vol); });
}

Dictionary CityBuilder::body_add_primitive(int id, int vol, int side, int storey, const Vector3 &at, const Dictionary &spec) {
	const V2 p = lot_point(*state_, id, at);
	const PrimitiveSpec shape = primitive_of(spec);
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_primitive_on(b, t, vol, side, storey, p.x, p.y, shape); });
}

Dictionary CityBuilder::drop_primitive(const Vector3 &at, const Dictionary &spec) {
	const PrimitiveSpec shape = primitive_of(spec);
	return run_edit(*state_, DROP_LABEL, [&](CityState &s) { return place_primitive(s, at.x, at.z, shape); });
}

} // namespace city
