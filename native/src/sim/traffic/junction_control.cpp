// Junction control: the control a node gets (signals, priority or none), its signal stages (opposite approaches
// together, the intergreens and greens of signal_timing.cpp) and the gate rules: which approaches may enter a
// connector now. A blocked entry acts as a stopped obstacle at the stop line (see vehicle_motion.cpp).
#include "sim/traffic/traffic_system.h"

#include "sim/roads/junction.h"

#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr int RANK_MAIN = 3, RANK_YIELD = 1; // free arms keep DirSeg::rank's default
constexpr int MAIN_PRIORITY = 2; // arms of this road priority or more push a junction toward signals
constexpr int SIGNAL_ARMS = 5, SIGNAL_MAIN_ARMS = 3;
constexpr uint32_t OFFSET_HASH = 2654435761u, OFFSET_STEPS = 1000; // where in its first green each junction starts
constexpr float STOPPED_MS = 0.5f, FIRST_COME_S = 0.3f; // stopped vehicles go first come, first served

constexpr float STOP_SETBACK_SHARE = 0.5f; // a short lane keeps at least half of itself in front of its stop line
constexpr int CROSSWALK_DEGREE = 3; // crosswalks are painted at nodes of this many arms or more (render crossing_node)

bool crosswalk_node(const CityState &s, int n) {
	const RoadNode &node = s.roads.nodes[n];
	if (int(node.segs.size()) < CROSSWALK_DEGREE) return false;
	for (int seg : node.segs) {
		const Profile &p = s.roads.profile_of(seg);
		if (p.ring || p.rail) return false;
	}
	return true;
}

struct Arm {
	int seg = -1, in = -1, priority = 0;
	V2 out; // outward direction from the node
};

std::vector<Arm> arms_of(const CityState &s, int n) {
	const LaneGraph &g = s.traffic.g;
	std::vector<Arm> arms;
	for (int si : s.roads.nodes[n].segs) {
		int din = si * 2 + (s.roads.segs[si].b == n ? 0 : 1), dout = si * 2 + (s.roads.segs[si].a == n ? 0 : 1);
		if (!g.ds[din].ok && !g.ds[dout].ok) continue;
		Arm a;
		a.seg = si;
		a.in = g.ds[din].ok ? din : -1;
		a.priority = s.roads.profile_of(si).priority;
		Pose p;
		if (a.in >= 0) { lane_pose(s, din, 0, g.ds[din].len, p); a.out = V2(-p.fx, -p.fz); }
		else { lane_pose(s, dout, 0, 0.f, p); a.out = V2(p.fx, p.fz); }
		arms.push_back(a);
	}
	return arms;
}

Control decide(const RoadNode &node, const std::vector<Arm> &arms, const TrafficParams &tp) {
	if (arms.size() < 3) return Control::NONE;
	if (node.ctl == CTL_SIGNAL) return Control::SIGNAL;
	if (node.ctl != CTL_AUTO) return Control::PRIORITY; // stop, yield, none and mini still drive as a priority junction
	std::vector<int> high;
	int lo = arms[0].priority, hi = lo;
	for (size_t i = 0; i < arms.size(); i++) {
		if (arms[i].priority >= MAIN_PRIORITY) high.push_back(int(i));
		lo = std::min(lo, arms[i].priority);
		hi = std::max(hi, arms[i].priority);
	}
	if (int(arms.size()) >= SIGNAL_ARMS || int(high.size()) >= SIGNAL_MAIN_ARMS) return Control::SIGNAL;
	if (high.size() == 2 && arms[high[0]].out.dot(arms[high[1]].out) > tp.through_pair_dot) return Control::SIGNAL;
	return hi > lo ? Control::PRIORITY : Control::NONE;
}

