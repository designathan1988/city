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
// NEAR and FAR as drawn; FELLED only what a clearing takes (its trees and shrubs, never a hedge or a bed).
enum class GardenView : uint8_t { NEAR, FAR, FELLED };

// The plants of the record's garden, in the world, on the ground under each; its trees and shrubs in a clearing (the player's cut, a
// blow's) are not there, but for clearing `standing` (PlantedTrees::cleared: those it took, as they stood).
void place_garden(const CityState &s, const TreeLook &T, const Building &rec, GardenView view, std::vector<TreeInstance> &out, int standing = -1);

} // namespace city
