// The per-lane and per-connector vehicle lists and the near-vehicle list (micro_list), plus the lane/connection
// queries the rest of the traffic uses. Only lists with a near vehicle are in active_lanes/active_conns, so a step
// walks the traffic near the camera, never every lane of the map (Roadcraft traffic.ts micro[]).
#include "sim/traffic/traffic_system.h"
#include "sim/traffic/lane_shift.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {

// `at` holds each key's place in `list` plus one (0: not on it), so a key leaves without a search: the last key takes its
// place, the order a search for it and the same swap gave.
void activate(std::vector<int> &list, std::vector<int> &at, int key) {
	if (key < 0 || key >= int(at.size()) || at[size_t(key)]) return;
	list.push_back(key);
	at[size_t(key)] = int(list.size());
}

void deactivate(std::vector<int> &list, std::vector<int> &at, int key) {
	if (key < 0 || key >= int(at.size()) || !at[size_t(key)]) return;
	const size_t i = size_t(at[size_t(key)] - 1);
	const int last = list.back();
	list[i] = last;
	at[size_t(last)] = int(i) + 1;
	list.pop_back();
	at[size_t(key)] = 0;
}

} // namespace

void lane_add(CityState &s, int key, int id) {
	TrafficState &t = s.traffic;
	if (key < 0 || key >= int(t.lane_vehicles.size())) return;
	auto &list = t.lane_vehicles[size_t(key)];
	const float at = t.vehicles[size_t(id)].s;
	auto pos = std::lower_bound(list.begin(), list.end(), at, [&](int o, float x) { return t.vehicles[size_t(o)].s < x; });
	list.insert(pos, id);
	activate(t.active_lanes, t.lane_active, key);
}

void lane_del(CityState &s, int key, int id) {
	TrafficState &t = s.traffic;
	if (key < 0 || key >= int(t.lane_vehicles.size())) return;
	auto &list = t.lane_vehicles[size_t(key)];
	list.erase(std::remove(list.begin(), list.end(), id), list.end());
	if (list.empty()) deactivate(t.active_lanes, t.lane_active, key);
}

void conn_push(CityState &s, int ci, int id) {
	TrafficState &t = s.traffic;
	if (ci < 0 || ci >= int(t.conn_vehicles.size())) return;
	t.conn_vehicles[size_t(ci)].push_back(id);
	activate(t.active_conns, t.conn_active, ci);
}

void conn_pop_front(CityState &s, int ci) {
	TrafficState &t = s.traffic;
	if (ci < 0 || ci >= int(t.conn_vehicles.size())) return;
	auto &list = t.conn_vehicles[size_t(ci)];
	if (list.empty()) return;
	list.erase(list.begin());
	if (list.empty()) deactivate(t.active_conns, t.conn_active, ci);
}

void conn_del(CityState &s, int ci, int id) {
	TrafficState &t = s.traffic;
	if (ci < 0 || ci >= int(t.conn_vehicles.size())) return;
	auto &list = t.conn_vehicles[size_t(ci)];
	list.erase(std::remove(list.begin(), list.end(), id), list.end());
	if (list.empty()) deactivate(t.active_conns, t.conn_active, ci);
}

void conn_clear(CityState &s, int ci) {
	TrafficState &t = s.traffic;
	if (ci < 0 || ci >= int(t.conn_vehicles.size())) return;
	t.conn_vehicles[size_t(ci)].clear();
	deactivate(t.active_conns, t.conn_active, ci);
}

void micro_add(TrafficState &t, int id) {
	Vehicle &v = t.vehicles[size_t(id)];
	if (v.micro) return;
	v.micro = true;
	v.micro_idx = int(t.micro_list.size());
	t.micro_list.push_back(id);
}

void micro_remove(TrafficState &t, int id) {
	Vehicle &v = t.vehicles[size_t(id)];
	if (!v.micro) return;
	shift_end(t, id); // out of the near model (the far one, a lot, the map): no lane change goes on

	v.micro = false;
	const size_t i = size_t(v.micro_idx);
	if (i < t.micro_list.size() && t.micro_list[i] == id) {
		const int last = t.micro_list.back();
		t.micro_list[i] = last;
		t.vehicles[size_t(last)].micro_idx = int(i);
		t.micro_list.pop_back();
	}
	v.micro_idx = -1;
}

int lane_load(const TrafficState &t, int key) {
	if (key < 0) return 0;
	int n = key < int(t.lane_vehicles.size()) ? int(t.lane_vehicles[size_t(key)].size()) : 0;
	if (key < int(t.meso_count.size())) n += t.meso_count[size_t(key)];
	return n;
}

bool lane_reaches(const CityState &s, int ds, int lane, int next) {
	const LaneGraph &g = s.traffic.g;
	const size_t key = size_t(ds) * MAX_LANES + lane;
	if (key >= g.lane_conn.size()) return false;
	for (int ci = g.lane_conn[key]; ci >= 0; ci = g.conns[size_t(ci)].next_from)
		if (g.conns[size_t(ci)].alive && g.conns[size_t(ci)].to_ds == next) return true;
	return false;
}

int conn_toward(const CityState &s, int ds, int lane, int to_ds) {
	const LaneGraph &g = s.traffic.g;
	const size_t key = size_t(ds) * MAX_LANES + lane;
	if (key >= g.lane_conn.size()) return -1;
	for (int ci = g.lane_conn[key]; ci >= 0; ci = g.conns[size_t(ci)].next_from)
		if (g.conns[size_t(ci)].alive && g.conns[size_t(ci)].to_ds == to_ds) return ci;
	return -1;
}

int lane_toward(const CityState &s, int ds, int to_ds, int near_lane) {
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	int best = -1, bd = MAX_LANES + 1;
	for (int lane = 0; lane < d.lanes; lane++) {
		if (!lane_reaches(s, ds, lane, to_ds)) continue;
		const int dist = std::abs(lane - near_lane);
		if (dist < bd) { bd = dist; best = lane; }
	}
	return best;
}

// A lane's list of connectors holds only its own (from_lane == lane): the one to `next` is conn_toward's.
int connector_to(const CityState &s, int ds, int lane, int next) {
	return ds < 0 ? -1 : conn_toward(s, ds, lane, next);
}

int pick_connector(const CityState &s, const Vehicle &v) {
	if (v.ds < 0 || v.ri + 1 >= v.route.len) return -1;
	return connector_to(s, v.ds, v.lane, route_at(s.traffic.route_pool, v.route, v.ri + 1));
}

int lane_head(const CityState &s, int vid) {
	const Vehicle &v = s.traffic.vehicles[vid];
	if (v.ds < 0) return -1;
	const auto &lane = s.traffic.lane_vehicles[size_t(v.ds) * MAX_LANES + v.lane];
	return lane.empty() ? -1 : lane.back();
}

void move_vehicle_lane(CityState &s, int id, int target) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	if (v.ds < 0 || target < 0 || target >= t.g.ds[size_t(v.ds)].lanes || target == v.lane) return;
	const DirSeg &d = t.g.ds[size_t(v.ds)];
	const int from = v.ds * MAX_LANES + v.lane;
	lane_del(s, from, id);
	const float across = d.u[v.lane] - d.u[target]; // where it is, seen from the new lane's middle (left of its way, as lane offsets)
	v.lane = target;
	lane_add(s, v.ds * MAX_LANES + target, id);
	shift_start(s, id, from, across); // the change is a path, its body still in the lane it left meanwhile (lane_shift.h)
}

} // namespace traffic
} // namespace city
