// A junction's plate cut into triangles on its own outline's points, none added: ear clipping, then Lawson's edge flips to a
// constrained Delaunay triangulation (every inner edge whose opposite corner lies inside its triangle's circumcircle flipped), which
// of all the triangulations of those points maximises the smallest angle (de Berg et al., Computational Geometry, 3rd ed., 9.2 and
// 9.6). The lab cuts its plates the same way (render/roads/pieceMesh.ts plateTriangles: earcut 3.2 and its `refine`): no fan from
// a middle, whose slivers are the worst a GPU shading in 2 x 2 pixel quads can draw.
#pragma once

#include "core/math.h"

#include <vector>

namespace city {

// Three indices into `ring` a triangle, each counter-clockwise on the plan (x right, y up), for a ring of either winding.
// Consecutive points closer than a micrometre count as one (a ring that repeats the point where two of its lines meet), and what
// is left of a ring with no area (points on one line) gives no triangle.
std::vector<int> plate_triangles(const std::vector<V2> &ring);

} // namespace city
