// ChangeLog consumption and per-frame budgets (render.max_chunk_rebuilds_per_frame, commit_budget_ms, upload_budget_mb).
#include "render/render_world.h"

#include "core/units.h"

#include <chrono>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float STREAM_DROP_K = 1.25f; // a streamed chunk is freed this far beyond its reach (no rebuild churn at the edge)
// Built results may wait for this many upload windows: past that, no new mesh is built until the bakes catch up (the
// waiting shells are CPU memory; a camera jump would otherwise pile up gigabytes of them).
constexpr int64_t BACKLOG_WINDOWS = 4;
} // namespace

bool RenderWorld::init(const RenderContext &ctx, PackedStringArray &errors) {
	bool ok = roads_.init(ctx, errors);
	ok = ok && props_.init(ctx, roads_.look(), errors);
	ok = ok && trees_.init(ctx, roads_.look(), errors);
	ok = ok && cover_.init(ctx, errors);
	ok = ok && water_.init(ctx, errors);
	ok = buildings_.init(ctx, errors) && ok;
	stage_.init(ctx.material("building", errors), ctx.material("building_ghost", errors));
	ok = agents_.init(ctx, errors) && ok;
	ok = overlays_.init(ctx, errors) && ok;
	ok = wear_.init(ctx, errors) && ok;
	return ok && errors.is_empty();
}

void RenderWorld::reset() {
	wear_.reset();
	roads_.reset();
	props_.reset();
	trees_.reset();
	cover_.reset();
	water_.reset();
	buildings_.reset();
	stage_.clear();
	agents_.reset();
	overlays_.reset();
	road_stream_.clear();
	props_stream_.clear();
	building_stream_.clear();
}

void RenderWorld::full_rebuild(CityState &s, Object *terrain3d) {
	reset();
	budget_ = std::max(1, int(s.catalog.tune.render_max_chunk_rebuilds_per_frame));
	if (terrain3d) {
		terrain_.push_all(s.terrain, terrain3d);
		terrain_.push_all_paint(s.terrain, s.nature.ground, terrain3d);
	}
	fog_.reset();
	canyon_.reset();
	shore_.reset();
	water_.mark_all(s.terrain);
	for (const RoadSegment &g : s.roads.segs)
		if (g.alive) {
			int64_t k = chunk_of(float((g.q.x0 + g.q.x1) / 2), float((g.q.z0 + g.q.z1) / 2));
			road_stream_.mark(k);
			overlays_.mark_dirty(k);
		}
	for (const RoadNode &n : s.roads.nodes)
		if (n.alive) {
			int64_t k = chunk_of(float(n.x), float(n.z));
			road_stream_.mark(k);
			overlays_.mark_dirty(k);
		}
	props_stream_.known = road_stream_.known;
	for (const Building &b : s.buildings.items)
		if (b.alive) building_stream_.mark(chunk_of(b.x, b.z));
	buildings_.set_ranges(s);
	std::lock_guard<std::mutex> log(s.log.mutex); // everything already in the map is drawn by the rebuild
	s.log.clear_render();
}

// Takes the render lists out of the ChangeLog under its own mutex (the map is held shared: change_log.h).
void RenderWorld::drain_log(CityState &s) {
	std::lock_guard<std::mutex> log(s.log.mutex);
	road_keys_.swap(s.log.road_chunks);
	building_keys_.swap(s.log.building_chunks);
	terrain_rects_.swap(s.log.terrain_rects);
	paint_rects_.swap(s.log.paint_rects);
	nature_keys_.swap(s.log.nature_chunks);
	water_keys_.swap(s.log.water_chunks);
	if (!building_keys_.empty()) canyon_.touch();
	if (!water_keys_.empty() || !terrain_rects_.empty()) shore_.touch();
}

