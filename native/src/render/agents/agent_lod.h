// A vehicle's mesh with no more triangles than its distance can show: the simplifier takes the mesh itself and removes what
// moves the surface by less than `error_m`, so a car 70 pixels long does not carry several triangles to a pixel.
#pragma once

#include "render/mesh_builder.h"

namespace city {

// `in` with its triangles reduced part by part (a part: one tag, colour and pivot), so a wheel still turns round its hub, a seat
// still goes with its seat and paint never crosses into trim. The simplifier only removes vertices: those left keep every attribute.
MeshBuilder simplify_parts(const MeshBuilder &in, float error_m);
// `in` as one shape, colours and parts ignored: for what only casts. The vertices left keep what a shader moves them by (a tree's wind).
MeshBuilder simplify_whole(const MeshBuilder &in, float error_m);

} // namespace city
