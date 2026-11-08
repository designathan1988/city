// FlowWarrants (flow_warrants.h): the MUTCD's volume warrants for a signal and for a Stop on every leg, with the game's step down.
#include "sim/traffic/flow_warrants.h"

#include <algorithm>

namespace city {

AutoControl warranted_control(const JunctionFlows &f, AutoControl current, const Tuning &tn) {
	// A control the junction already has, or holds more than, steps down only below this share of its volumes.
	const auto k = [&](AutoControl step) -> float { return int(current) >= int(step) ? tn.flow_step_down : 1; };
	const float ks = k(AutoControl::SIGNAL);
	const bool condition_a = f.major >= tn.flow_signal_major * ks && f.minor_max >= tn.flow_signal_minor * ks;
	const bool condition_b = f.major >= tn.flow_signal_major_b * ks && f.minor_max >= tn.flow_signal_minor_b * ks;
	if (condition_a || condition_b) return AutoControl::SIGNAL;
	const float kp = k(AutoControl::STOP);
	const bool multiway = f.major >= tn.flow_stop_major * kp && f.minor_total >= tn.flow_stop_minor * kp;
	return multiway ? AutoControl::STOP : AutoControl::PRIORITY;
}

float quiet_flow(const Tuning &tn) {
	return std::min(std::min(tn.flow_stop_major, tn.flow_signal_major), tn.flow_signal_major_b) * std::min(1.f, tn.flow_step_down);
}

bool FlowWarrants::due(float dt, const Tuning &tn) {
	since += dt;
	if (since < tn.flow_look_every_s) return false;
	since = 0;
	return true;
}

std::optional<AutoControl> FlowWarrants::choice(int node) const {
	const auto it = choices_.find(node);
	if (it == choices_.end()) return std::nullopt;
	return it->second.control;
}

bool FlowWarrants::evaluate(int node, const JunctionFlows &f, AutoControl initial, float now, const Tuning &tn) {
	Choice &c = choices_.try_emplace(node, Choice{ initial }).first->second;
	// Nothing is chosen before a whole trend window was measured, nor within one of the last change.
	if (now < tn.flow_trend_window || now - c.changed_at < tn.flow_trend_window) return false;
	const AutoControl next = warranted_control(f, c.control, tn);
	if (next == c.control) return false;
	c.control = next;
	c.changed_at = now;
	return true;
}

} // namespace city
