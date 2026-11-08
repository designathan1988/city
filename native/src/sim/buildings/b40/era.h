// The era a lot was built in (old, middle, new: buildings.json era_affinity): its neighbourhood's, a Voronoi cell of the plan
// (buildings.json eras.neighbourhood_m between its jittered seeds), since a town is built in waves a plan unit at a time (Conzen
// 1960, Alnwick: a study in town-plan analysis); now and then another (eras.infill_share), a building of a later or earlier
// wave among them. A function of the lot's place and seed only, so the body's key (record.h body_key) can name it.
#pragma once

#include "sim/buildings/b40/data.h"

#include <cstdint>

namespace city::b40 {

int era_at(const Data &data, float x, float z, uint32_t seed);

} // namespace city::b40
