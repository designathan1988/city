// Frame probes for the tests: where the last _process went (render_cost) and how the agents' poses arrive
// (pose_stats: publications, their gaps, frames shown past the newest pose).
#include "api/city_core.h"

#include "core/units.h"

#include <algorithm>

using namespace godot;

namespace city {

Dictionary CityCore::render_cost() const {
	const RenderWorld::Cost &c = render_.cost();
	Dictionary d;
	d["process"] = process_ms_;
	d["agents"] = agents_ms_;
	d["lock"] = c.lock;
	d["stream"] = c.stream;
	d["levels"] = c.levels;
	d["flush"] = c.flush;
	d["chunk_max"] = c.chunk_max;
	d["super_max"] = c.super_max;
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
	const int64_t last = st.last_tick_ns;
	st = PoseStats();
	st.last_tick_ns = last;
	return d;
}

} // namespace city
