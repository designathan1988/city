// A road layer's chunks (road_layer.h): a segment belongs to the chunk of its midpoint, as in the road chunks (road_renderer.cpp).
// The verge ribbon this file once emitted is gone: the terrain meets the paving's edge itself, through the ground seams
// (ground_seam.cpp, drawn by ground_seam_layer.h, which also holes the terrain under them).
#include "render/roads/road_layer.h"

#include "render/owned_segments.h"

using namespace godot;

namespace city {

RoadLayer::RoadLayer(const char *material, bool casts, JobLane lane, Emit emit) : material_name_(material), lane_(lane), emit_(emit) {
	casts_ = casts;
}

bool RoadLayer::init(const RenderContext &ctx, const StreetLook &look, RoadShapes &shapes, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material(material_name_, errors);
	surface_material_ = material_; // every chunk's mesh is made with it (surface_data.h)
	look_ = &look;
	shapes_ = &shapes;
	return errors.is_empty();
}

void RoadLayer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	submit_job(s, pool, JobKey{ lane_, key }, key, priority, [this, &s, key](MeshBuilder &mb) { emit_(RoadEmit{ s, *look_, mb, *shapes_ }, key); });
}

void emit_structure_chunk(const RoadEmit &e, int64_t key) {
	each_owned_segment(e.s, key, [&](int sid) { emit_structures(e, sid); });
}

} // namespace city
