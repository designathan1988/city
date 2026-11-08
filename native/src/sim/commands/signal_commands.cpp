// A junction's signal as the player sets it: adaptive or fixed time, the greens and the offset, and the green wave along the
// main road through it. The plans themselves are the traffic's (junction_control.cpp, green_wave.cpp).
#include "sim/commands/commands.h"

#include "sim/commands/edit_journal.h"
#include "sim/state/city_state.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {

namespace {
// Decided from the road network (traffic::control_of), not read from the traffic's junctions: those are rebuilt from the change
// log later, and right after an edit they still show the junction as it was.
bool is_signal(const CityState &s, int node) {
	return node >= 0 && node < int(s.roads.nodes.size()) && s.roads.nodes[size_t(node)].alive && traffic::control_of(s, node) == Control::SIGNAL;
}
} // namespace

EditResult set_signal(CityState &s, int node, const SignalSettings &to) {
	EditResult r;
	if (!is_signal(s, node)) {
		r.error = "tool.failed";
		return r;
	}
	const float most = s.catalog.traffic.signal_cycle_max_s; // MBST vol. V 5.9: no green nor offset past the longest cycle
	SignalSettings set = to;
	set.offset = std::clamp(set.offset, 0.f, most);
	for (float &g : set.greens) g = std::clamp(g, 0.f, most);
	j_set_signal(s, node, set);
	r.ok = true;
	return r;
}

EditResult green_wave(CityState &s, int node) {
	EditResult r;
	if (!is_signal(s, node)) {
		r.error = "tool.failed";
		return r;
	}
	const auto plans = traffic::green_wave_plans(s, node);
	if (plans.empty()) {
		r.error = "junction.greenWaveNone";
		return r;
	}
	for (const auto &[n, set] : plans) j_set_signal(s, n, set);
	r.ok = true;
	return r;
}

} // namespace city
