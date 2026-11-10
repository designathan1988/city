// Chunks of the ground round the roads: a segment belongs to the chunk of its midpoint and a node to the chunk of its position,
// as in the road chunks (road_renderer.cpp).
#include "render/roads/verge_renderer.h"

#include "render/owned_segments.h"
#include "render/roads/road_cut.h"
#include "render/roads/road_mesh.h"

using namespace godot;

namespace city {

bool VergeRenderer::init(const RenderContext &ctx, const StreetLook &look, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	surface_material_ = ctx.material("verge", errors);
	look_ = &look;
	return errors.is_empty();
}

void VergeRenderer::build_cpu(const CityView &s, int64_t key, MeshBuilder &mb) const {
	RoadEmit e{ s, *look_, mb };
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	each_owned_segment(s, key, [&](int sid) { emit_verge_segment(e, sid); });
	s.roads.node_hash.query(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int n) {
		const RoadNode &nd = s.roads.nodes[n];
		if (nd.alive && chunk_of(float(nd.x), float(nd.z)) == key) emit_verge_node(e, n);
	});
}

void VergeRenderer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	// the job copies its chunk's roads and their ground, then builds from the copy with the map let go
	submit_cut(pool, JobKey{ JobLane::VERGES, key }, key, priority, [this, &s, key]() { return cut_for_roads(s, *look_, key, CHUNK, CUT_ROADS); },
			[this, key](const CityCut &cut, MeshBuilder &mb) { build_cpu(cut, key, mb); });
}

} // namespace city
