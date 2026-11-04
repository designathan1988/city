// The ground round the roads, chunk by chunk (render/roads/road_mesh.h verge_ribbon): the road chunks' own streaming, in a
// material of its own, the terrain's grass (material "verge"), which the road shader has no samplers left for.
#pragma once

#include "core/job_pool.h"
#include "render/mesh_builder.h"
#include "render/render_context.h"
#include "render/roads/road_chunks.h"
#include "render/roads/road_look.h"
#include "sim/state/city_state.h"

namespace city {

class VergeRenderer : public RoadChunks {
public:
	bool init(const RenderContext &ctx, const StreetLook &look, godot::PackedStringArray &errors);
	void submit(const CityState &s, int64_t key, JobPool &pool, float priority);

private:
	void build_cpu(const CityState &s, int64_t key, MeshBuilder &mb) const;
	const StreetLook *look_ = nullptr;
};

} // namespace city
