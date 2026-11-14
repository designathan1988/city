// What the land beside a street puts on it and takes off it (street_flows.h): each directed segment's trips starting a second and
// the share of its traffic that ends its trip there, from the demand's own density (demand.cpp demand_for) and the zones at its
// sides at this hour (side_weight). One owner of the demand: the same numbers fill the window and the far streets. A trip's mean
// length is trip_decay_m, so a street ends length / mean of what runs through it and starts as much as holds its density in free
// flow.
#include "sim/traffic/street_flows.h"

#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

float flow_density(const CityState &s, int) { return float(demand_for(s, 1.0)); }

void flow_rates(CityState &s, int from, int to) {
	StreetFlows &f = s.traffic.flows;
	const TrafficParams &p = s.catalog.traffic;
	const LaneGraph &g = s.traffic.g;
	const float per_lane_m = float(demand_for(s, 1.0));
	for (int ds = from; ds < to; ds++) {
		const size_t k = size_t(ds);
		if (!f.live[k]) continue;
		const DirSeg &d = g.ds[k];
		f.start_rate[k] = per_lane_m * d.len * float(d.lanes) * d.speed / p.trip_decay_m * side_weight(s, d.seg, true);
		f.end_share[k] = std::min(1.f, d.len / p.trip_decay_m) * side_weight(s, d.seg, false);
	}
}

} // namespace traffic
} // namespace city
