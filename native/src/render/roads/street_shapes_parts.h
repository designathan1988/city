// The street render's shapes: the structures, the stays, a junction's outline, the section and the street trees (streets.json "shapes", read by road_look.cpp).
#pragma once

#include "core/shape_fields.h"

namespace city {

#define STREET_SHAPE_ROAD_STRUCTURES(X, XI, XD, XV, XVI) \
	X(soffit_band_m) X(deck_top_under_m) X(scan_m) X(scan_road_m) X(pier_flare) X(foot_sink_m) X(abutment_sink_m) \
	X(bent_first) X(bent_last) X(deck_clear_in_m) X(guard_base_under_m) X(portal_step_most_m) \
	X(portal_step_least_m) X(jamb_m) X(crown_over_m) X(ground_over_m) X(lintel_least_m) X(headwall_out_m) \
	X(headwall_h_m) X(portal_reach_m) X(wing_over_road_m) X(wing_least_m) X(slice_lap_m) X(near_end_m) \
	X(least_lean_m) X(portal_step_share)
struct StructureShape {
	SHAPE_DECLARE(STREET_SHAPE_ROAD_STRUCTURES)
};

#define STREET_SHAPE_ROAD_STAYS(X, XI, XD, XV, XVI) \
	X(strand_m) X(fan_low) X(fan_high) X(foot_sink_m) X(strut_h_m) X(leg_out_m) X(anchor_in_m) X(strut_over_m)
struct StayShape {
	SHAPE_DECLARE(STREET_SHAPE_ROAD_STAYS)
};

#define STREET_SHAPE_ROAD_SHAPES(X, XI, XD, XV, XVI) \
	XD(plate_min_retreat) XD(ctrl_max) XD(ctrl_min)
struct NodeOutlineShape {
	SHAPE_DECLARE(STREET_SHAPE_ROAD_SHAPES)
};

#define STREET_SHAPE_ROAD_SECTION(X, XI, XD, XV, XVI) \
	X(walk_keep)
struct SectionShape {
	SHAPE_DECLARE(STREET_SHAPE_ROAD_SECTION)
};

#define STREET_SHAPE_STREET_PROPS(X, XI, XD, XV, XVI) \
	X(canopy_scale_min) X(tree_scale_span) X(shrub_scale) XVI(species)
struct StreetPropsShape {
	SHAPE_DECLARE(STREET_SHAPE_STREET_PROPS)
};

} // namespace city
