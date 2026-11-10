// Chunks of the roads' structures: a segment's belong to the chunk of its midpoint, as in the road chunks (road_renderer.cpp).
#include "render/roads/structure_renderer.h"

#include "render/owned_segments.h"
#include "render/roads/road_cut.h"
#include "render/roads/road_mesh.h"

using namespace godot;

namespace city {

bool StructureRenderer::init(const RenderContext &ctx, const StreetLook &look, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("transit", errors);
	casts_ = true;
	look_ = &look;
	return errors.is_empty();
}

void StructureRenderer::build_cpu(const CityView &s, int64_t key, MeshBuilder &mb) const {
	RoadEmit e{ s, *look_, mb };
	each_owned_segment(s, key, [&](int sid) { emit_structures(e, sid); });
}

void StructureRenderer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	// the job copies its chunk's roads and their ground, then builds from the copy with the map let go
	submit_cut(pool, JobKey{ JobLane::STRUCTURES, key }, key, priority, [this, &s, key]() { return cut_for_roads(s, *look_, key, CHUNK, CUT_ROADS); },
			[this, key](const CityCut &cut, MeshBuilder &mb) { build_cpu(cut, key, mb); });
}

} // namespace city
