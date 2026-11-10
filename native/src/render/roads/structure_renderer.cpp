// Chunks of the roads' structures: a segment's belong to the chunk of its midpoint, as in the road chunks (road_renderer.cpp).
#include "render/roads/structure_renderer.h"

#include "render/owned_segments.h"
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

void StructureRenderer::build_cpu(const CityState &s, int64_t key, MeshBuilder &mb) const {
	RoadEmit e{ s, *look_, mb };
	each_owned_segment(s, key, [&](int sid) { emit_structures(e, sid); });
}

void StructureRenderer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	submit_job(s, pool, JobKey{ JobLane::STRUCTURES, key }, key, priority, [this, &s, key](MeshBuilder &mb) { build_cpu(s, key, mb); });
}

} // namespace city
