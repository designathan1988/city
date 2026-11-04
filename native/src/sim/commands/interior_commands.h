// The Interior layer's command: which buildings are cut open, and at which floor. Not an edit of the city (no undo, no
// save): it sets CityState::interior and marks the chunks of the buildings it opens or closes to be drawn again.
#pragma once

#include "sim/state/city_state.h"

namespace city {

struct InteriorRequest {
	bool on = false;
	int target = -1; // one building; or -1 with `radius`: every building within it of (cx, cz)
	int level = 0;
	int dir = 0; // the way the camera looks: a step of b40::CUT_DIRECTIONS
	float cx = 0, cz = 0, radius = 0;
};

// map_mutex held exclusively. Returns "" or the strings.csv key of the problem. A view turned off or a building that
// is not there leaves the layer off. The floor is limited to the building's (the highest one of the opened, for a radius).
const char *set_interior_view(CityState &s, const InteriorRequest &r);

// The floors of a building (its body's highest floor + 1); 0 when it is not there.
int interior_floors(const CityState &s, int id);

// Where a building's body stands: the middle of its volumes' bounding rectangle in the world (XZ), that rectangle's
// longest side, the body's floors and how high above its ground floor `level` is. The lot's middle is not it (a long lot
// keeps its house at the front): this is what the camera frames.
struct InteriorFocus {
	V2 at;
	float extent = 0;
	int floors = 0;
	float floor_height = 0;
};
InteriorFocus interior_focus(const CityState &s, int id, int level = 0);

} // namespace city
