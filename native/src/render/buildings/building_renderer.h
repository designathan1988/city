// Building chunk meshes: 256 m chunks carried by four levels
// (tuning render.building_detail_end_m = D, building_mid_end_m, building_far_end_m; shares in buildings.json "lod"):
//   parts  0 .. D x parts_share        instanced unit boxes in five thickness bands (material building_parts) + fine
//                                      geometry that is not a box (lamps, AC fans, awnings) in the "building" material
//   near   0 .. D x near_share         walls with real openings and reveals, panes, sills, bands, roofs, plant, lot
//   mid    near end ..                 walls with shader-painted windows, the real roofs (parapets, cornices, plant)
//   far    per SUPERCHUNK (core/grid.h) every building's masses in one mesh (material building_far)
// Near and mid switch inside a chunk by RenderingServer visibility ranges. The far mesh of a superchunk stands in for every chunk not drawn in its own detail: a chunk is drawn in detail once its centre comes within mid_end (measured with the eye height, as the visibility ranges measure; it leaves past mid_end x (1 + lod.far_hysteresis)) while its mid level is built, and the far mesh folds that chunk's buildings away (building_far.gdshader chunk_mask) - one switch per chunk, never a hole nor a building drawn twice.
// The mid level is built within mid_end (+ pad) on the ground, as the chunks are streamed. Large chunks are split into 128 m sub-chunks so every surface keeps 16-bit indices. A chunk lives while RenderWorld streams it (within far_end), a superchunk while one of its chunks does.
#pragma once

#include "render/buildings/building_surface.h"
#include "render/buildings/occluders.h"

#include "core/job_pool.h"
#include "sim/buildings/b40/data.h"
#include "render/buildings/building_bodies.h"
#include "render/buildings/building_job_inputs.h"
#include "render/buildings/level_fade.h"
#include "render/buildings/ruin_fracture.h"
#include "render/buildings/shell.h"
#include "render/buildings/signature_bodies.h"
#include "render/buildings/wear.h"
#include "render/chunk_jobs.h"
#include "render/chunk_queue.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"
#include "render/buildings/interior_emit.h"

#include <godot_cpp/classes/array_mesh.hpp>

#include <array>
#include <atomic>
#include <chrono>
#include <limits>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

