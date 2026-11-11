// A lot's front door and the way on foot to its street: the entrance lies in the front wall, the lot keeps the way from it to the
// person's gate free (b40/lot_ground.cpp keep_clear, b40/lot_layout.h people_way) and the gate stands in line with it (b40/lot_boundary.cpp),
// so the way is one straight line; where the gate stands off that line (moved off a crosswalk's landing, b40 LotEdges) the way bends
// along the middle of the front zone, half the door's wall in. World coordinates (XZ).
#pragma once

#include "core/math.h"
#include "sim/buildings/b40/record.h"

namespace city {

struct LotDoor {
	static constexpr int MAX_BENDS = 2;
	V2 door; // where a walker stands, `off` before the wall
	V2 bend[MAX_BENDS]; // the way's corners from the door out: in line with the door, then with the gate
	int bends = 0;
	V2 foot; // on the footway, `out` beyond the lot's front, in line with the gate
};

// False when no wall stands behind the entrance (an open lot).
bool lot_door(const b40::Body &b, const b40::LotFrame &f, float off, float out, LotDoor &o);

} // namespace city
