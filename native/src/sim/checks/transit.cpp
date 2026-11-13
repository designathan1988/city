// The bus stops' invariants. transit.stop_road_live: every bus stop is on no road (seg -1, inert) or on a live one; a stop left on a
// road gone would be served by the next road that takes the id, anywhere. transit.stop_on_its_road: a stop on a road stands by it,
// within its profile's half plus the stop's reach of its curve (transit_stop_by_road); a stop on a split road's first half, or on a
// reused id far away, does not. j_remove_segment leaves a stop on no road and a road laid again carries it (sim/commands/road_refs.h).
// Every bus stop in both passes, whole or scoped: the stops are the player's, few. A record: the stop's id (TransitStop::id). Cost
// O(bus stops), one closest point on a curve each; nothing allocated.
#include "sim/checks/records.h"

#include "sim/state/city_state.h"
#include "sim/transit/transit_ground.h"

namespace city::checks {

namespace {
constexpr const char *FEATURE = "transit";
constexpr const char *ROAD_LIVE = "stop_road_live";
constexpr const char *ON_ITS_ROAD = "stop_on_its_road";
} // namespace

void check_transit(const CityState &s, const CheckScope &, Broken &broken) {
	const RoadNetwork &net = s.roads;
	for (const TransitStop &q : s.transit.stops) {
		if (q.mode != TransitMode::BUS || q.seg < 0) continue;
		if (size_t(q.seg) >= net.segs.size() || !net.segs[size_t(q.seg)].alive) {
			broken.add(FEATURE, ROAD_LIVE, q.id);
			continue;
		}
		double dist = 0;
		if (!transit_stop_by_road(s, q.at, q.seg, dist)) broken.add(FEATURE, ON_ITS_ROAD, q.id);
	}
}

} // namespace city::checks
