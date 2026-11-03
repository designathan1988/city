// Building chunk meshes, Roadcraft "4.0" buildings (r40/render/buildings): 256 m chunks carried by four levels switched
// by RenderingServer visibility ranges (tuning render.building_detail_end_m = D, building_mid_end_m, building_far_end_m;
// shares in buildings.json "lod"):
//   parts  0 .. D x parts_share        instanced unit boxes in five thickness bands (material building_parts) + fine
//                                      geometry that is not a box (lamps, AC fans, awnings) in the "building" material
//   near   0 .. D x near_share         walls with real openings and reveals, panes, sills, bands, roofs, plant, lot
//   mid    near end .. mid_end         walls with shader-painted windows, the real roofs (parapets, cornices, plant)
//   far    mid_end .. 0                per 1024 m SUPERCHUNK: every building's masses in one mesh (material building_far)
// Large chunks are split into 128 m sub-chunks so every surface keeps 16-bit indices. A chunk is meshed only within
// far_end of the camera (RenderWorld streams it); near and mid levels lazily when the camera comes within their reach,
// freed far beyond.
#pragma once

#include "core/job_pool.h"
#include "render/buildings/b40_data.h"
#include "render/buildings/building_bodies.h"
#include "render/buildings/shell.h"
#include "render/chunk_queue.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <array>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace city {

class BuildingRenderer {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void rebuild_chunk(const CityState &s, int64_t key); // marks a 256 m chunk + its superchunk dirty
	void submit(const CityState &s, int64_t key, bool far, bool near, bool mid, float priority);
	void commit(const std::chrono::steady_clock::time_point &until);
	void drop_chunk(int64_t key); // frees every level of a chunk the camera left
	float far_end() const { return far_end_; }
	void set_ranges(const CityState &s);
	void update_view(const godot::Vector3 &cam, BuildBudget &budget);
	int pending() const;

private:
	enum Level { LEVEL_NEAR, LEVEL_MID, LEVELS };
	static constexpr int SUBS = 2; // CHUNK / SUBCHUNK: sub-chunks per axis inside a 256 m chunk
	static constexpr int SUB_COUNT = SUBS * SUBS;
	static constexpr int BAND_COUNT = 5;
	struct Chunk {
		std::array<MeshDraw, SUB_COUNT> near, parts, mid;
		std::array<MultiMeshDraw, BAND_COUNT> bands;
		bool built[LEVELS] = {};
		godot::Vector3 centre;
	};
	struct Super {
		MeshDraw far;
		bool dirty = false;
	};
	struct Built {
		int64_t key = 0;
		float priority = 0;
		uint64_t seq = 0;
		std::vector<b40::Shell> near, parts, mid; // one per sub-chunk
		std::array<std::vector<b40::BoxInstance>, BAND_COUNT> boxes;
		bool has_near = false, has_mid = false;
	};
	struct SuperBuilt {
		int64_t key = 0;
		float priority = 0;
		uint64_t seq = 0;
		b40::Shell far;
	};
	Built build_cpu(const CityState &s, int64_t key, bool near, bool mid) const;
	SuperBuilt build_super_cpu(const CityState &s, int64_t super_key) const;
	void commit_one(Built &b);
	void commit_super(SuperBuilt &b);
	void submit_super(const CityState &s, int64_t super_key, float priority);
	void drop_super(int64_t super_key);
	void drop(Chunk &ch, Level level);
	void ranges(Chunk &ch) const;
	float reach(Level level) const;
	void ranges_super(Super &sp) const;
	b40::Data data_;
	BuildingBodies bodies_;
	godot::RID scenario_, material_, material_far_, material_parts_, unit_box_;
	const CityState *state_ = nullptr;
	float parts_end_ = 0, near_end_ = 0, mid_end_ = 0, far_end_ = 0;
	std::array<float, BAND_COUNT> band_end_{};
	mutable std::mutex queue_mutex_;
	std::vector<Built> done_;
	std::unordered_map<int64_t, uint64_t> requested_; // key -> seq of the newest job
	std::unordered_map<int64_t, uint8_t> requested_mask_; // bit 1 far, 2 near, 4 mid
	uint64_t seq_ = 0;
	std::unordered_map<int64_t, Chunk> chunks_;
	std::vector<SuperBuilt> super_done_;
	std::unordered_map<int64_t, uint64_t> super_requested_;
	uint64_t super_seq_ = 0;
	std::unordered_map<int64_t, Super> supers_;
};

} // namespace city
