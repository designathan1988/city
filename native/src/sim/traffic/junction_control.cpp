// Junction control: the control a node gets (signals, priority or none), the road it is on (its main arms) and each approach's
// rank and rule, its signal stages (opposite approaches together, the intergreens and greens of signal_timing.cpp). Who may enter a
// connector now is junction_gate.cpp.
#include "sim/traffic/traffic_system.h"

#include "sim/roads/junction.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {

constexpr int RANK_MAIN = 3, RANK_YIELD = 1; // free arms keep DirSeg::rank's default
constexpr int RING_PRIORITY = 100; // over every class of road: whoever circulates has the way, whatever comes in
constexpr uint32_t OFFSET_HASH = 2654435761u, OFFSET_STEPS = 1000; // where in its first green each junction starts

struct Arm {
	int seg = -1, in = -1, priority = 0;
	int lanes = 0; // its road's travel lanes, both ways
	V2 out; // outward direction from the node
};

std::vector<Arm> arms_of(const CityState &s, int n) {
	const LaneGraph &g = s.traffic.g;
	std::vector<Arm> arms;
	for (int si : s.roads.nodes[n].segs) {
		int din = si * 2 + (s.roads.segs[si].b == n ? 0 : 1), dout = si * 2 + (s.roads.segs[si].a == n ? 0 : 1);
		if (!g.ds[din].ok && !g.ds[dout].ok) continue;
		const Profile &prof = s.roads.profile_of(si);
		Arm a;
		a.seg = si;
		a.in = g.ds[din].ok ? din : -1;
		a.priority = prof.ring ? RING_PRIORITY : prof.priority;
		a.lanes = prof.lanes_fwd() + prof.lanes_back();
		Pose p;
		if (a.in >= 0) { lane_pose(s, din, 0, g.ds[din].len, p); a.out = V2(-p.fx, -p.fz); }
		else { lane_pose(s, dout, 0, 0.f, p); a.out = V2(p.fx, p.fz); }
		arms.push_back(a);
	}
	return arms;
}

// The same arms as arms_of, in the same order, from the road network alone (no `in`, the direction from the segment's
// arm): a DirSeg is ok for a live segment between two nodes that is no rail line and has lanes that way (lane_graph.cpp
// build_dir), so an arm with lanes either way is one of these. What the render reads (control_of, approach_rule) comes
// from here: it holds only the map lock, and the lane graph catches up with an edit on the next traffic step.
std::vector<Arm> network_arms(const CityState &s, int n) {
	const RoadNode &node = s.roads.nodes[size_t(n)];
	std::vector<Arm> arms;
	for (int si : node.segs) {
		const RoadSegment &g = s.roads.segs[size_t(si)];
		if (!g.alive || g.a == g.b) continue;
		const Profile &p = s.roads.profile_of(si);
		if (p.rail || (p.fwd.empty() && p.back.empty())) continue;
		const city::Arm out = arm_of(g.q, p, g.a == n ? 0 : 1);
		Arm a;
		a.seg = si;
		a.priority = p.ring ? RING_PRIORITY : p.priority;
		a.lanes = p.lanes_fwd() + p.lanes_back();
		a.out = V2(float(out.dx), float(out.dz));
		arms.push_back(a);
	}
	return arms;
}

// The arms that carry the road a junction is on (the reference game's rule): of every pair of arms at least through_pair_dot apart (one
// road going on through), those whose lesser priority is the highest; `straightest`: the pair most nearly in line of those alone (one road
// chosen: the player's signs, the flows, the green wave), else every such pair (an automatic junction: every road of that rank going on
// through it has the way). None when no two arms are in line.
std::vector<char> main_arms(const std::vector<Arm> &arms, float through_dot, bool straightest) {
	std::vector<char> on(arms.size(), 0);
	bool found = false;
	int best = 0;
	float best_dot = 0;
	size_t pa = 0, pb = 0;
	for (size_t i = 0; i < arms.size(); i++)
		for (size_t k = i + 1; k < arms.size(); k++) {
			const float dot = arms[i].out.dot(arms[k].out);
			const int rank = std::min(arms[i].priority, arms[k].priority);
			if (dot >= through_dot || (found && (rank < best || (rank == best && dot >= best_dot)))) continue;
			found = true;
			best = rank;
			best_dot = dot;
			pa = i;
			pb = k;
		}
	if (!found) return on;
	on[pa] = on[pb] = 1;
	for (size_t i = 0; !straightest && i < arms.size(); i++)
		for (size_t k = i + 1; k < arms.size(); k++)
			if (arms[i].out.dot(arms[k].out) < through_dot && std::min(arms[i].priority, arms[k].priority) == best) on[i] = on[k] = 1;
	return on;
}

