// A building's body for the Build tools: what it holds, and its edits (sim/commands/body_ops.h) as script commands.
#include "api/city_core.h"
#include "api/edit_result.h"
#include "core/color.h"
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/buildings/b40/record.h"
#include "sim/commands/body_pick.h"
#include "sim/commands/body_ops.h"
#include "sim/persistence/body_codec.h"

#include <godot_cpp/variant/array.hpp>

using namespace godot;

namespace city {

namespace {
constexpr const char *EDIT_LABEL = "cmd.place.editBuilding";
constexpr int ALPHA_BITS = 8; // parse_hex_color packs RGBA; a body paints in RGB
constexpr float PICK_REACH_M = 20000.f; // past any camera the game has
constexpr int SIZE_AXES = 3;

PartSpec part_spec(const Dictionary &d) {
	PartSpec s;
	Array size = d.get("size", Array());
	s.kind = int(d.get("value", 0));
	s.fit = int(d.get("fit_kind", 0));
	if (size.size() == SIZE_AXES) {
		s.w = float(size[0]);
		s.d = float(size[1]);
		s.h = float(size[2]);
	}
	s.out = float(d.get("standoff_m", 0.0));
	s.rise = float(d.get("rise_m", 0.0));
	s.drop = float(d.get("drop_m", 0.0));
	return s;
}

// A point of the map in the frame of a building's lot.
V2 lot_point(const CityState &s, int id, const Vector3 &at) {
	if (id < 0 || id >= int(s.buildings.items.size())) return V2(0, 0);
	b40::LotFrame f = b40::lot_frame(s.buildings.items[id]);
	V2 rel = V2(at.x, at.z) - f.o;
	return V2(rel.x * f.e0.x + rel.y * f.e0.y, rel.x * f.e1.x + rel.y * f.e1.y);
}

template <typename F>
Dictionary edit(CityState &state, int id, F &&op) {
	return run_edit(state, EDIT_LABEL, [&](CityState &s) {
		return edit_body(s, id, [&](b40::Body &b) { return op(b, s.catalog.tune); });
	});
}
} // namespace

Dictionary CityCore::body_info(int id) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	const CityState &s = *state_;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) return d;
	const Building &rec = s.buildings.items[id];
	b40::Body body;
	b40::record_body(s.buildings.designs, rec, body);
	d["edited"] = rec.body != 0;
	d["digest"] = int64_t(body_digest(body));
	d["problem"] = String(body_problem(s.catalog.tune, body).c_str());
	d["ground_h"] = body.ground_h;
	d["storey_h"] = body.storey_h;
	d["parts"] = int(body.elements.size());
	d["last_part_y"] = body.elements.empty() ? 0.f : body.elements.back().y;
	Array volumes;
	for (const b40::Volume &v : body.volumes) {
		Dictionary e;
		e["x"] = v.x;
		e["y"] = v.y;
		e["w"] = v.w;
		e["d"] = v.d;
		e["base"] = v.base;
		e["storeys"] = v.storeys;
		e["roof"] = int(v.roof);
		Array bays;
		for (int side = 0; side < v.sides(); side++) bays.push_back(b40::bays_of(v, side));
		e["bays"] = bays;
		e["gear"] = int(v.gear.size());
		int flags = 0;
		for (const b40::RoofPart &p : v.gear) flags += p.flag.pattern != b40::FLAG_NONE ? 1 : 0;
		e["flags"] = flags;
		volumes.push_back(e);
	}
	d["volumes"] = volumes;
	return d;
}

Dictionary CityCore::body_pick(const Vector3 &origin, const Vector3 &dir) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	Vector3 n = dir.normalized();
	BodyHit hit = pick_body(*state_, Ray3{ origin.x, origin.y, origin.z, n.x, n.y, n.z }, PICK_REACH_M);
	if (hit.id < 0) return d;
	d["id"] = hit.id;
	d["volume"] = hit.volume;
	d["side"] = hit.side;
	d["storey"] = hit.storey;
	d["level"] = hit.level;
	d["bay"] = hit.bay;
	d["point"] = origin + n * hit.t;
	return d;
}

PackedVector3Array CityCore::body_outline(int id, int vol) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	PackedVector3Array out;
	const CityState &s = *state_;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) return out;
	const Building &rec = s.buildings.items[id];
	b40::Body body;
	b40::record_body(s.buildings.designs, rec, body);
	if (vol < 0 || vol >= int(body.volumes.size())) return out;
	const b40::Volume &v = body.volumes[size_t(vol)];
	b40::LotFrame f = b40::lot_frame(rec);
	float floor = b40::lot_floor(body, rec, f, [&](float x, float z) { return s.terrain.height_at(x, z); });
	for (int level : { v.base, v.top() })
		for (const b40::Point2 &p : b40::ring_of(v)) {
			V2 at = f.at(p.x, p.y);
			out.push_back(Vector3(at.x, floor + body.level_z(level), at.y));
		}
	return out;
}

Dictionary CityCore::body_storeys(int id, int vol, int count) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_set_storeys(b, t, vol, count); });
}

Dictionary CityCore::body_insert_storey(int id, int vol, int at, bool above) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_insert_storey(b, t, vol, at, above); });
}

Dictionary CityCore::body_resize(int id, int vol, int side, double delta, bool snap) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_resize(b, t, vol, side, float(delta), snap); });
}

Dictionary CityCore::body_add_wing(int id, int vol, int side, double depth, double length) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_add_wing(b, t, vol, side, float(depth), float(length)); });
}

