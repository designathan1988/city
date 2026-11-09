// A lane change as a path, not a jump (the Roadcraft lab's sim/params.ts laneChangeLength, laneChangeOffset, laneChangeSlope and
// sim/vehicles/integrate.ts applyLaneChange): the vehicle moves onto the lane it changes to with the sideways distance still to cover,
// covered over a length of road on a quintic eased at both ends; while it is under way its body still stands in the lane it left (its
// shadow there), which that lane's traffic follows as it follows a leader, until the body has left that lane. Numbers: traffic.json lc_*.
#pragma once

#include "sim/catalog/traffic_params.h"

namespace city {
struct CityState;
struct Vehicle;
struct TrafficState;

namespace traffic {

// The road a change of `across` metres sideways is driven over at `speed` by a body `length` long.
float shift_length(const TrafficParams &p, float across, float speed, float length);
// The offset still to cover after `travelled` of a change of `length` that started `start` across (0 once done).
float shift_offset(float start, float travelled, float length);
// The sideways offset gained per metre of road there: the tangent of the body's heading off its lane.
float shift_slope(float start, float travelled, float length);
// How far the body reaches sideways from its middle line at that heading.
float side_reach(const TrafficParams &p, const Vehicle &v);

// Vehicle `id` just moved from lane `from_key` to its lane, `across` metres from where it was: the path begins and its shadow stays
// on `from_key` until the offset is under half of `across` past its side reach.
void shift_start(CityState &s, int id, int from_key, float across);
// The path advanced by the `ds` metres the vehicle drove; the shadow let go once the body has left the old lane.
void shift_step(CityState &s, int id, float ds);
// The vehicle leaves the lanes (onto a connector, the far model, or the map): its shadow goes, and its path ends.
void shift_end(TrafficState &t, int id);

// Working across to lane `to` of its segment, the route's next road reached only from there (laneChange.ts mandatory, positioningSpeedCap):
// whether the changes still fit before the lane ends at a crawl, and the fastest speed at which they fit (never under a crawl).
bool position_fits(const CityState &s, const Vehicle &v, int to);
float position_speed(const CityState &s, const Vehicle &v, int to);
// Vehicle `id`'s lane at its route's next junction (`line` metres to its stop line): when the lane does not lead to the next road, the
// changes to one that does, easing `v0` to fit them and holding it `hold` metres short of the line; past room for them, a route from
// where this lane leads (route_from_lane). False when it has no way on at all (it leaves the scene).
bool way_on(CityState &s, int id, float line, float &v0, float &hold);

} // namespace traffic
} // namespace city
