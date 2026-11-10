// The gardens of the lots (the Roadcraft 4.0's render/gardenPlace.ts, its behaviour): what a body's layout planted, its trees,
// shrubs, runs of hedge and flower beds, stood on the ground as plants of the trees' renderer. The body is the one the simulation
// keeps (sim/buildings/b40/body_cache.h), so a chunk built again, near or far, makes no body.
#pragma once

#include "render/trees/tree_place.h"
#include "sim/buildings/b40/record.h"

#include <cstdint>
#include <vector>

namespace city {

// What a view of the chunk draws of a garden: near, every plant; far, one card each, and no flower, a clump under a pixel there.
enum class GardenView : uint8_t { NEAR, FAR };

// The plants of the record's garden, in the world, on the ground under each.
void place_garden(const CityView &s, const TreeLook &T, const Building &rec, GardenView view, std::vector<TreeInstance> &out);

} // namespace city
