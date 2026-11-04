// What the Interior layer shows: which buildings are drawn cut open, at which floor, for a camera looking which way. A view of
// the city, not part of it: not saved, not undone. The renderer's jobs copy it with the building records; the command that
// sets it (sim/commands/interior_commands.cpp) marks the chunks to draw again.
#pragma once

#include "sim/buildings/building_store.h"
#include "sim/buildings/interior/cut.h"

#include <cmath>
#include <cstdint>

namespace city {

struct InteriorView {
	bool on = false;
	int target = -1; // one building (the one in the middle of the view, or double-clicked)
	b40::CutView cut; // the floor and the camera direction
	// target < 0 with a radius: every building whose lot is within `radius` of (cx, cz)
	float cx = 0, cz = 0, radius = 0;
	uint32_t revision = 0; // moves with every change

	// Whether building `id`, a record `b`, is drawn cut open.
	bool cuts(int id, const Building &b) const {
		if (!on) return false;
		if (target >= 0) return id == target;
		return radius > 0 && std::hypot(b.x - cx, b.z - cz) <= radius;
	}
};

} // namespace city
