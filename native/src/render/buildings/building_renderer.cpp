// Building chunk streaming: the near (+ parts) and mid levels are built lazily, nearest first, when the camera comes
// within their reach (near end / mid end, plus a pad) and freed beyond 1.25x it. Every dirty chunk marks its 512 m
// superchunk, whose far/HLOD mesh is built within building_far_end_m (the city never vanishes when zooming out). See
// building_renderer.h for the switch between a chunk's own detail and the far mesh.
#include "render/buildings/building_renderer.h"

#include "core/laps.h"
#include "core/units.h"

#include "render/buildings/facade.h"
#include "render/mesh_builder.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f, REACH_PAD = 0.25f, DROP_K = 1.25f;
constexpr float PROJECTION_HEIGHT = 1080.f;
constexpr float VIEW_ANGLE_FALLBACK = 90.f, MIN_SCREEN_PX = 1e-3f;
constexpr int64_t SUPER_JOB_OFFSET = 1 << 20; // keeps superchunk job keys clear of every 256 m chunk key
constexpr int BAND_BIT_NEAR = 2, BAND_BIT_MID = 4;
constexpr int CHUNKS_PER_SUPER = int(SUPERCHUNK / CHUNK); // per axis

int64_t super_of(const Vector3 &centre) { return chunk_of(centre.x, centre.z, SUPERCHUNK); }

Vector3 super_centre(int64_t key) {
	return Vector3((float(key_x(key)) + HALF) * SUPERCHUNK, 0, (float(key_y(key)) + HALF) * SUPERCHUNK);
}

int64_t super_job_key(int64_t key) {
	return cell_key(key_x(key) + int(SUPER_JOB_OFFSET), key_y(key) + int(SUPER_JOB_OFFSET)) * 4 + 3;
}

void range(const MeshDraw &d, float begin, float end) {
	if (d.inst.is_valid())
		RenderingServer::get_singleton()->instance_geometry_set_visibility_range(d.inst, begin, end, 0, 0,
				RenderingServer::VISIBILITY_RANGE_FADE_DISABLED);
}

float horizontal(const Vector3 &cam, const Vector3 &at) {
	float dx = cam.x - at.x, dz = cam.z - at.z;
	return std::sqrt(dx * dx + dz * dz);
}

double now_s() { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }

// fn(key, chunk) for every streamed chunk of a superchunk.
template <typename Map, typename F>
void each_chunk_of(Map &chunks, int64_t super_key, F &&fn) {
	const int sx = key_x(super_key) * CHUNKS_PER_SUPER, sz = key_y(super_key) * CHUNKS_PER_SUPER;
	for (int j = 0; j < CHUNKS_PER_SUPER; j++)
		for (int i = 0; i < CHUNKS_PER_SUPER; i++) {
			auto it = chunks.find(cell_key(sx + i, sz + j));
			if (it != chunks.end()) fn(it->first, it->second);
		}
}
} // namespace

bool BuildingRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("building", errors);
	material_far_ = ctx.material("building_far", errors);
	material_parts_ = ctx.material("building_parts", errors);
	Dictionary t = ctx.table("buildings", errors);
	if (!errors.is_empty()) return false;
	if (!data_.load(t, errors)) return false;
	MeshBuilder unit;
	unit.box(Vector3(-HALF, -HALF, -HALF), Vector3(HALF, HALF, HALF), Color(1, 1, 1, 1), Vector2(), true);
	unit_box_ = unit.commit();
	return true;
}

void BuildingRenderer::reset() {
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
	for (auto &kv : supers_) kv.second.far.free();
	supers_.clear();
	bodies_.clear();
}

void BuildingRenderer::set_ranges(const CityState &s) {
	state_ = &s;
	float detail = s.catalog.tune.render_building_detail_end_m;
	parts_end_ = detail * data_.parts_share;
	near_end_ = detail * data_.near_share;
	mid_end_ = s.catalog.tune.render_building_mid_end_m;
	far_end_ = s.catalog.tune.render_building_far_end_m;
	float fov = data_.far_fov_deg > 0 ? data_.far_fov_deg : VIEW_ANGLE_FALLBACK;
	float half_tan = std::tan(fov * PI_F / float(DEG_PER_TURN));
	for (int i = 0; i < BAND_COUNT; i++) {
		const b40::PartBand &pb = data_.part_bands[size_t(std::min(i, int(data_.part_bands.size()) - 1))];
		float px = std::max(pb.px_min, MIN_SCREEN_PX);
		band_end_[size_t(i)] = std::min(pb.thick_max * PROJECTION_HEIGHT / (2 * half_tan * px), far_end_);
	}
}

