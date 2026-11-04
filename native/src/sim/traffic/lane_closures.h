// A lorry swinging across a street closes the stretches of lane it sweeps: the traffic stops short of them until it has passed.
// And what the rest of the traffic asks of the lorries.
#pragma once

#include "sim/traffic/traffic_state.h"

#include <vector>

namespace city {

struct CityState;

namespace traffic {

void sweep_lanes(const CityState &s, const std::vector<AxlePose> &poses, int from, int to, int seg, int owner, std::vector<LaneClosure> &out);
bool lane_clear(const CityState &s, const LaneClosure &c); // nobody on it or too near to stop short of it
void open_lanes(TrafficState &t, int owner);
float closure_ahead(const TrafficState &t, int key, float s); // metres to the closed stretch ahead on a lane, -1 for none

void step_lorries(CityState &s, float dt); // lorry_traffic.cpp
void lorry_gone(CityState &s, int id); // its vehicle leaves the traffic

} // namespace traffic
} // namespace city
