// The walls round somebody thrown: the volumes of the buildings near them, from the bodies the simulation keeps
// (sim/buildings/b40/body_cache.h).
#pragma once

#include "render/agents/ragdoll.h"
#include "sim/state/city_state.h"

#include <vector>

namespace city {

// The walls of the buildings standing within `reach` of (x, z). The caller holds the map's lock.
std::vector<BodyWall> walls_near(const CityState &s, float x, float z, float reach);

} // namespace city