Dictionary CityCore::body_add_setback(int id, int vol, double inset, int storeys) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_add_setback(b, t, vol, float(inset), storeys); });
}

Dictionary CityCore::body_split(int id, int vol, int level) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_split_at_floor(b, t, vol, level); });
}

Dictionary CityCore::body_remove_volume(int id, int vol) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_remove_volume(b, t, vol); });
}

Dictionary CityCore::body_heights(int id, double ground_h, double storey_h) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_set_heights(b, t, float(ground_h), float(storey_h)); });
}

int CityCore::body_bay(int id, int vol, int storey, int side, int bay) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) return b40::BAY_NONE;
	b40::Body body;
	b40::record_body(s.buildings.designs, s.buildings.items[id], body);
	if (vol < 0 || vol >= int(body.volumes.size())) return b40::BAY_NONE;
	const b40::Volume &v = body.volumes[size_t(vol)];
	return storey >= 0 && storey < v.storeys ? b40::bay_at(v, storey, side, bay) : int(b40::BAY_NONE);
}

Dictionary CityCore::body_set_bay(int id, int vol, int storey, int side, int bay, int what, int scope) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &) { return op_set_bay(b, vol, storey, side, bay, what, scope); });
}

Dictionary CityCore::body_set_zone(int id, int vol, int side, int storey0, int storey1, int bay0, int bay1, int what) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &) { return op_set_zone(b, vol, side, storey0, storey1, bay0, bay1, what); });
}

Dictionary CityCore::body_set_pattern(int id, int vol, int storey, int side, int pattern) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &) { return op_set_pattern(b, vol, storey, side, pattern); });
}

Dictionary CityCore::body_set_face(int id, int vol, int side, const Dictionary &patch) {
	b40::FaceGeometry f;
	f.window_w = float(patch.get("window_w", 0.0));
	f.window_h = float(patch.get("window_h", 0.0));
	f.bays = int(patch.get("bays", -1));
	f.sill = float(patch.get("sill", -1.0));
	f.pier_w = float(patch.get("pier_w", -1.0));
	f.pier_d = float(patch.get("pier_d", -1.0));
	f.pier_every = int(patch.get("pier_every", -1));
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_set_face(b, t, vol, side, f); });
}

Dictionary CityCore::body_set_relief(int id, int vol, const Dictionary &region, bool snap) {
	b40::Relief r;
	r.side = int(region.get("side", 0));
	r.bay0 = int(region.get("bay0", 0));
	r.bay1 = int(region.get("bay1", 0));
	r.storey0 = int(region.get("storey0", 0));
	r.storey1 = int(region.get("storey1", 0));
	r.depth = float(region.get("depth", 0.0));
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_set_relief(b, t, vol, r, snap); });
}

Dictionary CityCore::body_set_roof(int id, int vol, int roof) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &) { return op_set_roof(b, vol, roof); });
}

Dictionary CityCore::body_roof_shape(int id, int vol, double pitch, int ridge, int fall) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_roof_shape(b, t, vol, float(pitch), ridge, fall); });
}

Dictionary CityCore::body_add_gear(int id, int vol, const Vector3 &at, const Dictionary &spec) {
	PartSpec s = part_spec(spec);
	V2 local = lot_point(*state_, id, at);
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_add_gear(b, t, vol, s.kind, local.x, local.y, s.w, s.d, s.h); });
}

Dictionary CityCore::body_remove_gear(int id, int vol, int gear) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &) { return op_remove_gear(b, vol, gear); });
}

Dictionary CityCore::body_turn_gear(int id, int vol, int gear) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_turn_gear(b, t, vol, gear); });
}

Dictionary CityCore::body_set_flag(int id, int vol, const Vector3 &at, int pattern) {
	V2 local = lot_point(*state_, id, at);
	return edit(*state_, id, [&](b40::Body &b, const Tuning &) { return op_set_flag(b, vol, local.x, local.y, pattern); });
}

Dictionary CityCore::body_flag_colour(int id, int vol, const Vector3 &at, int slot, const String &colour) {
	V2 local = lot_point(*state_, id, at);
	uint32_t rgb = parse_hex_color(colour.utf8().get_data()) >> ALPHA_BITS;
	return edit(*state_, id, [&](b40::Body &b, const Tuning &) { return op_flag_colour(b, vol, local.x, local.y, slot, rgb); });
}

Dictionary CityCore::body_add_part(int id, int vol, int storey, int side, int bay, const Dictionary &spec) {
	PartSpec s = part_spec(spec);
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_add_part(b, t, vol, storey, side, bay, s); });
}

Dictionary CityCore::body_add_part_at(int id, const Vector3 &at, const Dictionary &spec) {
	PartSpec s = part_spec(spec);
	V2 local = lot_point(*state_, id, at);
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_add_part_at(b, t, local.x, local.y, b40::FACE_STREET, s); });
}

Dictionary CityCore::body_remove_part(int id, int part) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &) { return op_remove_part(b, part); });
}

Dictionary CityCore::body_turn_part(int id, int part) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &) { return op_turn_part(b, part); });
}

Dictionary CityCore::body_repeat_part(int id, int part, double spacing) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_repeat_part(b, t, part, float(spacing)); });
}

Dictionary CityCore::body_paint(int id, int scope, int vol, int side, int storey, int finish, const String &colour) {
	uint32_t rgb = parse_hex_color(colour.utf8().get_data()) >> ALPHA_BITS;
	return edit(*state_, id, [&](b40::Body &b, const Tuning &) { return op_paint(b, scope, vol, side, storey, finish, rgb); });
}

} // namespace city
