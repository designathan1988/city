// Building chunk meshes: 256 m chunks carried by four levels
// (tuning render.building_detail_end_m = D, building_mid_end_m, building_far_end_m; shares in buildings.json "lod"):
//   parts  0 .. D x parts_share        instanced unit boxes in five thickness bands (material building_parts) + fine
//                                      geometry that is not a box (lamps, AC fans, awnings) in the "building" material
//   near   0 .. D x near_share         walls with real openings and reveals, panes, sills, bands, roofs, plant, lot
//   mid    near end ..                 walls with shader-painted windows, the real roofs (parapets, cornices, plant)
//   far    per 512 m SUPERCHUNK        every building's masses in one mesh (material building_far)
// Near and mid switch inside a chunk by RenderingServer visibility ranges. The far mesh of a superchunk stands in for
// every chunk not drawn in its own detail: a chunk is drawn in detail once its centre comes within mid_end (measured
// with the eye height, as the visibility ranges measure; it leaves past mid_end x (1 + lod.far_hysteresis)) while its
// mid level is built, and the far mesh folds that chunk's buildings away (building_far.gdshader chunk_mask) - one
// switch per chunk, never a hole nor a building drawn twice. The mid level is built within mid_end (+ pad) on the
// ground, as the chunks are streamed. Large chunks are split into 128 m sub-chunks so every surface keeps 16-bit
// indices. A chunk lives while RenderWorld streams it (within far_end), a superchunk while one of its chunks does.
#pragma once

#include "core/job_pool.h"
#include "sim/buildings/b40/data.h"
#include "render/buildings/building_bodies.h"
#include "render/buildings/shell.h"
#include "render/chunk_jobs.h"
#include "render/chunk_queue.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <godot_cpp/classes/array_mesh.hpp>

#include <array>
#include <atomic>
#include <chrono>
#include <limits>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace city {

