// The building generator's shapes: every value one tunes in a generator file, one block per file (and the parts several
// files lay alike), filled from data/catalog/buildings.json "shapes" by api/buildings_loader.cpp (a missing key refuses the
// catalogue). The blocks are declared by part of the generator (shapes_bodies.h, shapes_lots.h, shapes_interior.h); this
// file lists them all and makes the Shapes a Data holds.
#pragma once

#include "sim/buildings/b40/shapes_bodies.h"
#include "sim/buildings/b40/shapes_lots.h"
#include "sim/buildings/b40/shapes_interior.h"

namespace city::b40 {

// Every block: its key in the table, its type and its list.
#define B40_SHAPES(S) \
	S(civic, CivicShape, B40_CIVIC) \
	S(services, ServiceShape, B40_SERVICES) \
	S(facade, FacadeShape, B40_FACADE) \
	S(massing, MassingShape, B40_MASSING) \
	S(house, HouseShape, B40_HOUSE) \
	S(tall, TallShape, B40_TALL) \
	S(works, WorksShape, B40_WORKS) \
	S(paint, PaintShape, B40_PAINT) \
	S(parts, LotPartShape, B40_PARTS) \
	S(back, LotBackShape, B40_BACK) \
	S(boundary, LotBoundaryShape, B40_BOUNDARY) \
	S(cars, LotCarsShape, B40_CARS) \
	S(front, LotFrontShape, B40_FRONT) \
	S(ground, LotGroundShape, B40_GROUND) \
	S(plan, LotPlanShape, B40_PLAN) \
	S(space, LotSpaceShape, B40_SPACE) \
	S(yard, LotYardShape, B40_YARD) \
	S(open, OpenLotShape, B40_OPEN) \
	S(interior, InteriorShape, B40_INTERIOR) \
	S(homes, InteriorHomeShape, B40_HOMES) \
	S(rooms, InteriorRoomShape, B40_ROOMS) \
	S(floor_plan, FloorPlanShape, B40_FLOOR_PLAN)

struct Shapes {
#define B40_SHAPE_MEMBER(key, Type, LIST) Type key;
	B40_SHAPES(B40_SHAPE_MEMBER)
#undef B40_SHAPE_MEMBER
};

} // namespace city::b40
