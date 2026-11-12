// Opt-in pedestrian probes share a transient point index instead of pairing every slot with every other slot.
// Candidate order is irrelevant to census counts; exact distance and relationship predicates stay with each probe.
#pragma once

#include "core/bounds_index.h"
#include "sim/traffic/traffic_state.h"

#include <cmath>
#include <limits>

namespace city {

class ProbeNeighbors {
public:
	explicit ProbeNeighbors(const TrafficState &traffic) : traffic_(traffic) {
		for (size_t i = 0; i < traffic.peds.size(); i++) {
			const auto &p = traffic.peds[i];
			if (p.alive) index_.insert(int(i), {p.cur.x, p.cur.z, p.cur.x, p.cur.z});
		}
	}
	// fn(other) returns false to stop after an existential match; no remaining subtree is then visited.
	template <typename F> void each(int id, float radius, F &&fn) const {
		const auto &p = traffic_.peds[size_t(id)];
		const float reach = std::fabs(radius), infinity = std::numeric_limits<float>::infinity();
		const Bounds2 box{std::nextafter(p.cur.x - reach, -infinity), std::nextafter(p.cur.z - reach, -infinity),
			std::nextafter(p.cur.x + reach, infinity), std::nextafter(p.cur.z + reach, infinity)};
		bool visiting = true;
		index_.query_if([&](const Bounds2 &bounds) { return visiting && bounds.overlaps(box); }, [&](int other) {
			if (visiting && other != id) visiting = fn(other);
		});
	}
private:
	const TrafficState &traffic_;
	BoundsIndex index_;
};

} // namespace city
