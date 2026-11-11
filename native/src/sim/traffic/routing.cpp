// Car routing (Roadcraft src/sim/routing.ts, traffic/micro.ts detour/reconsider): A* over directed segments, so lane
// connectors and one-way streets are respected. Edge time = (length / limit + a fixed extra) x (1 + gain r^2) with
// r = vehicles / slots on the segment (BPR); junction cost by turn (right, left, U) and signals; heuristic =
// straight distance at GUESS_SPEED (astar.h). Detours and
// reconsiderations are short searches that rejoin the old route a few segments on.
#include "sim/traffic/traffic_system.h"

#include "core/units.h"
#include "sim/traffic/astar.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

// BPR congestion factor of a directed segment (routing.ts: cap = max(1, len * lanes / slot)).
float load_factor(const CityState &s, int ds) {
	const TrafficState &t = s.traffic;
	const TrafficParams &tp = s.catalog.traffic;
	const DirSeg &d = t.g.ds[ds];
	if (t.lane_vehicles.size() < t.g.ds.size() * MAX_LANES) return 1.f;
	int n = 0;
	for (int l = 0; l < d.lanes; l++) n += int(t.lane_vehicles[size_t(ds) * MAX_LANES + l].size());
	float r = float(n) / std::max(1.f, d.len * d.lanes / tp.slot_m);
	return 1.f + tp.bpr_gain * r * r;
}

// Route of `v` = its current segment, `path` (starting at the segment after it) and what remained after index `j`.
void splice(CityState &s, Vehicle &v, const std::vector<int> &path, int j) {
	static thread_local std::vector<int> route;
	route.clear();
	route.push_back(v.ds);
	route.insert(route.end(), path.begin(), path.end());
	for (int k = j + 1; k < v.route.len; k++) route.push_back(route_at(s.traffic.route_pool, v.route, k));
	route_set(s.traffic.route_pool, v.route, s.traffic.route_waste, route.data(), int(route.size()));
	v.ri = 0;
}

void end_point(const CityState &s, int ds, V2 &p) {
	const DirSeg &d = s.traffic.g.ds[ds];
	p = V2(d.ex, d.ez);
}

// The connectors the router relaxes from `ds`: of those leaving it, to each segment they reach the one the largest body fits.
// Every connector from `ds` to one segment costs the same (the segment's time and load, the turn the two segments make, the node
// they share), so the others only relaxed it again to no effect; the targets keep the order each_conn_from walks them in.
const std::vector<int> &next_of(CityState &s, int ds) {
	TrafficState &t = s.traffic;
	const LaneGraph &g = t.g;
	if (t.route_next.size() < g.ds.size()) {
		t.route_next.resize(g.ds.size());
		t.route_next_stamp.resize(g.ds.size(), 0);
	}
	std::vector<int> &next = t.route_next[size_t(ds)];
	if (t.route_next_stamp[size_t(ds)] == s.lane_graph_serial + 1) return next;
	t.route_next_stamp[size_t(ds)] = s.lane_graph_serial + 1;
	next.clear();
	each_conn_from(g, ds, [&](int ci) {
		for (int &k : next)
			if (g.conns[size_t(k)].to_ds == g.conns[size_t(ci)].to_ds) {
				if (g.conns[size_t(ci)].max_class > g.conns[size_t(k)].max_class) k = ci;
				return;
			}
		next.push_back(ci);
	});
	return next;
}

} // namespace

float junction_cost(const CityState &s, const Conn &c) {
	const TrafficParams &tp = s.catalog.traffic;
	float turn = 0;
	switch (c.turn) {
		case Turn::RIGHT: turn = tp.turn_cost_right_s; break;
		case Turn::LEFT: turn = tp.turn_cost_left_s; break;
		case Turn::UTURN: turn = tp.turn_cost_uturn_s; break;
		default: break;
	}
	return turn + (s.traffic.g.junctions[c.node].ctl == Control::SIGNAL ? tp.signal_cost_s : 0.f);
}

