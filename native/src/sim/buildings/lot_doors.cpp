// A lot's front door (lot_doors.h).
#include "sim/buildings/lot_doors.h"

#include <limits>

namespace city {

bool lot_door(const b40::Body &b, const b40::LotFrame &f, float off, float out, LotDoor &o) {
	float wall = std::numeric_limits<float>::infinity();
	for (const b40::Volume &v : b.volumes)
		if (v.base == 0 && b.entrance_x >= v.x && b.entrance_x <= v.x + v.w && v.y < wall) wall = v.y;
	if (wall == std::numeric_limits<float>::infinity()) return false;
	o.door = f.at(b.entrance_x, wall - off);
	o.foot = f.at(b.entrance_x, -out);
	return true;
}

} // namespace city
