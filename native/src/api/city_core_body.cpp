// A building's body for the Build tools: what it holds, and its edits (sim/commands/body_ops.h) as script commands.
#include "sim/buildings/b40/blocks.h"
#include "api/body_api.h"
#include "api/city_builder.h"
#include "core/color.h"
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/buildings/b40/record.h"
#include "sim/commands/body_pick.h"
#include "sim/commands/body_ops.h"
#include "sim/persistence/body_codec.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/array.hpp>

using namespace godot;

namespace city {

namespace {
constexpr int ALPHA_BITS = 8; // parse_hex_color packs RGBA; a body paints in RGB
constexpr float PICK_REACH_M = 20000.f; // past any camera the game has
constexpr int RGB_BASE = 16;
constexpr uint32_t RGB_LEAD = 1u << 24;
} // namespace

Dictionary CityBuilder::body_info(int id) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	const CityState &s = *state_;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) return d;
	const Building &rec = s.buildings.items[id];
	b40::Body body;
	b40::record_body(s.buildings.designs, rec, body);
	d["edited"] = rec.body != 0;
	d["signature"] = int(body.signature);
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
		e["mode"] = int(v.mode);
		e["lift"] = v.lift;
		int flags = 0;
		for (const b40::RoofPart &p : v.gear) flags += p.flag.pattern != b40::FLAG_NONE ? 1 : 0;
		e["flags"] = flags;
		volumes.push_back(e);
	}
	d["volumes"] = volumes;
	b40::resolve_blocks(body);
	d["solids"] = int(body.volumes.size());
	return d;
}

Dictionary CityBuilder::body_pick(const Vector3 &origin, const Vector3 &dir) const {
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

PackedVector3Array CityBuilder::body_outline(int id, int vol) const {
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
			out.push_back(Vector3(at.x, floor + body.block_z(v, level), at.y));
		}
	return out;
}

Vector3 CityBuilder::body_at(int id, float x, float y) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) return Vector3();
	const Building &rec = s.buildings.items[id];
	b40::Body body;
	b40::record_body(s.buildings.designs, rec, body);
	b40::LotFrame f = b40::lot_frame(rec);
	float floor = b40::lot_floor(body, rec, f, [&](float hx, float hz) { return s.terrain.height_at(hx, hz); });
	V2 at = f.at(x, y);
	return Vector3(at.x, floor, at.y);
}

Dictionary CityBuilder::body_storeys(int id, int vol, int count) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_set_storeys(b, t, vol, count); });
}

Dictionary CityBuilder::body_insert_storey(int id, int vol, int at, bool above) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_insert_storey(b, t, vol, at, above); });
}

Dictionary CityBuilder::body_resize(int id, int vol, int side, double delta, bool snap) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_resize(b, t, vol, side, float(delta), snap); });
}

Dictionary CityBuilder::body_add_wing(int id, int vol, int side, double depth, double length) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_add_wing(b, t, vol, side, float(depth), float(length)); });
}

Dictionary CityBuilder::body_add_mass(int id, int vol, const Vector3 &from, const Vector3 &to, int action) {
	return edit_of_body_in_lot(*state_, id, [&](b40::Body &b, const Tuning &t, const b40::LotFrame &f) {
		const V2 a = lot_local(f, from), c = lot_local(f, to);
		return op_add_mass(b, t, vol, { a.x, a.y }, { c.x, c.y }, action);
	});
}

Dictionary CityBuilder::body_set_block_mode(int id, int vol, int mode) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_set_block_mode(b, vol, mode); });
}

// The rectangle a drag makes, square to the lot, as a band for the preview: from the middle of one side to the middle of the other.
Dictionary CityBuilder::body_mass_rect(int id, const Vector3 &from, const Vector3 &to) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	PackedVector3Array points;
	const V2 a = lot_point(*state_, id, from), c = lot_point(*state_, id, to);
	const b40::LotFrame f = b40::lot_frame(state_->buildings.items[id]);
	const float mid = (a.x + c.x) / 2;
	const V2 p = f.at(mid, a.y), q = f.at(mid, c.y);
	points.push_back(Vector3(p.x, from.y, p.y));
	points.push_back(Vector3(q.x, from.y, q.y));
	d["points"] = points;
	d["half_width"] = std::fabs(a.x - c.x) / 2;
	return d;
}

