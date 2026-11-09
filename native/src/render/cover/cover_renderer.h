// The ground cover streamed round the camera: every 256 m chunk within CoverKit::reach_m draws each variant of each kind
// as a MultiMesh. A plant stands on a lattice of its kind's spacing, kept by its density there: what the player painted
// (sim/nature/ground_paint.h), the biome's cactus (sim/nature/ecology.h), never on roads, lots or water. An unpainted
// map outside the thorn scrub places nothing. The elements the player laid (sim/nature/elements.h) are drawn here too, a
// MultiMesh per model. Chunks are built by jobs and rebuilt when their
// roads, lots or nature change.
#pragma once

#include "core/job_pool.h"
#include "render/chunk_queue.h"
#include "render/chunk_results.h"
#include "render/cover/cover_kit.h"
#include "render/cover/element_models.h"
#include "render/cover/shore_stones.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "render/trees/ground_kept.h"
#include "render/trees/tree_place.h"
#include "sim/state/city_state.h"

#include <array>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

class CoverRenderer {
public:
	CoverRenderer();
	bool init(const RenderContext &ctx, GroundKept &ground, godot::PackedStringArray &errors); // `ground`: shared with the trees
	void reset();
	void mark_dirty(int64_t key) {
		queue_.revs.mark(key);
		wet_.erase(key);
	}
	// The frame's build units (frame_admission.h): the marked chunks drawn again, the missing ones round the camera; the far let go.
	void offer(const CityState &s, const godot::Vector3 &cam, std::vector<FrameAdmission::Unit> &out);
	AdmittedQueue *queue() { return &queue_; }
	int pending() const { return pending_ + queue_.pending(); }

private:
	// A job's result: a buffer per draw, kinds in order, sent by the main thread when handed over.
	struct Built {
		int64_t key = 0;
		JobKey job;
		float priority = 0;
		uint64_t ticket = 0;
		bool baked = false;
		std::vector<godot::PackedFloat32Array> bufs;
		bool bakes() const { return false; }
		int64_t bytes() const {
			int64_t n = 0;
			for (const godot::PackedFloat32Array &b : bufs) n += b.size() * int64_t(sizeof(float));
			return n;
		}
		void bake() {}
	};
	std::shared_ptr<Built> build(const CityState &s, int64_t key) const;
	GroundKept *grounds_ = nullptr; // each chunk's ground, gathered once for the trees and the cover
	void place(const CityState &s, int64_t key, const ChunkGround &l, Built &out) const;
	void place_elements(const CityState &s, int64_t key, Built &out) const;
	void place_shore(const CityState &s, int64_t key, const ChunkGround &l, Built &out) const;
	float reach_of(const CityState &s, int64_t key);
	void commit_one(Built &b);
	void submit(const CityState &s, int64_t key, float priority);
	godot::RID scenario_, plant_material_, rock_material_, shore_material_;
	CoverKit kit_;
	ElementModels models_;
	ShoreLook shore_;
	int draws_ = 0;
	std::array<int, COVER_PIECES> first_{}; // each kind's first draw
	std::array<std::vector<OwnedMesh>, COVER_PIECES> piece_meshes_; // each kind's variants with their material
	std::vector<OwnedMesh> shore_meshes_; // the rocks with the shore's
	std::unordered_map<int64_t, std::vector<MultiMeshDraw>> chunks_;
	std::unordered_map<int64_t, bool> wet_; // which chunks have water by them: they stream as far as the shore
	ChunkResults<Built> queue_;
	int pending_ = 0; // units offered this frame and not taken
};

} // namespace city
