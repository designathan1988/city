// A lot's front door (lot_doors.h).
#include "sim/buildings/lot_doors.h"

#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr float IN_LINE_M = 0.05f; // a gate this near the door's line stands in line with it
constexpr float HALF = 0.5f;
} // namespace

bool lot_door(const b40::Body &b, const b40::LotFrame &f, float off, float out, LotDoor &o) {
	float wall = std::numeric_limits<float>::infinity();
	for (const b40::Volume &v : b.volumes)
		if (v.base == 0 && b.entrance_x >= v.x && b.entrance_x <= v.x + v.w && v.y < wall) wall = v.y;
	if (wall == std::numeric_limits<float>::infinity()) return false;
	// The person's gate of the front zone the way goes out by: the one nearest the door's line (a lot with a front zone has one).
	float gate_x = b.entrance_x, nearest = std::numeric_limits<float>::infinity();
	for (const b40::LotElement &e : b.elements)
		if (e.kind == b40::EL_GATE && e.y < wall && std::fabs(e.x - b.entrance_x) < nearest) {
			nearest = std::fabs(e.x - b.entrance_x);
			gate_x = e.x;
		}
	o.door = f.at(b.entrance_x, wall - off);
	o.bends = 0;
	if (std::fabs(gate_x - b.entrance_x) > IN_LINE_M) {
		const float link = wall * HALF;
		o.bend[o.bends++] = f.at(b.entrance_x, link);
		o.bend[o.bends++] = f.at(gate_x, link);
	}
	o.foot = f.at(gate_x, -out);
	return true;
}

} // namespace city
