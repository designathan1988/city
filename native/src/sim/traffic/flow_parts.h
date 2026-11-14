// What the far traffic's files share (street_flows.cpp the streets' tables and curves, flow_step.cpp the step, flow_nodes.cpp the
// nodes): how a street's curves are read back in time, and the parts of the step each file owns.
#pragma once

#include "sim/traffic/traffic_system.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace city {
namespace traffic {

constexpr float FLOW_TINY = 1e-6f; // a flow under this is none
constexpr float FLOW_SLOWEST_MS = 0.5f; // a street's free speed in the model, at least

inline float flow_wave_ms(const TrafficParams &p) { return p.flow_wave_kmh / KMH_PER_MS; }

// The cell of a ring of `n` that holds the count `back` steps before step `now` (-1: the step after).
inline int flow_cell(uint64_t now, int n, int back) { return int(uint64_t(int64_t(now) + int64_t(n) - int64_t(back)) % uint64_t(n)); }

// The count a ring held `back` steps before `now`: between the two steps either side of a fraction.
inline float flow_past(const StreetFlows &f, int at, int n, uint64_t now, float back) {
	const float b = std::min(float(n - 1), std::max(0.f, back));
	const int whole = int(b), next = std::min(n - 1, whole + 1);
	const float older = f.curve[size_t(at + flow_cell(now, n, next))], newer = f.curve[size_t(at + flow_cell(now, n, whole))];
	return newer + (older - newer) * (b - float(whole));
}

// The whole vehicles drawn of `n` on street `ds`: the whole part, and one more when the fraction passes the street's own share, a
// fixed one spread evenly over the streets (Knuth's multiplicative hash of its index). Most streets carry under one vehicle:
// rounded, none of them would show a car; so, what is drawn of them all adds up to what they carry, and no car comes and goes
// with a draw of chance.
constexpr uint32_t FLOW_SHARE_STEP = 2654435761u;
constexpr int FLOW_SHARE_BITS = 24, FLOW_SHARE_DROPPED = 8;
inline float flow_shown(int ds, float n) {
	const float own = float((uint32_t(ds) * FLOW_SHARE_STEP) >> FLOW_SHARE_DROPPED) / float(1u << FLOW_SHARE_BITS);
	return std::floor(std::max(0.f, n) + own);
}

// The vehicles on a street outside the window as of step `now`, and of them those standing in its queue (they had the time to
// reach its end and did not leave) (street_flows.cpp).
float flow_on(const StreetFlows &f, int ds, uint64_t now);
float flow_queue(const CityState &s, int ds, uint64_t now);
// `n` vehicles as whole ones: the fraction is one more with that chance, so many small streets keep their sum (street_flows.cpp).
float flow_whole(TrafficState &t, float n);
// One node of the step: what its approaches send is passed to the streets they turn onto (flow_nodes.cpp).
void flow_node(CityState &s, int node);

} // namespace traffic
} // namespace city
