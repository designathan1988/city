// Builds the street furniture chunk meshes and instances.
#include "render/props/prop_renderer.h"

#include <algorithm>

using namespace godot;

namespace city {

namespace {
constexpr float AABB_MIN_Y = -100.f, AABB_H = 700.f;
constexpr float NEVER_END = 0.f; // a visibility range without an end
} // namespace

void PropRenderer::Draws::free() {
	poles.free();
	wires_near.free();
	wires_far.free();
	boxes.free();
	beams.free();
	light.free();
	pools.free();
	count = 0;
}

bool PropRenderer::init(const RenderContext &ctx, const StreetLook &look, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("prop", errors);
	glow_material_ = ctx.material("lamp_glow", errors);
	look_ = &look;
	Dictionary t = ctx.table("streets", errors);
	if (!errors.is_empty()) return false;
	if (!t.has("props")) {
		errors.push_back("streets.json: falta props");
		return false;
	}
	if (!furniture_.load(t, errors) || !barriers_.load(t, errors) || !signs_.load(t, errors) || !kit_.build(t["props"], t["wires"], Dictionary(t["barriers"])["beam_section"], errors)) return false;
	atlas_.configure(signs_.sheet_w, signs_.sheet_h, signs_.cell, signs_.blade_w, signs_.blade_h);
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(glow_material_, "pool_far_m", ctx.catalog->tune.render_props_end_m);
	rs->material_set_param(glow_material_, "pool_handover_m", ctx.catalog->tune.render_pool_handover_m);
	pole_mesh_ = kit_.pole().bake();
	wire_near_mesh_ = kit_.wire(true).bake();
	wire_far_mesh_ = kit_.wire(false).bake();
	box_mesh_ = kit_.unit_box().bake();
	beam_mesh_ = kit_.w_beam().bake();
	light_mesh_ = kit_.light().bake();
	kit_.drop_baked();
	lamps_.init(scenario_, material_, kit_);
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
	atlas_.reset();
	std::lock_guard<std::mutex> lk(placed_mutex_);
	placed_.clear();
}

void PropRenderer::build_cpu(const CityState &s, int64_t key, MeshBuilder &mb) const {
	std::vector<PropPlace> props;
	StreetInstances inst;
	place_street_props(s, *look_, furniture_, key, props, inst);
	place_barriers(s, *look_, barriers_, key, inst.boxes, inst.beams);
	place_signs(s, *look_, signs_, atlas_, key, mb);
	for (const PropPlace &p : props) PropKit::append(mb, kit_.mesh(p.kind), p.basis, p.origin);
	for (const SignalHead &h : inst.heads) PropKit::append(mb, kit_.mesh(PropKind::SIGNAL), h.basis, h.origin);
	inst.pool_mesh = inst.pools.bake();
	std::lock_guard<std::mutex> lk(placed_mutex_);
	placed_[key] = std::move(inst);
}

void PropRenderer::show(Draws &d, StreetInstances &inst, const AABB &box) {
	instances_ -= d.count;
	d.count = int((inst.poles.size() + inst.wires.size() + inst.light.size()) / MM_PLAIN_STRIDE + (inst.boxes.size() + inst.beams.size()) / MM_STRIDE);
	instances_ += d.count;
	d.poles.set(scenario_, pole_mesh_.rid, material_, inst.poles, box, true, false);
	d.wires_near.set(scenario_, wire_near_mesh_.rid, material_, inst.wires, box, true, false);
	d.wires_far.set(scenario_, wire_far_mesh_.rid, material_, inst.wires, box, false, false);
	d.boxes.set(scenario_, box_mesh_.rid, material_, inst.boxes, box, true);
	d.beams.set(scenario_, beam_mesh_.rid, material_, inst.beams, box, true);
	d.light.set(scenario_, light_mesh_.rid, glow_material_, inst.light, box, false, false);
	d.pools.adopt(scenario_, glow_material_, false, inst.pool_mesh.release());
	if (d.pools.inst.is_valid()) RenderingServer::get_singleton()->instance_set_custom_aabb(d.pools.inst, box);
	d.wires_near.range(NEVER_END, furniture_.wire.near_end);
	d.wires_far.range(furniture_.wire.near_end, furniture_.wire.far_end);
}

void PropRenderer::commit_one(BuiltMesh &b) {
	MeshDraw &d = chunks_[b.key];
	const bool any = b.mesh.valid();
	d.adopt(scenario_, material_, true, b.mesh.release());
	const float pad = furniture_.wire.max_span, x0 = key_x(b.key) * CHUNK - pad, z0 = key_y(b.key) * CHUNK - pad;
	const AABB box(Vector3(x0, AABB_MIN_Y, z0), Vector3(CHUNK + pad * 2, AABB_H, CHUNK + pad * 2));
	if (any) RenderingServer::get_singleton()->instance_set_custom_aabb(d.inst, box);
	d.visible(any);
	StreetInstances inst;
	{
		std::lock_guard<std::mutex> lk(placed_mutex_);
		auto it = placed_.find(b.key);
		if (it != placed_.end()) {
			inst = std::move(it->second);
			placed_.erase(it);
		}
	}
	show(draws_[b.key], inst, box);
	lamps_.set_chunk(b.key, std::move(inst.heads));
}

void PropRenderer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	submit_job(s, pool, JobKey{ JobLane::PROPS, key }, key, priority, [this, &s, key](MeshBuilder &mb) { build_cpu(s, key, mb); });
}

void PropRenderer::drop_chunk(int64_t key) {
	forget(key);
	lamps_.drop_chunk(key);
	{
		std::lock_guard<std::mutex> lk(placed_mutex_);
		placed_.erase(key);
	}
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

void PropRenderer::set_pole_preview(const CityState &s, const std::vector<Vector3> &at, const std::vector<bool> &lamp, const std::vector<bool> &standing) {
	StreetInstances inst;
	place_pole_preview(s, *look_, furniture_, at, lamp, standing, inst);
	inst.pool_mesh = inst.pools.bake();
	Vector3 lo, hi;
	for (size_t i = 0; i < at.size(); i++) {
		lo = i == 0 ? at[i] : Vector3(std::min(lo.x, at[i].x), std::min(lo.y, at[i].y), std::min(lo.z, at[i].z));
		hi = i == 0 ? at[i] : Vector3(std::max(hi.x, at[i].x), std::max(hi.y, at[i].y), std::max(hi.z, at[i].z));
	}
	const float pad = furniture_.wire.max_span;
	const Vector3 room(pad, pad, pad);
	show(preview_, inst, AABB(lo - room, hi - lo + room * 2));
}

} // namespace city
