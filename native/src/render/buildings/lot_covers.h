// The ground covers of a placed lot (lawns, paving, asphalt, courts and their lines) as they lie on its ground: their one owner, which
// lot_emit.cpp draws and the probe lot_covers (api/city_core_lot_cover_probe.cpp) holds against one another. The lot's ground is read
// once, on a grid of the lot's frame whose nodes stand on its multiples, every cell split along the same diagonal; every cover is that
// one triangulated ground clipped by its outline (turned with its element), so two covers over one another are parallel everywhere and
// never cross. They lie in one order, by thickness and then as the body lays them: each over the plate by half its thickness and a step
// for every cover before it, so none shares another's depth.
#pragma once

#include "render/buildings/facade.h"

#include <functional>

namespace city::b40 {

constexpr int COVER_POLY_MOST = 8; // a triangle clipped by a cover's outline of four sides keeps at most seven corners

// One piece of a cover: its element's index in the body, the cover's place in the order (the higher lies over), and its corners in
// the world, a convex polygon facing up.
struct CoverPiece {
	int element = -1, rank = 0;
	int count = 0;
	godot::Vector3 p[COVER_POLY_MOST];
};

// Every cover of the placed body, piece by piece.
void lot_covers(const Placed &pl, const std::function<void(const CoverPiece &)> &out);

} // namespace city::b40
