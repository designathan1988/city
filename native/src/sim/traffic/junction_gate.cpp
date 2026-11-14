// The gate of a junction (junction_control.cpp sets its control and ranks): whether a vehicle may enter its connector now (its light,
// a Stop taken from a stand, room past the junction, walkers in its way, the conflicting movements by rank, movement and the right, a
// claim after waiting its share), why a head is held, and the connector a vehicle takes off its lane. A blocked entry acts as a stopped
// obstacle at the stop line (vehicle_motion.cpp).
#include "sim/traffic/motion_law.h"

#include "sim/traffic/light_rule.h"
#include "sim/traffic/signal_plan.h"

#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float STOPPED_MS = 0.5f, FIRST_COME_S = 0.3f; // stopped vehicles go first come, first served
constexpr float HALF_BODY = 0.5f;
constexpr int RANK_MAIN = 3; // DirSeg::rank of the road that has the way (junction_control.cpp)

int movement(Turn t) {
	switch (t) {
		case Turn::STRAIGHT: return 2;
		case Turn::RIGHT: return 1;
		case Turn::LEFT: return 0;
		default: return -1;
	}
}

// True when connector `o` goes before connector `c`: the rule's rank, then (but on a mini-roundabout, where whoever is on the
// circle has the way whatever its movement) the movement, then whoever comes from the right; on a mini-roundabout from the left
// (CTB art. 29 III: b, who circulates on it; c, elsewhere who comes by the driver's right).
bool outranks(const LaneGraph &g, const Conn &o, const Conn &c) {
	int ro = g.ds[o.from_ds].rank, rc = g.ds[c.from_ds].rank;
	if (ro != rc) return ro > rc;
	const bool circle = g.junctions[size_t(c.node)].circle;
	int mo = movement(o.turn), mc = movement(c.turn);
	if (!circle && mo != mc) return mo > mc;
	V2 hc(c.x1 - c.x0, c.z1 - c.z0), ho(o.cx - o.x0, o.cz - o.z0);
	const float from_right = ho.dot(V2(hc.y, -hc.x));
	return circle ? from_right < 0 : from_right > 0;
}

// How long this vehicle takes, from `dist` metres before the end of its lane, to be clear of connector `o`'s way: to the arc of its
// own way the sweeps kept on `o` for it (Conn::conflict_clear, junction_sweeps.cpp) and half its body more, at its pace through.
float clearing_time(const CityState &s, const Vehicle &me, int ci, const Conn &o, int theirs, float dist, float top) {
	const TrafficParams &tp = s.catalog.traffic;
	const int mine = body_class(tp, me.type);
	float arc = 0;
	for (size_t k = 0; k < o.conflicts.size(); k++)
		if (o.conflicts[k] == ci) arc = std::max(0.f, o.conflict_clear[k][size_t(mine * BODY_CLASSES + theirs)]);
	const float pace = std::max(tp.gap_clear_share * top, tp.gap_clear_least_ms);
	return (std::max(0.f, dist) + arc + tp.classes[int(me.type)].length * HALF_BODY) / pace;
}

// The zipper: where two lanes run into one, their heads take turns. This head's lane went into that exit lane last
// (TrafficState::merge_last) and another lane's head, bound for the same lane and of no lesser rank, is ready (near its line or
// standing): it has the next place, while this one has not waited its patience out (a turn left untaken is no turn).
bool gives_its_turn(const CityState &s, const Vehicle &me, const Conn &c) {
	const TrafficState &t = s.traffic;
	const TrafficParams &tp = s.catalog.traffic;
	const size_t to = size_t(c.to_ds) * MAX_LANES + size_t(c.to_lane), from = size_t(c.from_ds) * MAX_LANES + size_t(c.from_lane);
	const std::vector<int> &last = t.junction.merge_last;
	if (to >= last.size() || last[to] != int(from) || me.wait > tp.zip_patience_s) return false;
	for (int x = t.g.node_conn[size_t(c.node)]; x >= 0; x = t.g.conns[size_t(x)].next_at_node) {
		const Conn &o = t.g.conns[size_t(x)];
		if (!o.alive || o.to_ds != c.to_ds || o.to_lane != c.to_lane || (o.from_ds == c.from_ds && o.from_lane == c.from_lane)) continue;
		if (t.g.ds[size_t(o.from_ds)].rank < t.g.ds[size_t(c.from_ds)].rank || light_for(s, o.from_ds) == Light::RED) continue;
		const auto &lane = t.lane_vehicles[size_t(o.from_ds) * MAX_LANES + size_t(o.from_lane)];
		if (lane.empty()) continue;
		const Vehicle &head = t.vehicles[size_t(lane.back())];
		if (pick_connector(s, head) != x) continue;
		if (t.g.ds[size_t(o.from_ds)].len - head.s <= tp.zip_ready_m || head.v < STOPPED_MS) return true;
	}
	return false;
}

} // namespace

// The gap this driver takes before a movement that goes first (HCM's critical headway by movement, traffic.json gap_*): its
// turn's, a left off the road that has the way the least; a pushy driver takes less and a timid one more (temper), and the longer
// it has waited the less it asks, never under the floor.
float turn_gap(const TrafficParams &tp, const Vehicle &me, Turn turn, bool on_main) {
	const float base = turn == Turn::RIGHT ? tp.gap_right_s : (turn == Turn::STRAIGHT ? tp.gap_straight_s : (on_main ? tp.gap_main_left_s : tp.gap_left_s));
	const float own = base * (1 - tp.gap_temper * me.temper);
	return std::max(tp.gap_floor_s, own - std::min(tp.gap_impatience_most_s, tp.gap_impatience * me.wait));
}

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
	const float top = conn_top(s, me, c);
	if (peds_in_way(s, ci, front_time(s, me, line, top), front_time(s, me, dist + c.len, top))) return false;
	if (waiting_walkers(s, me, ci, line)) return false; // whoever waits at the kerb of a crossing on its way goes first
	bool claim = me.wait > tp.claim_after_s;
	// whoever has waited its share claims the turn: those who can still stop comfortably let it through
	const bool can_stop = line > me.v * me.v / (2 * tp.claim_brake) + 1;
	if (!claim && gives_its_turn(s, me, c)) return false;
	const int mine = body_class(tp, me.type);
	const float gap = turn_gap(tp, me, c.turn, t.g.ds[c.from_ds].rank == RANK_MAIN);
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
		if (pick_connector(s, head) != oc) continue;
		if (head.v < STOPPED_MS && !has_exit(s, o, lane.back())) continue; // it cannot go anyway
		bool both_stopped = head.v < STOPPED_MS && me.v < STOPPED_MS;
		if (both_stopped && me.wait > head.wait + FIRST_COME_S) continue; // first come, first served
		// the gap: that head reaches the end of its lane no sooner than this driver's gap for its turn, and never before this
		// vehicle's body is clear of its way with a margin
		const float arrives = (t.g.ds[o.from_ds].len - head.s) / std::max(head.v, tp.gap_slow_ms);
		const float clear = clearing_time(s, me, ci, o, body_class(tp, head.type), dist, top) + tp.gap_clear_margin_s;
		if (arrives < std::max(gap, clear)) return false;
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
