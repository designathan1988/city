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
class BuildingBodies;
class SignatureBodies;
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
	static constexpr float NO_FLOOR_M = -1e30f; // below any ground: no floor to rest on
	float floor = NO_FLOOR_M; // the height its building stood on (its lot's floor): a piece starting there rests on it
};

// The depth given to a wall face cut (the facade kit draws one face to a wall), and what a piece lying in a heap adds to it (ruin_sim.cpp).
constexpr float RUIN_WALL_DEPTH_M = 0.28f;

// A floor inside: its ring of corners, at its height.
using RuinSlab = std::vector<godot::Vector3>;

// `walls`: the shell as the near level draws it; `glass` its panes, drawn dark and broken in the walls' one material. A wall face is
// given a depth behind it (the facade kit draws one face to a wall). `sided`, a design of its own: the faces drawn with their other
// side (a roof both ways, a wall with its inside), which get no back; then the walls' faces, alone, get a back in their plane, so no
// back stands out through another face nor lies on one.
std::shared_ptr<RuinMesh> fracture_shell(const b40::Shell &walls, const b40::Shell &glass, const std::vector<RuinSlab> &slabs, uint32_t seed,
		const b40::Shell *sided = nullptr);
// The record drawn and cut, on a job's thread (ruin_body.cpp), from the body cache the building levels share; a design of its own from its
// cooked faces (`designs`).
std::shared_ptr<RuinMesh> ruin_of(BuildingBodies &bodies, SignatureBodies &designs, const b40::Data &data, const b40::BuildingLook &look, const Building &rec, const HeightPatch &ground);

} // namespace city
