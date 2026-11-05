// A road's cross-section as pieces: which surface each strip draws, where and how high, the ramp band of a footway at a dropped
// kerb and the island between two carriageways. road_segment.cpp extrudes them, road_markings.cpp paints around the island.
#pragma once

#include "render/roads/road_look.h"
#include "sim/catalog/profile_types.h"

#include <vector>

namespace city {

enum class Ramp : uint8_t { NONE, KERB, LOW_U0, LOW_U1 };

struct Piece {
	float u0 = 0, u1 = 0, h = 0;
	RoadMat mat = RoadMat::ASPHALT;
	godot::Color col;
	float edge0 = 0, edge1 = 0, lane = 0; // where wear is read: the surface's two edges, and the lanes' width
	int tone = 0;
	Ramp ramp = Ramp::NONE;
	godot::Color face; // what its vertical faces wear
	bool island = false;
};

// The raised island between two carriageways, if the profile has one: `i` is its strip, u0 and u1 its edges across the road,
// the lanes' inner edges. It stands on those edges as the 4.0 lays it: the planting fills the strips the profile keeps for its
// kerbs and the kerbs take the lanes' first centimetres, so traffic keeps the lanes the profile gives. A ring's island is not one.
bool island_at(const Profile &p, int i);
bool island_span(const Profile &p, float &u0, float &u1);

std::vector<Piece> pieces_of(const StreetLook &L, const Profile &p);
// The same section past an island's nose: bare carriageway where the island stood.
std::vector<Piece> without_island(const std::vector<Piece> &ps);

} // namespace city