// A dirty chunk keeps its meshes on screen until the new ones land: its built levels turn stale and its version moves
// on, so update_view remeshes it and a job already in flight (it may have read the map before the change) still lands
// but leaves the chunk stale (commit_one).
void BuildingRenderer::rebuild_chunk(const CityState &s, int64_t key) {
	(void)s;
	auto [found, fresh] = chunks_.try_emplace(key);
	Chunk &ch = found->second;
	ch.centre = Vector3((float(key_x(key)) + HALF) * CHUNK, 0, (float(key_y(key)) + HALF) * CHUNK);
	ch.version++;
	for (int l = 0; l < LEVELS; l++) ch.stale[l] = ch.built[l];
	Super &sp = supers_[super_of(ch.centre)];
	if (fresh) sp.chunks++;
	if (!sp.dirty) sp.due = now_s() + data_.far_rebuild_delay_s; // later changes ride on the same request
	sp.dirty = true;
}

// Chunk instances: near and mid hand over at the near end; the mid level has no end of its own (the chunk is shown or
// left to the far mesh as a whole: show). The far instance keeps no range (its chunk_mask hides the detailed chunks).
void BuildingRenderer::ranges(Chunk &ch) const {
	for (int i = 0; i < SUB_COUNT; i++) {
		range(ch.near[size_t(i)], 0, near_end_);
		range(ch.parts[size_t(i)], 0, parts_end_);
		range(ch.mid[size_t(i)], near_end_, 0);
	}
	for (int b = 0; b < BAND_COUNT; b++) ch.bands[size_t(b)].range(0, band_end_[size_t(b)]);
}

float BuildingRenderer::near_reach() const { return near_end_ + CHUNK * REACH_PAD; }

// The far mesh of a superchunk the camera left; it is meshed again (dirty) once the camera comes back within reach.
void BuildingRenderer::drop_far(int64_t key, Super &sp) {
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		super_requested_.erase(key);
	}
	sp.far.free();
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
	for (int i = 0; i < SUB_COUNT; i++) {
		if (level == LEVEL_NEAR) {
			ch.near[size_t(i)].free();
			ch.parts[size_t(i)].free();
		} else {
			ch.mid[size_t(i)].free();
		}
	}
	ch.built[level] = false;
	ch.stale[level] = false;
}

void BuildingRenderer::drop_bands(Chunk &ch) {
	for (MultiMeshDraw &d : ch.bands) d.free();
}

