// Builds the street furniture chunk meshes.
#include "render/props/prop_renderer.h"

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
	return furniture_.load(t, errors) && kit_.build(t["props"], errors);
}

void PropRenderer::reset() {
	for (auto &kv : chunks_) kv.second.free();
	chunks_.clear();
}

void PropRenderer::rebuild_chunk(const CityState &s, int64_t key) {
	MeshBuilder mb;
	std::vector<PropPlace> props;
	place_street_props(s, *look_, furniture_, key, props, mb);
	for (const PropPlace &p : props) PropKit::append(mb, kit_.mesh(p.kind), p.basis, p.origin);
	MeshDraw &d = chunks_[key];
	d.reset(scenario_, material_, true);
	if (!mb.empty()) mb.commit_to(d.mesh);
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	RenderingServer::get_singleton()->instance_set_custom_aabb(d.inst, AABB(Vector3(x0, AABB_MIN_Y, z0), Vector3(CHUNK, AABB_H, CHUNK)));
	d.visible(!mb.empty());
}

void PropRenderer::drop_chunk(int64_t key) {
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	it->second.free();
	chunks_.erase(it);
}

} // namespace city
