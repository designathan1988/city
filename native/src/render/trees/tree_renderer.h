// Trees streamed around the camera at two levels. Near: every 256 m chunk within TreeLook::near_m (plus its fade band)
// draws each species as two MultiMeshes over the same instances, its body (casting shadows) and its cards (materials
// "vegetation" and "vegetation_cards"). Far: every 1024 m superchunk within TreeLook::far_m draws one MultiMesh of far
// cards (the woods by cell, the lone, park and street trees one by one), baked as one mesh (render/trees/far_cards.h). The two meet at near_m: the vertex shader
// fades each tree in or out over fade_m by its own distance to the main camera (render/shaders/vegetation.gdshaderinc).
// Chunks are built by jobs and rebuilt when their roads or lots change.
#pragma once

#include "core/job_pool.h"
#include "render/chunk_jobs.h"
#include "render/chunk_queue.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "render/trees/far_cards.h"
#include "render/trees/garden_place.h"
#include "render/trees/tree_place.h"
#include "sim/state/city_cut.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

class TreeRenderer {
public:
	TreeRenderer();
	bool init(const RenderContext &ctx, const StreetLook &look, godot::PackedStringArray &errors);
	void reset();
	void mark_dirty(int64_t key);
	void set_share(float share); // the graphics tier: every drawn chunk is placed again
	int near_trees() const; // trees of the near level now
	int near_plants() const; // of them, the gardens' bushes and hedges
	// The frame's build units of both levels (frame_admission.h): the marked chunks drawn again, the missing ones round the
	// camera; the far ones let go.
	void offer(const CityState &s, const godot::Vector3 &cam, std::vector<FrameAdmission::Unit> &out);
	AdmittedQueue *near_queue() { return &tier_[0]; }
	AdmittedQueue *far_queue() { return &tier_[1]; }
	int64_t baking() const { return tier_[1].baking(); }
	int64_t waiting() const { return tier_[1].waiting(); }
	int pending() const;

private:
	struct Near {
		std::vector<MultiMeshDraw> body, cast, cards;
		int trees = 0, plants = 0; // every instance, and those of the gardens' bushes and hedges
	};
	// A job's result. Near: a buffer per species, sent by the main thread when handed over (`upload` bytes). Far: the superchunk's
	// cards, baked into one mesh by a job the upload ledger admits.
	struct Built {
		int64_t key = 0;
		JobKey job;
		float priority = 0;
		uint64_t ticket = 0;
		bool far = false, baked = false;
		std::vector<godot::PackedFloat32Array> bufs; // near: one per species
		int64_t upload = 0; // bytes the near buffers send, every draw of each
		std::vector<FarCard> cards; // far, until baked
		float centre = 0, half_tall = 0;
		OwnedMesh far_mesh; // far: the superchunk's cards
		bool bakes() const { return far; }
		int64_t bytes() const { return upload; }
		void bake();
	};
	struct FarDraw {
		godot::RID inst, mesh;
		void free();
	};
	using Tier = ChunkResults<Built>;
	CityCut gather(const CityState &s, int64_t key, bool far) const;
	std::shared_ptr<Built> build_near(const CityView &s, int64_t key) const;
	std::shared_ptr<Built> build_far(const CityView &s, int64_t key) const;
	void commit_near(Built &b);
	void commit_far(Built &b);
	void submit(const CityState &s, int64_t key, float priority, bool far);
	void stream(const CityState &s, const godot::Vector3 &cam, bool far, std::vector<FrameAdmission::Unit> &out);
	void set_params(const godot::RID &material) const;
	bool built(int64_t key, bool far) const { return far ? far_.count(key) > 0 : near_.count(key) > 0; }
	godot::RID scenario_, body_material_, card_material_;
	const StreetLook *look_ = nullptr;
	TreeLook trees_;
	TreeKit kit_;
	mutable GardenBook gardens_; // the workers' build_near and build_far fill it
	std::unordered_map<int64_t, Near> near_;
	std::unordered_map<int64_t, FarDraw> far_;
	Tier tier_[2]; // near, far
	int pending_ = 0; // units offered this frame and not taken
};

} // namespace city
