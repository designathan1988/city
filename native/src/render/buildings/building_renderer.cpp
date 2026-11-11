// Building chunk streaming: the near (+ parts) and mid levels are built lazily, nearest first, when the camera comes
// within their reach (near end / mid end, plus a pad) and freed beyond 1.25x it. Every dirty chunk marks its 512 m
// superchunk, whose far/HLOD mesh is built within building_far_end_m (the city never vanishes when zooming out). See
// building_renderer.h for the switch between a chunk's own detail and the far mesh.
#include "render/buildings/building_renderer.h"

#include "core/laps.h"
#include "core/units.h"

#include "render/buildings/bay_kit.h"
#include "render/buildings/building_stream.h"
#include "render/buildings/facade.h"
#include "render/buildings/flag_cloth.h"
#include "render/mesh_builder.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

using namespace building_stream;

namespace {
constexpr float HALF = 0.5f;

int64_t super_of(const Vector3 &centre) { return chunk_of(centre.x, centre.z, SUPERCHUNK); }
} // namespace

bool BuildingRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	bodies_.set_budget(size_t(ctx.catalog->tune.render_rooms_cache_mb * BYTES_PER_MIB));
	signatures_.init(ctx.scenario, LAYER_BUILDING_SHELLS);
	material_ = ctx.material("building", errors);
	material_glass_ = ctx.material("building_glass", errors);
	material_sheer_ = ctx.material("building_sheer", errors);
	material_card_ = ctx.material("building_card", errors);
	material_props_ = ctx.material("building_props", errors);
	material_cast_ = ctx.material("shadow_caster", errors);
	material_far_ = ctx.material("building_far", errors);
	material_lawn_ = ctx.material("verge", errors);
	material_wash_ = ctx.material("building_wash", errors);
	material_parts_ = ctx.material("building_parts", errors);
	chunk_materials_ = ChunkMaterials{ { material_, material_glass_, material_sheer_, material_card_, material_lawn_, material_wash_ }, { material_, material_, RID(), material_card_, material_lawn_, RID() }, material_, material_cast_ };
	Dictionary t = ctx.table("buildings", errors);
	if (!errors.is_empty()) return false;
	if (!look_.load(t, errors) || !look_.bay.load(ctx.tables.get("bay_props", Dictionary()), errors) || !look_.interior.load(ctx.table("furniture", errors), errors)) return false;
	look_.lot_plate = ctx.catalog->tune.lot_plate_m;
	// the unit meshes every chunk's draws share (mesh_draw.h shared): a chunk's draws are made when it streams in; the boxes and the
	// balcony pieces also throw their shadow through twins in the shadow material (commit_one)
	for (int k = 0; k < b40::BAY_PROPS; k++) prop_mesh_[size_t(k)] = look_.bay.models[size_t(k)].bake(material_props_, false).shared(material_cast_);
	MeshBuilder unit;
	unit.box(Vector3(-HALF, -HALF, -HALF), Vector3(HALF, HALF, HALF), Color(1, 1, 1, 1), Vector2(), true);
	unit_box_ = unit.bake(material_parts_, false).shared(material_cast_);
	unit_bay_ = b40::bay_mesh(material_);
	unit_flag_ = b40::cloth_mesh(material_parts_);
	return true;
}

void BuildingRenderer::reset() {
	signatures_.reset();
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.clear();
		requested_.clear();
		requested_mask_.clear();
		super_done_.clear();
		super_requested_.clear();
	}
	baking_.clear();
	super_baking_.clear();
	waiting_ = super_waiting_ = 0;
	for (auto &kv : chunks_) {
		drop(kv.second, LEVEL_NEAR);
		drop(kv.second, LEVEL_MID);
		drop_bands(kv.second);
	}
	chunks_.clear();
	for (auto &kv : supers_) kv.second.free();
	supers_.clear();
	bodies_.clear();
}

// A dirty chunk keeps its meshes on screen until the new ones land: its built levels turn stale and its version moves
// on, so update_view remeshes it and a job already in flight (it may have read the map before the change) still lands
// but leaves the chunk stale (commit_one).
void BuildingRenderer::rebuild_chunk(const CityState &, int64_t key) {
	auto [found, fresh] = chunks_.try_emplace(key);
	Chunk &ch = found->second;
	ch.centre = Vector3((float(key_x(key)) + HALF) * CHUNK, 0, (float(key_y(key)) + HALF) * CHUNK);
	ch.version++;
	for (int l = 0; l < LEVELS; l++) ch.stale[l] = ch.built[l];
	Super &sp = supers_[super_of(ch.centre)];
	if (fresh) sp.chunks++;
	if (!sp.dirty) sp.due = now_s() + look_.far_rebuild_delay_s; // later changes ride on the same request
	sp.dirty = true;
}

// The far mesh of a superchunk the camera left; it is meshed again (dirty) once the camera comes back within reach.
void BuildingRenderer::drop_far(int64_t key, Super &sp) {
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		super_requested_.erase(key);
	}
	sp.free();
	sp.built = sp.has_content = false;
	sp.dirty = true;
	sp.mask_sent = -1;
}

void BuildingRenderer::drop_chunk(int64_t key) {
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		requested_.erase(key);
		requested_mask_.erase(key);
	}
	signatures_.drop(key);
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	drop(it->second, LEVEL_NEAR);
	drop(it->second, LEVEL_MID);
	drop_bands(it->second);
	const int64_t super_key = super_of(it->second.centre);
	chunks_.erase(it);
	auto sp = supers_.find(super_key);
	if (sp == supers_.end() || --sp->second.chunks > 0) return;
	drop_far(super_key, sp->second); // a superchunk lives while one of its chunks is streamed
	supers_.erase(sp);
}

