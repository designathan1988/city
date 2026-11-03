// Walkers and vehicles at the crosswalks (Roadcraft traffic/junctions.ts carsOnCrossing/pedsInWay, signals.ts walkOk,
// state.ts PED_COURTESY/PED_CRITICAL_GAP/SPAN_REACH). At a light a walker crosses an arm only during the green of a
// stage that stops that arm; anywhere a walker steps off only when no vehicle is on the crosswalk or too close to
// stop for it (a vehicle that has stood at its open line for the courtesy time goes first). Vehicles give way to
// walkers on, or about to be on, the stretch of a crosswalk their lane drives over.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float MOVING_MS = 0.5f, NEAR_LINE_M = 3.f, COURTESY_NEAR_M = 4.f, MIN_BRAKE = 0.5f, MIN_FRONT_MS = 2.f;

float body(const CityState &s, const Vehicle &v) { return s.catalog.traffic.classes[int(v.type)].length; }
int in_ds(const CityState &s, int seg, int node) { return seg * 2 + (s.roads.segs[seg].b == node ? 0 : 1); }

// Metres to the stop line of the head of lane `key` (negative: over it).
float to_line(const CityState &s, int ds, const Vehicle &v) {
	const DirSeg &d = s.traffic.g.ds[ds];
	return d.len - v.s - d.stop_back;
}

// The head `v` of its lane has stood the courtesy time at its line and its light, if any, is green: it goes next.
bool courtesy_owed(const CityState &s, const Vehicle &v) {
	if (v.v > MOVING_MS || v.wait <= s.catalog.traffic.ped_courtesy_s) return false;
	return light_for(s, v.ds) == Light::GREEN;
}

// A head arriving on `ds` (into the node) would drive over the crosswalk of its own arm before a walker is across.
bool approaching(const CityState &s, int ds) {
	const TrafficState &t = s.traffic;
	const DirSeg &d = t.g.ds[ds];
	for (int l = 0; l < d.lanes; l++) {
		const auto &lane = t.lane_vehicles[size_t(ds) * MAX_LANES + l];
		if (lane.empty()) continue;
		const Vehicle &h = t.vehicles[lane.back()];
		float line = to_line(s, ds, h), b = std::max(MIN_BRAKE, s.catalog.traffic.classes[int(h.type)].b);
		if (h.pass >= 0 || line < 0 || (line < NEAR_LINE_M && h.v > MOVING_MS)) return true;
		if (h.v > MOVING_MS && line < h.v * h.v / (2 * b) + 1) return true; // too near to stop comfortably
		if (line < COURTESY_NEAR_M && courtesy_owed(s, h)) return true;
	}
	return false;
}

} // namespace

int green_stage(const CityState &s, int node) {
	const Junction &j = s.traffic.g.junctions[node];
	if (j.ctl != Control::SIGNAL || j.cycle <= 0) return -1;
	float x = float(std::fmod(s.traffic.time + j.offset, double(j.cycle)));
	for (size_t k = 0; k < j.green.size(); k++) {
		if (x < j.green[k]) return int(k);
		x -= j.green[k] + j.amber[k] + s.catalog.traffic.signal_all_red_s;
		if (x < 0) return -1;
	}
	return -1;
}

bool walk_ok(const CityState &s, int node, int arm_seg) {
	const Junction &j = s.traffic.g.junctions[node];
	if (j.ctl != Control::SIGNAL) return true;
	int stage = green_stage(s, node);
	if (stage < 0) return false;
	const DirSeg &d = s.traffic.g.ds[in_ds(s, arm_seg, node)];
	return !d.ok || d.stage != stage;
}

bool cars_on_crossing(const CityState &s, int node, int arm_seg) {
	const TrafficState &t = s.traffic;
	const TrafficParams &tp = s.catalog.traffic;
	for (int dir = 0; dir < 2; dir++) {
		int ds = arm_seg * 2 + dir;
		const DirSeg &d = t.g.ds[ds];
		if (!d.ok) continue;
		if (d.to == node) {
			if (approaching(s, ds)) return true;
			continue;
		}
		for (int l = 0; l < d.lanes; l++) { // leaving the node: the last one's body still over the crosswalk
			const auto &lane = t.lane_vehicles[size_t(ds) * MAX_LANES + l];
			if (!lane.empty() && t.vehicles[lane.front()].s - body(s, t.vehicles[lane.front()]) < tp.crosswalk_m) return true;
		}
	}
	const Junction &j = t.g.junctions[node];
	for (int x = j.conn_first; x < j.conn_first + j.conn_count; x++) {
		const Conn &c = t.g.conns[x];
		const auto &on = t.conn_vehicles[x];
		if (t.g.ds[c.from_ds].seg == arm_seg && !on.empty() && t.vehicles[on.back()].cs < body(s, t.vehicles[on.back()])) return true;
		if (t.g.ds[c.to_ds].seg != arm_seg) continue;
		if (!on.empty()) return true;
		// a head turning into this arm that has waited its share, or coming too soon to stop for someone stepping off
		const auto &lane = t.lane_vehicles[size_t(c.from_ds) * MAX_LANES + c.from_lane];
		if (lane.empty()) continue;
		const Vehicle &h = t.vehicles[lane.back()];
		if (pick_connector(s, h) != x) continue;
		float line = to_line(s, c.from_ds, h);
		if ((line < COURTESY_NEAR_M && courtesy_owed(s, h)) || (h.v > MOVING_MS && line / h.v < tp.ped_critical_gap_s)) return true;
	}
	return false;
}

bool peds_in_way(const CityState &s, int ci, float front_in, float front_out) {
	const TrafficState &t = s.traffic;
	const TrafficParams &tp = s.catalog.traffic;
	const Conn &c = t.g.conns[ci];
	const int seg_in = t.g.ds[c.from_ds].seg, seg_out = t.g.ds[c.to_ds].seg;
	for (int id : t.node_crossers[c.node]) {
		const Pedestrian &w = t.peds[id];
		const bool entry = w.cross_seg == seg_in;
		if (!entry && w.cross_seg != seg_out) continue;
		// where the lane meets the walker's line, in metres along it, and when the walker reaches and clears it
		const Waypoint &to = w.ways[w.wi];
		V2 dir = V2(to.x - w.lx, to.z - w.lz).norm();
		V2 q = entry ? V2(c.x0, c.z0) : V2(c.x1, c.z1);
		float at = (q - V2(w.lx, w.lz)).dot(dir), pace = std::max(MIN_BRAKE, w.speed);
		float clears = (at + tp.ped_span_reach_m - w.ls) / pace, reaches = (at - tp.ped_span_reach_m - w.ls) / pace;
		float front = entry ? front_in : front_out;
		if (clears > front && reaches < front + tp.ped_critical_gap_s) return true;
	}
	return false;
}

float front_time(const Vehicle &v, float metres) { return std::max(0.f, metres) / std::max(MIN_FRONT_MS, v.v); }

} // namespace traffic
} // namespace city
