// The walls round somebody thrown: the volumes of the buildings near them and their lots' walls, fences, railings, hedges, sheds and
// stalls, from the bodies the simulation keeps (sim/buildings/b40/body_cache.h).
#pragma once

#include "render/agents/ragdoll.h"
#include "sim/state/city_state.h"

#include <vector>

namespace city {

// The walls of the buildings standing within `reach` of (x, z), and of their lots. The caller holds the map's lock.
std::vector<BodyWall> walls_near(const CityState &s, float x, float z, float reach);

} // namespace city