void BuildingRenderer::drop(Chunk &ch, Level level) {
	if (level == LEVEL_NEAR) ch.occluder.clear();
	for (int i = 0; i < SUB_COUNT; i++) {
		if (level == LEVEL_NEAR) {
			ch.near[size_t(i)].free();
			ch.parts[size_t(i)].free();
		} else if (level == LEVEL_BAYS) {
			ch.mid_draws[size_t(i)][MID_BAYS].free();
		} else {
			ch.mid_hulls[size_t(i)].free();
			ch.mid_hull_meshes[size_t(i)].reset();
			ch.mid[size_t(i)].free();
		}
	}
	ch.built[level] = false;
	ch.stale[level] = false;
	ranges(ch);
}

void BuildingRenderer::drop_bands(Chunk &ch) {
	for (auto &sub : ch.prop_casts)
		for (ShadowTwin &d : sub) d.free();
	for (auto &sub : ch.bands)
		for (MultiMeshDraw &d : sub) d.free();
	for (auto &sub : ch.mid_draws)
		for (MultiMeshDraw &d : sub) d.free();
	for (ShadowTwin &d : ch.mid_casts) d.free();
	for (MeshDraw &d : ch.flat) d.free();
	ch.flags.free();
	ch.built[LEVEL_BAYS] = ch.stale[LEVEL_BAYS] = false; // the openings went with the mid level's draws
}

void BuildingRenderer::submit(const CityState &s, int64_t key, bool near, bool mid, bool bays, float priority) {
	auto ch = chunks_.find(key);
	if (!s.jobs || ch == chunks_.end()) return;
	const uint32_t version = ch->second.version;
	uint64_t seq;
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		seq = ++seq_;
		requested_[key] = seq;
		requested_mask_[key] = uint8_t((near ? BAND_BIT_NEAR : 0) | (mid ? BAND_BIT_MID : 0) | (bays ? BAND_BIT_BAYS : 0));
	}
	const JobKey job{ JobLane::BUILDING, key };
	s.jobs->submit(job, priority, [this, &s, key, job, seq, version, near, mid, bays, priority]() {
		Inputs in;
		{
			MapRead map(s); // only while copying: the build reads the copy
			in = gather(s, &key, 1);
		}
		Built b = build_cpu(in, key, near, mid, bays);
		b.job = job;
		b.seq = seq;
		b.version = version;
		b.priority = priority;
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.push_back(std::make_shared<Built>(std::move(b)));
	});
}

BuildingRenderer::SuperChunks BuildingRenderer::chunks_of(int64_t super_key) {
	SuperChunks chunks;
	const int sx = key_x(super_key) * CHUNKS_PER_SUPER, sz = key_y(super_key) * CHUNKS_PER_SUPER;
	for (int j = 0; j < CHUNKS_PER_SUPER; j++)
		for (int i = 0; i < CHUNKS_PER_SUPER; i++) chunks[size_t(j * CHUNKS_PER_SUPER + i)] = cell_key(sx + i, sz + j);
	return chunks;
}

void BuildingRenderer::submit_super(const CityState &s, int64_t key, float priority) {
	if (!s.jobs) return;
	uint64_t seq;
	auto sp = supers_.find(key);
	if (sp != supers_.end()) {
		sp->second.dirty = false; // a chunk dirtied later sets it again and resubmits once this lands
		sp->second.asked = now_s();
	}
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		seq = ++super_seq_;
		super_requested_[key] = seq;
	}
	s.jobs->submit(JobKey{ JobLane::BUILDING_SUPER, key }, priority, [this, &s, key, seq, priority]() {
		const SuperChunks chunks = chunks_of(key);
		Inputs in;
		{
			MapRead map(s); // only while copying: the build reads the copy
			in = gather(s, chunks.data(), int(chunks.size()));
		}
		size_t kept = 0; // the far level has no lots: a building in pieces is not in it at all (kept in order, each moved once)
		for (size_t r = 0; r < in.records.size(); r++) {
			if (in.broken->count(in.records[r].seed)) continue;
			if (kept != r) {
				in.records[kept] = std::move(in.records[r]);
				in.ids[kept] = in.ids[r];
			}
			kept++;
		}
		in.records.resize(kept);
		in.ids.resize(kept);
		SuperBuilt b = build_super_cpu(in, key);
		for (int64_t seen = super_bytes_max_.load(); b.bytes() > seen && !super_bytes_max_.compare_exchange_weak(seen, b.bytes());) {
		}
		b.job = JobKey{ JobLane::BUILDING_SUPER, key };
		b.seq = seq;
		b.priority = priority;
		std::lock_guard<std::mutex> lk(queue_mutex_);
		super_done_.push_back(std::make_shared<SuperBuilt>(std::move(b)));
	});
}

void BuildingRenderer::commit(UploadBudget &upload, const std::chrono::steady_clock::time_point &until, double &chunk_max,
		double &super_max) {
	chunk_max = super_max = 0;
	times_ = CommitTimes();
	if (!state_ || !state_->jobs) return;
	commit_baked(*state_->jobs, queue_mutex_, done_, requested_, baking_, upload, until, waiting_, [&](Built &b) {
		Laps one;
		commit_one(b);
		chunk_max = std::max(chunk_max, one.lap());
	}, &times_);
	commit_baked(*state_->jobs, queue_mutex_, super_done_, super_requested_, super_baking_, upload, until, super_waiting_,
			[&](SuperBuilt &b) {
				Laps one;
				commit_super(b);
				super_max = std::max(super_max, one.lap());
			}, &times_);
}

} // namespace city
