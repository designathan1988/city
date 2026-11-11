// A green wave: the signals along the main road through a junction on one cycle, each offset by the drive from the first at
// the road's speed, so a platoon let go by one reaches the next as its green begins (FHWA Signal Timing Manual, 2nd ed., cap. 6:
// a common cycle, offsets from the travel time). One way: from the first junction of the road on.
#include "sim/traffic/traffic_system.h"

#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr int MOST_STOPS = 12; // junctions walked each way
constexpr float ARM_SPEED_MIN_MS = 1.f; // a lane with no speed of its own still takes time to drive

struct Stop {
	int node = -1, from = -1; // from: the segment the platoon arrives on (-1 at the first)
	int to = -1;
	float travel = 0; // seconds from the first junction
};

int other_end(const RoadSegment &g, int n) {
	return g.a == n ? g.b : g.a;
}

// The leg of the main road through `n` (junction_control.cpp main_legs) other than `leg`, -1 when the road ends here or `leg` is no part of it.
int on_past(const CityState &s, int n, int leg) {
	const std::vector<int> m = main_legs(s, n);
	if (m.size() < 2) return -1;
	if (m[0] == leg) return m[1];
	return m[1] == leg ? m[0] : -1;
}

// Seconds to drive `seg` leaving `n`.
float drive(const CityState &s, int seg, int n) {
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	const DirSeg &d = s.traffic.g.ds[size_t(seg) * 2 + (g.a == n ? 0 : 1)];
	return float(g.len) / std::max(ARM_SPEED_MIN_MS, d.speed);
}

// The junctions of the main road through `start`, in driving order from the far end behind it.
std::vector<Stop> corridor(const CityState &s, int start) {
	const std::vector<int> m = main_legs(s, start);
	if (m.size() < 2) return {};
	// back along one leg to the road's first junction
	int origin = start, ahead = m[1];
	for (int i = 0, leg = m[0]; i < MOST_STOPS; i++) {
		const int next = other_end(s.roads.segs[size_t(leg)], origin);
		const int past = on_past(s, next, leg);
		ahead = leg;
		origin = next;
		if (past < 0) break;
		leg = past;
	}
	std::vector<Stop> out;
	float travel = 0;
	int at = origin, leg = ahead, from = -1;
	for (int i = 0; i <= 2 * MOST_STOPS && leg >= 0; i++) {
		out.push_back({ at, from, leg, travel });
		travel += drive(s, leg, at);
		const int next = other_end(s.roads.segs[size_t(leg)], at);
		from = leg;
		leg = on_past(s, next, leg);
		at = next;
	}
	if (from >= 0) out.push_back({ at, from, -1, travel });
	return out;
}

float plan_cycle(const Junction &j) {
	float total = 0;
	for (size_t k = 0; k < j.green.size(); k++) total += j.green[k] + j.amber[k] + j.all_red[k];
	return total;
}

// The stage that lets the corridor's through traffic go at this stop.
size_t main_stage(const CityState &s, const Stop &c) {
	const LaneGraph &g = s.traffic.g;
	const int seg = c.from >= 0 ? c.from : c.to;
	const RoadSegment &r = s.roads.segs[size_t(seg)];
	const int in = seg * 2 + (r.b == c.node ? 0 : 1), out = seg * 2 + (r.a == c.node ? 0 : 1);
	const int stage = c.from >= 0 ? g.ds[size_t(in)].stage : g.ds[size_t(out ^ 1)].stage;
	return size_t(std::max(0, stage));
}
} // namespace

std::vector<std::pair<int, SignalSettings>> green_wave_plans(const CityState &s, int n) {
	const LaneGraph &g = s.traffic.g;
	std::vector<Stop> signals;
	for (const Stop &c : corridor(s, n)) {
		const Junction &j = g.junctions[size_t(c.node)];
		if (j.ctl == Control::SIGNAL && !j.green.empty()) signals.push_back(c);
	}
	std::vector<std::pair<int, SignalSettings>> out;
	if (signals.size() < 2) return out;
	float cycle = 0;
	for (const Stop &c : signals) cycle = std::max(cycle, plan_cycle(g.junctions[size_t(c.node)]));
	for (const Stop &c : signals) {
		const Junction &j = g.junctions[size_t(c.node)];
		SignalSettings set;
		set.fixed = true;
		set.greens = j.green;
		const size_t main = std::min(main_stage(s, c), set.greens.size() - 1);
		set.greens[main] += cycle - plan_cycle(j); // the common cycle's spare goes to the road the wave runs on
		float main_start = 0;
		for (size_t k = 0; k < main; k++) main_start += set.greens[k] + j.amber[k] + j.all_red[k];
		const float offset = std::fmod(c.travel - main_start, cycle);
		set.offset = offset < 0 ? offset + cycle : offset;
		out.emplace_back(c.node, set);
	}
	return out;
}

} // namespace traffic
} // namespace city
