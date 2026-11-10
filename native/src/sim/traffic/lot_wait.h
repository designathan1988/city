// What holds a car of the lots off the lanes (lot_traffic.cpp), for the probes that say why a vehicle stands.
#pragma once

namespace city {

struct CityState;

namespace traffic {

// Another has its lot's way or its kerb, a body where it would step, the lanes its way sweeps not clear; none.
enum LotWait { LOT_WAIT_NONE, LOT_WAIT_ACCESS, LOT_WAIT_BODY, LOT_WAIT_LANES };
int lot_car_wait(const CityState &s, int id);
int lot_car_blocker(const CityState &s, int id); // the vehicle whose body stands where the lot car would step, -1 none

} // namespace traffic
} // namespace city
