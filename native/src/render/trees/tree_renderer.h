// Trees streamed around the camera at two levels. Near: every 256 m chunk within TreeLook::near_m (plus its fade band)
// draws each species as two MultiMeshes over the same instances, its body (casting shadows) and its cards (materials
// "vegetation" and "vegetation_cards"). Far: every 1024 m superchunk within TreeLook::far_m draws one MultiMesh of far
// cards (the woods by cell, the lone, park and street trees one by one), baked as one mesh (render/trees/far_cards.h). The two meet at near_m: the vertex shader
// fades each tree in or out over fade_m by its own distance to the main camera (render/shaders/vegetation.gdshaderinc).
// Chunks are built by jobs and rebuilt when their roads or lots change.
#pragma once

#include "core/job_pool.h"
#include "render/chunk_queue.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "render/trees/far_cards.h"
#include "render/trees/garden_place.h"
#include "render/trees/tree_place.h"

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

class TreeRenderer {
public:
	bool init(const RenderContext &ctx, const StreetLook &look, godot::PackedStringArray &errors);
	void reset();
	void mark_dirty(int64_t key);
	void set_share(float share); // the graphics tier: every drawn chunk is placed again
	int near_trees() const; // trees of the near level now
	int near_plants() const; // of them, the gardens' bushes and hedges
	void sync(const CityState &s, const godot::Vector3 &cam, BuildBudget &budget);
	void commit(const std::chrono::steady_clock::time_point &until);
	int pending() const;

private:
	struct Near {
		std::vector<MultiMeshDraw> body, cast, cards;
		int trees = 0, plants = 0; // every instance, and those of the gardens' bushes and hedges
	};
	struct Built {
		int64_t key = 0;
		float priority = 0;
		uint64_t seq = 0;
		std::vector<godot::PackedFloat32Array> bufs; // near: one per species
		OwnedMesh far; // far: the superchunk's cards
	};
	struct FarDraw {
		godot::RID inst, mesh;
		void free();
	};
	struct Tier {
		std::unordered_set<int64_t> dirty;
		std::vector<Built> done;
		std::unordered_map<int64_t, uint64_t> requested;
	};
	Built build_near(const CityState &s, int64_t key) const;
	Built build_far(const CityState &s, int64_t key) const;
	void commit_near(Built &b);
	void commit_far(Built &b);
	void submit(const CityState &s, int64_t key, float priority, bool far);
	void stream(const CityState &s, const godot::Vector3 &cam, BuildBudget &budget, bool far);
	void set_params(const godot::RID &material) const;
	godot::RID scenario_, body_material_, card_material_;
	const StreetLook *look_ = nullptr;
	TreeLook trees_;
	TreeKit kit_;
	mutable GardenBook gardens_; // the workers' build_near and build_far fill it
	std::unordered_map<int64_t, Near> near_;
	std::unordered_map<int64_t, FarDraw> far_;
	mutable std::mutex queue_mutex_;
	Tier tier_[2]; // near, far
	uint64_t seq_ = 0;
	int pending_ = 0;
};

} // namespace city