void BuildingRenderer::submit(const CityState &s, int64_t key, bool far, bool near, bool mid, float priority) {
	(void)far;
	auto ch = chunks_.find(key);
	if (!s.jobs || ch == chunks_.end()) return;
	const uint32_t version = ch->second.version;
	uint64_t seq;
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		seq = ++seq_;
		requested_[key] = seq;
		requested_mask_[key] = uint8_t((near ? BAND_BIT_NEAR : 0) | (mid ? BAND_BIT_MID : 0));
	}
	s.jobs->submit(key * 4 + 3, priority, [this, &s, key, seq, version, near, mid, priority]() { // job: Built::job
		Inputs in;
		{
			std::shared_lock<std::shared_mutex> map(s.map_mutex); // only while copying: the build reads the copy
			in = gather(s, &key, 1);
		}
		Built b = build_cpu(in, key, near, mid);
		b.job = key * 4 + 3;
		b.seq = seq;
		b.version = version;
		b.priority = priority;
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.push_back(std::make_shared<Built>(std::move(b)));
	});
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
	s.jobs->submit(super_job_key(key), priority, [this, &s, key, seq, priority]() {
		std::array<int64_t, CHUNKS_PER_SUPER * CHUNKS_PER_SUPER> chunks;
		const int sx = key_x(key) * CHUNKS_PER_SUPER, sz = key_y(key) * CHUNKS_PER_SUPER;
		for (int j = 0; j < CHUNKS_PER_SUPER; j++)
			for (int i = 0; i < CHUNKS_PER_SUPER; i++) chunks[size_t(j * CHUNKS_PER_SUPER + i)] = cell_key(sx + i, sz + j);
		Inputs in;
		{
			std::shared_lock<std::shared_mutex> map(s.map_mutex); // only while copying: the build reads the copy
			in = gather(s, chunks.data(), int(chunks.size()));
		}
		SuperBuilt b = build_super_cpu(in, key);
		for (int64_t seen = super_bytes_max_.load(); b.bytes() > seen && !super_bytes_max_.compare_exchange_weak(seen, b.bytes());) {
		}
		b.job = super_job_key(key);
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

int BuildingRenderer::pending() const {
	std::lock_guard<std::mutex> lk(queue_mutex_);
	return int(requested_.size() + done_.size() + super_requested_.size() + super_done_.size());
}

// A superchunk, every frame: each chunk's switch between its own detail and the far mesh, the levels it needs or no
// longer needs, and the far mesh's mask of the chunks drawn in detail (never a hole: a chunk leaves the far mesh only
// once its mid level is up, and shows itself whatever the distance while no far mesh stands in).
void BuildingRenderer::visit_super(int64_t key, Super &sp, const Vector3 &cam, float eye, std::vector<Want> &want) {
	const float leave = mid_end_ * (1 + data_.far_hysteresis), mid_reach = mid_end_ + CHUNK * REACH_PAD;
	const bool far_missing = !sp.built || !sp.has_content;
	const int sx = key_x(key) * CHUNKS_PER_SUPER, sz = key_y(key) * CHUNKS_PER_SUPER;
	int mask = 0;
	each_chunk_of(chunks_, key, [&](int64_t ck, Chunk &ch) {
		const float dc = horizontal(cam, ch.centre); // streaming of the levels: the ground distance
		const float d = std::sqrt(dc * dc + eye * eye); // the switch: with the eye height, as visibility ranges measure
		ch.detail = ch.detail ? d < leave : d < mid_end_;
		uint8_t levels = 0;
		if ((!ch.built[LEVEL_NEAR] || ch.stale[LEVEL_NEAR]) && dc < near_reach()) levels = uint8_t(levels | BAND_BIT_NEAR);
		else if (ch.built[LEVEL_NEAR] && dc > near_reach() * DROP_K) drop(ch, LEVEL_NEAR);
		// a chunk drawn in detail keeps its mid level current even in the hysteresis band past mid_reach
		if ((!ch.built[LEVEL_MID] || ch.stale[LEVEL_MID]) && (dc < mid_reach || ch.detail)) {
			levels = uint8_t(levels | BAND_BIT_MID);
		} else if (ch.built[LEVEL_MID] && !ch.detail && dc > mid_reach * DROP_K) {
			drop(ch, LEVEL_MID);
			drop_bands(ch);
		}
		const bool drawn = ch.built[LEVEL_MID] && (ch.detail || far_missing);
		if (drawn != ch.shown) show(ch, drawn);
		if (drawn) mask |= 1 << ((key_y(ck) - sz) * CHUNKS_PER_SUPER + (key_x(ck) - sx));
		if (levels == 0) return;
		std::lock_guard<std::mutex> lk(queue_mutex_);
		if (!requested_.count(ck)) want.push_back({ dc, ck, levels });
	});
	sp.mask = mask;
	send_mask(sp);
}

void BuildingRenderer::update_view(const Vector3 &cam, BuildBudget &budget) {
	if (!state_) return;
	// RenderWorld::sync calls this under the shared map lock (the ground height is a map read)
	const float eye = std::max(0.f, cam.y - state_->terrain.height_at(cam.x, cam.z));
	const double now = now_s();
	std::vector<Want> want, want_super;
	for (auto &[key, sp] : supers_) {
		// the far reach from the superchunk's centre, as the chunks are streamed (a nearest-point reach would hold a
		// ring of extra far meshes, a quarter more memory, for buildings past building_far_end_m)
		const float ground = horizontal(cam, super_centre(key));
		if (ground > far_end_ * DROP_K && sp.far.inst.is_valid()) drop_far(key, sp);
		visit_super(key, sp, cam, eye, want);
		// the first far mesh at once (nothing stands in yet); a rebuild of a built one on its clock
		const bool due = !sp.built || now >= std::max(sp.due, sp.asked + data_.far_rebuild_interval_s);
		if (sp.dirty && due && ground <= far_end_) {
			std::lock_guard<std::mutex> lk(queue_mutex_);
			if (!super_requested_.count(key)) want_super.push_back({ ground, key, 0 });
		}
	}
	std::sort(want.begin(), want.end(), [](const Want &a, const Want &b) { return a.d < b.d; });
	for (const Want &w : want) {
		if (!budget.take()) break;
		submit(*state_, w.key, false, (w.mask & BAND_BIT_NEAR) != 0, (w.mask & BAND_BIT_MID) != 0, w.d);
	}
	std::sort(want_super.begin(), want_super.end(), [](const Want &a, const Want &b) { return a.d < b.d; });
	for (const Want &w : want_super) {
		if (!budget.take()) break;
		submit_super(*state_, w.key, w.d);
	}
}

} // namespace city
