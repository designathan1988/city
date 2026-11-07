// The trains of a line on its loop, and where it crosses the lanes of the roads at grade (transit_sim.h).
#pragma once

#include "sim/transit/transit_sim.h"

namespace city {
namespace transit {

void place_rails(CityState &s); // the loops, trains and crossings made from the plan
// The roads `segs` changed (added, changed or gone) and the plan did not: the level crossings made again of each train line one of them
// may cross or crossed; the loops and their trains stay as they are (they are the plan's alone).
void refresh_crossings(CityState &s, const std::vector<int> &segs);
void train_zones(CityState &s); // what the walkers keep off this tick
void step_train(CityState &s, RailLine &r, Train &t, float dt);
int cars_of(const CityState &s, const RailLine &r);

} // namespace transit
} // namespace city
