// The one rule of the lights for vehicles (light_rule.cpp), for admission, commitments and the stops ahead. A vehicle has two
// distances to a line and the rule keeps them apart: the metres to the line itself, which it must not pass against the light, and
// the room to where it stops for it, short of the line by the stop margin. Whether it can still be kept behind its line is asked
// of the first; where it stops, of the second. The rule: whether it can stop comfortably (its class's b) behind a line; how far it
// needs to stop as the intergreen is timed (CONTRAN MBST vol. V 5.7.1: reaction, then signal_decel); whether a light gone against
// it stops it (the amber's rule; a red is an amber with nothing left); whether the light of approach `ds` takes back its
// commitment or holds it, `line_m` before its line, with connector `ci` past the line (-1: none from its lane), an amber's
// decision held until green; and the stop each vehicle sees down its route.
#pragma once

#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

// Where a vehicle stops for a line `line_m` ahead, in metres from its nose: short of the line by the stop margin where there is
// room for the margin, where it is within the margin (a vehicle standing there has stopped behind its line), none once its nose is
// past the line (negative).
float stop_room(const TrafficParams &p, float line_m);
// Whether it can be brought to rest behind a line `line_m` ahead braking comfortably.
bool stop_is_comfortable(const CityState &s, const Vehicle &v, float line_m);
float amber_reach(const TrafficParams &p, float v);
// A vehicle its light went against.
struct AmberApproach {
	float v = 0, line_m = 0; // its speed and the metres to its line
	float top = 0; // the speed it may take the line at
	float brake = 0, brake_hard = 0; // m/s²: its comfortable and its emergency braking
	float left = 0; // seconds of amber left; none: the light is red
};
bool amber_stops(const TrafficParams &p, const AmberApproach &a);
// Whether the light of `ds` takes back the commitment of a vehicle `line_m` before its line: at an amber by the rule above or by
// its decision held, at a red by the same rule with no amber left (whoever its emergency brake still keeps behind its line).
bool light_recalls(const CityState &s, const Vehicle &v, int ds, float line_m, int ci);
// Whether the light of `ds` holds a vehicle not committed to its junction: a red always, an amber as it takes a commitment back.
bool light_stops(const CityState &s, const Vehicle &v, int ds, float line_m, int ci);
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
