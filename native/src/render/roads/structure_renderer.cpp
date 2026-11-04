// The chunks of what holds a raised road up, streamed with the road chunks: render_world.cpp asks, road_structure.cpp draws.
#include "render/roads/structure_renderer.h"

#include "render/owned_segments.h"
#include "render/roads/road_mesh.h"

using namespace godot;

namespace city {

bool StructureRenderer::init(const RenderContext &ctx, const StreetLook &look, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("road", errors);
	look_ = &look;
	shadows_ = true;
	return errors.is_empty();
}

void StructureRenderer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	submit_job(s, pool, JobKey{ JobLane::STRUCTURES, key }, key, priority, [this, &s, key](MeshBuilder &mb) {
		RoadEmit e{ s, *look_, mb };
		each_owned_segment(s, key, [&](int sid) { emit_structure(e, sid); });
	});
}

} // namespace city
