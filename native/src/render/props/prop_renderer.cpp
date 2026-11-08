// Builds the street furniture chunk meshes and instances.
#include "render/props/prop_renderer.h"

#include "render/lamp_photometry.h"

#include <algorithm>

#include <godot_cpp/variant/string_name.hpp>

using namespace godot;

namespace city {

namespace {
constexpr float NEVER_END = 0.f; // a visibility range without an end
constexpr float HALF_DIAGONAL = 0.7071f; // of a chunk, from its middle to a corner
} // namespace

void PropRenderer::Draws::free() {
	if (cast.is_valid()) RenderingServer::get_singleton()->free_rid(cast);
	cast = RID();
	poles.free();
	pole_shadow_near.free();
	pole_shadow_far.free();
	wires_near.free();
	wires_far.free();
	boxes.free();
	beams.free();
	light.free();
	count = 0;
}

bool PropRenderer::init(const RenderContext &ctx, const StreetLook &look, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("prop", errors);
	glow_material_ = ctx.material("lamp_glow", errors);
	look_ = &look;
	const Dictionary table = ctx.table("streets", errors);
	if (!errors.is_empty()) return false;
	const DataValue t = DataReader{ errors, "streets.json" }.at(table); // render/data_reader.h
	if (!furniture_.load(t, errors) || !barriers_.load(t, errors) || !signs_.load(t, errors) || !kit_.build(t["props"], t["wires"], t["barriers"]["beam_section"], errors)) return false;
	atlas_.configure(signs_.sheet_w, signs_.sheet_h, signs_.cell, signs_.blade_w, signs_.blade_h);
	pole_mesh_ = kit_.pole_levels().bake();
	pole_cast_mesh_ = kit_.pole_cast_levels().bake();
	wire_near_mesh_ = kit_.wire(true).bake();
	wire_far_mesh_ = kit_.wire(false).bake();
	box_mesh_ = kit_.unit_box().bake();
	beam_mesh_ = kit_.w_beam().bake();
	light_mesh_ = kit_.light().bake();
	kit_.drop_baked();
	lamps_.init(scenario_, material_, kit_);
	lights_.init(scenario_, ctx.catalog->tune, RenderingServer::get_singleton()->material_get_param(glow_material_, "glow_color"));
	// The lamp's own light where the lights of the pool do not go, as shader globals every material that draws it reads: its lens
	// (props.gdshader) and the pools the road draws past them (road_body.gdshaderinc), the illuminance under a lamp at the utility
	// pole's height (render/lamp_photometry.h, the one owner of its photometry).
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->global_shader_parameter_set(StringName("street_lamp_lens"), lamp_lens_light(ctx.catalog->tune));
	rs->global_shader_parameter_set(StringName("street_lamp_foot"), lamp_foot_light(ctx.catalog->tune, furniture_.lamp_height));
	return true;
}

void PropRenderer::reset() {
	clear_queue();
	for (auto &kv : chunks_) kv.second.free();
	chunks_.clear();
	for (auto &kv : draws_) kv.second.free();
	draws_.clear();
	preview_.free();
	instances_ = 0;
	lamps_.reset();
	lights_.reset();
	atlas_.reset();
}

// The chunk's mesh and, in the same result, its instances (StreetInstances travels with the mesh it was placed with).
void PropRenderer::build_cpu(const CityState &s, int64_t key, MeshBuilder &mb, StreetInstances &inst) const {
	mb.levels = true;
	std::vector<PropPlace> props;
	place_street_props(s, *look_, furniture_, key, props, inst);
	place_barriers(s, *look_, barriers_, key, inst.boxes, inst.beams);
	place_signs(s, *look_, signs_, atlas_, key, mb);
	for (const PropPlace &p : props) PropKit::append(mb, kit_.mesh(p.kind), p.basis, p.origin);
	for (const SignalHead &h : inst.heads) PropKit::append(mb, kit_.mesh(PropKind::SIGNAL), h.basis, h.origin);
}

void PropRenderer::show(Draws &d, StreetInstances &inst, const AABB &box) {
	instances_ -= d.count;
	d.count = int((inst.poles.size() + inst.wires.size() + inst.light.size()) / MM_PLAIN_STRIDE + (inst.boxes.size() + inst.beams.size()) / MM_STRIDE);
	instances_ += d.count;
	d.poles.set(scenario_, pole_mesh_.rid, material_, inst.poles, box, false, LAYER_FURNITURE, false);
	d.pole_shadow_near.shadow_only = d.pole_shadow_far.shadow_only = true;
	d.pole_shadow_near.set(scenario_, pole_mesh_.rid, material_, inst.poles, box, true, LAYER_FURNITURE, false);
	d.pole_shadow_far.set(scenario_, pole_cast_mesh_.rid, material_, inst.poles, box, true, LAYER_FURNITURE, false);
	d.pole_shadow_near.range(NEVER_END, kit_.cast_from());
	d.pole_shadow_far.range(kit_.cast_from(), shadow_end());
	d.wires_near.set(scenario_, wire_near_mesh_.rid, material_, inst.wires, box, true, LAYER_FURNITURE, false);
	d.wires_far.set(scenario_, wire_far_mesh_.rid, material_, inst.wires, box, false, LAYER_FURNITURE, false);
	d.boxes.set(scenario_, box_mesh_.rid, material_, inst.boxes, box, true, LAYER_FURNITURE);
	d.beams.set(scenario_, beam_mesh_.rid, material_, inst.beams, box, true, LAYER_FURNITURE);
	d.light.set(scenario_, light_mesh_.rid, glow_material_, inst.light, box, false, LAYER_FURNITURE, false);
	d.wires_near.range(NEVER_END, furniture_.wire.near_end);
	d.wires_far.range(furniture_.wire.near_end, furniture_.wire.far_end);
}

// Past where the wires stop being tubes a pole or a bin is thinner than a texel of the farther cascades: its shadow costs the whole shadow pass and shows nothing. Measured by
// the middle of the chunk: the chunks a view stands over cast whole.
float PropRenderer::shadow_end() const {
	return furniture_.wire.near_end + float(CHUNK) * HALF_DIAGONAL;
}

void PropRenderer::cast_chunk(Draws &d, const RID &mesh, const AABB &box) {
	RenderingServer *rs = RenderingServer::get_singleton();
	if (d.cast.is_valid()) rs->free_rid(d.cast);
	d.cast = RID();
	if (!mesh.is_valid()) return;
	d.cast = rs->instance_create2(mesh, scenario_);
	rs->instance_set_layer_mask(d.cast, LAYER_FURNITURE);
	rs->instance_geometry_set_material_override(d.cast, material_);
	rs->instance_geometry_set_cast_shadows_setting(d.cast, RenderingServer::SHADOW_CASTING_SETTING_SHADOWS_ONLY);
	rs->instance_set_custom_aabb(d.cast, box);
	rs->instance_geometry_set_visibility_range(d.cast, 0, shadow_end(), 0, 0, RenderingServer::VISIBILITY_RANGE_FADE_DISABLED);
}

void PropRenderer::commit_one(Built &b) {
	MeshDraw &d = chunks_[b.key];
	const bool any = b.mesh.valid();
	cast_chunk(draws_[b.key], RID(), AABB()); // before the mesh it shows goes
	d.adopt(scenario_, material_, false, b.mesh.release(), LAYER_FURNITURE);
	const float pad = furniture_.wire.max_span, x0 = key_x(b.key) * CHUNK - pad, z0 = key_y(b.key) * CHUNK - pad;
	const AABB box(Vector3(x0, AABB_MIN_Y, z0), Vector3(CHUNK + pad * 2, AABB_H, CHUNK + pad * 2));
	if (any) RenderingServer::get_singleton()->instance_set_custom_aabb(d.inst, box);
	d.visible(any);
	if (any) cast_chunk(draws_[b.key], d.mesh, box);
	StreetInstances &inst = b.extra;
	show(draws_[b.key], inst, box);
	lamps_.set_chunk(b.key, std::move(inst.heads));
	lights_.set_chunk(b.key, std::move(inst.lamps));
}

void PropRenderer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	submit_job(s, pool, JobKey::of(JobLane::PROPS, key), key, priority, [this, &s, key](MeshBuilder &mb, StreetInstances &inst) { build_cpu(s, key, mb, inst); });
}

void PropRenderer::drop_chunk(int64_t key) {
	forget(key);
	lamps_.drop_chunk(key);
	lights_.drop_chunk(key);
	auto dr = draws_.find(key);
	if (dr != draws_.end()) {
		instances_ -= dr->second.count;
		dr->second.free();
		draws_.erase(dr);
	}
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	it->second.free();
	chunks_.erase(it);
}

void PropRenderer::set_pole_preview(const CityState &s, const std::vector<PreviewPole> &poles) {
	StreetInstances inst;
	place_pole_preview(s, *look_, furniture_, poles, inst);
	Vector3 lo, hi;
	for (size_t i = 0; i < poles.size(); i++) {
		const Vector3 &at = poles[i].at;
		lo = i == 0 ? at : Vector3(std::min(lo.x, at.x), std::min(lo.y, at.y), std::min(lo.z, at.z));
		hi = i == 0 ? at : Vector3(std::max(hi.x, at.x), std::max(hi.y, at.y), std::max(hi.z, at.z));
	}
	const float pad = furniture_.wire.max_span;
	const Vector3 room(pad, pad, pad);
	show(preview_, inst, AABB(lo - room, hi - lo + room * 2));
}

} // namespace city
