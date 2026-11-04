// A building record's body and its lot's frame in the world: the one place the render (BuildingBodies) and the
// simulation (sim/lots/lot_access, the lots' trees) get them from, so both read the same lot.
#pragma once

#include "core/math.h"
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/building_store.h"

#include <vector>

namespace city::b40 {

// The lot's frame on the ground (XZ): `o` the middle of the lot's street front, `e0` along the front (lot x), `e1` away
// from the street (lot y).
struct LotFrame {
	V2 o, e0, e1;
	V2 at(float x, float y) const { return o + e0 * x + e1 * y; }
	V2 dir(float x, float y) const { return e0 * x + e1 * y; }
};
LotFrame lot_frame(const Building &rec);

// The record's body: the version the player edited, or a civic building, an open lot or a grown building made from its seed.
void record_body(const Data &data, const Building &rec, Body &b);

// A tree the lot's layout planted, in the world: where, its crown's reach and its height (for the trees' renderer).
struct LotTree {
	V2 at;
	float crown = 0, height = 0;
};
void lot_trees(const Body &b, const LotFrame &f, std::vector<LotTree> &out);

} // namespace city::b40
