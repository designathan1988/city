// ChangeLog consumption and per-frame budgets (render.max_chunk_rebuilds_per_frame, commit_budget_ms, upload_budget_mb).
#include "render/render_world.h"

#include "render/owned_segments.h"
#include "sim/roads/road_grade.h"

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
enum Producer { ROAD_PRODUCER, FAR_ROAD_PRODUCER, VERGE_PRODUCER, STRUCTURE_PRODUCER,
	PROP_PRODUCER, BUILDING_PRODUCER, TREE_PRODUCER, COVER_PRODUCER, TRANSIT_PRODUCER, WATER_PRODUCER, PRODUCERS };
} // namespace

bool RenderWorld::init(const RenderContext &ctx, PackedStringArray &errors) {
	bool ok = roads_.init(ctx, errors);
	ok = ok && far_roads_.init(ctx, roads_.look(), errors);
	ok = ok && verges_.init(ctx, roads_.look(), errors);
	ok = ok && structures_.init(ctx, roads_.look(), errors);
	ok = ok && props_.init(ctx, roads_.look(), errors);
	ok = ok && trees_.init(ctx, roads_.look(), errors);
	ok = ok && cover_.init(ctx, errors);
	ok = ok && water_.init(ctx, errors);
	ok = buildings_.init(ctx, errors) && ok;
	stage_.use_designs(buildings_.own_designs());
	stage_.init(ctx.material("building", errors), ctx.material("building_glass", errors), ctx.material("building_sheer", errors), ctx.material("building_card", errors), ctx.material("building_ghost", errors));
	ok = agents_.init(ctx, errors) && ok;
	ok = transit_.init(ctx, errors) && ok;
	ok = overlays_.init(ctx, errors) && ok;
	ok = effects_.init(ctx, errors) && ok;
	effects_.set_people(&agents_.people());
	bind_ruin_maker();
	if (ok) {
		std::vector<Ref<Mesh>> bodies;
		std::vector<Vector3> sizes;
		for (const AgentWreck &w : agents_.wrecks()) {
			bodies.push_back(w.mesh);
			sizes.push_back(w.size);
		}
		effects_.set_wrecks(bodies, sizes);
	}
	ok = wear_.init(ctx, errors) && ok;
	return ok && errors.is_empty();
}

void RenderWorld::bind_ruin_maker() {
	canyon_.set_body_cache(buildings_.body_cache());
	effects_.set_ruin_maker([this, cache = buildings_.body_cache()](const b40::Data &data, const Building &rec, const HeightPatch &ground, const b40::BodyInput &body) {
		return ruin_of(data, buildings_.look(), rec, ground, body, cache);
	});
}

void RenderWorld::reset() {
	first_producer_ = 0;
	fog_.reset();
	canyon_.reset();
	shore_.reset();
	wear_.reset();
	roads_.reset();
	far_roads_.reset();
	verges_.reset();
	structures_.reset();
	props_.reset();
	trees_.reset();
	cover_.reset();
	water_.reset();
	buildings_.reset();
	stage_.clear();
	agents_.reset();
	transit_.reset();
	overlays_.reset();
	effects_.reset();
	bind_ruin_maker();
	road_stream_.clear();
	verge_stream_.clear();
	structure_stream_.clear();
	props_stream_.clear();
	building_stream_.clear();
}

void RenderWorld::full_rebuild(CityState &s, Object *terrain3d) {
	reset();
	budget_ = std::max(0, int(s.catalog.tune.render_max_chunk_rebuilds_per_frame));
	if (terrain3d) {
		terrain_.push_all(s.terrain, terrain3d);
		terrain_.push_all_paint(s.terrain, s.nature.ground, terrain3d);
	}
	water_.mark_all(s.terrain);
	for (const RoadSegment &g : s.roads.segs)
		if (g.alive) {
			int64_t k = chunk_of(float((g.q.x0 + g.q.x1) / 2), float((g.q.z0 + g.q.z1) / 2));
			road_stream_.mark(k);
			far_roads_.mark(k, s.jobs.get());
			if (structure_of(g.flags) != Structure::GROUND) structure_stream_.mark_far(k);
			overlays_.mark_dirty(k, s.jobs.get());
		}
	for (const RoadNode &n : s.roads.nodes)
		if (n.alive) {
			int64_t k = chunk_of(float(n.x), float(n.z));
			road_stream_.mark(k);
			far_roads_.mark(k, s.jobs.get());
			overlays_.mark_dirty(k, s.jobs.get());
		}
	verge_stream_.inherit_known(road_stream_);
	structure_stream_.inherit_known(road_stream_);
	props_stream_.inherit_known(road_stream_);
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
	prop_keys_.swap(s.log.prop_chunks);
	building_keys_.swap(s.log.building_chunks);
	view_keys_.swap(s.log.view_chunks);
	terrain_rects_.swap(s.log.terrain_rects);
	paint_rects_.swap(s.log.paint_rects);
	nature_keys_.swap(s.log.nature_chunks);
	water_keys_.swap(s.log.water_chunks);
	if (!building_keys_.empty()) canyon_.touch();
	if (!water_keys_.empty() || !terrain_rects_.empty()) shore_.touch(s, water_keys_, terrain_rects_);
}

