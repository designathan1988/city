// The per-lane and per-connector vehicle lists and the near-vehicle list (micro_list), plus the lane/connection
// queries the rest of the traffic uses. Only lists with a near vehicle are in active_lanes/active_conns, so a step
// walks the traffic near the camera, never every lane of the map (Roadcraft traffic.ts micro[]).
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cassert>

namespace city {
namespace traffic {

namespace {

void activate(std::vector<int> &list, std::vector<uint8_t> &flag, std::vector<int> &positions, int key) {
	if (key < 0 || key >= int(flag.size()) || flag[size_t(key)]) return;
	flag[size_t(key)] = 1;
	if (positions.size() < flag.size()) positions.resize(flag.size(), -1);
	positions[size_t(key)] = int(list.size());
	list.push_back(key);
}

void deactivate(std::vector<int> &list, std::vector<uint8_t> &flag, std::vector<int> &positions, int key) {
	if (key < 0 || key >= int(flag.size()) || !flag[size_t(key)]) return;
	const int at = positions[size_t(key)];
	assert(at >= 0 && size_t(at) < list.size() && list[size_t(at)] == key);
	const int moved = list.back(); list[size_t(at)] = moved; positions[size_t(moved)] = at;
	list.pop_back(); positions[size_t(key)] = -1; flag[size_t(key)] = 0;
}

} // namespace

void lane_add(CityState &s, int key, int id) {
	TrafficState &t = s.traffic;
	if (key < 0 || key >= int(t.lane_vehicles.size())) return;
	auto &list = t.lane_vehicles[size_t(key)];
	const float at = t.vehicles[size_t(id)].s;
	auto pos = std::lower_bound(list.begin(), list.end(), at, [&](int o, float x) { return t.vehicles[size_t(o)].s < x; });
	list.insert(pos, id);
	t.birth_reservations.erase(id); // a whole actor is now physical, no longer an input reservation
	t.route_counts.changed(size_t(key), list.size());
	activate(t.active_lanes, t.lane_active, t.lane_active_pos, key);
}

void lane_del(CityState &s, int key, int id) {
	TrafficState &t = s.traffic;
	if (key < 0 || key >= int(t.lane_vehicles.size())) return;
	auto &list = t.lane_vehicles[size_t(key)];
	list.erase(std::remove(list.begin(), list.end(), id), list.end());
	t.route_counts.changed(size_t(key), list.size());
	if (list.empty()) deactivate(t.active_lanes, t.lane_active, t.lane_active_pos, key);
}

void conn_push(CityState &s, int ci, int id) {
	TrafficState &t = s.traffic;
	if (ci < 0 || ci >= int(t.conn_vehicles.size())) return;
	t.conn_vehicles[size_t(ci)].push_back(id);
	activate(t.active_conns, t.conn_active, t.conn_active_pos, ci);
}

void conn_pop_front(CityState &s, int ci) {
	TrafficState &t = s.traffic;
	if (ci < 0 || ci >= int(t.conn_vehicles.size())) return;
	auto &list = t.conn_vehicles[size_t(ci)];
	if (list.empty()) return;
	list.erase(list.begin());
	if (list.empty()) deactivate(t.active_conns, t.conn_active, t.conn_active_pos, ci);
}

void conn_del(CityState &s, int ci, int id) {
	TrafficState &t = s.traffic;
	if (ci < 0 || ci >= int(t.conn_vehicles.size())) return;
	auto &list = t.conn_vehicles[size_t(ci)];
	list.erase(std::remove(list.begin(), list.end(), id), list.end());
	if (list.empty()) deactivate(t.active_conns, t.conn_active, t.conn_active_pos, ci);
}

void conn_clear(CityState &s, int ci) {
	TrafficState &t = s.traffic;
	if (ci < 0 || ci >= int(t.conn_vehicles.size())) return;
	t.conn_vehicles[size_t(ci)].clear();
	deactivate(t.active_conns, t.conn_active, t.conn_active_pos, ci);
}

void set_pass(TrafficState &t, int id, int conn) {
	Vehicle &v = t.vehicles[size_t(id)];
	if (v.pass == conn) return;
	if (v.pass >= 0 && size_t(v.pass) < t.conn_pass_first.size()) {
		if (v.pass_prev >= 0) t.vehicles[size_t(v.pass_prev)].pass_next = v.pass_next;
		else if (t.conn_pass_first[size_t(v.pass)] == id) t.conn_pass_first[size_t(v.pass)] = v.pass_next;
		if (v.pass_next >= 0) t.vehicles[size_t(v.pass_next)].pass_prev = v.pass_prev;
	}
	v.pass = conn; v.pass_prev = v.pass_next = -1;
	if (conn < 0) return;
	if (size_t(conn) >= t.conn_pass_first.size()) t.conn_pass_first.resize(size_t(conn) + 1, -1);
	v.pass_next = t.conn_pass_first[size_t(conn)];
	if (v.pass_next >= 0) t.vehicles[size_t(v.pass_next)].pass_prev = id;
	t.conn_pass_first[size_t(conn)] = id;
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

void move_vehicle_lane(CityState &s, int id, int target) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	if (v.ds < 0 || target < 0 || target >= t.g.ds[size_t(v.ds)].lanes || target == v.lane) return;
	const DirSeg &d = t.g.ds[size_t(v.ds)];
	lane_del(s, v.ds * MAX_LANES + v.lane, id);
	v.lc_off += d.u[v.lane] - d.u[target];
	v.lane = target;
	lane_add(s, v.ds * MAX_LANES + target, id);
}

} // namespace traffic
} // namespace city
