// A building broken for its fall (the 4.0's buildings/fracture.ts): its own faces cut by a jittered grid of Voronoi cells, each cell a piece
// of the wall with the wall's own paint, a back and a side along every cut, in broken masonry. All the pieces are one mesh, each vertex carrying
// its piece, so a ruin is one draw however many pieces fly (building_ruin.gdshader moves them by a texture of matrices, ruin_pieces.h).
#pragma once

#include "render/mesh_draw.h"

#include <godot_cpp/variant/vector3.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace city {

namespace b40 {
struct Data;
struct Shell;
struct BuildingLook;
}
struct Building;
struct HeightPatch;

struct RuinPiece {
	godot::Vector3 centre, half;
	float radius = 0, low = 0;
	int first_link = 0, links = 0; // its share of RuinMesh::links
};

// What a job hands the effects: the mesh made on its thread, the pieces and which of them hold each other (the bonds of a support graph: pieces
// in touching cells; a piece on the ground is bonded to the world, ruin_pieces.cpp).
struct RuinMesh {
	OwnedMesh mesh;
	std::vector<RuinPiece> pieces;
	std::vector<int> links;
	godot::Vector3 lo, hi;
	int64_t vertices = 0;
};

// A floor inside: its ring of corners, at its height.
using RuinSlab = std::vector<godot::Vector3>;

// `walls`: the shell as the near level draws it; `glass` its panes, drawn dark and broken in the walls' one material.
std::shared_ptr<RuinMesh> fracture_shell(const b40::Shell &walls, const b40::Shell &glass, const std::vector<RuinSlab> &slabs, uint32_t seed);
// The record drawn and cut, on a job's thread (ruin_body.cpp).
std::shared_ptr<RuinMesh> ruin_of(const b40::Data &data, const b40::BuildingLook &look, const Building &rec, const HeightPatch &ground);

} // namespace city
