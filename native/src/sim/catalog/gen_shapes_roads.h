// The city generator's shapes of the streets: the grid and its march, the graph, the tidy pass and the scenic city's runs (growth.json "shapes", read by api/catalog_loader.cpp).
#pragma once

#include "core/shape_fields.h"

namespace city {

#define GEN_SHAPE_ROADS(X, XI, XD, XV, XVI) \
	XD(block_along) XD(block_across) XD(grid_span) XD(centre_share) XI(avenue_every) XI(points_along) \
	XI(points_across) XD(edge_pad) XD(inside_margin) XD(land_clear) XD(max_wet) XD(max_local_grade) \
	XD(max_art_grade)
struct RoadsShape {
	SHAPE_DECLARE(GEN_SHAPE_ROADS)
};

#define GEN_SHAPE_GRAPH(X, XI, XD, XV, XVI) \
	XD(bucket) XD(merge) XD(collapse) XD(hug_cos) XD(hug_dist) XD(gen_seg_len) XI(max_links) XI(min_component) \
	XD(node_probe) XD(node_min_y) XD(side_probe_m)
struct GraphShape {
	SHAPE_DECLARE(GEN_SHAPE_GRAPH)
};

#define GEN_SHAPE_TIDY(X, XI, XD, XV, XVI) \
	XD(min_leg_angle) XD(end_gap) XD(min_piece) XD(cell) XD(pad) XD(on_road_min) XD(on_road_default) \
	XD(on_road_dy) XI(max_passes) XI(keeps_legs)
struct TidyShape {
	SHAPE_DECLARE(GEN_SHAPE_TIDY)
};

#define GEN_SHAPE_SCENIC(X, XI, XD, XV, XVI) \
	X(longest_run_m)
struct ScenicShape {
	SHAPE_DECLARE(GEN_SHAPE_SCENIC)
};

} // namespace city
