// The trains of a line on its loop, and where it crosses the lanes of the roads at grade (transit_sim.h).
#pragma once

#include "sim/transit/transit_sim.h"

namespace city {
namespace transit {

void place_rails(CityState &s, bool rebuild_loops = true); // road-only changes refresh crossings, retaining loops/trains
void train_zones(CityState &s); // what the walkers keep off this tick
void step_train(CityState &s, RailLine &r, Train &t, float dt);
int cars_of(const CityState &s, const RailLine &r);

} // namespace transit
} // namespace city
