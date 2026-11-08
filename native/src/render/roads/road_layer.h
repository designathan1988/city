// A layer drawn chunk by chunk on the road chunks' own streaming, from the road look and the shapes the road jobs share
// (road_shapes.h): one mesh a chunk in a material of its own: the structures, decks, bents, parapets, portals and the walls of
// cuttings in the plain concrete of material "transit" (vertex colour and UV say tone and finish), the only road layer that casts.
// A layer is told apart by its material, its lane in the job pool and what it emits for a chunk. The ground beside the roads, which
// hands the terrain's holes over with its meshes, is a layer of its own (ground_seam_layer.h).
#pragma once

#include "core/job_pool.h"
#include "render/render_context.h"
#include "render/roads/road_chunks.h"
#include "render/roads/road_mesh.h"
#include "sim/state/city_state.h"

namespace city {

class RoadLayer : public RoadChunks {
public:
	using Emit = void (*)(const RoadEmit &e, int64_t key); // what the layer draws of chunk `key`
	RoadLayer(const char *material, bool casts, JobLane lane, Emit emit);
	bool init(const RenderContext &ctx, const StreetLook &look, RoadShapes &shapes, godot::PackedStringArray &errors);
	void submit(const CityState &s, int64_t key, JobPool &pool, float priority);

private:
	const char *material_name_;
	JobLane lane_;
	Emit emit_;
	const StreetLook *look_ = nullptr;
	RoadShapes *shapes_ = nullptr; // the road renderer's
};

// The structures of the segments chunk `key` owns.
void emit_structure_chunk(const RoadEmit &e, int64_t key);

} // namespace city
