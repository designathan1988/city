// What holds a raised road up, chunk by chunk (road_structure.cpp): the road chunks' own streaming, and shadows, which a road on the
// ground has no need of and a viaduct does.
#pragma once

#include "core/job_pool.h"
#include "render/mesh_builder.h"
#include "render/render_context.h"
#include "render/roads/road_chunks.h"
#include "render/roads/road_look.h"
#include "sim/state/city_state.h"

namespace city {

class StructureRenderer : public RoadChunks {
public:
	bool init(const RenderContext &ctx, const StreetLook &look, godot::PackedStringArray &errors);
	void submit(const CityState &s, int64_t key, JobPool &pool, float priority);

private:
	const StreetLook *look_ = nullptr;
};

} // namespace city
