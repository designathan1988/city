// A simple ring cut into triangles on its own points, none added: ear clipping, then Lawson's edge flips to a constrained
// Delaunay triangulation (every inner edge whose opposite corner lies inside its triangle's circumcircle flipped), which of all the
// triangulations of those points maximises the smallest angle (de Berg et al., Computational Geometry, 3rd ed., 9.2 and 9.6). The
// one triangulator of plans: a junction's plate (render/roads/road_junction.cpp, as the lab's render/roads/pieceMesh.ts
// plateTriangles cuts it with earcut 3.2 and its `refine`: no fan from a middle, whose slivers are the worst a GPU shading in 2 x 2
// pixel quads can draw) and a lot's ring (sim/lots/lot_rings.cpp lot_triangles).
#pragma once

#include <vector>

namespace city {

// The ring as x, y pairs one after another (`xy`, in metres); three indices of its points a triangle, each counter-clockwise on the
// plan (x right, y up), for a ring of either winding. Consecutive points closer than a micrometre count as one (a ring that repeats
// the point where two of its lines meet), and what is left of a ring with no area (points on one line) gives no triangle.
std::vector<int> ring_triangles(const std::vector<double> &xy);

} // namespace city
