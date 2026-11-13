// The walls round somebody thrown: the volumes of the buildings near them and their lots' walls, fences, railings, hedges, sheds and
// stalls, from the bodies the simulation keeps (sim/buildings/b40/body_cache.h); and the street's pieces standing there, poles, posts,
// benches, bins, hydrants, plates, each part a box at its height (sim/traffic/street_volumes.h).
#pragma once

#include "render/agents/ragdoll.h"
#include "render/roads/road_look.h"
#include "sim/state/city_state.h"

#include <vector>

namespace city {

// The walls of the buildings standing within `reach` of (x, z), of their lots and of the street's pieces there, a piece's foot where
// the street's look sets it (street_props.h street_ground). The caller holds the map's lock.
std::vector<BodyWall> walls_about(const CityState &s, const StreetLook &look, float x, float z, float reach);

} // namespace city
