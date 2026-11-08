// The building render's shapes: every value a render file draws with, one block per file, filled from data/render/building_look.json "shapes" by building_look.cpp (a missing key refuses the table). The blocks are declared by what they draw (look_shapes_facade.h, look_shapes_lot.h, look_shapes_roof.h, look_shapes_levels.h); this file lists them and makes the LookShapes a BuildingLook holds.
#pragma once

#include "render/buildings/look_shapes_facade.h"
#include "render/buildings/look_shapes_lot.h"
#include "render/buildings/look_shapes_roof.h"
#include "render/buildings/look_shapes_levels.h"

namespace city::b40 {

// Every block: its key in the table, its type and its list.
#define LOOK_SHAPE_BLOCKS(S) \
	S(facade_parts, FacadePartsLook, LOOK_SHAPE_FACADE_PARTS) \
	S(facade_emit, FacadeEmitLook, LOOK_SHAPE_FACADE_EMIT) \
	S(part_models, PartModelsLook, LOOK_SHAPE_PART_MODELS) \
	S(placed, PlacedLook, LOOK_SHAPE_PLACED) \
	S(facade_bay, FacadeBayLook, LOOK_SHAPE_FACADE_BAY) \
	S(facade_props, FacadePropsLook, LOOK_SHAPE_FACADE_PROPS) \
	S(lot_emit, LotEmitLook, LOOK_SHAPE_LOT_EMIT) \
	S(lot_props, LotPropsLook, LOOK_SHAPE_LOT_PROPS) \
	S(lot_parts, LotPartsLook, LOOK_SHAPE_LOT_PARTS) \
	S(roof_emit, RoofEmitLook, LOOK_SHAPE_ROOF_EMIT) \
	S(roof_plant, RoofPlantLook, LOOK_SHAPE_ROOF_PLANT) \
	S(roof_terrace, RoofTerraceLook, LOOK_SHAPE_ROOF_TERRACE) \
	S(roof_gear, RoofGearLook, LOOK_SHAPE_ROOF_GEAR) \
	S(roof_pitched, RoofPitchedLook, LOOK_SHAPE_ROOF_PITCHED) \
	S(interior_emit, InteriorEmitLook, LOOK_SHAPE_INTERIOR_EMIT) \
	S(far_mass, FarMassLook, LOOK_SHAPE_FAR_MASS) \
	S(pane_rooms, PaneRoomsLook, LOOK_SHAPE_PANE_ROOMS) \
	S(building_bodies, BuildingBodiesLook, LOOK_SHAPE_BUILDING_BODIES)

struct LookShapes {
#define LOOK_SHAPE_MEMBER(key, Type, LIST) Type key;
	LOOK_SHAPE_BLOCKS(LOOK_SHAPE_MEMBER)
#undef LOOK_SHAPE_MEMBER
};

} // namespace city::b40
