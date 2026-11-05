// Road chunk meshes: every segment (by its midpoint) and node of a 256 m chunk meshed into one surface drawn with the
// "road" material; the pieces are in road_segment.cpp, road_junction.cpp and road_markings.cpp. The material's night
// pools repeat every lamp step of the simulation's street furniture (its only owner).
#include "render/roads/road_renderer.h"

#include "render/owned_segments.h"

#include "render/roads/road_mesh.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>

using namespace godot;

namespace city {

namespace {
constexpr float AABB_MIN_Y = -100.f, AABB_H = 700.f;
} // namespace

bool RoadRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("road", errors);
	Dictionary t = ctx.table("streets", errors);
	if (!errors.is_empty()) return false;
	if (!ctx.catalog) {
		errors.push_back("render sem catálogo");
		return false;
	}
	RenderingServer::get_singleton()->material_set_param(material_, "pool_step", ctx.catalog->furniture.lamps.span_m);
	if (!look_.load(t, errors)) return false;
	look_.ramp_full = ctx.catalog->furniture.crossing.zebra_from_m; // a dropped kerb is the zebra's: its ramp lands where the zebra does
	look_.ramp_flat = ctx.catalog->furniture.crossing.zebra_to_m;
	return true;
}

void RoadChunks::reset() {
	clear_queue();
	for (auto &kv : chunks_) kv.second.surface.free();
	chunks_.clear();
}

void RoadRenderer::build_cpu(const CityState &s, int64_t key, MeshBuilder &mb) const {
	RoadEmit e{ s, look_, mb };
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	each_owned_segment(s, key, [&](int sid) {
		emit_segment(e, sid);
		emit_markings(e, sid);
	});
	s.roads.node_hash.query(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int n) {
		const RoadNode &nd = s.roads.nodes[n];
		if (nd.alive && chunk_of(float(nd.x), float(nd.z)) == key) emit_node(e, n);
	});
}

void RoadChunks::commit_one(BuiltMesh &b) {
	float x0 = key_x(b.key) * CHUNK, z0 = key_y(b.key) * CHUNK;
	Chunk &c = chunks_[b.key];
	const bool any = b.mesh.valid();
	c.surface.adopt(scenario_, material_, false, b.mesh.release());
	if (any) RenderingServer::get_singleton()->instance_set_custom_aabb(c.surface.inst, AABB(Vector3(x0, AABB_MIN_Y, z0), Vector3(CHUNK, AABB_H, CHUNK)));
	c.surface.visible(any && drawn_);
	c.made = b.count;
}

RoadChunks::Count RoadChunks::count() const {
	Count n;
	for (const auto &kv : chunks_) {
		const MeshCount &m = kv.second.made;
		if (m.triangles == 0) continue;
		n.chunks++;
		n.all.triangles += m.triangles;
		n.all.vertices += m.vertices;
		n.all.bytes += m.bytes;
		n.largest = std::max(n.largest, m.bytes);
	}
	return n;
}

void RoadChunks::draw(bool on) {
	drawn_ = on;
	for (auto &kv : chunks_) kv.second.surface.visible(on && kv.second.made.triangles > 0);
}

void RoadRenderer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	submit_job(s, pool, JobKey{ JobLane::ROADS, key }, key, priority, [this, &s, key](MeshBuilder &mb) { build_cpu(s, key, mb); });
}

void RoadChunks::drop_chunk(int64_t key) {
	forget(key);
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	it->second.surface.free();
	chunks_.erase(it);
}

void RoadRenderer::update_view(const Vector3 &) {}

} // namespace city
