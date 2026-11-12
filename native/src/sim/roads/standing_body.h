// A body standing on the ground of the world: what a piece of the street is to everything else that stands there, whichever
// segment or junction placed it. A piece is placed in its own segment's frame (street_furniture.h FurnitureSpot); two pieces of
// two segments, or a piece and anything else, can only be asked about each other in the world, and this is that one form: a
// rectangle on the ground (its middle, the direction of its length, half its length and half its width). The street's audit reads
// it (api/city_core_bodies_probe.cpp); the placers are to ask it (log/pendencias.md V2, V3).
#pragma once

#include "core/math.h"
#include "sim/roads/street_furniture.h"

namespace city {

// Two bodies nearer than what they keep by under this are apart: a place is worked out in metres along a street, and a piece moved
// to stand just clear of another must not find it in the way again through the rounding of the sum it was moved by.
constexpr float BODY_ROUNDING_M = 1e-3f;

struct StandingBody {
	V2 at, along = V2(1, 0); // its middle on the ground; the unit direction its length runs in
	float half_along = 0, half_across = 0;
};

// The body of a piece of the street: its footprint where its segment's frame puts it, turned as the segment runs there.
StandingBody body_of(const RoadNetwork &net, const FurnitureSpot &s);
// How far apart two bodies stand: the least distance between their rectangles on the ground; zero or less when one stands in the
// other, minus how deep.
float bodies_apart(const StandingBody &a, const StandingBody &b);

// A piece in volume: one box of it in the world, its rectangle in plan and its span in height over the piece's foot. A piece is
// the boxes of its kind's parts (catalogue StreetFurnitureDef::parts, the list its mesh is made of), turned as the piece faces.
struct BodyBox {
	StandingBody plan;
	float y0 = 0, y1 = 0;
};
// The sets of its kind's parts a piece shows (PoleSet bits): a pole of the line its mast, its arm, its lamp when it is lit and,
// on one pole in a few, its transformer; a pole of the row across, or a lighting column, the column. The one owner of that
// choice: the render draws these parts, the bodies are made of them. 0 for a kind whose parts are all always there.
int spot_parts(const FurnitureSpot &s);
// The boxes of the piece at `s`, appended to `out`; nothing for a kind with no list of parts.
void body_boxes(const RoadNetwork &net, const StreetFurnitureDef &def, const FurnitureSpot &s, std::vector<BodyBox> &out);
// How far apart two pieces stand in volume: the least bodies_apart over their boxes that share some height; BIG when none do.
float volumes_apart(const std::vector<BodyBox> &a, const std::vector<BodyBox> &b);
// How far a piece's boxes reach from `at` in plan, at the most.
float boxes_reach(const std::vector<BodyBox> &boxes, const V2 &at);

} // namespace city
