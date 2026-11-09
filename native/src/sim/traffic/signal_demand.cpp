// Which stage of an actuated signal is asked for (Roadcraft lab src/sim/signals/fsm.ts stageHasDemand, pickNextStage, and the
// crossings' waiting the lab's controller reads): the vehicles over or before each approach's detection section, the far vehicles
// due at its line, the walkers waiting at the kerbs; whether another stage than the running one is asked for, and which runs next.
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {
size_t lane_key(int ds, int lane) { return size_t(ds) * MAX_LANES + size_t(lane); }

// Vehicles that want approach `ds` green now: one over its detection section (its call stands until it reaches the line), the head of a
// lane standing or rolling within that section, or a far vehicle due at the line within the drive over it. `queued` counts the
// vehicles on its lanes, near and far, for the weight of the stage's claim.
bool approach_calls(const CityState &s, int ds, int &queued) {
	const TrafficState &t = s.traffic;
	const DirSeg &d = t.g.ds[size_t(ds)];
	if (!d.ok) return false;
	bool calls = size_t(ds) < t.green_wanted.size() && t.green_wanted[size_t(ds)] > t.time;
	const float reach = std::max(d.detector, s.catalog.traffic.at_line_m);
	for (int l = 0; l < d.lanes; l++) {
		const size_t key = lane_key(ds, l);
		queued += lane_load(t, int(key));
		const std::vector<int> &lane = t.lane_vehicles[key];
		if (!lane.empty() && d.len - d.stop_back - t.vehicles[size_t(lane.back())].s <= reach) calls = true;
		const int far = key < t.meso_head.size() ? t.meso_head[key] : -1;
		if (far >= 0 && t.vehicles[size_t(far)].t_exit <= t.time + double(d.extension)) calls = true;
	}
	return calls;
}

// Stage `k` is asked for by its vehicles; `queued` gathers what stands on its approaches.
bool stage_calls(const CityState &s, const Junction &j, size_t k, int &queued) {
	bool calls = false;
	for (int ds : j.approaches[k]) calls = approach_calls(s, ds, queued) || calls;
	return calls;
}

// Somebody waits to cross an arm whose traffic stage `held` lets go (a call made within signal_walk_call_s).
bool walk_called(const CityState &s, const Junction &j, size_t held) {
	return held < j.walk_call.size() && s.traffic.time - j.walk_call[held] <= double(s.catalog.traffic.signal_walk_call_s);
}

// Stage `m` would serve somebody: its vehicles, or walkers held by another stage's traffic (any stage but theirs lets them cross).
bool stage_wanted(const CityState &s, const Junction &j, size_t m, int &queued) {
	bool wanted = stage_calls(s, j, m, queued);
	for (size_t a = 0; a < j.walk_call.size(); a++)
		if (a != m && walk_called(s, j, a)) wanted = true;
	return wanted;
}

// The longest anybody waiting at a kerb that stage `m` lets cross has stood there, seconds (0: nobody).
double walk_wait(const CityState &s, const Junction &j, size_t m) {
	double most = 0;
	for (size_t a = 0; a < j.walk_from.size(); a++)
		if (a != m && j.walk_from[a] >= 0 && walk_called(s, j, a)) most = std::max(most, s.traffic.time - j.walk_from[a]);
	return most;
}

} // namespace

// Another stage than the one running is asked for: by its vehicles, or by walkers who may not cross while this one runs.
bool signal_competing(const CityState &s, const Junction &j) {
	const size_t k = size_t(j.run);
	if (walk_called(s, j, k)) return true;
	for (size_t m = 0; m < j.green.size(); m++) {
		int queued = 0;
		if (m != k && stage_calls(s, j, m, queued)) return true;
	}
	return false;
}

// The stage after `k` (Roadcraft lab fsm.ts pickNextStage): of those asked for, the one waiting longest since its green, its wait
// weighted by its queue; one overdue past signal_starve_cycles of the cycle first, then one with somebody long at a kerb; with
// nobody asking, the next in order. Ties go to the nearest in order after `k`, so a plan runs the same on the same calls.
size_t next_signal_stage(const CityState &s, const Junction &j, size_t k) {
	const TrafficParams &tp = s.catalog.traffic;
	const size_t stages = j.green.size();
	const double now = s.traffic.time, overdue_s = double(j.cycle) * double(tp.signal_starve_cycles);
	auto sooner = [&](size_t a, size_t b) { return (a + stages - k) % stages < (b + stages - k) % stages; };
	auto better = [&](size_t m, double age, size_t best, double best_age) {
		return best == stages || age > best_age || (age == best_age && sooner(m, best));
	};
	size_t pick = stages, late = stages, walkers = stages;
	double pick_age = 0, late_age = 0, longest = double(tp.signal_walk_wait_s);
	for (size_t m = 0; m < stages; m++) {
		if (m == k) continue;
		int queued = 0;
		if (!stage_wanted(s, j, m, queued)) continue;
		const double waited = now - (m < j.served.size() ? j.served[m] : now);
		const double age = waited * (1.0 + double(queued) / double(tp.signal_queue_weight));
		if (better(m, age, pick, pick_age)) { pick = m; pick_age = age; }
		if (waited >= overdue_s && !j.approaches[m].empty() && better(m, age, late, late_age)) { late = m; late_age = age; }
		const double wait = walk_wait(s, j, m);
		if (wait >= longest) { longest = wait; walkers = m; }
	}
	if (late < stages) return late;
	if (walkers < stages) return walkers;
	return pick < stages ? pick : (k + 1) % stages;
}

// The walkers standing at the kerbs of the signalled junctions (Roadcraft lab: each crossing's waiting, read by its controller): each
// one calls for a stage other than the one whose traffic crosses its arm, and the oldest wait of each is kept, ended when nobody
// waits there any more.
void call_walkers(CityState &s) {
	TrafficState &t = s.traffic;
	LaneGraph &g = t.g;
	for (const Pedestrian &w : t.peds) {
		if (!w.alive || w.mode != PedMode::WAIT || w.way_node < 0 || size_t(w.way_node) >= g.junctions.size() || w.wi >= w.wn) continue;
		const int arm = w.ways[w.wi].cross;
		Junction &j = g.junctions[size_t(w.way_node)];
		if (arm < 0 || j.ctl != Control::SIGNAL || j.walk_call.empty()) continue;
		const DirSeg &in = g.ds[size_t(in_ds(s.roads, arm, w.way_node))];
		if (in.ok && in.stage >= 0 && size_t(in.stage) < j.walk_call.size()) j.walk_call[size_t(in.stage)] = t.time;
	}
	for (int n : g.signal_nodes) {
		Junction &j = g.junctions[size_t(n)];
		for (size_t a = 0; a < j.walk_call.size() && a < j.walk_from.size(); a++) {
			if (j.walk_call[a] != t.time) j.walk_from[a] = -1;
			else if (j.walk_from[a] < 0) j.walk_from[a] = t.time;
		}
	}
}

} // namespace traffic
} // namespace city
