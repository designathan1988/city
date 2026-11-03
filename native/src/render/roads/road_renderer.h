// Road chunk meshes (Roadcraft jobs/roadMesh.ts): each segment's cross-section extruded between its two mouths,
// junction plates with kerb returns filling the nodes (render/roads/road_mesh.h). A segment belongs to the chunk of its midpoint, a node to the chunk of its
// position. Materials: "road" (data/render/materials.json).
#pragma once

#include "core/job_pool.h"
#include "render/mesh_builder.h"
#include "render/render_context.h"
#include "render/roads/road_look.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace city {

class RoadRenderer {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void rebuild_chunk(const CityState &s, int64_t key); // synchronous fallback (full_rebuild)
	void submit(const CityState &s, int64_t key, JobPool &pool, float priority);
	void commit(const std::chrono::steady_clock::time_point &until);
	int pending() const;
	void drop_chunk(int64_t key);
	void update_view(const godot::Vector3 &cam);
	const StreetLook &look() const { return look_; }

private:
	struct Chunk {
		MeshDraw surface;
	};
	struct Built {
		int64_t key = 0;
		float priority = 0;
		uint64_t seq = 0;
		MeshBuilder mb;
	};
	Built build_cpu(const CityState &s, int64_t key) const;
	void commit_one(Built &b);
	godot::RID scenario_, material_;
	StreetLook look_;
	std::unordered_map<int64_t, Chunk> chunks_;
	mutable std::mutex queue_mutex_;
	std::vector<Built> done_;
	std::unordered_map<int64_t, uint64_t> requested_;
	uint64_t seq_ = 0;
};

} // namespace city