class BuildingRenderer {
public:
	const b40::BuildingLook &look() const { return look_; }
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void rebuild_chunk(const CityState &s, int64_t key); // a chunk to (re)mesh: its levels turn stale, its superchunk dirty
	void submit(const CityState &s, int64_t key, bool far, bool near, bool mid, float priority);
	// chunk_max / super_max: the slowest single chunk / superchunk commit, ms (render_cost probe)
	void commit(UploadBudget &upload, const std::chrono::steady_clock::time_point &until, double &chunk_max, double &super_max);
	void drop_chunk(int64_t key); // frees every level of a chunk the camera left
	float far_end() const { return far_end_; }
	void set_ranges(const CityState &s);
	void update_view(const godot::Vector3 &cam, BuildBudget &budget);
	godot::Dictionary building_cost() const; // triangles and bytes of every level's surfaces, whole city as it stands (tests)
	int pending() const; // what the view asks and has not got: unasked for want of budget, asked, or done and not yet drawn
	// What the view asked of the last frame and what the frame's budget left unasked.
	struct ViewNeed {
		int want = 0, unmet = 0, requested = 0, done = 0;
	};
	ViewNeed view_need() const;
	// The chunk at a point as the frame left it (the tests read it): bit 0 near built, 1 mid built, 2 shown, 3 detail, 4 known, 5 far mesh.
	int chunk_flags(float x, float z) const;
	int64_t baking() const { return baking_bytes(baking_) + baking_bytes(super_baking_); } // bytes still on a worker
	int64_t waiting() const { return waiting_ + super_waiting_; } // bytes built and waiting to bake (last commit)
	// The largest superchunk far mesh built so far, bytes of arrays (render_cost probe: against render.upload_budget_mb).
	int64_t super_bytes_max() const { return super_bytes_max_.load(); }
	const CommitTimes &commit_times() const { return times_; }

private:
	enum Level { LEVEL_NEAR, LEVEL_MID, LEVELS };
	static constexpr int SUBS = 2; // CHUNK / SUBCHUNK: sub-chunks per axis inside a 256 m chunk
	static constexpr int SUB_COUNT = SUBS * SUBS;
	static constexpr int BAND_COUNT = 5;
	// What the meshes hold, surface by surface (building_cost).
	enum SurfaceKind { HELD_NEAR_WALLS, HELD_NEAR_PANES, HELD_NEAR_GUARDS, HELD_NEAR_CARDS, HELD_NEAR_PIECES, HELD_MID_WALLS, HELD_MID_PANES, HELD_MID_CARDS, HELD_KINDS };
	struct Held {
		int64_t triangles = 0, bytes = 0;
		void add(const b40::Shell &shell, bool with_uv2) {
			triangles += shell.triangles();
			bytes += shell.bytes(with_uv2);
		}
	};
	struct Chunk {
		std::array<MeshDraw, SUB_COUNT> near, parts, mid;
		std::array<MultiMeshDraw, BAND_COUNT> bands;
		std::array<Held, HELD_KINDS> held;
		int64_t instances = 0; // the instanced parts of its bands
		bool built[LEVELS] = {};
		bool stale[LEVELS] = {}; // built, but the chunk changed since: remeshed when in reach
		uint32_t version = 0; // bumped by every change; a mesh built from an older one leaves the level stale
		bool detail = false; // within the detail switch (hysteresis)
		bool shown = false; // its instances are visible (drawn in detail); the far mesh hides it then
		godot::Vector3 centre;
	};
	struct Super {
		MeshDraw far;
		Held held;
		bool dirty = false;
		bool built = false; // the far mesh of the current content is committed
		bool has_content = false; // and holds buildings
		int mask = 0; // its chunks drawn in detail (bit = chunk in the superchunk): hidden in the far mesh
		int mask_sent = -1; // the chunk_mask last given to the far instance (-1: none yet)
		int chunks = 0; // its streamed chunks: the superchunk lives while one is
		// A built far mesh is rebuilt on its own clock: not before `due` (a delay
		// after the first change it waits for) nor within an interval of the previous request (`asked`).
		double due = 0, asked = -std::numeric_limits<double>::infinity();
	};
	// A level surface the job made (bake: the arrays converted on the job's thread, rs_util.h OwnedMesh); no mesh when
	// the shell held no triangle.
	struct Surface {
		OwnedMesh mesh;
		godot::AABB box;
		int glass_at = -1, sheer_at = -1, card_at = -1; // the mesh surfaces of the panes, the glass guards and the railings, -1: none
	};
	// A job's result: the shells it built, baked into surfaces by a later job the frame's upload budget admits
	// (chunk_jobs.h commit_baked), then handed to the chunk's instances.
	struct Built {
		int64_t key = 0;
		JobKey job;
		float priority = 0;
		uint64_t seq = 0;
		uint32_t version = 0; // the chunk's version when the job was submitted
		std::array<b40::Shell, SUB_COUNT> near_shell, parts_shell, mid_shell; // one per sub-chunk, until baked
		std::array<b40::Shell, SUB_COUNT> near_glass, mid_glass; // their panes and far walls: a surface of the same mesh
		std::array<b40::Shell, SUB_COUNT> near_sheer; // their glass guards: another
		std::array<b40::Shell, SUB_COUNT> near_cards, mid_cards; // their railings: another
		std::array<Surface, SUB_COUNT> near, parts, mid;
		std::array<godot::PackedFloat32Array, BAND_COUNT> bands; // the instanced parts, MultiMesh layout (rs_util.h)
		std::array<OwnedMesh, BAND_COUNT> band_mm; // their MultiMeshes, baked
		std::array<int, BAND_COUNT> band_count{};
		godot::RID unit_box; // the parts' box mesh
		bool has_near = false, has_mid = false, baked = false;
		std::array<Held, HELD_KINDS> held;
		int64_t bytes() const;
		void bake();
	};
	struct SuperBuilt {
		int64_t key = 0;
		JobKey job;
		float priority = 0;
		uint64_t seq = 0;
		b40::Shell far_shell;
		Surface far;
		Held held;
		bool baked = false;
		int64_t bytes() const { return far_shell.bytes(false); }
		void bake();
	};
	// What a job reads from the map, copied while it holds the map lock shared for a moment (gather): the records
	// of its chunks, in query order, and the final ground under their lots. The build itself runs without the lock.
	struct Inputs {
		std::vector<Building> records;
		HeightPatch ground;
	};
	struct Want {
		float d;
		int64_t key;
		uint8_t mask;
	};
	Inputs gather(const CityState &s, const int64_t *chunk_keys, int count) const;
	Built build_cpu(const Inputs &in, int64_t key, bool near, bool mid) const;
	SuperBuilt build_super_cpu(const Inputs &in, int64_t super_key) const;
	static Surface bake(const b40::Shell &shell, bool with_uv2, const b40::Shell *glass = nullptr, const b40::Shell *sheer = nullptr, const b40::Shell *cards = nullptr);
	static void put(MeshDraw &d, Surface &sf, const godot::RID &scenario, const godot::RID &material, bool shadows, bool shown);
	void put_level(MeshDraw &d, Surface &sf, const godot::RID &glass, bool shown) const;
	static godot::PackedFloat32Array bake_band(const std::vector<b40::BoxInstance> &boxes);
	void commit_one(Built &b);
	void commit_super(SuperBuilt &b);
	void submit_super(const CityState &s, int64_t super_key, float priority);
	void visit_super(int64_t super_key, Super &sp, const godot::Vector3 &cam, float eye, std::vector<Want> &want);
	void show(Chunk &ch, bool on); // the chunk drawn in detail or left to the far mesh
	void drop_far(int64_t super_key, Super &sp);
	void drop(Chunk &ch, Level level);
	void drop_bands(Chunk &ch);
	void ranges(Chunk &ch) const;
	float near_reach() const;
	void send_mask(Super &sp);
	// The generator's tables are the simulation's (read-only after boot); the drawing sizes and level shares are ours.
	const b40::Data &designs() const { return state_->buildings.designs; }
	b40::BuildingLook look_;
	BuildingBodies bodies_;
	godot::RID scenario_, material_, material_glass_, material_sheer_, material_card_, material_far_, material_parts_;
	ViewNeed need_;
	godot::Ref<godot::ArrayMesh> unit_box_; // the instanced parts' box (a resource: freed with the renderer)
	const CityState *state_ = nullptr;
	float parts_end_ = 0, near_end_ = 0, mid_end_ = 0, far_end_ = 0;
	std::array<float, BAND_COUNT> band_end_{};
	mutable std::mutex queue_mutex_;
	Results<Built> done_;
	std::unordered_map<int64_t, uint64_t> requested_; // key -> seq of the newest job
	BakingSet baking_, super_baking_;
	int64_t waiting_ = 0, super_waiting_ = 0;
	std::atomic<int64_t> super_bytes_max_{ 0 }; // written by the superchunk jobs
	CommitTimes times_; // the last commit's (render_cost probe)
	std::unordered_map<int64_t, uint8_t> requested_mask_; // bit 1 far, 2 near, 4 mid
	uint64_t seq_ = 0;
	std::unordered_map<int64_t, Chunk> chunks_;
	Results<SuperBuilt> super_done_;
	std::unordered_map<int64_t, uint64_t> super_requested_;
	uint64_t super_seq_ = 0;
	std::unordered_map<int64_t, Super> supers_;
};

} // namespace city
