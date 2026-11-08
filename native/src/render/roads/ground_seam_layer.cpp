// See ground_seam_layer.h.
#include "render/roads/ground_seam_layer.h"

using namespace godot;

namespace city {

bool GroundSeamLayer::init(const RenderContext &ctx, const StreetLook &look, RoadShapes &shapes, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("terrain_seam", errors);
	look_ = &look;
	shapes_ = &shapes;
	return errors.is_empty();
}

void GroundSeamLayer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	submit_job(s, pool, JobKey{ JobLane::SEAMS, key }, key, priority,
			[this, &s, key](MeshBuilder &mb, SeamResult &result) { emit_ground_seam(RoadEmit{ s, *look_, mb, *shapes_ }, key, result); });
}

int GroundSeamLayer::refused() const {
	int n = 0;
	for (const auto &kv : refused_) n += kv.second;
	return n;
}

} // namespace city
