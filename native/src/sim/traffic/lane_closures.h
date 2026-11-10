// A body off the lanes closes the stretches of lane its way sweeps, on every segment near it, before it moves over them and only
// when nobody is on them or too near to stop (take_lanes): the traffic stops short of them until it has passed. A lorry takes its
// street part before it leaves its lane and keeps it till its whole body stands in the lot; a lot car takes its whole way in
// before it leaves its lane, and on its way out the street part before its body first comes over a lane.
// And what the rest of the traffic asks of the lorries.
#pragma once

#include "sim/traffic/manoeuvre.h"
#include "sim/traffic/traffic_state.h"

#include <vector>

namespace city {

struct CityState;

namespace traffic {

void sweep_lanes(const CityState &s, const std::vector<AxlePose> &poses, int from, int to, int owner, std::vector<LaneClosure> &out);
// What a body of `type` sweeps along `path` from `from` to `to` metres, on every segment near it.
void sweep_path(const CityState &s, const Manoeuvre &path, float from, float to, VehicleType type, int owner, std::vector<LaneClosure> &out);
// Whether a body there stands over a lane.
bool lanes_under_body(const CityState &s, const OffPose &body, VehicleType type);
// The stretches taken as closures when every one is clear (lane_clear), else nothing: whether they were.
bool take_lanes(CityState &s, const std::vector<LaneClosure> &stretches);
bool lane_clear(const CityState &s, const LaneClosure &c); // nobody on it or too near to stop short of it
void open_lanes(TrafficState &t, int owner);
float closure_ahead(const TrafficState &t, int key, float s); // metres from arc s (negative before the lane) to the closed stretch starting ahead of it, -1 for none

void step_lorries(CityState &s, float dt); // lorry_traffic.cpp
void lorry_gone(CityState &s, int id); // its vehicle leaves the traffic

} // namespace traffic
} // namespace city