// The segments of the road the junction is on (main_arms over the network's arms, what both the traffic and the render read).
std::vector<int> main_segs(const CityState &s, int n, bool straightest) {
	const std::vector<Arm> arms = network_arms(s, n);
	const std::vector<char> on = main_arms(arms, s.catalog.traffic.through_pair_dot, straightest);
	std::vector<int> out;
	for (size_t k = 0; k < arms.size(); k++)
		if (on[k]) out.push_back(arms[k].seg);
	return out;
}

// Whether the road the junction is on is chosen by the player's signs (its straightest pair) or by the junction itself on automatic
// (every road of the highest rank going on through it).
bool signed_main(const RoadNode &node) { return node.ctl != CTL_AUTO; }

Control decide(const RoadNode &node, const std::vector<Arm> &arms, const TrafficParams &tp) {
	if (node.crossing != CROSSING_NONE && arms.size() == 2) return node.crossing == CROSSING_SIGNAL ? Control::SIGNAL : Control::PRIORITY;
	if (node.uturn != UTURN_NONE && arms.size() == 2) return Control::PRIORITY; // who turns back gives way to who goes on (outranks)
	if (arms.size() < 3) return Control::NONE;
	if (node.ctl == CTL_SIGNAL) return Control::SIGNAL;
	if (node.ctl != CTL_AUTO) return Control::PRIORITY; // stop, yield, none and mini: by rank, each leg's set in build_junction
	for (const Arm &a : arms)
		if (a.priority == RING_PRIORITY) return Control::PRIORITY; // a roundabout's entry gives way, and never takes a signal by itself
	if (node.auto_signal == AUTO_SIGNAL) return Control::SIGNAL; // the flows measured there chose: the geometry only speaks before they do
	if (node.auto_signal == AUTO_ALL_STOP) return Control::PRIORITY;
	int widest = 0;
	for (const Arm &a : arms) widest = std::max(widest, a.lanes);
	// the road the junction is on has the way and whatever joins it gives way; with every arm on it (a crossing of equal roads) or none in
	// line, nobody has it: by movement and from the right (CTB art. 29 III c)
	const std::vector<char> on = main_arms(arms, tp.through_pair_dot, false);
	const bool joined = std::find(on.begin(), on.end(), 1) != on.end() && std::find(on.begin(), on.end(), 0) != on.end();
	const Control by_rank = joined ? Control::PRIORITY : Control::NONE;
	if (node.auto_signal == 0) return by_rank;
	// Before the flows speak: a crossing of signal_arms arms or more runs on signals, and so does a T where one road carries
	// signal_lanes lanes or more (two each way); a T of narrower streets goes by rank.
	if (float(arms.size()) >= tp.signal_arms || float(widest) >= tp.signal_lanes) return Control::SIGNAL;
	return by_rank;
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
	if (s.roads.mid_block(n)) count++; // the road's two ways go together: the walkers' stage is one with nobody driving
	j.green.assign(count, tp.signal_safety_green_s);
	j.amber.assign(count, 0.f);
	j.all_red.assign(count, 0.f);
	j.approaches.assign(count, {});
	const bool walkers = s.roads.has_crosswalks(n);
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
	j.least = j.green;
	const SignalSettings &set = s.roads.nodes[size_t(n)].signal;
	j.fixed = set.fixed;
	j.offset = set.offset;
	for (int k = 0; set.fixed && k < count; k++) {
		if (size_t(k) < set.greens.size()) { j.cycle += std::max(0.f, set.greens[size_t(k)] - j.green[size_t(k)]); j.green[size_t(k)] = std::max(j.green[size_t(k)], set.greens[size_t(k)]); }
		j.green_max[size_t(k)] = j.green[size_t(k)];
	}
	j.run = 0;
	j.run_green = -1;
	j.run_start = s.traffic.time - double((uint32_t(n) * OFFSET_HASH) % OFFSET_STEPS) / OFFSET_STEPS * j.green[0];
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
		if (a.in >= 0) { g.ds[a.in].stage = -1; g.ds[a.in].rank = 2; g.ds[a.in].must_stop = false; g.ds[a.in].stop_back = 0; g.ds[a.in].detector = g.ds[a.in].extension = 0; }
	j.degree = int(arms.size());
	for (const Arm &a : arms)
		if (a.in >= 0) g.ds[a.in].stop_back = float(s.roads.stop_line_back(n, a.seg));
	j.ctl = control_of(s, n);
	if (j.ctl == Control::SIGNAL) make_stages(s, n, arms, j);
	file_signal(g, n);
	if (j.ctl != Control::PRIORITY) return;
	const RoadNode &node = s.roads.nodes[size_t(n)];
	const std::vector<int> main = main_segs(s, n, signed_main(node)); // the arms the signs read (approach_rule), from the same network
	const int everyone = leg_rule(node);
	for (const Arm &a : arms) {
		if (a.in < 0) continue;
		const int plate = plate_rule(s.roads, a.seg, n);
		const bool on = std::find(main.begin(), main.end(), a.seg) != main.end();
		g.ds[a.in].rank = on && !plate && !everyone ? RANK_MAIN : RANK_YIELD;
		g.ds[a.in].must_stop = everyone == RULE_STOP || plate == RULE_STOP;
	}
}

