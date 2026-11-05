// A vehicle's mesh with no more triangles than its distance can show. Each level was a separate hand-set build (stations, rows), so
// a car 100 m off, 70 pixels long, carried the 10 thousand triangles written for the close view: several to a pixel. The simplifier
// takes the mesh itself and removes what moves the surface by less than `error_m`.
#pragma once

#include "render/mesh_builder.h"

namespace city {

// `in` with its triangles reduced part by part (a part: one tag, colour and pivot), so a wheel still turns round its hub, a seat
// still goes with its seat and paint never crosses into trim. The simplifier only removes vertices: those left keep every attribute.
MeshBuilder simplify_parts(const MeshBuilder &in, float error_m);

} // namespace city
