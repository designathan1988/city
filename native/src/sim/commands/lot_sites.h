// Every building's lot graded to the street it faces (sim/terrain/lot_shape.h): its level the footway's top along its front less the
// lot's plate, the sidewalk in front of it graded with it, so the lot's covers (render/buildings/lot_emit.cpp, a plate over the
// ground) meet the footway's top and the floor (the highest ground + the plinth) stands at the street. Beside road_grade_sync.h, the
// one place the network and the ground are seen together, as the lots read the buildings, the street and the ground at once:
// settled after every edit (sync_grade), the city's generation and growth, and the load.
#pragma once

#include <vector>

namespace city {

struct CityState;

// The lots whose ground is due (the buildings that came, went or changed: BuildingStore::sites_due) and those facing a segment in
// `streets` (its heights moved, or it went), graded again under one held composition; what stands on that ground is marked.
void settle_sites(CityState &s, const std::vector<int> &streets);

} // namespace city
