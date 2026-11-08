// The building render's shapes of the levels: the building cut open, the far mass (building_look.json shapes, read by building_look.cpp).
#pragma once

#include "render/buildings/look_shape_fields.h"

namespace city::b40 {

#define LOOK_SHAPE_INTERIOR_EMIT(X, XI, XD, XV, XVI) \
	XI(flight_steps) X(floor_lift) X(wall_half) X(wall_clear) X(wall_max) X(lower_dot) X(shaft_wall) \
	X(shaft_clear) X(lift_door_w) X(lift_door_share) X(door_h) X(cab_gap) X(cab_lift) X(cab_h)
struct InteriorEmitLook {
	LOOK_SHAPE_INTERIOR_EMIT(LOOK_SHAPE_FLOAT, LOOK_SHAPE_COUNT, LOOK_SHAPE_DOUBLE, LOOK_SHAPE_FLOATS, LOOK_SHAPE_COUNTS)
};

#define LOOK_SHAPE_FAR_MASS(X, XI, XD, XV, XVI) \
	X(plinth_bury) X(mass_in_m)
struct FarMassLook {
	LOOK_SHAPE_FAR_MASS(LOOK_SHAPE_FLOAT, LOOK_SHAPE_COUNT, LOOK_SHAPE_DOUBLE, LOOK_SHAPE_FLOATS, LOOK_SHAPE_COUNTS)
};

} // namespace city::b40
