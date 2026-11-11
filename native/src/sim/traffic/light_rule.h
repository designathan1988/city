// The one rule of the lights for vehicles (light_rule.cpp), for admission, commitments and the stops ahead: the room a vehicle
// has to stop behind its line; whether it can stop comfortably (its class's b) within `stop_m`; how far it needs to stop as the
// intergreen is timed (CONTRAN MBST vol. V 5.7.1: reaction, then signal_decel); whether it stops at the amber of approach `ds`,
// `stop_m` ahead, with connector `ci` past the line (-1: none from its lane); whether the light of `ds` holds it (red always; amber
// by that rule, a decision held until green); and the stop each vehicle sees down its route.
#pragma once

#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

// The room to stop behind a line `line_m` ahead: short of it by the stop margin where there is room for the margin, at the line
// itself within the margin (a vehicle standing there has stopped behind it), none once its nose is past the line (negative).
float stop_room(const TrafficParams &p, float line_m);
bool stop_is_comfortable(const CityState &s, const Vehicle &v, float stop_m);
float amber_reach(const TrafficParams &p, float v);
struct AmberApproach {
	float v = 0, stop_m = 0; // its speed and the metres to where it stops
	float top = 0; // the speed it may take the line at
	float brake = 0, brake_hard = 0; // m/s²: its comfortable and its emergency braking
	float left = 0; // seconds of amber left
};
bool amber_stops(const TrafficParams &p, const AmberApproach &a);
bool stops_at_amber(const CityState &s, const Vehicle &v, int ds, float stop_m, int ci);
bool light_stops(const CityState &s, const Vehicle &v, int ds, float stop_m, int ci);
// Where a vehicle stops for the lights down its route: the first stop line within its horizon whose light holds it.
struct LightStop {
	float gap = NO_LEADER; // metres to where it stops
	bool here = false; // that stop is on the lane it is on: it does not cross its junction now
	int amber_hold = -1; // its amber decision from now on (Vehicle::amber_hold)
};
// The walk starts at route step `ri` on lane `lane`, whose start is `start` metres ahead (negative: it is on it); the
// last step's junction is never crossed.
LightStop lights_ahead(const CityState &s, const Vehicle &v, int ri, int lane, float start);

} // namespace traffic
} // namespace city
