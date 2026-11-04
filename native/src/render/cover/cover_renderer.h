// The ground cover streamed round the camera: every 256 m chunk within CoverKit::reach_m draws each variant of each kind
// as a MultiMesh. A plant stands on a lattice of its kind's spacing, kept by its density there: what the player painted
// (sim/nature/ground_paint.h), the biome's cactus (sim/nature/ecology.h), never on roads, lots or water. An unpainted
// map outside the thorn scrub places nothing. Chunks are built by jobs and rebuilt when their
// roads, lots or nature change.
#pragma once

#include "core/job_pool.h"
#include "render/chunk_queue.h"
#include "render/cover/cover_kit.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <array>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

class CoverRenderer {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void mark_dirty(int64_t key) { dirty_.insert(key); }
	void sync(const CityState &s, const godot::Vector3 &cam, BuildBudget &budget);
	void commit(const std::chrono::steady_clock::time_point &until);
	int pending() const;

private:
	struct Built {
		int64_t key = 0;
		float priority = 0;
		uint64_t seq = 0;
		std::vector<godot::PackedFloat32Array> bufs; // one per draw, kinds in order
	};
	Built build(const CityState &s, int64_t key) const;
	void place(const CityState &s, int64_t key, Built &out) const;
	void commit_one(Built &b);
	void submit(const CityState &s, int64_t key, float priority);
	godot::RID scenario_, plant_material_, rock_material_;
	CoverKit kit_;
	int draws_ = 0;
	std::array<int, COVER_PIECES> first_{}; // each kind's first draw
	std::unordered_map<int64_t, std::vector<MultiMeshDraw>> chunks_;
	std::unordered_set<int64_t> dirty_;
	mutable std::mutex queue_mutex_;
	std::vector<Built> done_;
	std::unordered_map<int64_t, uint64_t> requested_;
	uint64_t seq_ = 0;
	int pending_ = 0;
};

} // namespace city
