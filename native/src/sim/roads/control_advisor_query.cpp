// Roadcraft 4.0 src/sim/roads/controlAdvisor.ts: the queries (they only read).
// Translated from Roadcraft 4.0 src/sim/roads/controlAdvisor.ts by the converter; the owner is native/src/sim/roads.
#include "sim/roads/control_advisor.h"

namespace city {

namespace {

int rank(AutoControl c);

int rank(AutoControl c) {
	return ((c == AutoControl::SIGNAL ? 2 : (c == AutoControl::STOP ? 1 : 0)));
}

} // namespace

// Vehicles an hour into a junction below which `warrantedControl` is 'priority' whatever it has now (the least
// major flow of a step up, with the hysteresis).
float quiet_flow(const Tuning &tn) {
	return (std::min(std::min(tn.flow_stop_major, tn.flow_signal_major), tn.flow_signal_major_b) * std::min(1.f, tn.flow_step_down));
}

// The control a junction's flows warrant, given what it has now (hysteresis).
AutoControl warranted_control(float major, float minor_max, float minor_total, AutoControl current, const Tuning &tn) {
	auto k = [&](AutoControl step) -> float {
		return ((rank(current) >= rank(step) ? tn.flow_step_down : 1));
	};
	const bool signal = (major >= tn.flow_signal_major * k(AutoControl::SIGNAL) && minor_max >= tn.flow_signal_minor * k(AutoControl::SIGNAL)) || (major >= tn.flow_signal_major_b * k(AutoControl::SIGNAL) && minor_max >= tn.flow_signal_minor_b * k(AutoControl::SIGNAL));
	if (signal)
		return AutoControl::SIGNAL;
	const bool stop = major >= tn.flow_stop_major * k(AutoControl::STOP) && minor_total >= tn.flow_stop_minor * k(AutoControl::STOP);
	return (stop ? AutoControl::STOP : AutoControl::PRIORITY);
}

// The control chosen for `node`, or undefined before the advisor has spoken (the game's geometric default holds).
std::optional<AutoControl> ControlAdvisor::choice(int64_t node) {
	return ([&]() -> const AutoControl * { auto it = this->choices.find(node); return it == this->choices.end() ? nullptr : &it->second; }() ? std::optional<AutoControl>(*[&]() -> const AutoControl * { auto it = this->choices.find(node); return it == this->choices.end() ? nullptr : &it->second; }()) : std::nullopt);
}

// Whether it is time to evaluate again.
bool ControlAdvisor::tick(float dt) {
	this->since += dt;
	if (this->since < ControlAdvisor::every)
		return false;
	this->since = 0;
	return true;
}

// One junction's evaluation; returns true when its choice changed.
bool ControlAdvisor::evaluate(int64_t node, float major, float minor_max, float minor_total, AutoControl initial, float now, const Tuning &tn) {
	const AutoControl current = [&] { const auto found_1 = [&]() -> const AutoControl * { auto it = this->choices.find(node); return it == this->choices.end() ? nullptr : &it->second; }(); return found_1 ? *found_1 : AutoControl(initial); }();
	if (!(this->choices.count(node) != 0))
		this->choices[node] = initial;
	if (now < tn.flow_trend_window)
		return false;
	if (now - ([&] { const auto found_2 = [&]() -> const float * { auto it = this->changed_at.find(node); return it == this->changed_at.end() ? nullptr : &it->second; }(); return found_2 ? *found_2 : float(-std::numeric_limits<float>::infinity()); }()) < tn.flow_trend_window)
		return false;
	const AutoControl next = warranted_control(major, minor_max, minor_total, current, tn);
	if (next == current)
		return false;
	this->choices[node] = next;
	this->changed_at[node] = now;
	return true;
}

void ControlAdvisor::forget(const js::Set<int64_t> &nodes) {
	for (int64_t node : std::vector<int64_t>(js::keys_of(this->choices))) {
		if (!(nodes.count(node) != 0)) {
			this->choices.erase(node);
			this->changed_at.erase(node);
		}
	}
}

// These junctions' choices forgotten (gone with an edit).
void ControlAdvisor::drop(const js::Set<int64_t> &nodes) {
	for (int64_t node : nodes) {
		this->choices.erase(node);
		this->changed_at.erase(node);
	}
}

ControlAdvisor::ControlAdvisor() {
	this->choices = js::Map<int64_t, AutoControl>{};
	this->changed_at = js::Map<int64_t, float>{};
	this->since = 0;
}

} // namespace city
