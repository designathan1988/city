// The ground covers of a placed lot (lawns, paving, asphalt, courts and their lines) as they lie on its ground: their one owner, which
// lot_emit.cpp draws and the probe lot_covers (api/city_core_lot_cover_probe.cpp) holds against one another. The lot's ground is read
// once, on a grid of the lot's frame whose nodes stand on its multiples, every cell split along the same diagonal; every cover is that
// one triangulated ground clipped by its outline (sim/buildings/b40/element_shape.h: its rectangle turned with its element, or a round
// cover's ellipse), so two covers over one another are parallel everywhere and never cross. They lie in one order, by thickness and then as the body lays them: each over the plate by half its thickness and a step
// for every cover before it, so none shares another's depth.
#pragma once

#include "render/buildings/facade.h"

#include <functional>

namespace city::b40 {

constexpr int COVER_POLY_MOST = 8; // a piece's corners at the most: what is laid with more is handed as pieces fanned from one corner
// A piece's element and rank when it is the lot's own lawn: a lot that is no rectangle (Body::lot_ring) is lawn to its outline under
// every cover its plan lays, and every cover of it is held to that outline, so nothing of a lot is drawn past its own line.
constexpr int LOT_LAWN = -1;

// One piece of a cover: its element's index in the body, the cover's place in the order (the higher lies over), and its corners in
// the world, a convex polygon facing up. The pieces of one polygon share their corners, so they share their edges.
struct CoverPiece {
	int element = -1, rank = 0;
	int count = 0;
	godot::Vector3 p[COVER_POLY_MOST];
};

// Every cover of the placed body, piece by piece.
void lot_covers(const Placed &pl, const std::function<void(const CoverPiece &)> &out);

} // namespace city::b40
