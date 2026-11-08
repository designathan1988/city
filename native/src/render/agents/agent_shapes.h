// The traffic agents' shapes: every size a part builder of render/agents draws with, one block per file, filled from data/render/agents.json "shapes" by agent_renderer.cpp (a missing key refuses the table). The blocks are declared by part (agent_shapes_parts.h, agent_shapes_car.h); this file lists them all and makes the AgentShapes the renderer holds.
#pragma once

#include "render/agents/agent_shapes_parts.h"
#include "render/agents/agent_shapes_car.h"

namespace city {

// Every block: its key in the table, its type and its list.
#define AGENT_SHAPE_BLOCKS(S) \
	S(agent_cabin, CabinShape, AGENT_SHAPE_AGENT_CABIN) \
	S(agent_parts, PartsShape, AGENT_SHAPE_AGENT_PARTS) \
	S(agent_wreck, WreckShape, AGENT_SHAPE_AGENT_WRECK) \
	S(bicycle_mesh, BicycleShape, AGENT_SHAPE_BICYCLE_MESH) \
	S(bus_body, BusShape, AGENT_SHAPE_BUS_BODY) \
	S(heavy, HeavyShape, AGENT_SHAPE_HEAVY) \
	S(heavy_meshes, HeavyMeshesShape, AGENT_SHAPE_HEAVY_MESHES) \
	S(car_body, CarBodyShape, AGENT_SHAPE_CAR_BODY) \
	S(car_parts, CarPartsShape, AGENT_SHAPE_CAR_PARTS) \
	S(car_shape, CarShapeShape, AGENT_SHAPE_CAR_SHAPE)

struct AgentShapes {
#define AGENT_SHAPE_MEMBER(key, Type, LIST) Type key;
	AGENT_SHAPE_BLOCKS(AGENT_SHAPE_MEMBER)
#undef AGENT_SHAPE_MEMBER
};

} // namespace city
