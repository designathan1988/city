// CityProbes: the flows and the control they warrant, for the tests. `flow_replay` runs the measure and the warrants alone through a
// fixed script (a busy junction fed at known rates, then left quiet); `flow_warrant` asks the rule alone; `flow_junction`,
// `flow_push` and `flow_state` work on the city.
#include "api/state_locks.h"
#include "api/city_probes.h"

#include "sim/simulation/simulation.h"
#include "sim/state/city_state.h"
#include "sim/traffic/flow_control.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <mutex>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {

// The script's junction and legs: two legs of the major street, each entered every MAIN_EVERY_S, and one minor leg every
// MINOR_EVERY_S, for BUSY_S (ten time constants of the measure, so its rate stands at the fed one), then nothing for QUIET_S.
constexpr int REPLAY_NODE = 1, MAIN_A = 5, MAIN_B = 6, MINOR = 7;
constexpr int MAIN_EVERY_S = 4, MINOR_EVERY_S = 12;
constexpr int BUSY_S = 3000, QUIET_S = 3000;
constexpr float SECONDS_PER_HOUR = 3600; // a vehicle every n seconds is 3600 / n an hour

const char *name_of(std::optional<AutoControl> c) {
	if (!c) return "none";
	return *c == AutoControl::SIGNAL ? "signal" : *c == AutoControl::STOP ? "stop" : "priority";
}

void feed(FlowMeasure &f, int second, int a, int b, int minor, int node) {
	if (second % MAIN_EVERY_S == 0) {
		f.record(node, a);
		f.record(node, b);
	}
	if (second % MINOR_EVERY_S == 0) f.record(node, minor);
}

} // namespace

Dictionary CityProbes::flow_replay() {
	const Tuning &tn = state_->catalog.tune;
	FlowMeasure f;
	FlowWarrants w;
	Dictionary out;
	int changed_at = -1;
	for (int second = 1; second <= BUSY_S + QUIET_S; second++) {
		if (second <= BUSY_S) feed(f, second, MAIN_A, MAIN_B, MINOR, REPLAY_NODE);
		f.step(1, tn);
		if (w.due(1, tn)) {
			const float minor = f.rate(REPLAY_NODE, MINOR, tn);
			const JunctionFlows flows{ f.rate(REPLAY_NODE, MAIN_A, tn) + f.rate(REPLAY_NODE, MAIN_B, tn), minor, minor };
			if (w.evaluate(REPLAY_NODE, flows, AutoControl::PRIORITY, f.measured, tn) && changed_at < 0) changed_at = second;
		}
		if (second == BUSY_S) {
			out["busy_major"] = f.rate(REPLAY_NODE, MAIN_A, tn);
			out["busy_minor"] = f.rate(REPLAY_NODE, MINOR, tn);
			out["busy_choice"] = name_of(w.choice(REPLAY_NODE));
		}
	}
	out["fed_major"] = SECONDS_PER_HOUR / MAIN_EVERY_S;
	out["fed_minor"] = SECONDS_PER_HOUR / MINOR_EVERY_S;
	out["quiet_s"] = QUIET_S;
	out["quiet_major"] = f.rate(REPLAY_NODE, MAIN_A, tn);
	out["quiet_choice"] = name_of(w.choice(REPLAY_NODE));
	out["changed_at"] = changed_at;
	out["quiet_flow"] = quiet_flow(tn);
	return out;
}

String CityProbes::flow_warrant(float major, float minor_max, float minor_total, int current) const {
	return name_of(warranted_control(JunctionFlows{ major, minor_max, minor_total }, AutoControl(current), state_->catalog.tune));
}

int CityProbes::flow_junction() {
	CityState &s = *state_;
	std::shared_lock<std::shared_mutex> map(s.map_mutex);
	for (size_t n = 0; n < s.roads.nodes.size(); n++) {
		const RoadNode &node = s.roads.nodes[n];
		if (node.alive && node.ctl == CTL_AUTO && node.segs.size() == 3 && traffic::control_of(s, int(n)) != Control::SIGNAL) return int(n);
	}
	return -1;
}

Dictionary CityProbes::flow_push(int n, int seconds) {
	if (!sim_->hold_still()) return Dictionary(); // beside a running clock its ticks would land among these seconds
	CityState &s = *state_;
	{
		std::unique_lock<std::shared_mutex> map(s.map_mutex); // the node and its legs are read under it too
		std::lock_guard<std::mutex> tr(s.traffic_mutex);
		if (n < 0 || n >= int(s.roads.nodes.size())) return Dictionary(); // flow_junction found none
		const RoadNode &node = s.roads.nodes[size_t(n)];
		s.camera_x.store(float(node.x));
		s.camera_z.store(float(node.z));
		const std::vector<int> main = traffic::main_legs(s, n);
		int minor = -1;
		for (int seg : node.segs)
			if (seg != main[0] && seg != main[1]) minor = seg;
		FlowControl &c = s.traffic.flow_control;
		for (int second = 1; second <= seconds; second++) {
			feed(c.flow, second, main[0], main[1], minor, n);
			traffic::step_flow_control(s, 1);
		}
		traffic::apply_flow_control(s);
	}
	return flow_state(n);
}

Dictionary CityProbes::flow_state(int n) {
	CityState &s = *state_;
	const TrafficRead lk(s); // control_of reads the road network
	const FlowControl &c = s.traffic.flow_control;
	Dictionary out;
	out["measured"] = c.flow.measured;
	out["advised"] = int(c.advised.size());
	out["choice"] = n >= 0 ? name_of(c.warrants.choice(n)) : "none";
	out["signal"] = n >= 0 && traffic::control_of(s, n) == Control::SIGNAL;
	return out;
}

} // namespace city
