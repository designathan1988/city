// The Build drawer's Selection panel: the precise numbers of what is picked (a block, a face, a free part, a thing on a roof) and
// the edits a typed number makes (sim/commands/body_ops.h), each one step of undo; and the whole-building edits of the Builder's
// selection: copied beside itself, mirrored, turned, given a use, a level's own height, two blocks made one, an outline drawn.
#include "api/body_api.h"
#include "api/city_builder.h"
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/b40/roof_shape.h"
#include "sim/commands/body_ops.h"
#include "sim/state/city_state.h"

#include <godot_cpp/core/class_db.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

using namespace godot;

namespace city {

namespace {
// A face's composed proportions, as the panel shows them.
Dictionary face_fields(const b40::Volume &v, int side, int bay, int storey) {
	Dictionary f;
	const b40::FaceGeometry &g = v.faces[side];
	f["bays"] = b40::bays_of(v, side);
	f["window_w"] = g.window_w;
	f["window_h"] = g.window_h;
	f["sill"] = g.sill;
	f["pier_w"] = g.pier_w;
	f["pier_d"] = g.pier_d;
	f["pier_every"] = g.pier_every;
	if (bay >= 0 && storey >= 0) f["depth"] = b40::relief_depth(v, side, bay, storey); // the bay picked: how far it stands from the wall
	return f;
}
} // namespace

// What the Selection panel shows of a pick: the building's heights; the block's size, place, levels, plan area and roof; the face's
// proportions (`side` >= 0) and the relief of its bay (`bay`, `storey`); the free part's size (`part` >= 0) or the roof thing's height (`gear` >= 0). {} when the pick is gone.
Dictionary CityBuilder::body_fields(int id, int vol, int side, int bay, int storey, int part, int gear) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	const CityState &s = *state_;
	const std::shared_ptr<const b40::Body> held = body_of_record(s, id);
	if (!held) return d;
	const b40::Body &body = *held;
	d["ground_h"] = body.ground_h;
	d["storey_h"] = body.storey_h;
	d["function"] = int(s.buildings.items[size_t(id)].fn);
	d["zone"] = int(s.buildings.items[size_t(id)].zone);
	if (vol < 0 || vol >= int(body.volumes.size())) return d;
	const b40::Volume &v = body.volumes[size_t(vol)];
	Dictionary block;
	block["storeys"] = v.storeys;
	block["w"] = v.w;
	block["d"] = v.d;
	block["base"] = v.base;
	block["lift"] = v.lift;
	block["mode"] = int(v.mode);
	block["area"] = std::fabs(b40::ring_area(b40::ring_of(v)));
	const b40::Volume *r = reference_of(body, v);
	if (r != nullptr) block["offset"] = Vector2(v.x + v.w / 2 - r->x - r->w / 2, v.y + v.d / 2 - r->y - r->d / 2);
	const b40::RoofShape &K = s.buildings.designs.shapes.roof;
	block["roof"] = int(v.roof);
	block["pitch"] = b40::roof_pitch_deg(K, v);
	block["ridge_x"] = b40::ridge_along_x(v);
	block["ridge"] = v.ridge;
	block["fall"] = v.fall;
	const int level = v.base + std::clamp(storey, 0, v.storeys - 1); // the level picked: its own height, or the building's
	block["level"] = level;
	block["level_h"] = body.level_h(level);
	d["block"] = block;
	if (side >= 0 && side < v.sides()) d["face"] = face_fields(v, side, bay, storey);
	if (part >= 0 && part < int(body.elements.size())) {
		const b40::LotElement &e = body.elements[size_t(part)];
		const bool front = e.facing == b40::FACE_STREET || e.facing == b40::FACE_BACK;
		Dictionary p;
		p["kind"] = int(e.kind);
		p["across"] = front ? e.w : e.d;
		p["along"] = front ? e.d : e.w;
		p["h"] = e.h;
		d["part"] = p;
	}
	if (gear >= 0 && gear < int(v.gear.size())) {
		Dictionary g;
		g["kind"] = int(v.gear[size_t(gear)].kind);
		g["h"] = v.gear[size_t(gear)].h;
		d["gear"] = g;
	}
	return d;
}

Dictionary CityBuilder::body_block_size(int id, int vol, int axis, double metres) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_block_size(b, t, vol, axis, float(metres)); });
}

