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

constexpr float MOVING_MS = 0.5f, NEAR_LINE_M = 3.f, COURTESY_NEAR_M = 4.f, MIN_BRAKE = 0.5f, MIN_FRONT_MS = 2.f; // MIN_FRONT_MS: the slowest top front_time assumes

float body(const CityState &s, const Vehicle &v) { return s.catalog.traffic.classes[int(v.type)].length; }

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

// A head arriving on `ds` (into the node) would drive over the crosswalk of its own arm before a walker is across: it is too near to
// stop, or comes within the gap `gap` the walker asks (a head standing at its line is not arriving).
bool approaching(const CityState &s, int ds, float gap) {
	const TrafficState &t = s.traffic;
	const DirSeg &d = t.g.ds[ds];
	for (int l = 0; l < d.lanes; l++) {
		const auto &lane = t.lane_vehicles[size_t(ds) * MAX_LANES + l];
		if (lane.empty()) continue;
		const Vehicle &h = t.vehicles[lane.back()];
		float line = to_line(s, ds, h), b = std::max(MIN_BRAKE, s.catalog.traffic.classes[int(h.type)].b);
		if (h.pass >= 0 || line < 0 || (line < NEAR_LINE_M && h.v > MOVING_MS)) return true;
		if (h.v > MOVING_MS && line < h.v * h.v / (2 * b) + 1) return true; // too near to stop comfortably
		if (h.v > MOVING_MS && line / h.v < gap) return true;
		if (line < COURTESY_NEAR_M && courtesy_owed(s, h)) return true;
	}
	return false;
}

// The gap a walker held at the kerb of cache key `key` asks of the traffic now (Roadcraft lab crossings/permission.ts pedGapAccepted):
// ped_critical_gap_s, less its impatience, never under ped_gap_floor_s.
float asked_gap(const CityState &s, int key) {
	const TrafficParams &tp = s.catalog.traffic;
	const TrafficState &t = s.traffic;
	const double from = key >= 0 && size_t(key) < t.cross_held_from.size() ? t.cross_held_from[size_t(key)] : -1.0;
	const float waited = from < 0 ? 0.f : float(t.time - from);
	return std::max(tp.ped_gap_floor_s, tp.ped_critical_gap_s - std::min(tp.ped_gap_impatience_max_s, tp.ped_gap_impatience_rate * waited));
}

// The traffic holds (`held`) or lets go the walkers at the kerb of cache key `key`: when they were last held, and since when.
void note_held(CityState &s, int key, bool held) {
	TrafficState &t = s.traffic;
	if (key < 0) return;
	if (t.cross_held.size() <= size_t(key)) {
		t.cross_held.resize(t.cross_stamp.size(), -double(BIG));
		t.cross_held_from.resize(t.cross_stamp.size(), -1.0);
	}
	if (size_t(key) >= t.cross_held.size()) return;
	if (!held) {
		t.cross_held_from[size_t(key)] = -1;
		return;
	}
	t.cross_held[size_t(key)] = t.time;
	if (t.cross_held_from[size_t(key)] < 0) t.cross_held_from[size_t(key)] = t.time;
}

} // namespace

// Index of `arm_seg` among the node's arms (the crosswalk cache key); -1 when the node does not keep it.
int arm_slot(const CityState &s, int node, int arm_seg) {
	if (node < 0 || node >= int(s.roads.nodes.size())) return -1;
	const auto &segs = s.roads.nodes[size_t(node)].segs;
	for (int i = 0; i < int(segs.size()) && i < NODE_ARMS_MAX; i++)
		if (segs[size_t(i)] == arm_seg) return i;
	return -1;
}

// The answers of one step are reused: walkers and vehicles ask the same (node, arm) many times per tick. Kind 0 is
// walk_ok, kind 1 is cars_on_crossing.
int cache_key(const CityState &s, int node, int arm_seg, int kind) {
	const int slot = arm_slot(s, node, arm_seg);
	if (slot < 0) return -1;
	return (node * NODE_ARMS_MAX + slot) * CROSS_CACHE_KINDS + kind;
}

bool cached(const CityState &s, int key, int kind, bool &out) {
	if (key < 0 || size_t(key) >= s.traffic.cross_stamp.size()) return false;
	if (s.traffic.cross_stamp[size_t(key)] != uint32_t(s.traffic.tick)) return false;
	out = (kind == 0 ? s.traffic.cross_walk[size_t(key)] : s.traffic.cross_cars[size_t(key)]) != 0;
	return true;
}

void store(CityState &s, int key, int kind, bool value) {
	if (key < 0 || size_t(key) >= s.traffic.cross_stamp.size()) return;
	s.traffic.cross_stamp[size_t(key)] = uint32_t(s.traffic.tick);
	(kind == 0 ? s.traffic.cross_walk[size_t(key)] : s.traffic.cross_cars[size_t(key)]) = value ? 1 : 0;
}

