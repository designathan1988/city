// A lot's front door and the way on foot to its street: the entrance lies in the front wall, the lot keeps the way from it
// straight to the gate free (b40/lot_ground.cpp keep_clear) and the person's gate stands in line with it
// (b40/lot_boundary.cpp), so the way is one straight line. World coordinates (XZ).
#pragma once

#include "core/math.h"
#include "sim/buildings/b40/record.h"

namespace city {

struct LotDoor {
	V2 door; // where a walker stands, `off` before the wall
	V2 foot; // on the footway, `out` beyond the lot's front
};

// False when no wall stands behind the entrance (an open lot).
bool lot_door(const b40::Body &b, const b40::LotFrame &f, float off, float out, LotDoor &o);

} // namespace city
