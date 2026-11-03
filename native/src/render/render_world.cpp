// ChangeLog consumption and per-frame budgets (render.max_chunk_rebuilds_per_frame).
#include "render/render_world.h"

#include <chrono>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float STREAM_DROP_K = 1.25f; // a streamed chunk is freed this far beyond its reach (no rebuild churn at the edge)
} // namespace

bool RenderWorld::init(const RenderContext &ctx, PackedStringArray &errors) {
	bool ok = roads_.init(ctx, errors);
	ok = ok && props_.init(ctx, roads_.look(), errors);
	ok = ok && trees_.init(ctx, roads_.look(), props_.furniture(), errors);
	ok = buildings_.init(ctx, errors) && ok;
	ok = agents_.init(ctx, errors) && ok;
	ok = overlays_.init(ctx, errors) && ok;
	return ok && errors.is_empty();
}

void RenderWorld::reset() {
	roads_.reset();
	props_.reset();
	trees_.reset();
	buildings_.reset();
	agents_.reset();
	overlays_.reset();
	road_stream_.clear();
	props_stream_.clear();
	building_stream_.clear();
}

void RenderWorld::full_rebuild(CityState &s, Object *terrain3d) {
	reset();
	budget_ = std::max(1, int(s.catalog.tune.render_max_chunk_rebuilds_per_frame));
	if (terrain3d) terrain_.push_all(s.terrain, terrain3d);
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
	s.log.clear_render();
}

void RenderWorld::sync(CityState &s, Object *terrain3d, const Vector3 &cam) {
	{
		std::unique_lock<std::shared_mutex> lk(s.map_mutex);
		if (terrain3d && !s.log.terrain_rects.empty()) terrain_.push_rects(s.terrain, terrain3d, s.log.terrain_rects);
		// Producers append one key per segment/building; dedup once before three consumers walk the same list.
		ChangeLog::dedup(s.log.road_chunks);
		ChangeLog::dedup(s.log.building_chunks);
		for (int64_t k : s.log.road_chunks) {
			road_stream_.mark(k);
			props_stream_.mark(k);
			trees_.mark_dirty(k);
			overlays_.mark_dirty(k);
		}
		for (int64_t k : s.log.building_chunks) {
			building_stream_.mark(k);
			trees_.mark_dirty(k);
		}
		s.log.clear_render();
	}
	const Tuning &t = s.catalog.tune;
	BuildBudget budget(std::max(1, int(t.render_max_chunk_rebuilds_per_frame)));
	{
		std::shared_lock<std::shared_mutex> lk(s.map_mutex); // the decision phase only reads the map
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
		trees_.sync(s, cam, budget);
		buildings_.update_view(cam, budget);
	}
	const auto until = std::chrono::steady_clock::now() + std::chrono::milliseconds(int64_t(t.render_commit_budget_ms));
	roads_.commit(until);
	props_.commit(until);
	buildings_.commit(until);
	trees_.commit(until);
}

} // namespace city
