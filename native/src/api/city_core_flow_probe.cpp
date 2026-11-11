// CityProbes: the flows and the control they warrant, for the tests. `flow_replay` runs the translated classes alone through a fixed
// script, the same one tools/converter/scripts/call40.mjs runs on the Roadcraft 4.0, so a number off is a translation off.
// `flow_push` and `flow_state` work on the city.
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

// Past what a float holds: two ids that only an integer keeps apart.
constexpr int64_t REPLAY_NODE = 33554433;
constexpr int64_t MAIN_A = 5, MAIN_B = 6, MINOR = 7;
constexpr int MAIN_EVERY_S = 4, MINOR_EVERY_S = 12, EVALUATE_EVERY_S = 10;
constexpr int BUSY_S = 1000, QUIET_S = 3000;

const char *name_of(std::optional<AutoControl> c) {
	if (!c) return "none";
	return *c == AutoControl::SIGNAL ? "signal" : *c == AutoControl::STOP ? "stop" : "priority";
}

void feed(FlowStats &f, int second, int64_t a, int64_t b, int64_t minor, int64_t node) {
	if (second % MAIN_EVERY_S == 0) {
		f.record(node, a);
		f.record(node, b);
	}
	if (second % MINOR_EVERY_S == 0) f.record(node, minor);
}

} // namespace

Dictionary CityProbes::flow_replay() {
	const Tuning &tn = state_->catalog.tune;
	FlowStats f;
	ControlAdvisor a;
	Dictionary out;
	int changed_at = -1;
	for (int second = 1; second <= BUSY_S + QUIET_S; second++) {
		if (second <= BUSY_S) feed(f, second, MAIN_A, MAIN_B, MINOR, REPLAY_NODE);
		f.step(1, tn);
		if (second % EVALUATE_EVERY_S == 0) {
			const float major = f.rate(REPLAY_NODE, MAIN_A, tn) + f.rate(REPLAY_NODE, MAIN_B, tn);
			const float minor = f.rate(REPLAY_NODE, MINOR, tn);
			if (a.evaluate(REPLAY_NODE, major, minor, minor, AutoControl::PRIORITY, f.measured, tn) && changed_at < 0) changed_at = second;
		}
		if (second == BUSY_S) {
			out["busy_major"] = f.rate(REPLAY_NODE, MAIN_A, tn);
			out["busy_minor"] = f.rate(REPLAY_NODE, MINOR, tn);
			out["busy_choice"] = name_of(a.choice(REPLAY_NODE));
		}
	}
	out["quiet_major"] = f.rate(REPLAY_NODE, MAIN_A, tn);
	out["quiet_choice"] = name_of(a.choice(REPLAY_NODE));
	out["changed_at"] = changed_at;
	out["quiet_flow"] = quiet_flow(tn);
	out["warranted"] = name_of(warranted_control(tn.flow_signal_major, tn.flow_signal_minor, tn.flow_signal_minor, AutoControl::PRIORITY, tn));
	return out;
}

Dictionary CityProbes::flow_push(int n, int seconds) {
	if (!sim_->hold_still()) return Dictionary(); // beside a running clock its ticks would land among these seconds
	CityState &s = *state_;
	const RoadNode &node = s.roads.nodes[size_t(n)];
	s.camera_x.store(float(node.x));
	s.camera_z.store(float(node.z));
	const std::vector<int> main = traffic::main_legs(s, n);
	if (main.size() < 2) return flow_state(n); // no road goes through it: no major flow to push
	int minor = -1;
	for (int seg : node.segs)
		if (seg != main[0] && seg != main[1]) minor = seg;
	{
		std::unique_lock<std::shared_mutex> map(s.map_mutex);
		std::lock_guard<std::mutex> tr(s.traffic_mutex);
		FlowControl &c = s.traffic.flow_control;
		for (int second = 1; second <= seconds; second++) {
			feed(*c.flow, second, main[0], main[1], minor, n);
			traffic::step_flow_control(s, 1);
		}
		traffic::apply_flow_control(s);
	}
	return flow_state(n);
}

Dictionary CityProbes::flow_state(int n) {
	CityState &s = *state_;
	std::lock_guard<std::mutex> tr(s.traffic_mutex);
	const FlowControl &c = s.traffic.flow_control;
	Dictionary out;
	out["measured"] = c.flow->measured;
	out["advised"] = c.advised.size();
	out["choice"] = n >= 0 ? name_of(c.advisor->choice(n)) : "none";
	out["signal"] = n >= 0 && traffic::control_of(s, n) == Control::SIGNAL;
	int giving_way = 0; // the arms whose traffic gives way there (a yield or a stop plate: approach_rule), the others have the way
	if (n >= 0 && n < int(s.roads.nodes.size()))
		for (int seg : s.roads.nodes[size_t(n)].segs) giving_way += traffic::approach_rule(s, n, seg) != RULE_NONE ? 1 : 0;
	out["giving_way"] = giving_way;
	return out;
}

} // namespace city
