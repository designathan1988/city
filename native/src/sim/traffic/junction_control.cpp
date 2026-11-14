// Junction control: the control a node gets (signals, priority or none), the road it is on (its main arms) and each approach's
// rank and rule (an unsigned junction's highway first, a mini-roundabout's circle), its signal plan asked of signal_plan.cpp and
// kept running through an edit. Who may enter a connector now is junction_gate.cpp.
#include "sim/traffic/traffic_system.h"

#include "sim/roads/junction.h"
#include "sim/traffic/signal_plan.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {

constexpr int RANK_MAIN = 3, RANK_FREE = 2, RANK_YIELD = 1; // RANK_FREE is DirSeg::rank's default
constexpr int RING_PRIORITY = 100; // over every class of road: whoever circulates has the way, whatever comes in

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

// A junction with no control at all (the player's "none"): nobody has the way but a highway's flow over the roads that meet it
// (CTB art. 29 III a: where only one flow comes off a highway, whoever travels on it; the article's words are in traffic.json's
// `_junction_doc`); with no highway, or nothing but highways, none (art. 29 III c: who comes by the driver's right,
// junction_gate.cpp outranks).
std::vector<int> highway_segs(const CityState &s, int n) {
	const std::vector<Arm> arms = network_arms(s, n);
	std::vector<int> out;
	for (const Arm &a : arms)
		if (s.roads.profile_of(a.seg).road_class == RoadClass::HIGHWAY) out.push_back(a.seg);
	if (out.size() == arms.size()) out.clear();
	return out;
}

// The arms that have the way at a junction that goes by rank: the unsigned junction's highways, else the road it is on.
std::vector<int> ranked_main(const CityState &s, int n, const RoadNode &node) {
	return node.ctl == CTL_NONE ? highway_segs(s, n) : main_segs(s, n, signed_main(node));
}

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

// The junction's arms as its signal plan reads them (signal_plan.h).
std::vector<StageArm> stage_arms(const std::vector<Arm> &arms) {
	std::vector<StageArm> out;
	for (const Arm &a : arms) out.push_back({ a.seg, a.in, a.out });
	return out;
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
		if (a.in >= 0) { g.ds[a.in].stage = g.ds[a.in].turn_stage = -1; g.ds[a.in].rank = RANK_FREE; g.ds[a.in].must_stop = false; g.ds[a.in].stop_back = 0; g.ds[a.in].detector = g.ds[a.in].extension = 0; }
	j.degree = int(arms.size());
	for (const Arm &a : arms)
		if (a.in >= 0) g.ds[a.in].stop_back = float(s.roads.stop_line_back(n, a.seg));
	j.ctl = control_of(s, n);
	if (j.ctl == Control::SIGNAL) plan_stages(s, n, stage_arms(arms), j);
	file_signal(g, n);
	if (j.ctl != Control::PRIORITY) return;
	const RoadNode &node = s.roads.nodes[size_t(n)];
	j.circle = node.ctl == CTL_MINI; // every leg gives way and whoever is on the circle goes first (junction_gate.cpp outranks)
	const std::vector<int> main = ranked_main(s, n, node); // the arms the signs read (approach_rule), from the same network
	const int everyone = leg_rule(node);
	const int lesser = node.ctl == CTL_NONE ? RANK_FREE : RANK_YIELD; // unsigned: nobody gives way by a rule
	for (const Arm &a : arms) {
		if (a.in < 0) continue;
		const int plate = plate_rule(s.roads, a.seg, n);
		const bool on = std::find(main.begin(), main.end(), a.seg) != main.end();
		g.ds[a.in].rank = on && !plate && !everyone ? RANK_MAIN : (plate ? RANK_YIELD : lesser);
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
	if (node.ctl == CTL_NONE) return RULE_NONE; // no control: no plate, no paint (who has the way: ranked_main, outranks)
	const std::vector<int> main = main_segs(s, n, signed_main(node));
	return std::find(main.begin(), main.end(), seg) != main.end() ? RULE_NONE : RULE_YIELD;
}

void replan_signal(CityState &s, int n) {
	LaneGraph &g = s.traffic.g;
	if (n < 0 || n >= int(g.junctions.size()) || n >= int(s.roads.nodes.size()) || !s.roads.nodes[size_t(n)].alive) return;
	const Junction was = g.junctions[size_t(n)];
	build_junction(s, n);
	Junction &j = g.junctions[size_t(n)];
	if (j.ctl != Control::SIGNAL || was.ctl != Control::SIGNAL || was.green.empty() || j.green.empty()) return;
	// no jump from one green to another: the new plan goes on from the stage that holds most of the approaches that were green, as
	// that green began (walkers started on it), its intergreen after it; with none of them left it starts in an all-red
	const std::vector<int> &green = was.approaches[size_t(was.run)];
	int keep = -1, most = 0;
	for (size_t k = 0; k < j.approaches.size(); k++) {
		int shared = 0;
		for (int ds : j.approaches[k]) shared += std::find(green.begin(), green.end(), ds) != green.end() ? 1 : 0;
		if (shared > most) { most = shared; keep = int(k); }
	}
	if (keep < 0) {
		j.run = 0;
		j.run_green = 0;
		j.run_start = s.traffic.time - double(j.amber[0]);
		j.settle = j.run_start + double(j.amber[0]) + double(j.all_red[0]);
		return;
	}
	j.run = keep;
	j.run_start = was.run_start;
	j.run_green = was.run_green;
	const double green_end = j.run_green >= 0 ? double(j.run_green) : std::max(s.traffic.time - j.run_start, double(j.green[size_t(keep)]));
	j.settle = j.run_start + green_end + j.amber[size_t(keep)] + j.all_red[size_t(keep)];
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
