// A body off the lanes (a lorry swinging across a street, a car going into or out of a lot) closes the stretches of lane it sweeps:
// the traffic stops short of them until it has passed. And what the rest of the traffic asks of the lorries.
#pragma once

#include "sim/traffic/traffic_state.h"

#include <vector>

namespace city {

struct CityState;

namespace traffic {

// The stretches of every lane a body (half length and width) sweeps at `poses`, `margin` beyond either way, closed for `owner`.
void sweep_body(const CityState &s, float half_len, float half_w, const std::vector<OffPose> &poses, float margin, int owner, std::vector<LaneClosure> &out);
bool on_lanes(const CityState &s, float half_len, float half_w, const OffPose &q); // a body at `q` stands on some lane
// A lorry's swing over axle poses `from`..`to` (every other one), its body the truck's.
void sweep_lanes(const CityState &s, const std::vector<AxlePose> &poses, int from, int to, int owner, std::vector<LaneClosure> &out);
bool lane_clear(const CityState &s, const LaneClosure &c); // nobody on it or too near to stop short of it, no other owner holding it
void open_lanes(TrafficState &t, int owner);
float closure_ahead(const TrafficState &t, int key, float s); // metres to the closed stretch ahead on a lane, -1 for none
// Some stretch of lane `key` between `from` and `to` (metres along it) is held by a body off the lanes: nothing is put there.
bool stretch_held(const TrafficState &t, int key, float from, float to);

void step_lorries(CityState &s, float dt); // lorry_traffic.cpp
void lorry_gone(CityState &s, int id); // its vehicle leaves the traffic
// The one setter of Lorry::phase (lorry_phase.cpp LORRY_PHASES): false, the phase kept, for a move a delivery never makes.
bool enter_lorry_phase(Lorry &l, LorryPhase next);

} // namespace traffic
} // namespace city