bool walk_ok(CityState &s, int node, int arm_seg) {
	const int key = cache_key(s, node, arm_seg, 0);
	bool cached_ok = false;
	if (cached(s, key, 0, cached_ok)) return cached_ok;
	const Junction &j = s.traffic.g.junctions[size_t(node)];
	bool ok = true;
	if (j.ctl == Control::SIGNAL) {
		const DirSeg &d = s.traffic.g.ds[size_t(in_ds(s.roads, arm_seg, node))];
		ok = walk_signal(s, node, arm_seg, d.ok ? d.stage : -1);
	}
	store(s, key, 0, ok);
	return ok;
}

bool cars_on_crossing(CityState &s, int node, int arm_seg) {
	const int cache = cache_key(s, node, arm_seg, 1);
	bool cached_cars = false;
	if (cached(s, cache, 1, cached_cars)) return cached_cars;
	const TrafficState &t = s.traffic;
	const float gap = asked_gap(s, cache);
	auto done = [&](bool v) {
		store(s, cache, 1, v);
		note_held(s, cache, v);
		return v;
	};
	for (int dir = 0; dir < 2; dir++) {
		int ds = arm_seg * 2 + dir;
		const DirSeg &d = t.g.ds[size_t(ds)];
		if (!d.ok) continue;
		if (d.to == node) {
			if (approaching(s, ds, gap)) return done(true);
			continue;
		}
		for (int l = 0; l < d.lanes; l++) { // leaving the node: the last one's body still over the crosswalk
			const auto &lane = t.lane_vehicles[size_t(ds) * MAX_LANES + l];
			if (!lane.empty() && t.vehicles[size_t(lane.front())].s - body(s, t.vehicles[size_t(lane.front())]) < s.catalog.furniture.crossing.cleared())
				return done(true);
		}
	}
	for (int x = t.g.node_conn[size_t(node)]; x >= 0; x = t.g.conns[size_t(x)].next_at_node) {
		const Conn &c = t.g.conns[size_t(x)];
		if (!c.alive) continue;
		const auto &on = t.conn_vehicles[size_t(x)];
		if (t.g.ds[size_t(c.from_ds)].seg == arm_seg && !on.empty() &&
				t.vehicles[size_t(on.back())].cs < body(s, t.vehicles[size_t(on.back())])) return done(true);
		if (t.g.ds[size_t(c.to_ds)].seg != arm_seg) continue;
		if (!on.empty()) return done(true);
		// a head turning into this arm that has waited its share, or coming too soon to stop for someone stepping off
		const auto &lane = t.lane_vehicles[size_t(c.from_ds) * MAX_LANES + c.from_lane];
		if (lane.empty()) continue;
		const Vehicle &h = t.vehicles[size_t(lane.back())];
		if (pick_connector(s, h) != x) continue;
		float line = to_line(s, c.from_ds, h);
		if ((line < COURTESY_NEAR_M && courtesy_owed(s, h)) || (h.v > MOVING_MS && line / h.v < gap)) return done(true);
	}
	return done(false);
}

bool walkers_waiting(const CityState &s, int ci) {
	const TrafficState &t = s.traffic;
	const TrafficParams &tp = s.catalog.traffic;
	const Conn &c = t.g.conns[size_t(ci)];
	if (!s.roads.has_crosswalks(c.node) || size_t(c.node) >= t.node_walkers.size()) return false;
	const int arms[2] = { t.g.ds[size_t(c.from_ds)].seg, t.g.ds[size_t(c.to_ds)].seg };
	const V2 lane[2] = { V2(c.x0, c.z0), V2(c.x1, c.z1) }; // where the connector meets each arm's crosswalk
	for (int k = 0; k < 2; k++) {
		const int key = cache_key(s, c.node, arms[k], 1);
		if (key < 0 || size_t(key) >= t.cross_held.size() || t.time - t.cross_held[size_t(key)] > double(tp.ped_wait_call_s)) continue;
		// only those who would soon be in its path after stepping off: its lane's stretch of the zebra starts within their reach
		for (const CrowdEntry &e : t.node_walkers[size_t(c.node)]) {
			const Pedestrian &w = t.peds[size_t(e.id)];
			if (!w.alive || w.mode != PedMode::WAIT || w.wi >= w.wn || w.ways[w.wi].cross != arms[k]) continue;
			if ((lane[k] - V2(e.x, e.z)).len() - tp.ped_span_reach_m <= tp.ped_wait_reach_m) return true;
		}
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

float front_time(const CityState &s, const Vehicle &v, float metres, float top) {
	const float d = std::max(0.f, metres), a = std::max(MIN_BRAKE, s.catalog.traffic.classes[int(v.type)].a);
	const float v0 = std::max(0.f, v.v), vt = std::max({ top, v0, MIN_FRONT_MS });
	const float t_up = (vt - v0) / a, d_up = (v0 + vt) * 0.5f * t_up;
	if (d <= d_up) return (std::sqrt(v0 * v0 + 2 * a * d) - v0) / a;
	return t_up + (d - d_up) / vt;
}

} // namespace traffic
} // namespace city