std::vector<int> main_legs(const CityState &s, int n) { return main_segs(s, n, true); }

int approach_rule(const CityState &s, int n, int seg) {
	if (control_of(s, n) != Control::PRIORITY) return RULE_NONE;
	const int plate = plate_rule(s.roads, seg, n);
	if (plate) return plate;
	const RoadNode &node = s.roads.nodes[size_t(n)];
	if (const int everyone = leg_rule(node)) return everyone;
	const std::vector<int> main = main_segs(s, n, signed_main(node));
	return std::find(main.begin(), main.end(), seg) != main.end() ? RULE_NONE : RULE_YIELD;
}

void replan_signal(CityState &s, int n) {
	LaneGraph &g = s.traffic.g;
	if (n < 0 || n >= int(g.junctions.size()) || n >= int(s.roads.nodes.size()) || !s.roads.nodes[size_t(n)].alive) return;
	const Junction was = g.junctions[size_t(n)];
	build_junction(s, n);
	Junction &j = g.junctions[size_t(n)];
	if (j.ctl != Control::SIGNAL || was.ctl != Control::SIGNAL || was.green.empty() || j.green.size() != was.green.size()) return;
	// no jump from one green to another: the stage running keeps at least its minimum green (walkers started on it) and its intergreen
	const size_t k = size_t(was.run);
	j.run = was.run;
	j.run_start = was.run_start;
	j.run_green = was.run_green >= 0 ? was.run_green : float(std::max(s.traffic.time - was.run_start, double(was.green[k])));
	j.settle = j.run_start + double(j.run_green) + j.amber[k] + j.all_red[k];
}

Control control_of(const CityState &s, int n) {
	if (n < 0 || n >= int(s.roads.nodes.size())) return Control::NONE;
	return decide(s.roads.nodes[size_t(n)], network_arms(s, n), s.catalog.traffic);
}

void build_junctions(CityState &s) {
	s.traffic.g.junctions.assign(s.roads.nodes.size(), Junction());
	s.traffic.g.signal_nodes.clear();
	s.traffic.g.signal_at.assign(s.roads.nodes.size(), 0);
	for (int n = 0; n < int(s.roads.nodes.size()); n++)
		if (s.roads.nodes[n].alive) build_junction(s, n);
}

} // namespace traffic
} // namespace city
