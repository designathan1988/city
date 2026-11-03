// Building chunk meshes, Roadcraft "4.0" buildings (r40/render/buildings): 256 m chunks carried by four levels
// (tuning render.building_detail_end_m = D, building_mid_end_m, building_far_end_m; shares in buildings.json "lod"):
//   parts  0 .. D x parts_share        instanced unit boxes in five thickness bands (material building_parts) + fine
//                                      geometry that is not a box (lamps, AC fans, awnings) in the "building" material
//   near   0 .. D x near_share         walls with real openings and reveals, panes, sills, bands, roofs, plant, lot
//   mid    near end ..                 walls with shader-painted windows, the real roofs (parapets, cornices, plant)
//   far    per 1024 m SUPERCHUNK       every building's masses in one mesh (material building_far)
// The levels inside a chunk switch by RenderingServer visibility ranges; the far level stands in for its 16 chunks by
// the Roadcraft plate rule (cellLayer.ts): it turns far when its nearest point (with the eye height) is beyond mid_end,
// with lod.far_hysteresis, and it is drawn while far or while the chunks it covered are not remeshed yet (never a
// hole). Every chunk instance has its superchunk's far instance as visibility parent: Godot hides them while the far
// one is drawn (a parent without a range never counts as "too close") and lets their own ranges decide while it is
// hidden. RenderingServer::free_rid does not undo that link on either side, so it is undone before every free.
// Large chunks are split into 128 m sub-chunks so every surface keeps 16-bit indices. A chunk lives while RenderWorld
// streams it (within far_end); its near level is meshed within reach of the camera, its mid level while its
// superchunk is not far.
#pragma once

#include "core/job_pool.h"
#include "render/buildings/b40_data.h"
#include "render/buildings/building_bodies.h"
#include "render/buildings/shell.h"
#include "render/chunk_queue.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <godot_cpp/classes/array_mesh.hpp>

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
	void rebuild_chunk(const CityState &s, int64_t key); // a chunk to (re)mesh: its levels turn stale, its superchunk dirty
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
		bool stale[LEVELS] = {}; // built, but the chunk changed since: remeshed when in reach
		uint32_t version = 0; // bumped by every change; a mesh built from an older one leaves the level stale
		godot::Vector3 centre;
	};
	struct Super {
		MeshDraw far;
		bool dirty = false;
		bool built = false; // the far mesh of the current content is committed
		bool has_content = false; // and holds buildings
		bool far_state = false; // beyond the switch (plate rule, with hysteresis)
		bool covering = false; // its chunks' mid levels went while it stood in: drawn until they are all back
		bool shown = false; // the far instance is visible
	};
	struct Built {
		int64_t key = 0;
		float priority = 0;
		uint64_t seq = 0;
		uint32_t version = 0; // the chunk's version when the job was submitted
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
	struct Want {
		float d;
		int64_t key;
		uint8_t mask;
	};
	Built build_cpu(const CityState &s, int64_t key, bool near, bool mid) const;
	SuperBuilt build_super_cpu(const CityState &s, int64_t super_key) const;
	void commit_one(Built &b);
	void commit_super(SuperBuilt &b);
	void submit_super(const CityState &s, int64_t super_key, float priority);
	void visit_super(int64_t super_key, Super &sp, const godot::Vector3 &cam, float eye, std::vector<Want> &want);
	void drop_super(int64_t super_key);
	void drop(Chunk &ch, Level level);
	void drop_bands(Chunk &ch);
	void ranges(Chunk &ch) const;
	float near_reach() const;
	float super_distance(int64_t super_key, const godot::Vector3 &cam, float eye) const;
	godot::RID far_parent(const Chunk &ch) const; // the chunk's superchunk far instance (invalid when none)
	void link(Chunk &ch, const godot::RID &parent) const; // visibility parent of every instance of the chunk
	void link_super(int64_t super_key, const godot::RID &parent); // ... of every chunk of the superchunk
	void show_far(Super &sp);
	static void release(MeshDraw &d); // undoes the visibility link, then frees
	static void release(MultiMeshDraw &d);
	b40::Data data_;
	BuildingBodies bodies_;
	godot::RID scenario_, material_, material_far_, material_parts_;
	godot::Ref<godot::ArrayMesh> unit_box_; // the instanced parts' box (a resource: freed with the renderer)
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