Dictionary CityBuilder::body_block_offset(int id, int vol, int axis, double metres) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_block_offset(b, vol, axis, float(metres)); });
}

Dictionary CityBuilder::body_block_base(int id, int vol, int level) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_block_base(b, t, vol, level); });
}

Dictionary CityBuilder::body_size_part(int id, int part, int axis, double metres) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_size_part(b, t, part, axis, float(metres)); });
}

Dictionary CityBuilder::body_size_gear(int id, int vol, int gear, double metres) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_size_gear(b, t, vol, gear, float(metres)); });
}

Dictionary CityBuilder::building_duplicate(int id) {
	return run_edit(*state_, "cmd.place.editBuilding", [&](CityState &s) { return duplicate_building(s, id); });
}

Dictionary CityBuilder::body_mirror(int id) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_mirror(b); });
}

Dictionary CityBuilder::body_union(int id, int vol) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_union_blocks(b, t, vol); });
}

Dictionary CityBuilder::body_level_height(int id, int level, double metres) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_set_level_height(b, t, level, float(metres)); });
}

Dictionary CityBuilder::body_add_mass_ring(int id, int vol, const PackedVector3Array &points, int action) {
	return edit_of_body_in_lot(*state_, id, [&](b40::Body &b, const Tuning &t, const b40::LotFrame &f) {
		std::vector<V2> ring;
		for (const Vector3 &p : points) ring.push_back(lot_local(f, p));
		return op_add_mass_ring(b, t, vol, std::move(ring), action);
	});
}

Dictionary CityBuilder::building_turn(int id, double radians) {
	return run_edit(*state_, "cmd.place.editBuilding", [&](CityState &s) { return turn_building(s, id, radians); });
}

Dictionary CityBuilder::building_function(int id, int fn) {
	return run_edit(*state_, "cmd.place.editBuilding", [&](CityState &s) { return set_function(s, id, fn); });
}

Dictionary CityBuilder::place_sketch(const PackedVector3Array &points) {
	std::vector<V2> outline;
	for (const Vector3 &p : points) outline.push_back({ p.x, p.z });
	return run_edit(*state_, "cmd.place.editBuilding", [&](CityState &s) { return city::place_sketch(s, outline); });
}

// The script names of the Selection panel's and the whole-building edits (CityBuilder::_bind_methods calls it).
void CityBuilder::bind_selection() {
	ClassDB::bind_method(D_METHOD("body_fields", "id", "vol", "side", "bay", "storey", "part", "gear"), &CityBuilder::body_fields);
	ClassDB::bind_method(D_METHOD("body_block_size", "id", "vol", "axis", "metres"), &CityBuilder::body_block_size);
	ClassDB::bind_method(D_METHOD("body_block_offset", "id", "vol", "axis", "metres"), &CityBuilder::body_block_offset);
	ClassDB::bind_method(D_METHOD("body_block_base", "id", "vol", "level"), &CityBuilder::body_block_base);
	ClassDB::bind_method(D_METHOD("body_size_part", "id", "part", "axis", "metres"), &CityBuilder::body_size_part);
	ClassDB::bind_method(D_METHOD("body_size_gear", "id", "vol", "gear", "metres"), &CityBuilder::body_size_gear);
	ClassDB::bind_method(D_METHOD("building_duplicate", "id"), &CityBuilder::building_duplicate);
	ClassDB::bind_method(D_METHOD("body_mirror", "id"), &CityBuilder::body_mirror);
	ClassDB::bind_method(D_METHOD("body_union", "id", "vol"), &CityBuilder::body_union);
	ClassDB::bind_method(D_METHOD("body_level_height", "id", "level", "metres"), &CityBuilder::body_level_height);
	ClassDB::bind_method(D_METHOD("body_add_mass_ring", "id", "vol", "points", "action"), &CityBuilder::body_add_mass_ring);
	ClassDB::bind_method(D_METHOD("building_turn", "id", "radians"), &CityBuilder::building_turn);
	ClassDB::bind_method(D_METHOD("building_function", "id", "fn"), &CityBuilder::building_function);
	ClassDB::bind_method(D_METHOD("place_sketch", "points"), &CityBuilder::place_sketch);
}

} // namespace city
