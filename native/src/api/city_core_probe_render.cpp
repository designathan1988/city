// Probes for the tests: where the last _process went (render_cost), the road meshes' size (road_mesh_counts) and how the agents' poses
// arrive (pose_stats: publications, their gaps, frames shown past the newest pose). The traffic's own probe is city_core_probe.cpp.
#include "api/city_core.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

Dictionary CityCore::agent_cost() const { return render_.agent_cost(); }

// The wear field counted cell by cell: a scan of the whole field, for the captures only (never per frame).
Dictionary CityCore::wear_probe() const {
	Dictionary d;
	d["worn_cells"] = render_.worn_cells();
	d["wear_publications"] = render_.wear_publications();
	return d;
}

Dictionary CityCore::render_cost() const {
	const RenderWorld::Cost &c = render_.cost();
	Dictionary d;
	d["process"] = process_ms_; // constant time: the game reads it on every long frame (health log), so it must not make one
	d["agents"] = agents_ms_;
	d["lock"] = c.lock;
	d["stream"] = c.stream;
	d["levels"] = c.levels;
	d["flush"] = c.flush;
	d["chunk_max"] = c.chunk_max;
	d["super_max"] = c.super_max;
	d["super_mb_max"] = double(render_.super_bytes_max()) / BYTES_PER_MIB; // largest far mesh built
	d["b_scan"] = c.building.scan;
	d["b_freed"] = c.building.freed;
	d["b_sort"] = c.building.sort;
	d["b_admit"] = c.building.admit;
	d["b_hand"] = c.building.hand;
	d["b_tail"] = c.building.tail;
	d["roads"] = c.roads;
	d["props"] = c.props;
	d["buildings"] = c.buildings;
	d["trees"] = c.trees;
	d["overlays"] = overlays_ms_;
	d["uploads"] = c.uploads; // MiB
	const BuildingRenderer::ViewNeed need = render_.building_need();
	d["backlog"] = c.backlog;
	d["budget_left"] = c.budget_left;
	d["b_want"] = need.want;
	d["b_unmet"] = need.unmet;
	d["b_requested"] = need.requested;
	d["b_done"] = need.done;
	d["jobs_replaced"] = int64_t(state_->jobs ? state_->jobs->replaced() : 0);
	return d;
}

namespace {
Dictionary count_of(const RoadChunks::Count &n) {
	Dictionary d;
	d["chunks"] = n.chunks; // one instance and one draw call each
	d["triangles"] = n.all.triangles;
	d["vertices"] = n.all.vertices;
	d["bytes"] = n.all.bytes;
	d["largest_bytes"] = n.largest;
	return d;
}
} // namespace

Dictionary CityCore::road_mesh_counts() const {
	Dictionary d;
	d["roads"] = count_of(render_.road_count());
	d["verges"] = count_of(render_.verge_count());
	d["wear_bytes"] = render_.wear_bytes();
	return d;
}

namespace {
constexpr float EARLY_GAP = 0.5f, LATE_GAP = 1.5f; // pose_stats: a publication this share of a tick after the last
} // namespace

void CityCore::note_pose(const PoseFrame &f, bool fresh, const PoseSample &at) {
	PoseStats &st = pose_stats_;
	st.frames++;
	if (at.beyond) st.held++; // the moment drawn is past the newest pose: the agents stand still this frame
	if (at.before) st.behind++; // older than the trail: they jump to its oldest pose
	if (!fresh || f.period_ns <= 0) return;
	const int64_t newest = f.stamp_ns[POSE_TRAIL - 1];
	if (st.last_tick_ns > 0) {
		const int64_t gap = newest - st.last_tick_ns;
		st.gap_min_ns = st.publications > 0 ? std::min(st.gap_min_ns, gap) : gap;
		st.gap_max_ns = std::max(st.gap_max_ns, gap);
		if (float(gap) < EARLY_GAP * float(f.period_ns)) st.early++;
		if (float(gap) > LATE_GAP * float(f.period_ns)) st.late++;
		st.publications++;
	}
	st.last_tick_ns = newest;
}

Dictionary CityCore::pose_stats() {
	PoseStats &st = pose_stats_;
	Dictionary d;
	d["frames"] = st.frames;
	d["held"] = st.held;
	d["behind"] = st.behind;
	d["publications"] = st.publications;
	d["early"] = st.early;
	d["late"] = st.late;
	d["gap_min_ms"] = double(st.gap_min_ns) / NS_PER_MS;
	d["gap_max_ms"] = double(st.gap_max_ns) / NS_PER_MS;
	d["sim_wakeups"] = int64_t(sim_.wakeups()); // since the clock started (not reset by this call)
	const int64_t last = st.last_tick_ns;
	st = PoseStats();
	st.last_tick_ns = last;
	return d;
}

} // namespace city
