// The far level of the roads: each chunk of a superchunk's segments and nodes into one surface, its vertices tagged with their chunk.
#include "render/roads/far_road_renderer.h"

#include "render/render_layers.h"
#include "render/roads/road_cut.h"
#include "render/roads/road_mesh.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr int CHUNKS_PER_SUPER = int(SUPERCHUNK / CHUNK); // per axis
constexpr float MID = 0.5f;

int64_t super_of(int64_t chunk) { return chunk_of((float(key_x(chunk)) + MID) * CHUNK, (float(key_y(chunk)) + MID) * CHUNK, SUPERCHUNK); }

int bit_of(int64_t chunk, int64_t super) {
	return (key_y(chunk) - key_y(super) * CHUNKS_PER_SUPER) * CHUNKS_PER_SUPER + (key_x(chunk) - key_x(super) * CHUNKS_PER_SUPER);
}
} // namespace

bool FarRoadRenderer::init(const RenderContext &ctx, const StreetLook &look, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("road_fade", errors); // road_far.gdshader, by the road material's fade_shader
	look_ = &look;
	return errors.is_empty();
}

void FarRoadRenderer::reset() {
	clear_queue();
	for (auto &kv : supers_) kv.second.surface.free();
	supers_.clear();
	dirty_.clear();
	folded_ = 0;
	fresh_ = false;
}

void FarRoadRenderer::mark(int64_t chunk) { dirty_.insert(super_of(chunk)); }

void FarRoadRenderer::build_cpu(const CityView &s, int64_t key, MeshBuilder &mb) const {
	const RoadEmit e{ s, *look_, mb, true };
	for (int j = 0; j < CHUNKS_PER_SUPER; j++)
		for (int i = 0; i < CHUNKS_PER_SUPER; i++) {
			const float x0 = float(key_x(key) * CHUNKS_PER_SUPER + i) * CHUNK, z0 = float(key_y(key) * CHUNKS_PER_SUPER + j) * CHUNK;
			const int64_t chunk = chunk_of(x0 + CHUNK * MID, z0 + CHUNK * MID);
			emit_chunk(e, chunk);
			mb.tag(uint8_t(bit_of(chunk, key)));
		}
}

void FarRoadRenderer::offer(const CityState &s, JobPool &pool, const Vector3 &cam, float reach, float drop, std::vector<FrameAdmission::Unit> &out) {
	auto away = [&cam](int64_t key) {
		const float dx = (float(key_x(key)) + MID) * SUPERCHUNK - cam.x, dz = (float(key_y(key)) + MID) * SUPERCHUNK - cam.z;
		return std::sqrt(dx * dx + dz * dz);
	};
	const float within = reach + SUPERCHUNK * MID * std::sqrt(2.f); // from its middle: a superchunk with any ground within reach
	for (auto it = supers_.begin(); it != supers_.end();) {
		if (away(it->first) <= within * drop || busy(it->first)) {
			++it;
			continue;
		}
		it->second.surface.free(); // the eye left: its mesh goes, and it is made again when the eye comes back
		dirty_.insert(it->first);
		it = supers_.erase(it);
	}
	waiting_ = 0;
	for (int64_t key : dirty_) {
		if (busy(key)) continue; // its result lands first; the superchunk stays dirty and is asked again then
		const float d = away(key);
		if (d > within) continue; // out of reach: it waits
		waiting_++;
		out.push_back(FrameAdmission::Unit{ supers_.count(key) ? 1 : 0, d, 1, [this, &s, &pool, key, d]() {
					   dirty_.erase(key);
					   // the job copies its superchunk's roads and their ground, then builds from the copy with the map let go
					   submit_cut(pool, JobKey{ JobLane::ROADS_FAR, key }, key, d, [this, &s, key]() { return cut_for_roads(s, *look_, key, SUPERCHUNK, CUT_ROADS); },
							   [this, key](const CityCut &cut, MeshBuilder &mb) { build_cpu(cut, key, mb); });
				   } });
	}
}

void FarRoadRenderer::commit_one(BuiltMesh &b) {
	Super &sp = supers_[b.key];
	const bool any = b.mesh.valid();
	sp.surface.adopt(scenario_, material_, false, b.mesh.release(), LAYER_ROADS);
	sp.surface.visible(any);
	sp.sent = -1;
	fresh_ = true;
	if (!any) return;
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->instance_set_custom_aabb(sp.surface.inst, chunk_bounds(b.key, SUPERCHUNK, b.bounds));
	rs->instance_geometry_set_shader_parameter(sp.surface.inst, "far_pull", look_->far_pull);
}

void FarRoadRenderer::send(Super &sp) const {
	if (sp.mask == sp.sent || !sp.surface.inst.is_valid()) return;
	RenderingServer::get_singleton()->instance_geometry_set_shader_parameter(sp.surface.inst, "chunk_mask", sp.mask);
	sp.sent = sp.mask;
}

void FarRoadRenderer::fold(const RoadChunks &detail) {
	if (!fresh_ && folded_ == detail.revision()) return;
	folded_ = detail.revision();
	fresh_ = false;
	for (auto &kv : supers_) kv.second.mask = 0;
	detail.each_standing([&](int64_t chunk) {
		const int64_t key = super_of(chunk);
		auto it = supers_.find(key);
		if (it != supers_.end()) it->second.mask |= 1 << bit_of(chunk, key);
	});
	for (auto &kv : supers_) send(kv.second);
}

} // namespace city
