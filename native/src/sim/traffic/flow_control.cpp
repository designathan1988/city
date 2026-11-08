// FlowControl (flow_control.h): the junctions on automatic looked at by their measured flows, and the choices published to the network.
#include "sim/traffic/flow_control.h"

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

JunctionFlows flows_at(CityState &s, int n, const RoadNode &node) {
	const FlowMeasure &flow = s.traffic.flow_control.flow;
	const std::vector<int> main = main_legs(s, n);
	JunctionFlows f;
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

// One junction looked at: whether the flows can still change its control (a quiet junction on priority leaves the list until
// it is counted again).
bool look_at(CityState &s, int n) {
	FlowControl &c = s.traffic.flow_control;
	if (n >= int(s.roads.nodes.size())) return false;
	const RoadNode &node = s.roads.nodes[size_t(n)];
	if (!node.alive || node.ctl != CTL_AUTO || node.segs.size() < LEAST_ARMS) return false;
	if (!counted_here(s, node)) {
		c.flow.hold(n);
		return true;
	}
	c.flow.resume(n);
	const JunctionFlows f = flows_at(s, n, node);
	const AutoControl initial = control_of(s, n) == Control::SIGNAL ? AutoControl::SIGNAL : AutoControl::PRIORITY;
	if (c.warrants.evaluate(n, f, initial, c.flow.measured, s.catalog.tune)) c.changed.push_back(n);
	return !(c.warrants.choice(n) == AutoControl::PRIORITY && f.major + f.minor_total < quiet_flow(s.catalog.tune));
}

} // namespace

void count_entry(CityState &s, int connector) {
	const LaneGraph &g = s.traffic.g;
	const Conn &conn = g.conns[size_t(connector)];
	if (conn.node >= 0 && conn.from_ds >= 0) s.traffic.flow_control.flow.record(conn.node, g.ds[size_t(conn.from_ds)].seg);
}

void step_flow_control(CityState &s, float dt) {
	FlowControl &c = s.traffic.flow_control;
	c.flow.step(dt, s.catalog.tune);
	for (int n : c.flow.take_counted())
		if (std::find(c.advised.begin(), c.advised.end(), n) == c.advised.end()) c.advised.push_back(n);
	if (!c.warrants.due(dt, s.catalog.tune)) return;
	const std::vector<int> looked = c.advised;
	for (int n : looked)
		if (!look_at(s, n)) c.advised.erase(std::find(c.advised.begin(), c.advised.end(), n));
}

void apply_flow_control(CityState &s) {
	FlowControl &c = s.traffic.flow_control;
	for (int n : c.changed) {
		const std::optional<AutoControl> choice = c.warrants.choice(n);
		if (!choice || n >= int(s.roads.nodes.size())) continue;
		RoadNode &node = s.roads.nodes[size_t(n)];
		const int8_t signal = *choice == AutoControl::SIGNAL ? AUTO_SIGNAL : *choice == AutoControl::STOP ? AUTO_ALL_STOP : int8_t(0);
		if (!node.alive || node.auto_signal == signal) continue;
		node.auto_signal = signal;
		s.log.net_node(n);
	}
	c.changed.clear();
}

} // namespace city::traffic