class BuildingRenderer {
public:
	void set_detail_scale(float scale); // the graphics tier's share of the near level's reach
	void set_lots_drawn(bool on) { lots_drawn_.store(on); } // the Effects panel; the caller redraws everything
	const b40::BuildingLook &look() const { return look_; }
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void set_broken(const std::unordered_set<uint32_t> &seeds); // the buildings drawn in pieces, by the effects
	// The walls of a record a blow brings down, broken (ruin_fracture.h), from the bodies the levels draw.
	std::shared_ptr<RuinMesh> ruin_of(const b40::Data &data, const Building &rec, const HeightPatch &ground) const;
	void rebuild_chunk(const CityState &s, int64_t key); // a chunk to (re)mesh: its levels turn stale, its superchunk dirty
	// A chunk whose near level is drawn again with the same buildings (the Interior layer cut some open or closed them): the
	// far mesh draws them whole either way, so its superchunk stays.
	void restyle_chunk(int64_t key);
	void submit(const CityState &s, int64_t key, bool far, bool near, bool mid, float priority);
	void commit(UploadBudget &upload, const std::chrono::steady_clock::time_point &until, double &chunk_max, double &super_max); // chunk_max, super_max: the slowest commit, ms
	void drop_chunk(int64_t key); // frees every level of a chunk the camera left
	float far_end() const { return far_end_; }
	void set_ranges(const CityState &s);
	void update_view(const godot::Vector3 &cam, BuildBudget &budget);
	// What the inside of building `id` costs cut open at `level` for a camera along step `dir` (the map's lock held shared by the caller).
	BuildingInteriorCost interior_cost(const CityState &s, int id, int level, int dir) const;
	godot::Dictionary building_cost() const; // triangles and bytes of every level's surfaces, whole city as it stands (tests)
	godot::Dictionary bay_census(); // how many bays the city has and how many different ones, at two grains of size
	int pending() const; // what the view asks and has not got: unasked for want of budget, asked, or done and not yet drawn
	BuildingViewNeed view_need() const;
	// The chunk at a point as the frame left it (the tests read it): known, near and mid built, shown, in detail, under a far mesh.
	godot::Dictionary chunk_view(float x, float z) const;
	int64_t baking() const { return baking_bytes(baking_) + baking_bytes(super_baking_); } // bytes still on a worker
	int64_t waiting() const { return waiting_ + super_waiting_; } // bytes built and waiting to bake (last commit)
	// The largest superchunk far mesh built so far, bytes of arrays (render_cost probe: against render.upload_budget_mb).
	int64_t super_bytes_max() const { return super_bytes_max_.load(); }
	float stream_reach(const CityState &s, const godot::Vector3 &cam) const { // how far from the eye its chunks are known
		return far_end_ + std::max(0.f, cam.y - s.terrain.height_at(cam.x, cam.z)) * look_.far_per_eye;
	}
	const CommitTimes &commit_times() const { return times_; }
	SignatureBodies *own_designs() { return &signatures_; }

private:
	enum Level { LEVEL_NEAR, LEVEL_MID, LEVELS };
	static constexpr int SUBS = SUBCHUNKS_PER_CHUNK; // sub-chunks per axis inside a chunk (core/grid.h)
	static constexpr int SUB_COUNT = SUBS * SUBS;
	static constexpr int BAND_COUNT = 5; // of boxes; the kinds of bay_props.h follow them
	static constexpr int BAND_DRAWS = BAND_COUNT + b40::BAY_PROPS;
	// Under the first band's thickness a part is finer than a shadow texel: the others cast, and the first balcony pieces after them.
	static constexpr int BAND_CASTS = BAND_COUNT - 1 + b40::BAY_PROP_SHADOWED;
	static constexpr int cast_of(int band) { return band == 0 || band >= BAND_COUNT + b40::BAY_PROP_SHADOWED ? -1 : band - 1; }
	// The mid level's instances: its openings, what stands out of its walls, and of those the boxes large enough for its shadow map.
	enum MidDraw { MID_BAYS, MID_BOXES, MID_SOLIDS, MID_DRAWS };
	static constexpr int MID_SOLID_BAND = 3; // of the part bands: past this far a shadow texel is a hand wide, and a thinner box is under it
	static constexpr float REACH_PAD = 0.25f; // of a chunk: the stream starts a level a little before its range reaches the eye
	using Held = HeldCost;
	using Surface = LevelSurface;
	struct Chunk {
		std::array<MeshDraw, SUB_COUNT> near, parts, mid;
		std::array<MeshDraw, SUB_COUNT> flat; // past bay_end_: the mid level's bays as merged rectangles, a bay too small on the screen to pay for its triangles
		std::array<std::array<MultiMeshDraw, BAND_DRAWS>, SUB_COUNT> bands; // by sub-chunk: they end with the near shell they stand on
		std::array<std::array<ShadowTwin, BAND_CASTS>, SUB_COUNT> prop_casts; // what the boxes and the balcony pieces throw, to lod.cast_props_m
		std::array<std::array<MultiMeshDraw, MID_DRAWS>, SUB_COUNT> mid_draws;
		std::array<ShadowTwin, SUB_COUNT> mid_casts; // what the mid level's solids throw
		std::array<ShadowTwin, SUB_COUNT> mid_hulls; // what its walls throw (cast_hull.h)
		std::array<OwnedMesh, SUB_COUNT> mid_hull_meshes;
		// A visibility range is measured to the centre of the instance's box (docs: Visibility ranges): every level of a sub-chunk takes this
		// one box, so they hand over at one distance, with neither both drawn nor a hole between them.
		std::array<godot::AABB, SUB_COUNT> reach_box;
		MultiMeshDraw flags; // the cloths of its masts (flag_cloth.h), with the near level
		std::array<Held, HELD_KINDS> held;
		int64_t instances = 0, props = 0; // the instanced parts of its bands and flags, and the bay props among them
		bool built[LEVELS] = {};
		bool stale[LEVELS] = {}; // built, but the chunk changed since: remeshed when in reach
		double wear_due = std::numeric_limits<double>::infinity(); // the city minute one of its buildings looks more worn (wear.h): remeshed then
		uint32_t version = 0; // bumped by every change; a mesh built from an older one leaves the level stale
		bool detail = false; // within the detail switch (hysteresis)
		bool shown = false; // its instances are visible (drawn in detail); the far mesh hides it then
		int64_t key = 0;
		std::array<SubFade, SUB_COUNT> fade{}; // each sub-chunk's hand-over between its near level and its mid one (level_fade.h)
		bool cut = false; // its near level has a building cut open: drawn whatever the distance, the mid level and the far mesh stand aside
		godot::Vector3 centre;
		ChunkOccluder occluder; // the near level's walls (occluders.h)
	};
	using Super = FarDraw;
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
		std::array<b40::Shell, SUB_COUNT> near_washes; // the lanterns' light on the walls: another, added to the wall
		std::array<b40::Shell, SUB_COUNT> near_lawns, mid_lawns; // their lawns: another, in the terrain's grass
		std::array<b40::Shell, SUB_COUNT> flat_shell, flat_glass; // the mid level's far bays (Chunk::flat): its walls are all far walls, in the glass surface
		std::array<Surface, SUB_COUNT> near, parts, mid, flat;
		std::array<OwnedMesh, SUB_COUNT> mid_hull;
		std::vector<SignaturePlace> signatures; // the buildings of the chunk with a design of their own
		float hull_error_m = 0; double wear_due = std::numeric_limits<double>::infinity(); // the first minute a building of it looks more worn
		std::array<std::array<godot::PackedFloat32Array, BAND_DRAWS>, SUB_COUNT> bands; // the instanced parts, MultiMesh layout (rs_util.h)
		std::array<std::array<OwnedMesh, BAND_DRAWS>, SUB_COUNT> band_mm; // their MultiMeshes, baked
		std::array<std::array<int, BAND_DRAWS>, SUB_COUNT> band_count{};
		std::array<godot::RID, BAND_DRAWS> band_mesh; // what each draws
		std::array<std::array<godot::PackedFloat32Array, MID_DRAWS>, SUB_COUNT> mid_buf; // the mid level's instances, as the bands, by sub-chunk
		std::array<std::array<OwnedMesh, MID_DRAWS>, SUB_COUNT> mid_mm;
		std::array<std::array<int, MID_DRAWS>, SUB_COUNT> mid_count{};
		std::array<std::array<godot::AABB, MID_DRAWS>, SUB_COUNT> mid_box;
		std::array<std::array<godot::AABB, BAND_DRAWS>, SUB_COUNT> band_box; // what each band really covers
		godot::AABB flag_box;
		std::array<godot::RID, MID_DRAWS> mid_mesh;
		godot::PackedFloat32Array flags; // the flags, MultiMesh layout (flag_cloth.h bake_flags)
		OwnedMesh flag_mm;
		int flag_count = 0;
		godot::RID unit_flag; // the flags' cloth
		OccluderWalls walls;
		bool has_near = false, has_mid = false, baked = false;
		bool cut = false; // a building of the near level is cut open
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
		bool baked = false; double wear_due = std::numeric_limits<double>::infinity(); // the first minute a building of it looks more worn
		int64_t bytes() const { return far_shell.bytes(false); }
		void bake();
	};
	using Inputs = BuildingJobInputs; // what a job reads from the map (building_job_inputs.h)
	struct Want {
		float d;
		int64_t key;
		uint8_t mask;
	};
	Inputs gather(const CityState &s, const int64_t *chunk_keys, int count) const;
	mutable std::mutex broken_mutex_; // the jobs gather on their own threads: they take the set published last, never a copy of it
	std::shared_ptr<const std::unordered_set<uint32_t>> broken_ = std::make_shared<const std::unordered_set<uint32_t>>();
	void file_parts(Built &out, std::array<b40::Shell, SUB_COUNT> &parts_shells) const;
	static void link_shells(Built &out, std::array<b40::Shell, SUB_COUNT> &mid_parts);
	Built build_cpu(const Inputs &in, int64_t key, bool near, bool mid) const;
	SuperBuilt build_super_cpu(const Inputs &in, int64_t super_key) const;
	static Surface bake(const b40::Shell &shell, bool with_uv2, const b40::Shell *glass = nullptr, const b40::Shell *sheer = nullptr, const b40::Shell *cards = nullptr, const b40::Shell *lawns = nullptr, const b40::Shell *washes = nullptr);
	static void put(MeshDraw &d, Surface &sf, const godot::RID &scenario, const godot::RID &material, bool shadows, bool shown,
			RenderLayer layer);
	void put_level(MeshDraw &d, Surface &sf, const godot::RID &glass, bool shown, RenderLayer layer, bool shadows = true) const;
	static godot::PackedFloat32Array bake_band(const std::vector<b40::BoxInstance> &boxes);
	static godot::PackedFloat32Array bake_props(const std::vector<b40::PropInstance> &props);
	static godot::PackedFloat32Array bake_bays(const std::vector<b40::BayInstance> &bays);
	void commit_one(Built &b);
	void fit_reach(Chunk &ch, const Built &b);
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
	b40::WearRule wear_; // how run down each building is drawn (wear.h)
	void wear_on(double minutes); // the chunks and far meshes whose buildings look more worn by now drawn again (building_renderer_wear.cpp)
	mutable BuildingBodies bodies_; // every job's: thread-safe, emptied by reset()
	godot::RID material_cast_; // shadow_caster: no vertex() of its own
	void fade_levels(Chunk &ch, const godot::Vector3 &cam); // the sub-chunks in the band of a hand-over (building_renderer_reach.cpp)
	FadeClock fade_clock_; // how far a dissolve moves between two views (level_fade.h)
	void fade_near(Chunk &ch, size_t sub, bool on, float keep);
	void fade_parts(Chunk &ch, size_t sub, bool on, float keep);
	void warm_fades(const Chunk &ch);
	FadeTwins fade_twins_; // the discarding twins' materials and the instances that warm their pipelines (level_fade.h)
	godot::RID scenario_, material_, material_glass_, material_sheer_, material_card_, material_props_, material_far_, material_lawn_, material_wash_, material_parts_;
	BuildingViewNeed need_;
	std::array<godot::Ref<godot::ArrayMesh>, b40::BAY_PROPS> prop_mesh_; // made once, as the box
	godot::Ref<godot::ArrayMesh> unit_box_, unit_flag_, unit_bay_; // the instanced parts' box and the flags' cloth (resources: freed with the renderer)
	const CityState *state_ = nullptr;
	float parts_end_ = 0, near_end_ = 0, mid_end_ = 0, far_end_ = 0;
	float bay_end_ = 0; // how far a bay of the mid level is geometry: past it, it is under ~12 pixels and its triangles cost more than they show
	std::array<float, BAND_DRAWS> band_end_{};
	float detail_scale_ = 1; // the graphics tier's share of the near reach (set_detail_scale)
	std::atomic<bool> lots_drawn_{ true }; // read by the mesh jobs
	mutable std::mutex queue_mutex_;
	Results<Built> done_;
	std::unordered_map<int64_t, uint64_t> requested_; // key -> seq of the newest job
	BakingSet baking_, super_baking_;
	int64_t waiting_ = 0, super_waiting_ = 0;
	std::atomic<int64_t> super_bytes_max_{ 0 }; // written by the superchunk jobs
	CommitTimes times_; // the last commit's (render_cost probe)
	std::unordered_map<int64_t, uint8_t> requested_mask_; // BAND_BIT_NEAR, BAND_BIT_MID (the far level is the superchunk's)
	uint64_t seq_ = 0;
	std::unordered_map<int64_t, Chunk> chunks_;
	SignatureBodies signatures_;
	Results<SuperBuilt> super_done_;
	std::unordered_map<int64_t, uint64_t> super_requested_;
	uint64_t super_seq_ = 0;
	std::unordered_map<int64_t, Super> supers_;
};

} // namespace city
