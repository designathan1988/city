// Road chunk meshes: every segment (by its midpoint) and node of a 256 m chunk meshed into one surface drawn with the
// "road" material; the pieces are in road_segment.cpp, road_junction.cpp and road_markings.cpp. The material's night
// pools repeat every lamp step of the simulation's street furniture (its only owner).
#include "render/roads/road_renderer.h"

#include "render/roads/road_cut.h"
#include "render/roads/road_mesh.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>

using namespace godot;

namespace city {

bool RoadRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("road", errors);
	Dictionary t = ctx.table("streets", errors);
	if (!errors.is_empty()) return false;
	if (!ctx.catalog) {
		errors.push_back("render sem catálogo");
		return false;
	}
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "pool_step", ctx.catalog->furniture.lamps.span_m);
	if (!look_.load(t, errors)) return false;
	look_.ramp_full = ctx.catalog->furniture.crossing.zebra_from_m; // a dropped kerb is the zebra's: its ramp lands where the zebra does
	look_.ramp_flat = ctx.catalog->furniture.crossing.zebra_to_m;
	return true;
}

void RoadChunks::reset() {
	clear_queue();
	for (auto &kv : chunks_) kv.second.surface.free();
	chunks_.clear();
	revision_++;
}

void RoadRenderer::build_cpu(const CityView &s, int64_t key, MeshBuilder &mb) const {
	emit_chunk(RoadEmit{ s, look_, mb }, key);
}

// A chunk draws every segment whose middle it holds, whole: its box is the chunk's column grown to what the job emitted
// (asked only for a chunk that emitted something).
AABB chunk_bounds(int64_t key, float size, const AABB &emitted) {
	const AABB column(Vector3(float(key_x(key)) * size, AABB_MIN_Y, float(key_y(key)) * size), Vector3(size, AABB_H, size));
	return column.merge(emitted);
}

void RoadChunks::commit_one(BuiltMesh &b) {
	Chunk &c = chunks_[b.key];
	const bool any = b.mesh.valid();
	c.surface.adopt(scenario_, material_, casts_, b.mesh.release(), LAYER_ROADS);
	if (any) RenderingServer::get_singleton()->instance_set_custom_aabb(c.surface.inst, chunk_bounds(b.key, CHUNK, b.bounds));
	c.surface.visible(any && drawn_);
	c.made = b.count;
	revision_++;
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

// The job copies its chunk's roads, their ground and the lots that face them (the parking paint keeps off their car gates), then
// builds from the copy with the map let go.
void RoadRenderer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	submit_cut(pool, JobKey{ JobLane::ROADS, key }, key, priority,
			[this, &s, key]() { return cut_for_roads(s, look_, key, CHUNK, CUT_ROADS | CUT_BUILDINGS); },
			[this, key](const CityCut &cut, MeshBuilder &mb) { build_cpu(cut, key, mb); });
}

void RoadChunks::drop_chunk(int64_t key) {
	forget(key);
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	it->second.surface.free();
	chunks_.erase(it);
	revision_++;
}

void RoadRenderer::update_view(const Vector3 &) {}

} // namespace city
