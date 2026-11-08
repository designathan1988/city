// The ground under a road, shaped to it: derived from the road's solved profile (sim/commands/road_grade_sync.cpp), never
// journaled nor saved. Under the paving it is pulled well below the deck; beside it, level with the paving's outer top on each
// side, so the terrain meets the paving's edge at its own height (render/roads/ground_seam.cpp cuts the cells the edge crosses);
// then a batter back to the natural ground as long as the earthwork it carries; lifted off where the road stands on piers, left
// whole over a bore; the authority (weight) of the road at a sample decides, not its distance.
// Applied by Heightfield::recompose (road_shape.cpp) with the journaled modifiers, the greater weight winning.
#pragma once

#include <cstdint>
#include <vector>

namespace city {

enum RoadShapeKind : uint8_t { SHAPE_RAISED = 1, SHAPE_SUNKEN = 2, SHAPE_MANUAL = 4, SHAPE_WALLS = SHAPE_MANUAL << 1 };

struct RoadShape {
	bool alive = false;
	std::vector<float> pts; // x, surface height, z of each station
	float half = 0; // half the casing
	float edge = 0; // half the paving: where the ground beside it starts
	float rise[2] = { 0, 0 }; // the paving's outer top over the deck, right (0, -u) and left (1, +u)
	// Past each end (the first station's and the last's) the paving goes round it, as where other roads meet; or stops straight
	// across (`cut`: the ground past it beside the road, level with the deck it stops at); `disc`: a disc of paving round the end
	// of that radius (0 none), as a turning bulb's.
	bool cut[2] = { false, false };
	float disc[2] = { 0, 0 };
	uint8_t kind = 0; // RoadShapeKind bits
	float minx = 0, minz = 0, maxx = 0, maxz = 0; // where it can shape (its stations grown by the widest reach)
};

// The farthest past its casing any road can shape: the casing's hold and the widest batter (SHAPE_INNER + SHAPE_SHOULDER_MAX).
float road_shape_reach();
// How far past its casing a cutting held by retaining walls is dug.
float cut_wall_reach();

} // namespace city
