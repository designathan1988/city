// Building chunk streaming: the near (+ parts) and mid levels are built lazily, nearest first, when the camera comes
// within their reach and freed beyond 1.5x it; every dirty chunk marks its 1024 m superchunk, whose far/HLOD mesh is
// built within building_far_end_m and has no visibility end (the city never vanishes when zooming out).
#include "render/buildings/building_renderer.h"
#include "render/chunk_jobs.h"

#include "render/buildings/facade.h"
#include "render/mesh_builder.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float AABB_MIN_Y = -100.f, AABB_H = 700.f;
constexpr float HALF = 0.5f, REACH_PAD = 0.25f, DROP_K = 1.25f;
constexpr float PROJECTION_HEIGHT = 1080.f;
constexpr float VIEW_ANGLE_FALLBACK = 90.f, FULL_TURN_DEG = 360.f, MIN_SCREEN_PX = 1e-3f;
constexpr int64_t SUPER_JOB_OFFSET = 1 << 20; // keeps superchunk job keys clear of every 256 m chunk key
constexpr int BAND_BIT_NEAR = 2, BAND_BIT_MID = 4;

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
	unit_box_ = RenderingServer::get_singleton()->mesh_create();
	unit.commit_to(unit_box_);
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
	for (auto &kv : chunks_) {
		Chunk &ch = kv.second;
		for (int i = 0; i < SUB_COUNT; i++) {
			ch.near[i].free(); ch.parts[i].free(); ch.mid[i].free();
		}
		for (MultiMeshDraw &d : ch.bands) d.free();
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
	float half_tan = std::tan(fov * PI_F / FULL_TURN_DEG);
	for (int i = 0; i < BAND_COUNT; i++) {
		const b40::PartBand &pb = data_.part_bands[size_t(std::min(i, int(data_.part_bands.size()) - 1))];
		float px = std::max(pb.px_min, MIN_SCREEN_PX);
		band_end_[size_t(i)] = std::min(pb.thick_max * PROJECTION_HEIGHT / (2 * half_tan * px), far_end_);
	}
}

void BuildingRenderer::rebuild_chunk(const CityState &s, int64_t key) {
	(void)s;
	Chunk &ch = chunks_[key];
	ch.centre = Vector3((float(key_x(key)) + HALF) * CHUNK, 0, (float(key_y(key)) + HALF) * CHUNK);
	supers_[super_of(ch.centre)].dirty = true;
}

void BuildingRenderer::ranges(Chunk &ch) const {
	range(ch.near[0], 0, near_end_);
	for (int i = 1; i < SUB_COUNT; i++) range(ch.near[size_t(i)], 0, near_end_);
	for (int i = 0; i < SUB_COUNT; i++) {
		range(ch.parts[size_t(i)], 0, parts_end_);
		range(ch.mid[size_t(i)], near_end_, mid_end_);
	}
	for (int b = 0; b < BAND_COUNT; b++) ch.bands[size_t(b)].range(0, band_end_[size_t(b)]);
}

void BuildingRenderer::ranges_super(Super &sp) const { range(sp.far, mid_end_, 0); }

float BuildingRenderer::reach(Level level) const { return (level == LEVEL_NEAR ? near_end_ : mid_end_) + CHUNK * REACH_PAD; }

void BuildingRenderer::drop_super(int64_t key) {
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		super_requested_.erase(key);
	}
	auto it = supers_.find(key);
	if (it != supers_.end()) it->second.far.free();
}

void BuildingRenderer::drop_chunk(int64_t key) {
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		requested_.erase(key);
		requested_mask_.erase(key);
	}
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	Chunk &ch = it->second;
	for (int i = 0; i < SUB_COUNT; i++) {
		ch.near[i].free(); ch.parts[i].free(); ch.mid[i].free();
	}
	for (MultiMeshDraw &d : ch.bands) d.free();
	chunks_.erase(it);
}

void BuildingRenderer::drop(Chunk &ch, Level level) {
	if (level == LEVEL_NEAR) {
		for (int i = 0; i < SUB_COUNT; i++) { ch.near[i].free(); ch.parts[i].free(); }
	}
	else
		for (int i = 0; i < SUB_COUNT; i++) ch.mid[i].free();
	ch.built[level] = false;
}

void BuildingRenderer::submit(const CityState &s, int64_t key, bool far, bool near, bool mid, float priority) {
	(void)far;
	if (!s.jobs) return;
	uint64_t seq;
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		seq = ++seq_;
		requested_[key] = seq;
		requested_mask_[key] = uint8_t((near ? BAND_BIT_NEAR : 0) | (mid ? BAND_BIT_MID : 0));
	}
	s.jobs->submit(key * 4 + 3, priority, [this, &s, key, seq, near, mid, priority]() {
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		Built b = build_cpu(s, key, near, mid);
		b.seq = seq;
		b.priority = priority;
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.push_back(std::move(b));
	});
}

