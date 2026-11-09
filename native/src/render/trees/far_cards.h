// The far cards of a superchunk as one static mesh, four vertices a card, made on a job thread. Not a MultiMesh:
// Godot 4.7 keeps three CPU copies of every MultiMesh's buffer for physics interpolation (servers/rendering/storage/
// mesh_storage.cpp multimesh_allocate_data: _data_curr, _data_prev and _data_interpolated, sized whether or not the
// MultiMesh is interpolated), 240 bytes a card that a mesh does not hold. Vertex layout, read by
// render/shaders/vegetation.gdshaderinc (tree_vertex, its far-mesh path):
//   VERTEX   the tree's foot (world);
//   COLOR    rgb the card's tone (sRGB), a the card centre's height over the tree's;
//   CUSTOM0  (half floats) the card's width and the tree's height in metres, the species index, the card's half height
//            over the tree's (its non-zero value marks the far-mesh path);
//   the corner is the vertex's index modulo 4.
#pragma once

#include "render/rs_util.h"
#include "render/trees/tree_place.h"

#include <vector>

namespace city {

// The mesh of these cards (none when there are none); `centre` and `half_tall` are the card's height and half height as
// shares of the tree's.
OwnedMesh bake_far_cards(const std::vector<FarCard> &cards, float centre, float half_tall, const godot::RID &material); // made with `material`
// The bytes of the arrays bake_far_cards hands to the RenderingServer for that many cards (the upload ledger's price).
int64_t far_cards_bytes(size_t cards);

} // namespace city
