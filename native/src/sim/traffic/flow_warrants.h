// The control a junction on automatic warrants from its measured flows, after the MUTCD (FHWA, 2009 edition):
// - a signal by Warrant 1, eight-hour vehicular volume (section 4C.02, Table 4C-1, the 100% column for one lane each way):
//   Condition A, minimum vehicular volume (flow.signal_major on the major street's two approaches together and
//   flow.signal_minor on the busier minor approach), or Condition B, interruption of continuous traffic (flow.signal_major_b,
//   flow.signal_minor_b);
// - a Stop on every leg by the multi-way stop volumes (section 2B.07, paragraph 04 C: flow.stop_major on the major approaches
//   together, flow.stop_minor on the minor approaches together);
// - otherwise priority, the major street going on.
// The values are data/catalog/tuning.json flow. The rest is this game's: the manual's volumes hold for any eight hours of an
// average day, the game's for its measured trend (flow_measure.h); a control steps down only below flow.step_down of what put it
// there, and changes at most once a trend window, so a junction does not flicker at a threshold.
#pragma once

#include "sim/catalog/catalog.h"

#include <limits>
#include <optional>
#include <unordered_map>

namespace city {

// In the order of how much they hold the traffic: a step up is to a greater value.
enum class AutoControl : int { PRIORITY, STOP, SIGNAL };

struct JunctionFlows {
	float major = 0; // vehicles an hour entering from the major street's legs, together
	float minor_max = 0; // from the busiest other leg
	float minor_total = 0; // from the other legs, together
};

// What the flows warrant, given what the junction has now (a control it has steps down only below flow.step_down of its volumes).
AutoControl warranted_control(const JunctionFlows &f, AutoControl current, const Tuning &tn);

// Vehicles an hour into a junction below which no control is warranted whatever it has: the least major volume of a step up,
// with the step down.
float quiet_flow(const Tuning &tn);

// The choices made, one a junction, each kept a trend window at least.
struct FlowWarrants {
	float since = 0; // seconds since the junctions were last looked at
	bool due(float dt, const Tuning &tn); // true once flow.look_every_s has passed since the last look
	std::optional<AutoControl> choice(int node) const; // none before the junction was first looked at
	// One junction looked at, with `initial` the control it shows before any choice and `now` the seconds measured; true when its
	// choice changed.
	bool evaluate(int node, const JunctionFlows &f, AutoControl initial, float now, const Tuning &tn);

private:
	struct Choice {
		AutoControl control = AutoControl::PRIORITY;
		float changed_at = -std::numeric_limits<float>::infinity();
	};
	std::unordered_map<int, Choice> choices_;
};

} // namespace city
