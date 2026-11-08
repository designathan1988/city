// The building render's shapes of the levels and the whole: the building cut open, the far mass, the rooms behind the panes, the body's tone (building_look.json shapes, read by building_look.cpp).
#pragma once

#include "core/shape_fields.h"

namespace city::b40 {

#define LOOK_SHAPE_INTERIOR_EMIT(X, XI, XD, XV, XVI) \
	XI(flight_steps) X(floor_lift) X(wall_half) X(wall_clear) X(wall_max) X(lower_dot) X(shaft_wall) \
	X(shaft_clear) X(lift_door_w) X(lift_door_share) X(door_h) X(cab_gap) X(cab_lift) X(cab_h)
struct InteriorEmitLook {
	SHAPE_DECLARE(LOOK_SHAPE_INTERIOR_EMIT)
};

#define LOOK_SHAPE_FAR_MASS(X, XI, XD, XV, XVI) \
	X(plinth_bury) X(mass_in_m)
struct FarMassLook {
	SHAPE_DECLARE(LOOK_SHAPE_FAR_MASS)
};

#define LOOK_SHAPE_PANE_ROOMS(X, XI, XD, XV, XVI) \
	X(most_deep) X(most_wide) X(inside) X(off_glass) X(least_deep) X(least_wide) X(nearest_piece) X(off_centre_k)
struct PaneRoomsLook {
	SHAPE_DECLARE(LOOK_SHAPE_PANE_ROOMS)
};

#define LOOK_SHAPE_BUILDING_BODIES(X, XI, XD, XV, XVI) \
	X(shade_base) X(shade_span)
struct BuildingBodiesLook {
	SHAPE_DECLARE(LOOK_SHAPE_BUILDING_BODIES)
};

} // namespace city::b40
