// ChangeLog consumption and the frame's one admission (render.max_chunk_rebuilds_per_frame, commit_budget_ms, upload_budget_mb:
// frame_admission.h).
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
} // namespace

bool RenderWorld::init(const RenderContext &ctx, PackedStringArray &errors) {
	bool ok = roads_.init(ctx, errors);
	ok = ok && far_roads_.init(ctx, roads_.look(), roads_.shapes(), errors);
	ok = ok && verges_.init(ctx, roads_.look(), roads_.shapes(), errors);
	ok = ok && structures_.init(ctx, roads_.look(), roads_.shapes(), errors);
	ok = ok && props_.init(ctx, roads_.look(), errors);
	ground_.set_budget(size_t(ctx.catalog->tune.render_ground_cache_mb * BYTES_PER_MIB));
	ok = ok && trees_.init(ctx, roads_.look(), ground_, errors);
	ok = ok && cover_.init(ctx, ground_, errors);
	ok = ok && water_.init(ctx, errors);
	ok = buildings_.init(ctx, errors) && ok;
	stage_.use_designs(buildings_.own_designs());
	stage_.init(ctx.material("building", errors), ctx.material("building_glass", errors), ctx.material("building_sheer", errors), ctx.material("building_card", errors), ctx.material("building_ghost", errors));
	ok = agents_.init(ctx, errors) && ok;
	ok = transit_.init(ctx, errors) && ok;
	ok = overlays_.init(ctx, errors) && ok;
	ok = effects_.init(ctx, errors) && ok;
	effects_.set_people(&agents_.people());
	effects_.set_ruin_maker([this](const b40::Data &data, const Building &rec, const HeightPatch &ground) { return buildings_.ruin_of(data, rec, ground); });
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

void RenderWorld::reset() {
	frame_.reset();
	upload_debt_ = 0;
	carried_bytes_ = 0;
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
	road_stream_.clear();
	raised_stream_.clear();
	props_stream_.clear();
	building_stream_.clear();
}

void RenderWorld::full_rebuild(CityState &s, Object *terrain3d) {
	reset();
	if (terrain3d) {
		terrain_.push_all(s.terrain, terrain3d);
		terrain_.push_all_paint(s.terrain, s.nature.ground, terrain3d);
	}
	fog_.reset();
	canyon_.reset();
	shore_.reset();
	water_.mark_wet(s.terrain);
	for (const RoadSegment &g : s.roads.segs)
		if (g.alive) {
			int64_t k = chunk_of(float((g.q.x0 + g.q.x1) / 2), float((g.q.z0 + g.q.z1) / 2));
			road_stream_.mark(k);
			far_roads_.mark(k);
			if (structure_of(g.flags) != Structure::GROUND) raised_stream_.mark(k);
			overlays_.mark_dirty(k);
		}
	for (const RoadNode &n : s.roads.nodes)
		if (n.alive) {
			int64_t k = chunk_of(float(n.x), float(n.z));
			road_stream_.mark(k);
			far_roads_.mark(k);
			overlays_.mark_dirty(k);
		}
	props_stream_.adopt_known(road_stream_);
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
	if (!building_keys_.empty()) canyon_.touch(building_keys_);
	if (!water_keys_.empty() || !terrain_rects_.empty()) shore_.touch();
	if (!terrain_rects_.empty()) fog_.touch_terrain(s, terrain_rects_);
}

// The landscape's changes: the trees and ground cover of the touched chunks are marked, the water's chunks and surface follow.
void RenderWorld::sync_nature() {
	ChangeLog::dedup(nature_keys_);
	for (int64_t k : nature_keys_) {
		trees_.mark_dirty(k);
		cover_.mark_dirty(k);
	}
	nature_keys_.clear();
	for (int64_t k : water_keys_) {
		water_.mark(k);
		cover_.mark_dirty(k); // the stones of the shore stand by the water's level
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
		road_stream_.mark(k);
		far_roads_.mark(k);
		each_owned_segment(s, k, [&](int sid) {
			if (structure_of(s.roads.segs[size_t(sid)].flags) != Structure::GROUND) raised_stream_.mark(k);
		});
		if (raised_stream_.known.count(k)) raised_stream_.rebuild(k);
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
	for (int64_t k : prop_keys_) { // what the player built on a street (sim/street): its props and the trees round it
		props_stream_.mark(k);
		trees_.mark_dirty(k);
	}
	transit_.touch(road_keys_);
	transit_.touch_terrain(s, terrain_rects_);
	for (int64_t k : view_keys_) buildings_.restyle_chunk(k);
	road_keys_.clear();
	building_keys_.clear();
	view_keys_.clear();
	prop_keys_.clear();
}

int64_t RenderWorld::baking() const {
	return roads_.baking() + far_roads_.baking() + verges_.baking() + structures_.baking() + props_.baking() + buildings_.baking() + trees_.baking() +
			water_.baking();
}

int64_t RenderWorld::waiting() const {
	return roads_.waiting() + far_roads_.waiting() + verges_.waiting() + structures_.waiting() + props_.waiting() + buildings_.waiting() + trees_.waiting() +
			water_.waiting();
}

// The streamed chunks' build units: a road chunk's are its road, verge and (unless it stands out from afar) structure jobs, one
// credit each, asked together; a chunk with a result on its way waits for it, marked.
void RenderWorld::offer_streams(const CityState &s, const Vector3 &cam, std::vector<FrameAdmission::Unit> &units) {
	const Tuning &t = s.catalog.tune;
	JobPool &pool = *s.jobs;
	auto one = [](int64_t) { return 1; };
	road_stream_.offer(cam.x, cam.z, t.render_road_end_m, STREAM_DROP_K, [&](int64_t k) { return raised_stream_.known.count(k) ? 2 : 3; }, units,
			[this, &s, &pool](int64_t k, float d) {
				roads_.submit(s, k, pool, d);
				verges_.submit(s, k, pool, d);
				if (!raised_stream_.known.count(k)) structures_.submit(s, k, pool, d);
			},
			[this](int64_t k) {
				roads_.drop_chunk(k);
				verges_.drop_chunk(k);
				if (!raised_stream_.known.count(k)) structures_.drop_chunk(k);
			},
			[this](int64_t k) { return roads_.busy(k) || verges_.busy(k) || (!raised_stream_.known.count(k) && structures_.busy(k)); });
	raised_stream_.offer(cam.x, cam.z, buildings_.stream_reach(s, cam), STREAM_DROP_K, one, units,
			[this, &s, &pool](int64_t k, float d) { structures_.submit(s, k, pool, d); }, [this](int64_t k) { structures_.drop_chunk(k); },
			[this](int64_t k) { return structures_.busy(k); });
	far_roads_.offer(s, pool, cam, units);
	props_stream_.offer(cam.x, cam.z, t.render_props_end_m, STREAM_DROP_K, one, units, [this, &s, &pool](int64_t k, float d) { props_.submit(s, k, pool, d); },
			[this](int64_t k) { props_.drop_chunk(k); }, [this](int64_t k) { return props_.busy(k); });
	// a building chunk's "build" only registers it (its levels are meshed and admitted by update_view), so no credit is spent
	// on it and a superchunk knows all its chunks at once
	building_stream_.update_all(cam.x, cam.z, buildings_.stream_reach(s, cam), STREAM_DROP_K, [this, &s](int64_t k, float) { buildings_.rebuild_chunk(s, k); },
			[this](int64_t k) { buildings_.drop_chunk(k); });
}

void RenderWorld::sync(CityState &s, Object *terrain3d, const Vector3 &cam) {
	const Tuning &t = s.catalog.tune;
	using clock = std::chrono::steady_clock;
	// One deadline from here: the drain, the fields, the flush of the uploads and every commit are inside it.
	const clock::time_point until = clock::now() + std::chrono::milliseconds(int64_t(t.render_commit_budget_ms));
	const int64_t window = int64_t(t.render_upload_budget_mb * BYTES_PER_MIB);
	const bool backlog = waiting() > window * BACKLOG_WINDOWS;
	const bool uploads_due = ++upload_frame_ >= upload_every_;
	if (uploads_due) upload_frame_ = 0;
	if (frame_) upload_debt_ = frame_->upload.debt_after(); // what the last frame (and its overlays) left owing
	frame_.emplace(until, backlog ? 0 : std::max(1, t.render_max_chunk_rebuilds_per_frame), UploadBudget(uploads_due ? window : 0, baking(), upload_debt_));
	FrameAdmission &f = *frame_;
	f.upload.charge(carried_bytes_); // the wear's tiles sent since the last sync
	carried_bytes_ = 0;
	Laps laps;
	{
		// Only reads the map: shared, so a frame never waits for the mesh jobs (they read it shared too).
		std::shared_lock<std::shared_mutex> lk(s.map_mutex);
		cost_.lock = laps.lap();
		drain_log(s);
		agents_.people().set_rain(s.nature.weather.rain); // the walkers' umbrellas follow the weather (worn_things.h)
		effects_.touch_terrain(s, terrain_rects_);
		effects_.sync(s, f);
		const std::vector<int64_t> repaint = effects_.take_repaint();
		if (!repaint.empty()) {
			buildings_.set_broken(effects_.broken());
			building_keys_.insert(building_keys_.end(), repaint.begin(), repaint.end());
		}
		// The terrain an edit changed is sent this frame (the roads and the shore of the same edit stand on it): paid into the
		// ledger, into debt if it must, which the deferrable uploads of the next frames repay.
		if (terrain3d && !terrain_rects_.empty()) f.upload.charge(terrain_.push_rects(s.terrain, terrain3d, terrain_rects_));
		if (terrain3d && !paint_rects_.empty()) f.upload.charge(terrain_.push_paint(s.terrain, s.nature.ground, terrain3d, paint_rects_));
		paint_rects_.clear();
		sync_nature();
		mark_changed(s);
		terrain_rects_.clear();
		units_.clear();
		offer_streams(s, cam, units_);
		cost_.stream = laps.lap();
		transit_.sync(s, f);
		// The buildings first, with the frame's credits: a chunk they leave for the next frame is a hole in the city (rank 0 of the
		// frame's order, which update_view keeps in its own); then every other producer's units in one order.
		buildings_.update_view(cam, f.build);
		trees_.offer(s, cam, units_);
		cover_.offer(s, cam, units_);
		water_.offer(s, cam, units_);
		f.submit(units_);
		units_.clear();
		cost_.levels = laps.lap();
		cost_.backlog = backlog;
		cost_.budget_left = f.build.left;
	}
	// The fields' jobs take the map themselves; their windows follow the view as the chunks do (one per-frame driver: here).
	fog_.sync(s, cam.x, cam.z, f);
	canyon_.sync(s, cam.x, cam.z, f);
	shore_.sync(s, f);
	laps.lap();
	cost_.flush = f.flush_uploads();
	laps.lap();
	SignatureBodies &designs = *buildings_.own_designs();
	designs.set_pool(s.jobs.get());
	designs.sync(f); // the kinds of design a job made ready, before the chunks that stand them commit
	buildings_.commit(f.upload, f.until, cost_.chunk_max, cost_.super_max);
	cost_.buildings = laps.lap();
	cost_.building = buildings_.commit_times();
	// Every other chunk queue in the frame's one order: the bakes the ledger admits, then the hand-overs while the deadline lasts.
	enum Hand { ROADS, FAR_ROADS, VERGES, STRUCTURES, PROPS, TREES_NEAR, TREES_FAR, COVER, WATER }; // the queues below, in their order
	f.commit({ &roads_, &far_roads_, &verges_, &structures_, &props_, trees_.near_queue(), trees_.far_queue(), cover_.queue(), &water_ }, &hand_ms_);
	far_roads_.fold(roads_);
	cost_.roads = hand_ms_[ROADS] + hand_ms_[FAR_ROADS] + hand_ms_[VERGES] + hand_ms_[STRUCTURES];
	cost_.props = hand_ms_[PROPS];
	cost_.trees = hand_ms_[TREES_NEAR] + hand_ms_[TREES_FAR] + hand_ms_[COVER];
	laps.lap();
	cost_.uploads = double(f.upload.used) / BYTES_PER_MIB;
}

} // namespace city