void RenderWorld::sync(CityState &s, Object *terrain3d, const Vector3 &cam) {
	const Tuning &t = s.catalog.tune;
	const int64_t window = int64_t(t.render_upload_budget_mb * BYTES_PER_MIB);
	const bool backlog = roads_.waiting() + props_.waiting() + buildings_.waiting() > window * BACKLOG_WINDOWS;
	BuildBudget budget(backlog ? 0 : std::max(1, int(t.render_max_chunk_rebuilds_per_frame)));
	Laps laps;
	{
		// Only reads the map: shared, so a frame never waits for the mesh jobs (they read it shared too).
		std::shared_lock<std::shared_mutex> lk(s.map_mutex);
		cost_.lock = laps.lap();
		drain_log(s);
		if (terrain3d && !terrain_rects_.empty()) terrain_.push_rects(s.terrain, terrain3d, terrain_rects_);
		if (terrain3d && !paint_rects_.empty()) terrain_.push_paint(s.terrain, s.nature.ground, terrain3d, paint_rects_);
		paint_rects_.clear();
		ChangeLog::dedup(nature_keys_);
		for (int64_t k : nature_keys_) {
			trees_.mark_dirty(k);
			cover_.mark_dirty(k);
		}
		nature_keys_.clear();
		for (int64_t k : water_keys_) water_.mark(k);
		water_keys_.clear();
		water_.sync(s.terrain);
		// Producers append one key per segment/building; dedup once before three consumers walk the same list.
		ChangeLog::dedup(road_keys_);
		ChangeLog::dedup(building_keys_);
		for (int64_t k : road_keys_) {
			road_stream_.mark(k);
			props_stream_.mark(k);
			trees_.mark_dirty(k);
			cover_.mark_dirty(k);
			overlays_.mark_dirty(k);
		}
		for (int64_t k : building_keys_) {
			building_stream_.mark(k);
			trees_.mark_dirty(k);
			cover_.mark_dirty(k);
		}
		road_keys_.clear();
		building_keys_.clear();
		terrain_rects_.clear();
		road_stream_.update(cam.x, cam.z, t.render_road_end_m, STREAM_DROP_K, budget,
				[&](int64_t k) { roads_.submit(s, k, *s.jobs, 0.f); }, [&](int64_t k) { roads_.drop_chunk(k); });
		props_stream_.update(cam.x, cam.z, t.render_props_end_m, STREAM_DROP_K, budget,
				[&](int64_t k) { props_.submit(s, k, *s.jobs, 0.f); }, [&](int64_t k) { props_.drop_chunk(k); });
		// a building chunk's "build" only registers it (its levels are meshed and budgeted by update_view), so the
		// shared mesh budget is not spent on it and a superchunk knows all its chunks at once
		BuildBudget registry = BuildBudget::unlimited();
		building_stream_.update(cam.x, cam.z, t.render_building_far_end_m, STREAM_DROP_K, registry,
				[&](int64_t k) { buildings_.rebuild_chunk(s, k); }, [&](int64_t k) { buildings_.drop_chunk(k); });
		roads_.update_view(cam);
		cost_.stream = laps.lap();
		trees_.sync(s, cam, budget);
		cover_.sync(s, cam, budget);
		buildings_.update_view(cam, budget);
		cost_.levels = laps.lap();
	}
	const auto until = std::chrono::steady_clock::now() + std::chrono::milliseconds(int64_t(t.render_commit_budget_ms));
	UploadBudget upload(window, roads_.baking() + props_.baking() + buildings_.baking());
	RenderingServer::get_singleton()->get_video_adapter_name(); // a wrapped call: runs the queued commands first
	cost_.flush = laps.lap();
	roads_.commit(*s.jobs, upload, until);
	cost_.roads = laps.lap();
	props_.commit(*s.jobs, upload, until);
	cost_.props = laps.lap();
	buildings_.commit(upload, until, cost_.chunk_max, cost_.super_max);
	cost_.buildings = laps.lap();
	cost_.building = buildings_.commit_times();
	trees_.commit(until);
	cover_.commit(until);
	cost_.trees = laps.lap();
	cost_.uploads = double(upload.used) / BYTES_PER_MIB;
}

} // namespace city