bool find_route(CityState &s, int from_ds, int goal_ds, std::vector<int> &out, VehicleType type, int max_expanded, int banned_ds) {
	static thread_local AStarScratch sc;
	const LaneGraph &g = s.traffic.g;
	const TrafficParams &tp = s.catalog.traffic;
	const bool by_bike = type == VehicleType::BICYCLE;
	const int cls = body_class(tp, type); // a turn too tight for its body is no way for it (junction_sweeps.cpp)
	out.clear();
	if (from_ds < 0 || goal_ds < 0 || !g.ds[from_ds].ok || !g.ds[goal_ds].ok) return false;
	note_search(s);
	V2 goal;
	end_point(s, goal_ds, goal);
	const float guess = tp.guess_speed_kmh / KMH_PER_MS;
	auto h = [&](int ds) { V2 p; end_point(s, ds, p); return (p - goal).len() / guess; };
	auto expand = [&](int ds, auto relax) {
		for (int ci : next_of(s, ds)) {
			const Conn &c = g.conns[size_t(ci)];
			if (c.to_ds == banned_ds || c.max_class < cls) continue;
			const DirSeg &d = g.ds[size_t(c.to_ds)];
			if (by_bike && !d.bikes) continue;
			relax(c.to_ds, (d.len / std::max(d.speed, 1.f) + tp.edge_extra_s) * load_factor(s, c.to_ds) + junction_cost(s, c));
		}
	};
	const int limit = max_expanded < 0 ? int(tp.route_max_expanded) : max_expanded;
	return astar(sc, g.ds.size(), from_ds, goal_ds, limit, h, expand, out);
}

bool detour(CityState &s, int id, int blocked, bool need_room) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[id];
	const LaneGraph &g = t.g;
	const int rejoin = int(s.catalog.traffic.rejoin_ds), local = int(s.catalog.traffic.detour_max_expanded);
	std::vector<int> path;
	bool found = false;
	each_conn_from(g, v.ds, [&](int ci) {
		const Conn &c = g.conns[size_t(ci)];
		if (found || c.from_lane != v.lane || ci == blocked || c.turn == Turn::UTURN || !may_request_search(s)) return;
		if (need_room && !has_exit(s, c, id)) return;
		const int j = std::min(v.route.len - 1, v.ri + 1 + rejoin);
		const int at_j = route_at(t.route_pool, v.route, j);
		if (c.max_class < body_class(s.catalog.traffic, v.type)) return;
		if (j > v.ri + 1 && at_j != c.to_ds && find_route(s, c.to_ds, at_j, path, v.type, local)) {
			splice(s, v, path, j);
			found = true;
			return;
		}
		const int goal = route_at(t.route_pool, v.route, v.route.len - 1);
		if (!find_route(s, c.to_ds, goal, path, v.type)) return;
		splice(s, v, path, v.route.len);
		found = true;
	});
	if (found) t.reroutes++;
	return found;
}

bool reconsider(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[id];
	if (!may_request_search(s) || v.ri + 1 >= v.route.len) return false;
	const int j = std::min(v.route.len - 1, v.ri + int(s.catalog.traffic.rejoin_ds));
	std::vector<int> path;
	if (!find_route(s, v.ds, route_at(t.route_pool, v.route, j), path, v.type, int(s.catalog.traffic.detour_max_expanded)) || path.size() < 2) return false;
	// only a detour this lane can reach, and only if it differs from the old route
	if (connector_to(s, v.ds, v.lane, path[1], body_class(s.catalog.traffic, v.type)) < 0) return false;
	if (int(path.size()) == j - v.ri + 1) {
		bool same = true;
		for (size_t k = 0; k < path.size() && same; k++)
			same = route_at(t.route_pool, v.route, v.ri + int(k)) == path[k];
		if (same) return false;
	}
	path.erase(path.begin());
	splice(s, v, path, j);
	t.reroutes++;
	return true;
}

// The route says a turn the vehicle's lane cannot make (a connector was removed or the lanes were rearranged): try
// another lane of the same segment that does, then plan again from where it is. False when it has no way on.
bool retry_route(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	if (v.ds < 0 || v.ds >= int(t.g.ds.size()) || v.ri + 1 >= v.route.len) return false;
	const int next = route_at(t.route_pool, v.route, v.ri + 1);
	int alt = -1, bd = MAX_LANES + 1;
	const int cls = body_class(s.catalog.traffic, v.type);
	for (int lane = 0; lane < t.g.ds[size_t(v.ds)].lanes; lane++) {
		if (lane == v.lane || connector_to(s, v.ds, lane, next, cls) < 0) continue;
		const int d = std::abs(lane - v.lane);
		if (d < bd) { bd = d; alt = lane; }
	}
	if (alt >= 0) {
		// into it once its body has room there (the gaps of a forced change); meanwhile it waits at its line on its own lane
		if (lane_room(s, id, alt)) move_vehicle_lane(s, id, alt);
		return true;
	}
	const int goal = route_at(t.route_pool, v.route, v.route.len - 1);
	std::vector<int> path;
	if (goal < 0 || !find_route(s, v.ds, goal, path, v.type) || path.empty()) return false;
	route_set(t.route_pool, v.route, t.route_waste, path.data(), int(path.size()));
	v.ri = 0;
	t.reroutes++;
	return true;
}

} // namespace traffic
} // namespace city
