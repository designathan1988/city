// Builds the street furniture chunk meshes.
#include "render/props/prop_renderer.h"

#include <algorithm>

using namespace godot;

namespace city {

namespace {
constexpr float AABB_MIN_Y = -100.f, AABB_H = 700.f;
} // namespace

bool PropRenderer::init(const RenderContext &ctx, const StreetLook &look, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("prop", errors);
	look_ = &look;
	Dictionary t = ctx.table("streets", errors);
	if (!errors.is_empty()) return false;
	if (!t.has("props")) {
		errors.push_back("streets.json: falta props");
		return false;
	}
	if (!furniture_.load(t, errors) || !kit_.build(t["props"], errors)) return false;
	lamps_.init(scenario_, material_, kit_);
	return true;
}

void PropRenderer::reset() {
	clear_queue();
	for (auto &kv : chunks_) kv.second.free();
	chunks_.clear();
	lamps_.reset();
	std::lock_guard<std::mutex> lk(heads_mutex_);
	heads_.clear();
}

void PropRenderer::build_cpu(const CityState &s, int64_t key, MeshBuilder &mb) const {
	std::vector<PropPlace> props;
	std::vector<SignalHead> heads;
	place_street_props(s, *look_, furniture_, key, props, mb, heads);
	for (const PropPlace &p : props) PropKit::append(mb, kit_.mesh(p.kind), p.basis, p.origin);
	for (const SignalHead &h : heads) PropKit::append(mb, kit_.mesh(PropKind::SIGNAL), h.basis, h.origin);
	std::lock_guard<std::mutex> lk(heads_mutex_);
	heads_[key] = std::move(heads);
}

void PropRenderer::commit_one(BuiltMesh &b) {
	MeshDraw &d = chunks_[b.key];
	const bool any = b.mesh.valid();
	d.adopt(scenario_, material_, true, b.mesh.release());
	float x0 = key_x(b.key) * CHUNK, z0 = key_y(b.key) * CHUNK;
	if (any) RenderingServer::get_singleton()->instance_set_custom_aabb(d.inst, AABB(Vector3(x0, AABB_MIN_Y, z0), Vector3(CHUNK, AABB_H, CHUNK)));
	d.visible(any);
	std::vector<SignalHead> heads;
	{
		std::lock_guard<std::mutex> lk(heads_mutex_);
		auto it = heads_.find(b.key);
		if (it != heads_.end()) {
			heads = std::move(it->second);
			heads_.erase(it);
		}
	}
	lamps_.set_chunk(b.key, std::move(heads));
}

void PropRenderer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	submit_job(s, pool, key * 4 + 1, key, priority, [this, &s, key](MeshBuilder &mb) { build_cpu(s, key, mb); });
}

void PropRenderer::drop_chunk(int64_t key) {
	forget(key);
	lamps_.drop_chunk(key);
	{
		std::lock_guard<std::mutex> lk(heads_mutex_);
		heads_.erase(key);
	}
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	it->second.free();
	chunks_.erase(it);
}

} // namespace city
