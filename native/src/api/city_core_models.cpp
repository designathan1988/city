// Build > Models for the script side: the catalogue, a body in hand as bytes (a model, a building, a file of the player's), where its
// ghost goes, setting it down and drawing it apart from the city.
#include "api/body_doc.h"
#include "api/city_builder.h"
#include "api/edit_result.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/b40/roof_shape.h"
#include "sim/commands/model_ops.h"
#include "sim/persistence/body_codec.h"
#include "sim/state/city_state.h"
#include "render/render_world.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <algorithm>
#include <cstring>

using namespace godot;

namespace city {

namespace {
constexpr const char *PLACE_LABEL = "cmd.build.model";

PackedByteArray bytes_of(const b40::Body &b) {
	ByteWriter w;
	write_body(w, b);
	PackedByteArray out;
	out.resize(int64_t(w.buf.size()));
	std::memcpy(out.ptrw(), w.buf.data(), w.buf.size());
	return out;
}

bool body_of(const PackedByteArray &bytes, b40::Body &b) {
	ByteReader r;
	r.p = bytes.ptr();
	r.n = size_t(bytes.size());
	return !bytes.is_empty() && read_body(r, b) && r.ok;
}
} // namespace

Array CityBuilder::model_list() const {
	Array out;
	for (const b40::ModelSpec &m : state_->buildings.designs.models.items) {
		Dictionary d;
		d["id"] = String(m.id.c_str());
		d["group"] = String(m.group.c_str());
		d["function"] = m.function;
		d["zone"] = m.zone;
		out.push_back(d);
	}
	return out;
}

Dictionary CityBuilder::doc_body(const Dictionary &doc) const {
	Dictionary out;
	b40::Body b;
	float front_x = 0, front_y = 0;
	int function = 0;
	PackedStringArray errors;
	if (body_from_doc(doc, state_->buildings.designs, b, front_x, front_y, function, errors)) {
		// a document may ask more than a storey takes: read as the 4.0 reads it (serialize.ts height), up to the limit
		const Tuning &t = state_->catalog.tune;
		b.storey_h = std::min(b.storey_h, float(t.build_max_storey_m));
		b.ground_h = std::min(b.ground_h, float(t.build_max_ground_m));
		out["bytes"] = bytes_of(b);
		out["front"] = Vector2(front_x, front_y);
		out["function"] = function;
	}
	out["errors"] = errors;
	return out;
}

PackedByteArray CityBuilder::model_bytes(const String &id) const {
	const b40::Data &data = state_->buildings.designs;
	const b40::ModelSpec *m = data.models.find(std::string(id.utf8().get_data()));
	if (!m) return PackedByteArray();
	b40::Body b;
	model_body(data, *m, b);
	return bytes_of(b);
}

// What a building is, to keep as a model: its body and what it is used for.
Dictionary CityBuilder::building_bytes(int id) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	const CityState &s = *state_;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[size_t(id)].alive) return d;
	const Building &rec = s.buildings.items[size_t(id)];
	b40::Body b = *b40::record_body(s.buildings.designs, rec); // a copy: fit_plan moves it onto its plan
	fit_plan(b);
	d["bytes"] = bytes_of(b);
	d["function"] = rec.fn;
	d["zone"] = rec.zone;
	return d;
}

Vector3 CityBuilder::body_size(const PackedByteArray &bytes) const {
	b40::Body b;
	if (!body_of(bytes, b)) return Vector3();
	float top = 0;
	for (const b40::Volume &v : b.volumes) top = std::max(top, b.level_z(v.top()) + b40::roof_rise(v));
	return Vector3(b.plan.W, b.plan.D, top);
}

Dictionary CityBuilder::place_fit(const Vector3 &at, const Vector2 &size, double rot) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	PlaceSnap p = snap_place(*state_, size.x, size.y, at.x, at.z, rot);
	Dictionary d;
	d["x"] = p.x;
	d["z"] = p.z;
	d["rot"] = p.rot;
	d["kind"] = p.kind;
	return d;
}

String CityBuilder::place_check(const PackedByteArray &bytes, double x, double z, double rot) const {
	b40::Body b;
	if (!body_of(bytes, b)) return "tool.failed";
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return String(place_problem(*state_, b, x, z, rot).c_str());
}

