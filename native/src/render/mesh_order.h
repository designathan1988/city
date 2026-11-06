// The order of a surface's triangles. They left the emitters in the order things were built, so the GPU's cache of finished vertices
// missed and ran the same vertex again and again, in the picture and in each cascade of the sun. Reordered, the same triangles run
// each vertex about once: nothing moves, only the work. On the job that bakes the mesh, once a mesh, never by frame.
#pragma once

#include <cstddef>
#include <cstdint>

namespace city {

// `idx`: `count` indices into `vertices` vertices, reordered in place (meshopt_optimizeVertexCache, Forsyth's method as its author
// tuned it). Not for a surface that blends: there the order is part of the picture.
void order_triangles(int32_t *idx, size_t count, size_t vertices);

} // namespace city
