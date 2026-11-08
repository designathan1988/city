// The traffic agents' shapes of the passenger car: its lofted body, its ends and parts, its style's shape (agents.json "shapes", read by agent_renderer.cpp).
#pragma once

#include "core/shape_fields.h"

namespace city {

#define AGENT_SHAPE_CAR_BODY(X, XI, XD, XV, XVI) \
	X(bed_lip_m) X(panel_in_m)
struct CarBodyShape {
	SHAPE_DECLARE(AGENT_SHAPE_CAR_BODY)
};

#define AGENT_SHAPE_CAR_PARTS(X, XI, XD, XV, XVI) \
	X(lamp_depth_m) X(lamp_sink_m) X(indicator_sink_m) X(indicator_drop_m) X(indicator_rise_m) X(scan_step_m) \
	X(scan_reach_m) X(nose_back_m) X(tail_back_m) X(grille_rise_m) X(grille_sink_m) X(grille_low) X(bumper_share) \
	X(plate_thick_m) X(plate_proud_m) X(plate_front) X(plate_rear) X(handle_sink_m) X(handle_drop_m) \
	X(handle_reach_m) X(handle_share) X(handle_ahead_m) X(root_in_m) X(root_drop_m) X(tip_back_m) X(tip_out_m) \
	X(housing_back_m) X(housing_drop_m) X(housing_out_m) X(mirror_glass_proud_m)
struct CarPartsShape {
	SHAPE_DECLARE(AGENT_SHAPE_CAR_PARTS)
};

#define AGENT_SHAPE_CAR_SHAPE(X, XI, XD, XV, XVI) \
	X(bottom_in_m) X(arch_centre) X(roof_gap_m) X(sill_room_m) X(belt_room_m) X(flank_low) X(flank_high) \
	X(roof_edge_share) XV(crown_rows) X(roll_depth)
struct CarShapeShape {
	SHAPE_DECLARE(AGENT_SHAPE_CAR_SHAPE)
};

} // namespace city