Dictionary CityBuilder::body_set_plan(int id, int vol, int shape) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_set_plan(b, t, vol, shape); });
}

Dictionary CityBuilder::body_add_setback(int id, int vol, double inset, int storeys) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_add_setback(b, t, vol, float(inset), storeys); });
}

Dictionary CityBuilder::body_split(int id, int vol, int level) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_split_at_floor(b, t, vol, level); });
}

Dictionary CityBuilder::body_remove_volume(int id, int vol) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_remove_volume(b, t, vol); });
}

Dictionary CityBuilder::body_heights(int id, double ground_h, double storey_h) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_set_heights(b, t, float(ground_h), float(storey_h)); });
}

int CityBuilder::body_bay(int id, int vol, int storey, int side, int bay) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) return b40::BAY_NONE;
	b40::Body body;
	b40::record_body(s.buildings.designs, s.buildings.items[id], body);
	if (vol < 0 || vol >= int(body.volumes.size())) return b40::BAY_NONE;
	const b40::Volume &v = body.volumes[size_t(vol)];
	return storey >= 0 && storey < v.storeys ? b40::bay_at(v, storey, side, bay) : int(b40::BAY_NONE);
}

Dictionary CityBuilder::body_set_bay(int id, int vol, int storey, int side, int bay, int what, int scope) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_set_bay(b, vol, storey, side, bay, what, scope); });
}

Dictionary CityBuilder::body_set_zone(int id, int vol, int side, int storey0, int storey1, int bay0, int bay1, int what) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_set_zone(b, vol, side, storey0, storey1, bay0, bay1, what); });
}

Dictionary CityBuilder::body_set_pattern(int id, int vol, int storey, int side, int pattern) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_set_pattern(b, vol, storey, side, pattern); });
}

Dictionary CityBuilder::body_set_face(int id, int vol, int side, const Dictionary &patch) {
	b40::FaceGeometry f;
	f.window_w = float(patch.get("window_w", 0.0));
	f.window_h = float(patch.get("window_h", 0.0));
	f.bays = int(patch.get("bays", -1));
	f.sill = float(patch.get("sill", -1.0));
	f.pier_w = float(patch.get("pier_w", -1.0));
	f.pier_d = float(patch.get("pier_d", -1.0));
	f.pier_every = int(patch.get("pier_every", -1));
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_set_face(b, t, vol, side, f); });
}

Dictionary CityBuilder::body_set_relief(int id, int vol, const Dictionary &region, bool snap) {
	b40::Relief r;
	r.side = int(region.get("side", 0));
	r.bay0 = int(region.get("bay0", 0));
	r.bay1 = int(region.get("bay1", 0));
	r.storey0 = int(region.get("storey0", 0));
	r.storey1 = int(region.get("storey1", 0));
	r.depth = float(region.get("depth", 0.0));
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_set_relief(b, t, vol, r, snap); });
}

Dictionary CityBuilder::body_set_roof(int id, int vol, int roof) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_set_roof(b, vol, roof); });
}

Dictionary CityBuilder::body_roof_shape(int id, int vol, double pitch, int ridge, int fall) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_roof_shape(b, t, vol, float(pitch), ridge, fall); });
}

Dictionary CityBuilder::body_add_gear(int id, int vol, const Vector3 &at, const Dictionary &spec) {
	PartSpec s = part_spec(spec);
	return edit_of_body_in_lot(*state_, id, [&](b40::Body &b, const Tuning &t, const b40::LotFrame &f) {
		const V2 local = lot_local(f, at);
		return op_add_gear(b, t, vol, s.kind, local.x, local.y, s.w, s.d, s.h);
	});
}

