// The street render's shapes: every size a street emitter of render/roads and render/props draws with beyond the road's section, one block per file, filled from data/render/streets.json "shapes" by road_look.cpp (a missing key refuses the table); the StreetShapes the StreetLook holds.
#pragma once

#include "render/roads/street_shapes_parts.h"

namespace city {

// Every block: its key in the table, its type and its list.
#define STREET_SHAPE_BLOCKS(S) \
	S(road_structures, StructureShape, STREET_SHAPE_ROAD_STRUCTURES) \
	S(road_stays, StayShape, STREET_SHAPE_ROAD_STAYS) \
	S(road_shapes, NodeOutlineShape, STREET_SHAPE_ROAD_SHAPES) \
	S(road_section, SectionShape, STREET_SHAPE_ROAD_SECTION) \
	S(street_props, StreetPropsShape, STREET_SHAPE_STREET_PROPS)

struct StreetShapes {
#define STREET_SHAPE_MEMBER(key, Type, LIST) Type key;
	STREET_SHAPE_BLOCKS(STREET_SHAPE_MEMBER)
#undef STREET_SHAPE_MEMBER
};

} // namespace city
