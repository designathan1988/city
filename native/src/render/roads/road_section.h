// A road's cross-section as pieces: which surface each strip draws, where and how high, the ramp band of a footway at a dropped
// kerb and the island between two carriageways. road_segment.cpp extrudes them, road_markings.cpp paints around the island.
#pragma once

#include "render/roads/road_look.h"
#include "sim/catalog/profile_types.h"

#include <vector>

namespace city {

// How a piece follows a lowered kerb (render/roads/kerb_drop.h): a kerb drops; a band of the footway beside it slopes down to it.
enum class Ramp : uint8_t { NONE, KERB, BAND };

constexpr int LINE_YELLOW = 4; // Piece::line0, line1: past every LineKind

struct Piece {
	float u0 = 0, u1 = 0, h = 0;
	RoadMat mat = RoadMat::ASPHALT;
	godot::Color col;
	float edge0 = 0, edge1 = 0, lane = 0; // where wear is read: the surface's two edges, and the lanes' width
	float lane_u0 = 0; // a lane's own edge, which its tracks are counted from
	// The line painted on each of a lane's two edges (u0, u1), as the far level draws it: its LineKind, LINE_YELLOW more
	// when it is yellow; 0 where the profile states none.
	int line0 = 0, line1 = 0;
	int tone = 0;
	Ramp ramp = Ramp::NONE;
	float kerb_d0 = 0, kerb_d1 = 0, band = 0; // a footway band's edges' distances from its kerb, and its footway's crossing band
	godot::Color face; // what its vertical faces wear
	bool island = false;
};

// The raised island between two carriageways, if the profile has one: `i` is its strip, u0 and u1 its edges across the road,
// the lanes' inner edges. It stands on those edges as the 4.0 lays it: the planting fills the strips the profile keeps for its
// kerbs and the kerbs take the lanes' first centimetres, so traffic keeps the lanes the profile gives. A ring's island is not one.
bool island_at(const Profile &p, int i);
bool island_span(const Profile &p, float &u0, float &u1);

// `drops`: the raised footways beside the outer kerbs split into the bands a lowered kerb slopes (road_section.cpp walk), for a
// surface drawn near; without it they run whole (the far meshes, the verge), where a lowered kerb is under a pixel.
std::vector<Piece> pieces_of(const StreetLook &L, const Profile &p, bool drops);
// The same section past an island's nose: bare carriageway where the island stood.
std::vector<Piece> without_island(const std::vector<Piece> &ps);

} // namespace city
