// The trains of a line on its loop, and where it crosses the lanes of the roads at grade (transit_sim.h).
#pragma once

#include "sim/transit/transit_sim.h"

namespace city {
namespace transit {

void place_rails(CityState &s); // the loops, trains and crossings made from the plan
void step_train(CityState &s, RailLine &r, Train &t, float dt);
int cars_of(const CityState &s, const RailLine &r);

} // namespace transit
} // namespace city