void make_stages(CityState &s, int n, const std::vector<Arm> &arms, Junction &j) {
	const TrafficParams &tp = s.catalog.traffic;
	std::vector<int> stage(arms.size(), -1);
	int count = 0;
	for (size_t i = 0; i < arms.size(); i++) {
		if (stage[i] >= 0) continue;
		stage[i] = count;
		int best = -1;
		float bd = tp.stage_opposite_dot;
		for (size_t k = 0; k < arms.size(); k++) {
			float d = arms[i].out.dot(arms[k].out);
			if (stage[k] < 0 && d < bd) { bd = d; best = int(k); }
		}
		if (best >= 0) stage[best] = count;
		count++;
	}
	j.green.assign(count, tp.signal_safety_green_s);
	j.amber.assign(count, 0.f);
	j.all_red.assign(count, 0.f);
	j.approaches.assign(count, {});
	const bool walkers = crosswalk_node(s, n);
	for (size_t i = 0; i < arms.size(); i++) {
		if (arms[i].in < 0) continue;
		DirSeg &d = s.traffic.g.ds[arms[i].in];
		d.stage = stage[i];
		j.approaches[stage[i]].push_back(arms[i].in);
		float amber = 0, all_red = 0;
		intergreen(s, n, arms[i].in, walkers, amber, all_red);
		j.amber[stage[i]] = std::max(j.amber[stage[i]], amber);
		j.all_red[stage[i]] = std::max(j.all_red[stage[i]], all_red);
		j.green[stage[i]] = std::max(j.green[stage[i]], detection(s, arms[i].in));
	}
	// MBST vol. V 5.14: a stage's green lets the walkers it lets cross have their own safety green and still clear
	for (size_t i = 0; walkers && i < arms.size(); i++)
		for (int k = 0; k < count; k++) {
			if (arms[i].in >= 0 && stage[i] == k) continue; // its own traffic runs: nobody crosses it now
			const float need = tp.ped_safety_green_s + ped_clearance(s, arms[i].seg) + tp.ped_all_red_s;
			j.green[k] = std::max(j.green[k], need - j.amber[k] - j.all_red[k]);
		}
	// MBST vol. V 7.3.3 and 5.9: the longest cycle less the intergreens, in equal shares (no fixed plan's flows to weigh)
	float lost = 0;
	for (int k = 0; k < count; k++) lost += j.amber[k] + j.all_red[k];
	j.green_max.assign(count, 0.f);
	j.cycle = lost;
	for (int k = 0; k < count; k++) {
		j.green_max[k] = std::max(j.green[k], (tp.signal_cycle_max_s - lost) / float(count));
		j.cycle += j.green[k];
	}
	j.run = 0;
	j.run_green = -1;
	j.run_start = s.traffic.time - double((uint32_t(n) * OFFSET_HASH) % OFFSET_STEPS) / OFFSET_STEPS * j.green[0];
}

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

// A vehicle on the k-th conflicting connector has not yet cleared the crossing point.
bool conflict_busy(const CityState &s, const Conn &c, size_t k) {
	const TrafficState &t = s.traffic;
	for (int id : t.conn_vehicles[c.conflicts[k]]) {
		const Vehicle &o = t.vehicles[id];
		if (o.cs - s.catalog.traffic.classes[int(o.type)].length < c.conflict_at[k] + s.catalog.traffic.conflict_clear_m) return true;
	}
	return false;
}

} // namespace

void build_junction(CityState &s, int n) {
	LaneGraph &g = s.traffic.g;
	if (n < 0 || n >= int(s.roads.nodes.size())) return;
	if (n >= int(g.junctions.size())) g.junctions.resize(size_t(n) + 1);
	Junction &j = g.junctions[size_t(n)];
	j = Junction();
	if (n >= int(g.node_conn.size())) g.node_conn.resize(size_t(n) + 1, -1);
	std::vector<Arm> arms = arms_of(s, n);
	// the approaches start clean: an old stage, rank or stop line must not outlive the rebuild
	for (const Arm &a : arms)
		if (a.in >= 0) { g.ds[a.in].stage = -1; g.ds[a.in].rank = 2; g.ds[a.in].stop_back = 0; g.ds[a.in].detector = g.ds[a.in].extension = 0; }
	j.degree = int(arms.size());
	if (crosswalk_node(s, n))
		for (const Arm &a : arms)
			if (a.in >= 0) g.ds[a.in].stop_back = std::min(s.catalog.furniture.crossing.stop_back(), g.ds[a.in].len * STOP_SETBACK_SHARE);
	j.ctl = control_of(s, n);
	if (j.ctl == Control::SIGNAL) make_stages(s, n, arms, j);
	if (j.ctl != Control::PRIORITY) return;
	std::vector<int> order;
	for (size_t i = 0; i < arms.size(); i++) order.push_back(int(i));
	std::stable_sort(order.begin(), order.end(), [&](int a, int b) { return arms[a].priority > arms[b].priority; });
	for (size_t k = 0; k < order.size(); k++)
		if (arms[order[k]].in >= 0) g.ds[arms[order[k]].in].rank = k < 2 ? RANK_MAIN : RANK_YIELD;
}

