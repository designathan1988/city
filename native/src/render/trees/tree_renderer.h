// Trees streamed around the camera: every 256 m chunk within TreeLook::view_m gets one MultiMesh per species and level
// of detail (near / far by distance), built a few chunks per frame and rebuilt when its roads or lots change.
// Material "prop" (tint parts take INSTANCE_CUSTOM.rgb).
#pragma once

#include "core/job_pool.h"
#include "render/chunk_queue.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "render/trees/tree_place.h"

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

class TreeRenderer {
public:
	bool init(const RenderContext &ctx, const StreetLook &look, const FurnitureLook &furniture, godot::PackedStringArray &errors);
	void reset();
	void mark_dirty(int64_t key) { dirty_.insert(key); }
	void sync(const CityState &s, const godot::Vector3 &cam, BuildBudget &budget);
	void commit(const std::chrono::steady_clock::time_point &until);
	int pending() const;

private:
	struct Chunk {
		std::vector<MultiMeshDraw> near, far;
		bool has_near = false;
	};
	struct Built {
		int64_t key = 0;
		float priority = 0;
		uint64_t seq = 0;
		bool near = false; // the chunk is inside the near range: build that level too (far is always needed)
		std::vector<godot::PackedFloat32Array> bufs;
	};
	Built build_cpu(const CityState &s, int64_t key) const;
	void commit_one(Built &b);
	void submit(const CityState &s, int64_t key, float priority);
	godot::RID scenario_, material_;
	const StreetLook *look_ = nullptr;
	const FurnitureLook *furniture_ = nullptr;
	TreeLook trees_;
	TreeKit kit_;
	std::unordered_map<int64_t, Chunk> chunks_;
	std::unordered_set<int64_t> dirty_;
	mutable std::mutex queue_mutex_;
	std::vector<Built> done_;
	std::unordered_map<int64_t, uint64_t> requested_;
	uint64_t seq_ = 0;
	int pending_ = 0;
};

} // namespace city
