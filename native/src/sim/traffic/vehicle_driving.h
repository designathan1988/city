// How each near vehicle is driven, on top of CityState: the driver behind the wheel and the bends it takes (vehicle_speed.cpp), the
// lane change as a path (lane_change_path.cpp), what it signals and how its wheels sit (vehicle_signals.cpp). Declared apart from
// traffic_system.h, which includes it: the vehicle's own model, by its own files.
#pragma once

#include "sim/state/city_state.h"

namespace city {
namespace traffic {

// vehicle_speed.cpp: the driver behind the wheel, drawn when the vehicle is made; the class's pedals, headway and gap as that driver
// works them; the most it takes the bends of connector `c` at with its front `front` metres along it (negative: before it), braked
// for in advance (NO_LEADER: none in reach).
constexpr int DRIVER_SEED_SHIFT = 32; // a driver's stream: the traffic seed in the high word, the vehicle and tick's hash below
void draw_driver(const TrafficParams &p, Vehicle &v, Rng &rng); // its temper and speed drift (the drift's phase is new_vehicle's)
VehicleClass driver_of(const CityState &s, const Vehicle &v);
float curve_cap(const CityState &s, const Vehicle &v, const Conn &c, float front);
float conn_top(const CityState &s, const Vehicle &v, const Conn &c); // its wish on connector `c` before the bends: its exit's limit
float idm_accel(const VehicleClass &c, float v, float v0, float gap, float vl);

// lane_change_path.cpp: a change begun with `offset` metres still to cover sideways; driven `metres` on; the offset left at
// `behind` metres back along the path (0: the front); the fastest a vehicle that must still reach `target_lane` before its line,
// `to_line` ahead, drives so the changes fit (NO_LEADER: no need to ease off).
void start_lane_change(const CityState &s, Vehicle &v, float offset);
void drive_lane_change(Vehicle &v, float metres);
float lane_change_offset(const Vehicle &v, float behind);
float positioning_cap(const CityState &s, const Vehicle &v, int target_lane, float to_line);

// vehicle_signals.cpp: the side a vehicle's indicators flash on now (a lane change, the lane its turn needs, the turn ahead).
AgentSignal indicator_side(const CityState &s, const Vehicle &v);
// The bend of the path under its front, 1/m, positive to its left: its lane's or connector's, and the lane change it drives (the lab's
// vehicleSignals.ts pathCurvature).
float path_curvature(const CityState &s, const Vehicle &v);

// The fastest `v` goes `dist` before its lane's end on the way onto `c`: the bends of `c` braked for in advance (NO_LEADER: none in reach).
float turn_limit(const CityState &s, const Vehicle &v, const Conn &c, float dist);

} // namespace traffic
} // namespace city
