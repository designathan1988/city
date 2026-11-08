// The terrain met by the roads at grade on their own vertices: no band of grass laid over the ground beside a road. The terrain is a
// grid of cells (Heightfield, `spacing` a side); a road's paving lies on it with edges anywhere, across a cell, on a slant, round a
// curve. Every cell the paving's outline crosses is cut along it, as a TIN takes a breakline: the part of the cell outside the paving,
// triangulated (Clipper2's constrained Delaunay) on the cell's corners and the outline's own points, drawn in the terrain's own
// surface (terrain_seam.gdshader reads a corner's height and normal from the terrain's maps). The terrain itself is left whole: under
// such a cell it stands lower than the seam (sim/terrain/road_shape.cpp: level with the paving's top beside the road, well below it
// under the paving), so the seam covers it at every level of the terrain's detail and no hole in it can open onto nothing.
// The paving is what the road jobs draw: the up-facing triangles of the segments and nodes near the chunk, those at grade (not on a
// deck, not in a bore). A corner keeps the terrain's height; a point of the outline the paving's height there.
#pragma once

#include "render/roads/road_mesh.h"

namespace city {

// What a chunk's seam reports besides its mesh: the cut cells no triangulation took (the probe and the layer count them).
struct SeamResult {
	int refused = 0;
};

// UV2.x of a seam vertex that is a corner of the terrain's grid: the shader takes its height and normal from the terrain's maps.
constexpr float SEAM_CORNER = 1.f;

// The cut cells chunk `key` owns (a cell belongs to the chunk of its middle), triangulated outside the paving; and the ground behind
// the retaining walls of the segments it owns (road_walls.cpp).
void emit_ground_seam(const RoadEmit &e, int64_t key, SeamResult &result);

// What chunk `key`'s seam is, for the probe: its cut cells, its triangles, the cells refused, the outline points no paving triangle
// gave a height, and the worst cell where the seam's area plus the paving's inside the cell is not the cell's, in square metres.
struct SeamProbe {
	int cells = 0, cut = 0, triangles = 0, refused = 0, unheighted = 0;
	double worst_cover_m2 = 0;
	godot::Vector3 lost; // the last point of a triangle no side of its polygon held (y: the polygon's ring count)
};
SeamProbe probe_ground_seam(const CityState &s, const StreetLook &L, RoadShapes &shapes, int64_t key);

} // namespace city