Dictionary CityBuilder::body_remove_gear(int id, int vol, int gear) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_remove_gear(b, vol, gear); });
}

Dictionary CityBuilder::body_turn_gear(int id, int vol, int gear) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_turn_gear(b, t, vol, gear); });
}

Dictionary CityBuilder::body_set_flag(int id, int vol, const Vector3 &at, int pattern) {
	return edit_of_body_in_lot(*state_, id, [&](b40::Body &b, const Tuning &, const b40::LotFrame &f) {
		const V2 local = lot_local(f, at);
		return op_set_flag(b, vol, local.x, local.y, pattern);
	});
}

Dictionary CityBuilder::body_flag_colour(int id, int vol, const Vector3 &at, int slot, const String &colour) {
	uint32_t rgb = parse_hex_color(colour.utf8().get_data()) >> ALPHA_BITS;
	return edit_of_body_in_lot(*state_, id, [&](b40::Body &b, const Tuning &, const b40::LotFrame &f) {
		const V2 local = lot_local(f, at);
		return op_flag_colour(b, vol, local.x, local.y, slot, rgb);
	});
}

Dictionary CityBuilder::body_add_part(int id, int vol, int storey, int side, int bay, const Dictionary &spec) {
	PartSpec s = part_spec(spec);
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_add_part(b, t, vol, storey, side, bay, s); });
}

Dictionary CityBuilder::body_add_part_at(int id, const Vector3 &at, const Dictionary &spec) {
	PartSpec s = part_spec(spec);
	return edit_of_body_in_lot(*state_, id, [&](b40::Body &b, const Tuning &t, const b40::LotFrame &f) {
		const V2 local = lot_local(f, at);
		return op_add_part_at(b, t, local.x, local.y, b40::FACE_STREET, s);
	});
}

Dictionary CityBuilder::body_add_run(int id, int vol, int storey, const PackedVector3Array &points, const Dictionary &spec) {
	const PartSpec s = part_spec(spec);
	return edit_of_body_in_lot(*state_, id, [&](b40::Body &b, const Tuning &t, const b40::LotFrame &f) {
		std::vector<b40::Point2> path;
		for (const Vector3 &p : points) {
			const V2 at = lot_local(f, p);
			path.push_back({ at.x, at.y });
		}
		const int level = vol >= 0 && vol < int(b.volumes.size()) ? b.volumes[size_t(vol)].base + storey : 0;
		return op_add_run(b, t, path, level, s);
	});
}

// What a face of a block is finished in: the side of its storey, else the block's, else the building's.
Dictionary CityBuilder::body_paint_at(int id, int vol, int side, int storey) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	const CityState &s = *state_;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) return d;
	b40::Body body;
	b40::record_body(s.buildings.designs, s.buildings.items[id], body);
	if (vol < 0 || vol >= int(body.volumes.size())) return d;
	const b40::Volume &v = body.volumes[size_t(vol)];
	auto own = v.storey_paint.find(b40::side_key(storey, side));
	const b40::Paint &p = own != v.storey_paint.end() ? body.paints[size_t(own->second)] : body.wall_of(v);
	d["finish"] = int(p.finish);
	d["rgb"] = "#" + String::num_int64(int64_t(p.rgb | RGB_LEAD), RGB_BASE).substr(1); // the leading digit keeps the zeros of a dark red
	return d;
}

Dictionary CityBuilder::body_remove_part(int id, int part) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_remove_part(b, part); });
}

Dictionary CityBuilder::body_turn_part(int id, int part) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_turn_part(b, part); });
}

Dictionary CityBuilder::body_repeat_part(int id, int part, double spacing) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_repeat_part(b, t, part, float(spacing)); });
}

Dictionary CityBuilder::body_paint(int id, int scope, int vol, int side, int storey, int finish, const String &colour) {
	uint32_t rgb = parse_hex_color(colour.utf8().get_data()) >> ALPHA_BITS;
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_paint(b, scope, vol, side, storey, finish, rgb); });
}

} // namespace city
