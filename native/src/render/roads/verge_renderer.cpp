// Chunks of the ground round the roads: a segment belongs to the chunk of its midpoint and a node to the chunk of its position,
// as in the road chunks (road_renderer.cpp).
#include "render/roads/verge_renderer.h"

#include "render/owned_segments.h"
#include "render/roads/road_mesh.h"

using namespace godot;

namespace city {

bool VergeRenderer::init(const RenderContext &ctx, const StreetLook &look, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("verge", errors);
	look_ = &look;
	return errors.is_empty();
}

void VergeRenderer::build_cpu(const CityState &s, int64_t key, MeshBuilder &mb) const {
	RoadEmit e{ s, *look_, mb };
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	each_owned_segment(s, key, [&](int sid) { emit_verge_segment(e, sid); });
	s.roads.node_hash.query(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int n) {
		const RoadNode &nd = s.roads.nodes[n];
		if (nd.alive && chunk_of(float(nd.x), float(nd.z)) == key) emit_verge_node(e, n);
	});
}

void VergeRenderer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	submit_job(s, pool, JobKey{ JobLane::VERGES, key }, key, priority, [this, &s, key](MeshBuilder &mb) { build_cpu(s, key, mb); });
}

} // namespace city
