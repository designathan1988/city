// The gardens of the lots (the Roadcraft 4.0's render/gardenPlace.ts, its behaviour): what a body's layout planted, its trees,
// shrubs, runs of hedge and flower beds, stood on the ground as plants of the trees' renderer. The body is the one the simulation
// keeps (sim/buildings/b40/body_cache.h), so a chunk built again, near or far, makes no body.
#pragma once

#include "render/trees/tree_place.h"
#include "sim/buildings/b40/record.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace city {

// What a view of the chunk draws of a garden: near, every plant; far, one card each, and no flower, a clump under a pixel there.
enum class GardenView : uint8_t { NEAR, FAR };

// The plants of the record's garden, in the world, on the ground under each. Every plant is sized by what its species' model draws
// (the kit's measure, render/trees/drawn_body.h), so it stands inside the room its element is: the room the lot's layout kept off
// the boundary, the walls and the other parts.
void place_garden(const CityView &s, const TreeLook &T, const TreeKit &kit, const Building &rec, GardenView view, std::vector<TreeInstance> &out);
// One plant element of a lot whose frame is `f`, as place_garden stands each of a record's; `draws`: its stream of draws.
void place_plant(const CityView &s, const TreeLook &T, const TreeKit &kit, const b40::LotFrame &f, const b40::LotElement &p, uint32_t draws, GardenView view,
		std::vector<TreeInstance> &out);
// The room a plant element is, as the frame it is laid in and the element in that frame: the lot's own, or, for an element laid at
// an angle (a hedge traced across the lot), the lot's frame turned about the element's middle, the element at its origin (as the
// lot's parts are drawn: render/buildings/lot_emit.cpp).
struct PlantRoom {
	b40::LotFrame f;
	b40::LotElement p;
};
PlantRoom plant_room(const b40::LotFrame &f, const b40::LotElement &p);
// Whether the element is a plant of the gardens: a tree, a shrub, a run of hedge or a flower bed.
bool planted_kind(b40::Element kind);
// Each plant element of the record's lot with its stream of draws, in the body's order: what place_garden stands.
void each_garden_plant(const CityView &s, const Building &rec, const std::function<void(const b40::LotElement &, uint32_t)> &fn);

} // namespace city
