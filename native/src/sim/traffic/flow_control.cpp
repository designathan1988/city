// FlowControl (flow_control.h): what stepControlAdvisor and applyAutoControl of the Roadcraft 4.0 do, on the game's network.
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
		const int8_t signal = *choice == AutoControl::SIGNAL ? 1 : 0;
		if (!node.alive || node.auto_signal == signal) continue;
		node.auto_signal = signal;
		s.log.net_node(n);
	}
	c.changed.clear();
}

} // namespace city::traffic