Dictionary CityBuilder::place_hover(const PackedByteArray &bytes, const Vector3 &at, const Vector2 &size, double rot) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const PlaceSnap p = snap_place(s, size.x, size.y, at.x, at.z, rot);
	Dictionary d;
	d["x"] = p.x;
	d["z"] = p.z;
	d["rot"] = p.rot;
	d["kind"] = p.kind;
	d["y"] = s.ready ? s.terrain.height_at(float(p.x), float(p.z)) : 0.f;
	HoverCheck &h = hover_;
	if (!h.made || h.x != p.x || h.z != p.z || h.rot != p.rot || h.revision != s.revision || h.bytes != bytes) {
		b40::Body b;
		h.problem = body_of(bytes, b) ? String(place_problem(s, b, p.x, p.z, p.rot).c_str()) : String("tool.failed");
		h.bytes = bytes;
		h.x = p.x;
		h.z = p.z;
		h.rot = p.rot;
		h.revision = s.revision;
		h.made = true;
	}
	d["problem"] = h.problem;
	return d;
}

Dictionary CityBuilder::place_model(const PackedByteArray &bytes, double x, double z, double rot, int fn, int zone) {
	b40::Body b;
	if (!body_of(bytes, b)) return Dictionary();
	return run_edit(*state_, PLACE_LABEL, [&](CityState &s) { return place_body(s, b, x, z, rot, fn, zone); });
}

int CityBuilder::stage_show(const PackedByteArray &bytes, const RID &scenario, bool ghost) {
	b40::Body b;
	return body_of(bytes, b) ? render_->stage_show(state_->buildings.designs, b, scenario, ghost) : 0;
}

void CityBuilder::stage_move(int handle, const Transform3D &at) { render_->stage().move(handle, at); }
void CityBuilder::stage_free(int handle) { render_->stage().remove(handle); }
int CityBuilder::stage_triangles(int handle) const { return render_->stage().triangles(handle); }