void BuildingRenderer::submit_super(const CityState &s, int64_t key, float priority) {
	if (!s.jobs) return;
	uint64_t seq;
	auto sp = supers_.find(key);
	if (sp != supers_.end()) sp->second.dirty = false; // a chunk dirtied later sets it again and resubmits once this lands
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		seq = ++super_seq_;
		super_requested_[key] = seq;
	}
	s.jobs->submit(super_job_key(key), priority, [this, &s, key, seq, priority]() {
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		SuperBuilt b = build_super_cpu(s, key);
		b.seq = seq;
		b.priority = priority;
		std::lock_guard<std::mutex> lk(queue_mutex_);
		super_done_.push_back(std::move(b));
	});
}

void BuildingRenderer::commit(const std::chrono::steady_clock::time_point &until) {
	commit_done(queue_mutex_, done_, requested_, until, [this](Built &b) { commit_one(b); });
}

int BuildingRenderer::pending() const {
	std::lock_guard<std::mutex> lk(queue_mutex_);
	return int(requested_.size() + done_.size() + super_requested_.size() + super_done_.size());
}

void BuildingRenderer::update_view(const Vector3 &cam, BuildBudget &budget) {
	if (!state_) return;
	for (auto it = chunks_.begin(); it != chunks_.end();) {
		float dx = cam.x - it->second.centre.x, dz = cam.z - it->second.centre.z;
		float d = std::sqrt(dx * dx + dz * dz);
		if (d > reach(LEVEL_MID) * DROP_K) {
			Chunk &ch = it->second;
			for (int i = 0; i < SUB_COUNT; i++) { ch.near[i].free(); ch.parts[i].free(); ch.mid[i].free(); }
			for (MultiMeshDraw &mm : ch.bands) mm.free();
			it = chunks_.erase(it);
			continue;
		}
		++it;
	}
	struct Want {
		float d;
		int64_t key;
		uint8_t mask;
	};
	std::vector<Want> want;
	int r = int(std::ceil(reach(LEVEL_MID) / CHUNK)), cx = int(std::floor(cam.x / CHUNK)), cz = int(std::floor(cam.z / CHUNK));
	for (int j = cz - r; j <= cz + r; j++)
		for (int i = cx - r; i <= cx + r; i++) {
			int64_t key = cell_key(i, j);
			auto it = chunks_.find(key);
			if (it == chunks_.end()) continue;
			Chunk &ch = it->second;
			float dx = cam.x - ch.centre.x, dz = cam.z - ch.centre.z;
			float d = std::sqrt(dx * dx + dz * dz); // horizontal: the eye's height is not a distance to the chunk
			uint8_t mask = 0;
			for (int l = 0; l < LEVELS; l++) {
				Level level = Level(l);
				if (!ch.built[l] && d < reach(level)) mask = uint8_t(mask | (l == LEVEL_NEAR ? BAND_BIT_NEAR : BAND_BIT_MID));
				else if (ch.built[l] && d > reach(level) * DROP_K) drop(ch, level);
			}
			if (mask == 0) continue;
			std::lock_guard<std::mutex> lk(queue_mutex_);
			if (!requested_.count(key)) want.push_back({ d, key, mask });
		}
	std::sort(want.begin(), want.end(), [](const Want &a, const Want &b) { return a.d < b.d; });
	for (const Want &w : want) {
		if (!budget.take()) break;
		submit(*state_, w.key, false, (w.mask & BAND_BIT_NEAR) != 0, (w.mask & BAND_BIT_MID) != 0, w.d);
	}
	std::vector<Want> want_super;
	for (auto it = supers_.begin(); it != supers_.end();) {
		Vector3 c = super_centre(it->first);
		float dx = cam.x - c.x, dz = cam.z - c.z, d = std::sqrt(dx * dx + dz * dz);
		if (d > far_end_ * DROP_K) {
			drop_super(it->first);
			it = supers_.erase(it);
			continue;
		}
		if (it->second.dirty && d <= far_end_) {
			std::lock_guard<std::mutex> lk(queue_mutex_);
			if (!super_requested_.count(it->first)) want_super.push_back({ d, it->first, 0 });
		}
		++it;
	}
	std::sort(want_super.begin(), want_super.end(), [](const Want &a, const Want &b) { return a.d < b.d; });
	for (const Want &w : want_super) {
		if (!budget.take()) break;
		submit_super(*state_, w.key, w.d);
	}
}

} // namespace city