// The landscape's changes: the trees and ground cover of the touched chunks are marked, the water's chunks and surface follow.
void RenderWorld::sync_nature(CityState &s) {
	ChangeLog::dedup(nature_keys_);
	for (int64_t k : nature_keys_) {
		trees_.mark_dirty(k, s.jobs.get());
		cover_.mark_dirty(k, s.jobs.get());
	}
	nature_keys_.clear();
	for (int64_t k : water_keys_) {
		water_.mark(k);
		cover_.mark_dirty(k, s.jobs.get()); // the stones of the shore stand by the water's level
	}
	water_keys_.clear();
}

// The chunks the edits drained from the change log touched, marked for every renderer that draws them, and the lists emptied.
void RenderWorld::mark_changed(const CityState &s) {
	// Producers append one key per segment/building; dedup once before the consumers walk the same list.
	ChangeLog::dedup(road_keys_);
	ChangeLog::dedup(building_keys_);
	ChangeLog::dedup(view_keys_);
	ChangeLog::dedup(prop_keys_);
	for (int64_t k : road_keys_) {
		roads_.invalidate(s.jobs.get(), JobKey{ JobLane::ROADS, k });
		verges_.invalidate(s.jobs.get(), JobKey{ JobLane::VERGES, k });
		structures_.invalidate(s.jobs.get(), JobKey{ JobLane::STRUCTURES, k });
		props_.invalidate(s.jobs.get(), JobKey{ JobLane::PROPS, k });
		road_stream_.mark(k);
		verge_stream_.mark(k);
		structure_stream_.mark(k);
		far_roads_.mark(k, s.jobs.get());
		each_owned_segment(s, k, [&](int sid) {
			if (structure_of(s.roads.segs[size_t(sid)].flags) != Structure::GROUND) structure_stream_.mark_far(k);
		});
		props_stream_.mark(k);
		trees_.mark_dirty(k, s.jobs.get());
		cover_.mark_dirty(k, s.jobs.get());
		overlays_.mark_dirty(k, s.jobs.get());
	}
	for (int64_t k : building_keys_) {
		building_stream_.mark(k);
		trees_.mark_dirty(k, s.jobs.get());
		cover_.mark_dirty(k, s.jobs.get());
	}
	for (int64_t k : prop_keys_) { // what the player built on a street (sim/street): its props and the trees round it
		props_.invalidate(s.jobs.get(), JobKey{ JobLane::PROPS, k });
		props_stream_.mark(k);
		trees_.mark_dirty(k, s.jobs.get());
	}
	transit_.touch(road_keys_);
	transit_.touch_terrain(s, terrain_rects_);
	for (int64_t k : view_keys_) buildings_.restyle_chunk(k);
	road_keys_.clear();
	building_keys_.clear();
	view_keys_.clear();
	prop_keys_.clear();
}

