// A building cut open at a floor, to look inside (the Interior layer): the one owner of how it is cut. The cut body is the
// body with everything above the floor seen taken away; the renderer draws it with the same emitters as a whole one, and
// adds the floor, cores, walls and furniture (render/buildings/interior_emit.cpp). Never stored: the body of the record
// is untouched.
#pragma once

#include "sim/buildings/b40/types.h"

namespace city::b40 {

// The camera directions a cut tells apart: eight steps of 45 degrees, from +x (world) towards +z.
constexpr int CUT_DIRECTIONS = 8;

// Which floor to cut at and the way the camera looks (a step of CUT_DIRECTIONS, world).
struct CutView {
	int level = 0;
	int dir = 0;
};

// The highest floor of the body: its last storey.
int top_level(const Body &b);
// The unit direction of a step (world x, z).
Point2 cut_direction(int dir);
// A step of the world direction (x, z): the nearest of CUT_DIRECTIONS.
int cut_step(float x, float z);

// The body cut open at `level` (limited to the body's floors) for a camera looking along `view` in the body's own frame
// (lot x, y): the volumes that start above are gone, the others lose their storeys above; cut_level and cut_view say so.
Body cut_open(const Body &b, int level, Point2 view);

} // namespace city::b40
