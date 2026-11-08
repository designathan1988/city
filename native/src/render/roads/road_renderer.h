// Road chunk meshes: each segment's cross-section extruded between its two mouths, junction plates with kerb returns
// filling the nodes (render/roads/road_mesh.h). A segment belongs to the chunk of its midpoint, a node to the chunk of
// its position. Materials: "road" (data/render/materials.json).
#pragma once

#include "core/job_pool.h"
#include "render/mesh_builder.h"
#include "render/render_context.h"
#include "render/roads/road_chunks.h"
#include "render/roads/road_look.h"
#include "render/roads/road_shapes.h"
#include "sim/state/city_state.h"

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace city {

class RoadRenderer : public RoadChunks {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void submit(const CityState &s, int64_t key, JobPool &pool, float priority);
	const StreetLook &look() const { return look_; }
	RoadShapes &shapes() const { return shapes_; } // the nodes' and walls' shapes every road job shares (road_shapes.h)

private:
	void build_cpu(const CityState &s, int64_t key, MeshBuilder &mb) const;
	StreetLook look_;
	mutable RoadShapes shapes_;
};

} // namespace city
