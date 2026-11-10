// What the traffic probe says of bodies (city_core_probe.cpp traffic_probe): walkers and vehicles standing in one another, walkers in
// the street's furniture, and what holds the vehicle that has stood longest when a body off the lanes does.
#pragma once

#include <godot_cpp/variant/dictionary.hpp>

namespace city {

struct CityState;

// Into `d`: people_overlaps, people_in_furniture, vehicle_overlaps; with `pairs`, the pairs of each (the deepest first).
void bodies_in_one_another(const CityState &s, bool pairs, godot::Dictionary &d);
// Into `d`, for the vehicle `who` that has stood longest: the body off the lanes in its path (stuck_off_body) or whose closure holds
// it (stuck_closed_by), and who stands where that one would step (stuck_blocker).
void bodies_holding(const CityState &s, int who, godot::Dictionary &d);

} // namespace city
