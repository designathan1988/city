// The gate of a junction (junction_control.cpp sets its control and ranks): whether a vehicle may enter its connector now (its light,
// a Stop taken from a stand, room past the junction, walkers in its way, the conflicting movements by rank, movement and the right, a
// claim after waiting its share), why a head is held, and the connector a vehicle takes off its lane. A blocked entry acts as a stopped
// obstacle at the stop line (vehicle_motion.cpp).
#include "sim/traffic/traffic_system.h"

#include "sim/traffic/light_rule.h"

#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float STOPPED_MS = 0.5f, FIRST_COME_S = 0.3f; // stopped vehicles go first come, first served

int movement(Turn t) {
	switch (t) {
		case Turn::STRAIGHT: return 2;
		case Turn::RIGHT: return 1;
		case Turn::LEFT: return 0;
		default: return -1;
	}
}

// True when connector `o` goes before connector `c` (rule rank, movement, then whoever comes from the right).
bool outranks(const LaneGraph &g, const Conn &o, const Conn &c) {
	int ro = g.ds[o.from_ds].rank, rc = g.ds[c.from_ds].rank;
	if (ro != rc) return ro > rc;
	int mo = movement(o.turn), mc = movement(c.turn);
	if (mo != mc) return mo > mc;
	V2 hc(c.x1 - c.x0, c.z1 - c.z0), ho(o.cx - o.x0, o.cz - o.z0);
	return ho.dot(V2(hc.y, -hc.x)) > 0;
}

} // namespace

bool may_enter(const CityState &s, int vid, int ci, float dist) {
	const TrafficState &t = s.traffic;
	const TrafficParams &tp = s.catalog.traffic;
	const Conn &c = t.g.conns[ci];
	const Vehicle &me = t.vehicles[vid];
	// road joint (two-way node without control): no junction to block; IDM follows whoever is beyond it
	if (is_splice(s, c.node)) return true;
	const float line = dist - t.g.ds[c.from_ds].stop_back;
	if (light_stops(s, me, c.from_ds, line, ci)) return false;
	if (t.g.ds[c.from_ds].must_stop && me.stood_at != c.from_ds) return false; // a Stop is more than a lower rank: it is taken from a stand
	if (!has_exit(s, c, vid)) return false;
	const float top = top_speed(s, me, conn_speed(s, c));
	if (peds_in_way(s, ci, front_time(s, me, line, top), front_time(s, me, dist + c.len, top))) return false;
	bool claim = me.wait > tp.claim_after_s;
	// whoever has waited its share claims the turn: those who can still stop comfortably let it through
	const bool can_stop = line > me.v * me.v / (2 * tp.claim_brake) + 1;
	const int mine = body_class(tp, me.type);
	for (size_t k = 0; k < c.conflicts.size(); k++) {
		if (conflict_busy(s, c, k, mine)) return false;
		int holder = pass_holder(s, c.conflicts[k]);
		if (holder >= 0 && holder != vid) return false; // someone over its line for a crossing connector
		int oc = c.conflicts[k];
		if (!claim && can_stop && t.time - t.conn_claim_t[oc] < tp.claim_hold_s) return false;
		const Conn &o = t.g.conns[oc];
		if (claim || !outranks(t.g, o, c) || light_for(s, o.from_ds) == Light::RED) continue;
		const auto &lane = t.lane_vehicles[size_t(o.from_ds) * MAX_LANES + o.from_lane];
		if (lane.empty()) continue;
		const Vehicle &head = t.vehicles[lane.back()];
		float to_line = t.g.ds[o.from_ds].len - head.s;
		if (to_line >= std::max(tp.yield_near_m, head.v * tp.yield_look_s) || pick_connector(s, head) != oc) continue;
		if (head.v < STOPPED_MS && !has_exit(s, o, lane.back())) continue; // it cannot go anyway
		bool both_stopped = head.v < STOPPED_MS && me.v < STOPPED_MS;
		if (both_stopped && me.wait > head.wait + FIRST_COME_S) continue; // first come, first served
		return false;
	}
	return true;
}

Hold hold_reason(const CityState &s, int vid) {
	const TrafficState &t = s.traffic;
	const Vehicle &v = t.vehicles[vid];
	if (v.conn >= 0) return Hold::ON_CONNECTOR;
	const auto &lane = t.lane_vehicles[size_t(v.ds) * MAX_LANES + v.lane];
	if (lane.empty() || lane.back() != vid) return Hold::QUEUED;
	int ci = pick_connector(s, v);
	if (ci < 0) return Hold::FREE;
	const Conn &c = t.g.conns[ci];
	if (c.from_lane != v.lane) return Hold::WRONG_LANE;
	if (light_for(s, c.from_ds) != Light::GREEN) return Hold::RED;
	if (!is_splice(s, c.node) && !has_exit(s, c, vid)) return Hold::EXIT_ROOM;
	if (peds_in_way(s, ci)) return Hold::CROSSING;
	for (size_t k = 0; k < c.conflicts.size(); k++)
		if (conflict_busy(s, c, k, body_class(s.catalog.traffic, v.type))) return Hold::CONFLICT_BUSY;
	return may_enter(s, vid, ci, t.g.ds[v.ds].len - v.s) ? Hold::FREE : Hold::YIELD;
}

int lane_head(const CityState &s, int vid) {
	const Vehicle &v = s.traffic.vehicles[vid];
	if (v.ds < 0) return -1;
	const auto &lane = s.traffic.lane_vehicles[size_t(v.ds) * MAX_LANES + v.lane];
	return lane.empty() ? -1 : lane.back();
}

int connector_to(const CityState &s, int ds, int lane, int next, int cls) {
	const LaneGraph &g = s.traffic.g;
	const size_t key = size_t(ds) * MAX_LANES + lane;
	if (ds < 0 || key >= g.lane_conn.size()) return -1;
	int best = -1, bd = MAX_LANES * 2;
	for (int ci = g.lane_conn[key]; ci >= 0; ci = g.conns[size_t(ci)].next_from) {
		const Conn &c = g.conns[size_t(ci)];
		if (!c.alive || c.to_ds != next || c.max_class < cls) continue;
		int d = std::abs(c.from_lane - lane);
		if (d < bd) { bd = d; best = ci; }
	}
	return best;
}

int pick_connector(const CityState &s, const Vehicle &v) {
	if (v.ds < 0 || v.ri + 1 >= v.route.len) return -1;
	return connector_to(s, v.ds, v.lane, route_at(s.traffic.route_pool, v.route, v.ri + 1), body_class(s.catalog.traffic, v.type));
}

} // namespace traffic
} // namespace city
