// See ground_seam_layer.h.
#include "render/roads/ground_seam_layer.h"

#include "render/roads/road_cut.h"

using namespace godot;

namespace city {

bool GroundSeamLayer::init(const RenderContext &ctx, const StreetLook &look, RoadShapes &shapes, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("terrain_seam", errors);
	surface_material_ = material_; // every chunk's mesh is made with it (surface_data.h)
	look_ = &look;
	shapes_ = &shapes;
	return errors.is_empty();
}

// The job copies the roads whose paving comes near its chunk and their ground, then builds from the copy with the map let go.
void GroundSeamLayer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	submit_cut(pool, JobKey{ JobLane::SEAMS, key }, key, priority, [this, &s, key]() { return cut_for_seams(s, *look_, key); },
			[this, key](const CityCut &cut, MeshBuilder &mb, SeamResult &result) { emit_ground_seam(RoadEmit{ cut, *look_, mb, *shapes_ }, key, result); });
}

int GroundSeamLayer::refused() const {
	int n = 0;
	for (const auto &kv : refused_) n += kv.second;
	return n;
}

} // namespace city