Control control_of(const CityState &s, int n) {
	if (n < 0 || n >= int(s.roads.nodes.size())) return Control::NONE;
	const RoadNode &node = s.roads.nodes[size_t(n)];
	std::vector<Arm> arms; // an arm with lanes either way (DirSeg::ok), facing out of the node
	for (int si : node.segs) {
		const RoadSegment &g = s.roads.segs[size_t(si)];
		if (!g.alive || g.a == g.b) continue;
		const Profile &p = s.roads.profile_of(si);
		if (p.rail || (p.fwd.empty() && p.back.empty())) continue;
		const city::Arm out = arm_of(g.q, p, g.a == n ? 0 : 1);
		Arm a;
		a.seg = si;
		a.priority = p.priority;
		a.out = V2(float(out.dx), float(out.dz));
		arms.push_back(a);
	}
	return decide(node, arms, s.catalog.traffic);
}

void build_junctions(CityState &s) {
	s.traffic.g.junctions.assign(s.roads.nodes.size(), Junction());
	for (int n = 0; n < int(s.roads.nodes.size()); n++)
		if (s.roads.nodes[n].alive) build_junction(s, n);
}

bool may_enter(const CityState &s, int vid, int ci, float dist) {
	const TrafficState &t = s.traffic;
	const TrafficParams &tp = s.catalog.traffic;
	const Conn &c = t.g.conns[ci];
	const Vehicle &me = t.vehicles[vid];
	// road joint (two-way node without control): no junction to block; IDM follows whoever is beyond it
	if (is_splice(s, c.node)) return true;
	const float line = dist - t.g.ds[c.from_ds].stop_back;
	if (light_stops(s, me, c.from_ds, line - tp.stop_margin_m, ci)) return false;
	if (!has_exit(s, c, vid)) return false;
	const float top = conn_speed(s, c) * me.vf;
	if (peds_in_way(s, ci, front_time(s, me, line, top), front_time(s, me, dist + c.len, top))) return false;
	bool claim = me.wait > tp.claim_after_s;
	// whoever has waited its share claims the turn: those who can still stop comfortably let it through
	const bool can_stop = line > me.v * me.v / (2 * tp.claim_brake) + 1;
	for (size_t k = 0; k < c.conflicts.size(); k++) {
		if (conflict_busy(s, c, k)) return false;
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
		if (conflict_busy(s, c, k)) return Hold::CONFLICT_BUSY;
	return may_enter(s, vid, ci, t.g.ds[v.ds].len - v.s) ? Hold::FREE : Hold::YIELD;
}

int lane_head(const CityState &s, int vid) {
	const Vehicle &v = s.traffic.vehicles[vid];
	if (v.ds < 0) return -1;
	const auto &lane = s.traffic.lane_vehicles[size_t(v.ds) * MAX_LANES + v.lane];
	return lane.empty() ? -1 : lane.back();
}

int connector_to(const CityState &s, int ds, int lane, int next) {
	const LaneGraph &g = s.traffic.g;
	const size_t key = size_t(ds) * MAX_LANES + lane;
	if (ds < 0 || key >= g.lane_conn.size()) return -1;
	int best = -1, bd = MAX_LANES * 2;
	for (int ci = g.lane_conn[key]; ci >= 0; ci = g.conns[size_t(ci)].next_from) {
		const Conn &c = g.conns[size_t(ci)];
		if (!c.alive || c.to_ds != next) continue;
		int d = std::abs(c.from_lane - lane);
		if (d < bd) { bd = d; best = ci; }
	}
	return best;
}

int pick_connector(const CityState &s, const Vehicle &v) {
	if (v.ds < 0 || v.ri + 1 >= v.route.len) return -1;
	return connector_to(s, v.ds, v.lane, route_at(s.traffic.route_pool, v.route, v.ri + 1));
}

} // namespace traffic
} // namespace city