void CityBuilder::_bind_methods() {
	ClassDB::bind_method(D_METHOD("model_list"), &CityBuilder::model_list);
	ClassDB::bind_method(D_METHOD("model_bytes", "id"), &CityBuilder::model_bytes);
	ClassDB::bind_method(D_METHOD("building_bytes", "id"), &CityBuilder::building_bytes);
	ClassDB::bind_method(D_METHOD("body_size", "bytes"), &CityBuilder::body_size);
	ClassDB::bind_method(D_METHOD("place_fit", "at", "size", "rot"), &CityBuilder::place_fit);
	ClassDB::bind_method(D_METHOD("doc_body", "doc"), &CityBuilder::doc_body);
	ClassDB::bind_method(D_METHOD("place_check", "bytes", "x", "z", "rot"), &CityBuilder::place_check);
	ClassDB::bind_method(D_METHOD("place_hover", "bytes", "at", "size", "rot"), &CityBuilder::place_hover);
	ClassDB::bind_method(D_METHOD("place_model", "bytes", "x", "z", "rot", "function", "zone"), &CityBuilder::place_model);
	ClassDB::bind_method(D_METHOD("stage_show", "bytes", "scenario", "ghost"), &CityBuilder::stage_show);
	ClassDB::bind_method(D_METHOD("stage_move", "handle", "at"), &CityBuilder::stage_move);
	ClassDB::bind_method(D_METHOD("stage_free", "handle"), &CityBuilder::stage_free);
	ClassDB::bind_method(D_METHOD("stage_triangles", "handle"), &CityBuilder::stage_triangles);
	ClassDB::bind_method(D_METHOD("body_info", "id"), &CityBuilder::body_info);
	ClassDB::bind_method(D_METHOD("body_pick", "origin", "dir"), &CityBuilder::body_pick);
	ClassDB::bind_method(D_METHOD("body_outline", "id", "vol"), &CityBuilder::body_outline);
	ClassDB::bind_method(D_METHOD("body_at", "id", "x", "y"), &CityBuilder::body_at);
	ClassDB::bind_method(D_METHOD("body_storeys", "id", "vol", "count"), &CityBuilder::body_storeys);
	ClassDB::bind_method(D_METHOD("body_insert_storey", "id", "vol", "at", "above"), &CityBuilder::body_insert_storey);
	ClassDB::bind_method(D_METHOD("body_resize", "id", "vol", "side", "delta", "snap"), &CityBuilder::body_resize);
	ClassDB::bind_method(D_METHOD("body_add_wing", "id", "vol", "side", "depth", "length"), &CityBuilder::body_add_wing);
	ClassDB::bind_method(D_METHOD("body_set_plan", "id", "vol", "shape"), &CityBuilder::body_set_plan);
	ClassDB::bind_method(D_METHOD("body_add_mass", "id", "vol", "from", "to", "action"), &CityBuilder::body_add_mass);
	ClassDB::bind_method(D_METHOD("body_set_block_mode", "id", "vol", "mode"), &CityBuilder::body_set_block_mode);
	ClassDB::bind_method(D_METHOD("body_mass_rect", "id", "from", "to"), &CityBuilder::body_mass_rect);
	ClassDB::bind_method(D_METHOD("body_extrude_face", "id", "vol", "side", "distance"), &CityBuilder::body_extrude_face);
	ClassDB::bind_method(D_METHOD("body_extrude_block", "id", "vol", "side", "depth"), &CityBuilder::body_extrude_block);
	ClassDB::bind_method(D_METHOD("body_inset_face", "id", "vol", "side", "depth"), &CityBuilder::body_inset_face);
	ClassDB::bind_method(D_METHOD("body_offset", "id", "vol", "distance"), &CityBuilder::body_offset);
	ClassDB::bind_method(D_METHOD("body_bevel", "id", "vol", "distance", "corner"), &CityBuilder::body_bevel);
	ClassDB::bind_method(D_METHOD("body_move_point", "id", "vol", "index", "at", "snap"), &CityBuilder::body_move_point);
	ClassDB::bind_method(D_METHOD("body_change_point", "id", "vol", "index", "remove"), &CityBuilder::body_change_point);
	ClassDB::bind_method(D_METHOD("body_copy_block", "id", "vol"), &CityBuilder::body_copy_block);
	ClassDB::bind_method(D_METHOD("body_center_block", "id", "vol"), &CityBuilder::body_center_block);
	ClassDB::bind_method(D_METHOD("body_turn_block", "id", "vol", "degrees"), &CityBuilder::body_turn_block);
	ClassDB::bind_method(D_METHOD("body_slide_block", "id", "vol", "from", "to", "snap"), &CityBuilder::body_slide_block);
	ClassDB::bind_method(D_METHOD("body_detach", "id", "vol"), &CityBuilder::body_detach);
	ClassDB::bind_method(D_METHOD("body_add_primitive", "id", "vol", "side", "storey", "at", "spec"), &CityBuilder::body_add_primitive);
	ClassDB::bind_method(D_METHOD("drop_primitive", "at", "spec"), &CityBuilder::drop_primitive);
	ClassDB::bind_method(D_METHOD("body_handles", "id", "vol", "points", "eye", "pick"), &CityBuilder::body_handles);
	ClassDB::bind_method(D_METHOD("body_draft", "id", "spec"), &CityBuilder::body_draft);
	ClassDB::bind_method(D_METHOD("body_apply", "id", "spec"), &CityBuilder::body_apply);
	ClassDB::bind_method(D_METHOD("building_place", "id", "x", "z", "rot"), &CityBuilder::building_place);
	ClassDB::bind_method(D_METHOD("build_reference", "triangles", "views", "x", "z", "rot", "replaced"), &CityBuilder::build_reference);
	ClassDB::bind_method(D_METHOD("body_add_core", "id", "at", "kind"), &CityBuilder::body_add_core);
	ClassDB::bind_method(D_METHOD("body_add_run", "id", "vol", "storey", "points", "spec"), &CityBuilder::body_add_run);
	ClassDB::bind_method(D_METHOD("body_paint_at", "id", "vol", "side", "storey"), &CityBuilder::body_paint_at);
	ClassDB::bind_method(D_METHOD("body_remove_core", "id", "at"), &CityBuilder::body_remove_core);
	ClassDB::bind_method(D_METHOD("body_put_piece", "id", "level", "kind", "at", "angle"), &CityBuilder::body_put_piece);
	ClassDB::bind_method(D_METHOD("body_move_piece", "id", "level", "index", "at"), &CityBuilder::body_move_piece);
	ClassDB::bind_method(D_METHOD("body_turn_piece", "id", "level", "index"), &CityBuilder::body_turn_piece);
	ClassDB::bind_method(D_METHOD("body_remove_piece", "id", "level", "at"), &CityBuilder::body_remove_piece);
	ClassDB::bind_method(D_METHOD("body_piece_at", "id", "level", "at"), &CityBuilder::body_piece_at);
	ClassDB::bind_method(D_METHOD("body_floor_y", "id", "level"), &CityBuilder::body_floor_y);
	ClassDB::bind_method(D_METHOD("body_piece_size", "kind"), &CityBuilder::body_piece_size);
	ClassDB::bind_method(D_METHOD("body_add_setback", "id", "vol", "inset", "storeys"), &CityBuilder::body_add_setback);
	ClassDB::bind_method(D_METHOD("body_split", "id", "vol", "level"), &CityBuilder::body_split);
	ClassDB::bind_method(D_METHOD("body_remove_volume", "id", "vol"), &CityBuilder::body_remove_volume);
	ClassDB::bind_method(D_METHOD("body_heights", "id", "ground_h", "storey_h"), &CityBuilder::body_heights);
	ClassDB::bind_method(D_METHOD("body_bay", "id", "vol", "storey", "side", "bay"), &CityBuilder::body_bay);
	ClassDB::bind_method(D_METHOD("body_set_bay", "id", "vol", "storey", "side", "bay", "what", "scope"), &CityBuilder::body_set_bay);
	ClassDB::bind_method(D_METHOD("body_set_zone", "id", "vol", "side", "storey0", "storey1", "bay0", "bay1", "what"), &CityBuilder::body_set_zone);
	ClassDB::bind_method(D_METHOD("body_set_pattern", "id", "vol", "storey", "side", "pattern"), &CityBuilder::body_set_pattern);
	ClassDB::bind_method(D_METHOD("body_set_face", "id", "vol", "side", "patch"), &CityBuilder::body_set_face);
	ClassDB::bind_method(D_METHOD("body_set_relief", "id", "vol", "region", "snap"), &CityBuilder::body_set_relief);
	ClassDB::bind_method(D_METHOD("body_set_roof", "id", "vol", "roof"), &CityBuilder::body_set_roof);
	ClassDB::bind_method(D_METHOD("body_roof_shape", "id", "vol", "pitch", "ridge", "fall"), &CityBuilder::body_roof_shape);
	ClassDB::bind_method(D_METHOD("body_paint", "id", "scope", "vol", "side", "storey", "finish", "colour"), &CityBuilder::body_paint);
	ClassDB::bind_method(D_METHOD("body_add_gear", "id", "vol", "at", "spec"), &CityBuilder::body_add_gear);
	ClassDB::bind_method(D_METHOD("body_remove_gear", "id", "vol", "gear"), &CityBuilder::body_remove_gear);
	ClassDB::bind_method(D_METHOD("body_turn_gear", "id", "vol", "gear"), &CityBuilder::body_turn_gear);
	ClassDB::bind_method(D_METHOD("body_set_flag", "id", "vol", "at", "pattern"), &CityBuilder::body_set_flag);
	ClassDB::bind_method(D_METHOD("body_flag_colour", "id", "vol", "at", "slot", "colour"), &CityBuilder::body_flag_colour);
	ClassDB::bind_method(D_METHOD("body_add_part", "id", "vol", "storey", "side", "bay", "spec"), &CityBuilder::body_add_part);
	ClassDB::bind_method(D_METHOD("body_add_part_at", "id", "at", "spec"), &CityBuilder::body_add_part_at);
	ClassDB::bind_method(D_METHOD("body_remove_part", "id", "part"), &CityBuilder::body_remove_part);
	ClassDB::bind_method(D_METHOD("body_turn_part", "id", "part"), &CityBuilder::body_turn_part);
	ClassDB::bind_method(D_METHOD("body_repeat_part", "id", "part", "spacing"), &CityBuilder::body_repeat_part);
}

} // namespace city