void RenderWorld::sync(CityState &s, Object *terrain3d, const Vector3 &cam) {
	const Tuning &t = s.catalog.tune;
	const int64_t window = int64_t(t.render_upload_budget_mb * BYTES_PER_MIB);
	const bool backlog = roads_.waiting() + far_roads_.waiting() + verges_.waiting() + structures_.waiting() + props_.waiting() + buildings_.waiting() > window * BACKLOG_WINDOWS;
	BuildBudget budget(backlog ? 0 : std::max(0, int(t.render_max_chunk_rebuilds_per_frame)));
	Laps laps;
	{
		// Only reads the map: shared, so a frame never waits for the mesh jobs (they read it shared too).
		std::shared_lock<std::shared_mutex> lk(s.map_mutex);
		cost_.lock = laps.lap();
		drain_log(s);
		effects_.sync(s);
		const std::vector<int64_t> repaint = effects_.take_repaint();
		if (!repaint.empty()) {
			buildings_.set_broken(effects_.broken());
			building_keys_.insert(building_keys_.end(), repaint.begin(), repaint.end());
		}
		if (terrain3d && !terrain_rects_.empty()) terrain_.push_rects(s.terrain, terrain3d, terrain_rects_);
		if (terrain3d && !paint_rects_.empty()) terrain_.push_paint(s.terrain, s.nature.ground, terrain3d, paint_rects_);
		paint_rects_.clear();
		sync_nature(s);
		mark_changed(s);
		terrain_rects_.clear();
		// Registration has no geometry admission; every builder below shares the rotating producer order.
		BuildBudget registry = BuildBudget::unlimited();
		building_stream_.update(cam.x, cam.z, buildings_.stream_reach(s, cam), STREAM_DROP_K, registry,
				[&](int64_t k) { buildings_.rebuild_chunk(s, k); }, [&](int64_t k) { buildings_.drop_chunk(k); });
		roads_.update_view(cam);
		cost_.stream = laps.lap();
		cost_.levels = laps.lap();
		for (int turn = 0; turn < PRODUCERS; turn++) {
			const int producer = (first_producer_ + turn) % PRODUCERS;
			switch (producer) {
				case ROAD_PRODUCER:
					road_stream_.update(cam.x, cam.z, t.render_road_end_m, STREAM_DROP_K, budget,
						[&](int64_t k) { roads_.submit(s, k, *s.jobs, 0.f); }, [&](int64_t k) { roads_.invalidate(s.jobs.get(), JobKey{ JobLane::ROADS, k }); roads_.drop_chunk(k); });
					break;
				case FAR_ROAD_PRODUCER: far_roads_.update(s, *s.jobs, budget, cam); break;
				case VERGE_PRODUCER:
					verge_stream_.update(cam.x, cam.z, t.render_road_end_m, STREAM_DROP_K, budget,
						[&](int64_t k) { verges_.submit(s, k, *s.jobs, 0.f); }, [&](int64_t k) { verges_.invalidate(s.jobs.get(), JobKey{ JobLane::VERGES, k }); verges_.drop_chunk(k); });
					break;
				case STRUCTURE_PRODUCER:
					structure_stream_.update(cam.x, cam.z, t.render_road_end_m, STREAM_DROP_K, budget,
						[&](int64_t k) { structures_.submit(s, k, *s.jobs, 0.f); }, [&](int64_t k) { structures_.invalidate(s.jobs.get(), JobKey{ JobLane::STRUCTURES, k }); structures_.drop_chunk(k); }, buildings_.stream_reach(s, cam));
					break;
				case PROP_PRODUCER:
					props_stream_.update(cam.x, cam.z, t.render_props_end_m, STREAM_DROP_K, budget,
						[&](int64_t k) { props_.submit(s, k, *s.jobs, 0.f); }, [&](int64_t k) { props_.invalidate(s.jobs.get(), JobKey{ JobLane::PROPS, k }); props_.drop_chunk(k); });
					break;
				case BUILDING_PRODUCER: buildings_.update_view(cam, budget); break;
				case TREE_PRODUCER: trees_.sync(s, cam, budget); break;
				case COVER_PRODUCER: cover_.sync(s, cam, budget); break;
				case TRANSIT_PRODUCER: transit_.sync(s, budget); break;
				case WATER_PRODUCER: water_.sync(s.terrain, budget); break;
			}
			const double elapsed = laps.lap();
			if (producer < BUILDING_PRODUCER) cost_.stream += elapsed; else cost_.levels += elapsed;
		}
		cost_.backlog = backlog;
		cost_.budget_left = budget.left;
	}
	// The fields' jobs take the map themselves; their windows follow the view as the chunks do (one per-frame driver: here).
	fog_.sync(s, cam.x, cam.z);
	canyon_.sync(s, cam.x, cam.z);
	shore_.sync(s);
	cost_.fields = laps.lap();
	cost_.flush = 0; // legacy probe field: no artificial RenderingServer drain
	const auto until = std::chrono::steady_clock::now() + std::chrono::milliseconds(int64_t(t.render_commit_budget_ms));
	const bool uploads_due = ++upload_frame_ >= upload_every_;
	if (uploads_due) upload_frame_ = 0;
	UploadBudget upload(uploads_due ? window : 0, roads_.baking() + far_roads_.baking() + verges_.baking() + structures_.baking() + props_.baking() + buildings_.baking());
	cost_.roads = cost_.props = cost_.buildings = cost_.trees = 0;
	for (int turn = 0; turn < PRODUCERS; turn++) {
		const int producer = (first_producer_ + turn) % PRODUCERS;
		switch (producer) {
			case ROAD_PRODUCER: roads_.commit(*s.jobs, upload, until); break;
			case FAR_ROAD_PRODUCER: far_roads_.commit(*s.jobs, upload, until); break;
			case VERGE_PRODUCER: verges_.commit(*s.jobs, upload, until); break;
			case STRUCTURE_PRODUCER: structures_.commit(*s.jobs, upload, until); break;
			case PROP_PRODUCER: props_.commit(*s.jobs, upload, until); break;
			case BUILDING_PRODUCER: buildings_.commit(upload, until, cost_.chunk_max, cost_.super_max); break;
			case TREE_PRODUCER: trees_.commit(until); break;
			case COVER_PRODUCER: cover_.commit(until); break;
			case TRANSIT_PRODUCER: case WATER_PRODUCER: break; // GPU publication in continuations still needs common S04 admission
		}
		const double elapsed = laps.lap();
		if (producer < PROP_PRODUCER) cost_.roads += elapsed;
		else if (producer == PROP_PRODUCER) cost_.props += elapsed;
		else if (producer == BUILDING_PRODUCER) cost_.buildings += elapsed;
		else cost_.trees += elapsed;
	}
	far_roads_.fold(roads_); // both road queues have had their turn
	cost_.roads += laps.lap();
	cost_.building = buildings_.commit_times();
	first_producer_ = (first_producer_ + 1) % PRODUCERS;
	cost_.uploads = double(upload.used) / BYTES_PER_MIB;
}

} // namespace city
