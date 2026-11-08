// The city generator's shapes: every value one tunes in a sim/citygen file, one block per file, filled from data/catalog/growth.json "shapes" by api/catalog_loader.cpp (a missing key refuses the catalogue). The blocks are declared by part (gen_shapes_roads.h, gen_shapes_lots.h); this file lists them all and makes the GenShapes the GrowthDef holds.
#pragma once

#include "sim/catalog/gen_shapes_roads.h"
#include "sim/catalog/gen_shapes_lots.h"

namespace city {

// Every block: its key in the table, its type and its list.
#define GEN_SHAPE_BLOCKS(S) \
	S(roads, RoadsShape, GEN_SHAPE_ROADS) \
	S(graph, GraphShape, GEN_SHAPE_GRAPH) \
	S(tidy, TidyShape, GEN_SHAPE_TIDY) \
	S(scenic, ScenicShape, GEN_SHAPE_SCENIC) \
	S(block_lots, BlocksShape, GEN_SHAPE_BLOCK_LOTS) \
	S(strips, StripsShape, GEN_SHAPE_STRIPS) \
	S(lots, LotsShape, GEN_SHAPE_LOTS) \
	S(lot_land, LotLandShape, GEN_SHAPE_LOT_LAND) \
	S(paving, PavingShape, GEN_SHAPE_PAVING) \
	S(squares, SquaresShape, GEN_SHAPE_SQUARES) \
	S(lot_rings, LotRingsShape, GEN_SHAPE_LOT_RINGS)

struct GenShapes {
#define GEN_SHAPE_MEMBER(key, Type, LIST) Type key;
	GEN_SHAPE_BLOCKS(GEN_SHAPE_MEMBER)
#undef GEN_SHAPE_MEMBER
};

} // namespace city
