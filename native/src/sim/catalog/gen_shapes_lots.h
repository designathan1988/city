// The city generator's shapes of the lots: the blocks, the strips along the streets, the lots' sizes, a lot cut back to the land, the paving, the squares and the drawn lots' cuts and joins (growth.json "shapes", read by api/catalog_loader.cpp).
#pragma once

#include "core/shape_fields.h"

namespace city {

#define GEN_SHAPE_BLOCK_LOTS(X, XI, XD, XV, XVI) \
	XD(two_rows) XD(res) XD(max_block) XD(plate_share) XD(lattice_start) XD(lattice_step) XD(land_share) \
	XD(street_probe)
struct BlocksShape {
	SHAPE_DECLARE(GEN_SHAPE_BLOCK_LOTS)
};

#define GEN_SHAPE_STRIPS(X, XI, XD, XV, XVI) \
	XD(res) XD(road_pad) XD(dry_min) XD(wet_clear) XD(max_relief) XD(wet_step) XD(facing_probe) XD(facing_pad) \
	XI(arc_steps) XI(facing_samples) X(straight_on)
struct StripsShape {
	SHAPE_DECLARE(GEN_SHAPE_STRIPS)
};

#define GEN_SHAPE_LOTS(X, XI, XD, XV, XVI) \
	XD(front_pad) XD(width_trim) XD(depth_m) XD(frontage_m) XD(least_m) XD(works_depth) XD(works_frontage) \
	XD(along_step) XD(behind_footway) XD(back_in) XD(paving_gap) XD(reach_m)
struct LotsShape {
	SHAPE_DECLARE(GEN_SHAPE_LOTS)
};

#define GEN_SHAPE_LOT_LAND(X, XI, XD, XV, XVI) \
	XD(crossed_area_m2) XD(straight_back_m) XD(square_share) XD(square_reach_m) XD(front_slack_m) \
	XD(front_search_m) XD(lot_snap_share) XD(back_span) XD(apart_m)
struct LotLandShape {
	SHAPE_DECLARE(GEN_SHAPE_LOT_LAND)
};

#define GEN_SHAPE_PAVING(X, XI, XD, XV, XVI) \
	XD(along_pace_m) XD(plate_reach_m) XD(outline_slack_units)
struct PavingShape {
	SHAPE_DECLARE(GEN_SHAPE_PAVING)
};

#define GEN_SHAPE_SQUARES(X, XI, XD, XV, XVI) \
	XD(file_cell_m) X(lawn_h) X(lattice_from_m) X(seam_m) X(plaza_wide)
struct SquaresShape {
	SHAPE_DECLARE(GEN_SHAPE_SQUARES)
};

#define GEN_SHAPE_LOT_RINGS(X, XI, XD, XV, XVI) \
	XD(near_m) XD(cut_past_m) XD(side_least_m) XD(off_angle_m) XD(inner_share)
struct LotRingsShape {
	SHAPE_DECLARE(GEN_SHAPE_LOT_RINGS)
};

} // namespace city
