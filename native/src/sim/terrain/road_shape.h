// The ground under and beside a road, shaped to it: derived from the road's solved profile (sim/commands/road_grade_sync.cpp),
// never journaled nor saved. Under the paving it is pulled below the surface; past the paving's outer edge it stands level with
// that edge's top over a held band, then a batter back to the natural ground as long as the earthwork it carries. The ground is
// the road's embankment until a deck carries the road (ground_carries), and is left whole over a bore; the authority (weight) of
// the road at a sample decides, not its distance. Applied by Heightfield::recompose (road_shape.cpp) with the journaled
// modifiers, the greater weight winning. The terrain owns the ground beside a road: the road's mesh draws no ground of its own.
#pragma once

#include "sim/catalog/catalog.h"

#include <cstdint>
#include <vector>

namespace city {

enum RoadShapeKind : uint8_t { SHAPE_RAISED = 1, SHAPE_SUNKEN = 2, SHAPE_MANUAL = 4, SHAPE_WALLS = SHAPE_MANUAL << 1 };

struct RoadShape {
	bool alive = false;
	std::vector<float> pts; // x, surface height, z of each station
	float half = 0; // half the casing
	float paving = 0; // half the paved width: past it the ground stands level with the paving's outer top
	float rise = 0; // how far that outer top stands over the road's surface (a raised footway's kerb, or none)
	uint8_t kind = 0; // RoadShapeKind bits
	float minx = 0, minz = 0, maxx = 0, maxz = 0; // where it can shape (its stations grown by the widest reach)
};

// The farthest past its casing any road can shape: the held band and the widest batter.
float road_shape_reach(const Tuning &t);
// How far past its casing a cutting held by retaining walls is dug.
float cut_wall_reach(const Tuning &t);
// The share of a road the ground carries where the road stands `lift` over the natural ground: whole on the ground and on its
// embankment, none where a deck carries it. A road built as a viaduct or a bridge lets the ground go from grade_lift_on_m; a road
// raised by hand keeps its embankment up to structure_raised_m, where its deck begins (render/roads/road_structures.cpp draws the
// deck wherever this is under whole, so the road always stands on one or the other).
float ground_carries(uint8_t kind, float lift, const Tuning &t);

} // namespace city
