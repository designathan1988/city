// FlowControl (flow_control.h): what stepControlAdvisor and applyAutoControl of the Roadcraft 4.0 do, on the game's network.
// A junction's control chosen here is an input of the furniture of its arms (their signal posts): told as the player's own edits tell
// it (the kept placement dropped, the props drawn again), without moving the streets' revision, since the value is derived.
#include "sim/traffic/flow_control.h"

#include "core/grid.h"
#include "sim/state/city_state.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city::traffic {

namespace {

// Junctions from three arms on (a road going on has no control).
constexpr size_t LEAST_ARMS = 3;

// Out of the micro window nothing is counted: a junction there is judged by nothing, and its lights would follow the camera.
bool counted_here(const CityState &s, const RoadNode &node) {
	for (int seg : node.segs)
		if (in_window(s, seg * 2) || in_window(s, seg * 2 + 1)) return true;
	return false;
}

struct Flows {
	float major = 0, minor_max = 0, minor_total = 0;
};

Flows flows_at(CityState &s, int n, const RoadNode &node) {
	FlowStats &flow = *s.traffic.flow_control.flow;
	const std::vector<int> main = main_legs(s, n);
	Flows f;
	for (int seg : node.segs) {
		const float rate = flow.rate(n, seg, s.catalog.tune);
		if (std::find(main.begin(), main.end(), seg) != main.end()) {
			f.major += rate;
		} else {
			f.minor_max = std::max(f.minor_max, rate);
			f.minor_total += rate;
		}
	}
	return f;
}

// Leg `seg` of node `n` forgotten in a table keyed by node then leg; the node's entry goes when it holds no leg.
template <typename Legs>
void drop_leg(js::Map<int64_t, Legs> &by_node, int64_t n, int64_t seg) {
	auto it = by_node.find(n);
	if (it == by_node.end()) return;
	it->second.erase(seg);
	if (it->second.empty()) by_node.erase(n);
}

// The legs of node `n` that are not among `arms` forgotten (a js::Map erases while it is walked, as JavaScript's does).
template <typename Legs>
void keep_arms(js::Map<int64_t, Legs> &by_node, int64_t n, const std::vector<int> &arms) {
	auto it = by_node.find(n);
	if (it == by_node.end()) return;
	auto &legs = it->second;
	for (const auto &leg : legs) {
		const int64_t seg = leg.first;
		if (std::find(arms.begin(), arms.end(), int(seg)) == arms.end()) legs.erase(seg);
	}
	if (legs.empty()) by_node.erase(n);
}

// Every key of node `n`: what was measured at it and the control chosen for it.
void forget_node_keys(FlowControl &c, int64_t n) {
	FlowStats &f = *c.flow;
	f.rates.erase(n);
	f.counts.erase(n);
	f.held.erase(n);
	f.counted.erase(n);
	c.advisor->choices.erase(n);
	c.advisor->changed_at.erase(n);
	c.advised.remove(n);
}

// The junction's control changed: each arm's kept furniture is placed again when next read and its props drawn again, from every
// chunk its box touches (the one that draws it among them).
void tell_arms(CityState &s, const RoadNode &node) {
	for (int seg : node.segs) {
		s.furniture_kept.forget_seg(seg);
		s.roads.mark_surface(seg); // what is written on its lanes follows the rule (render/roads/road_legends.cpp)
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		for_chunks(g.minx, g.minz, g.maxx, g.maxz, [&](int64_t k) { s.log.prop_chunks.push_back(k); });
	}
}

// One junction looked at: whether the advisor goes on looking at it.
bool look_at(CityState &s, int n) {
	FlowControl &c = s.traffic.flow_control;
	if (n >= int(s.roads.nodes.size())) return false;
	const RoadNode &node = s.roads.nodes[size_t(n)];
	if (!node.alive || node.ctl != CTL_AUTO || node.segs.size() < LEAST_ARMS) return false;
	if (!counted_here(s, node)) {
		c.flow->hold(n);
		return true;
	}
	c.flow->resume(n);
	const Flows f = flows_at(s, n, node);
	const AutoControl initial = control_of(s, n) == Control::SIGNAL ? AutoControl::SIGNAL : AutoControl::PRIORITY;
	if (c.advisor->evaluate(n, f.major, f.minor_max, f.minor_total, initial, c.flow->measured, s.catalog.tune)) c.changed.push_back(n);
	return !(c.advisor->choice(n) == AutoControl::PRIORITY && f.major + f.minor_total < quiet_flow(s.catalog.tune));
}

} // namespace

void count_entry(CityState &s, int connector) {
	const LaneGraph &g = s.traffic.g;
	const Conn &conn = g.conns[size_t(connector)];
	if (conn.node >= 0 && conn.from_ds >= 0) s.traffic.flow_control.flow->record(conn.node, g.ds[size_t(conn.from_ds)].seg);
}

void step_flow_control(CityState &s, float dt) {
	FlowControl &c = s.traffic.flow_control;
	c.flow->step(dt, s.catalog.tune);
	c.flow->take_counted(c.advised);
	if (!c.advisor->tick(dt)) return;
	const std::vector<int64_t> looked(c.advised.begin(), c.advised.end());
	for (int64_t n : looked)
		if (!look_at(s, int(n))) c.advised.remove(n);
}

void apply_flow_control(CityState &s) {
	FlowControl &c = s.traffic.flow_control;
	for (int n : c.changed) {
		const std::optional<AutoControl> choice = c.advisor->choice(n);
		if (!choice || n >= int(s.roads.nodes.size())) continue;
		RoadNode &node = s.roads.nodes[size_t(n)];
		const int8_t signal = *choice == AutoControl::SIGNAL ? AUTO_SIGNAL : *choice == AutoControl::STOP ? AUTO_ALL_STOP : int8_t(0);
		if (!node.alive || node.auto_signal == signal) continue;
		node.auto_signal = signal;
		s.log.net_node(n);
		tell_arms(s, node); // derived and never saved (road_network.h): no touch, the streets' revision stays
	}
	c.changed.clear();
}

void forget_flows(CityState &s, const std::vector<int> &removed_segs, const std::vector<int> &nodes, const std::vector<int> &gone_nodes) {
	FlowControl &c = s.traffic.flow_control;
	FlowStats &f = *c.flow;
	const RoadNetwork &net = s.roads;
	for (int seg : removed_segs) { // the dead record keeps its nodes (RoadNetwork::remove_segment); one laid again on the id has new ones
		if (seg < 0 || size_t(seg) >= net.segs.size()) continue;
		const RoadSegment &g = net.segs[size_t(seg)];
		for (int n : { g.a, g.b }) {
			drop_leg(f.rates, n, seg);
			drop_leg(f.counts, n, seg);
		}
	}
	for (int n : gone_nodes) forget_node_keys(c, n);
	for (int n : nodes) {
		if (n < 0 || size_t(n) >= net.nodes.size()) continue;
		const RoadNode &node = net.nodes[size_t(n)];
		if (!node.alive) { // removed by a path that is not a command's own (an undo of its making)
			forget_node_keys(c, n);
			continue;
		}
		keep_arms(f.rates, n, node.segs);
		keep_arms(f.counts, n, node.segs);
	}
}

} // namespace city::traffic
