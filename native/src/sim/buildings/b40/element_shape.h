// The shape a lot element has on the ground (types.h LotElement), its one owner: the rectangle of its width and depth turned about
// its middle by its angle, or, a round one (a plaza, a round-point, a fountain's basin, a court's circle), the ellipse that rectangle
// holds, as the polygon of ROUND_SIDES corners standing on it. Who lays the ground covers (render/buildings/lot_covers.cpp), who
// draws a basin and its rim (render/buildings/lot_props.cpp), who stands trees clear of a paving (open_lots.cpp) and the probes that
// hold the lots' grounds (api/city_core_lot_cover_probe.cpp, city_core_lot_parts_probe.cpp, city_core_square_probe.cpp) read this
// one outline: what is drawn, what is planted round it and what is tested are the same shape.
#pragma once

#include "sim/buildings/b40/types.h"

namespace city::b40 {

constexpr int ROUND_SIDES = 32; // the corners of a round element's outline: on a plaza 14 m across they stand under 4 cm inside the circle
constexpr int OUTLINE_MOST = ROUND_SIDES;

// A box of the lot's frame.
struct Footprint {
	float x0 = 0, y0 = 0, x1 = 0, y1 = 0;
};

// An element's outline in the lot's frame (x along the front, y away from the street): a convex polygon, its corners anticlockwise.
struct Outline {
	int n = 0;
	Point2 p[OUTLINE_MOST];

	Footprint bounds() const; // the box that holds it
	float area() const;
	bool holds(float x, float y) const; // the point lies on it (its sides are part of it)
	bool meets(const Footprint &f) const; // it and the box share ground (sharing a line is not meeting)
	Point2 nearest(float x, float y) const; // its point nearest (x, y): the point itself where it lies on it
	// The stretch of a line inside it: across the lot at depth `line` (from..to along x) or along the lot at `line` across (along y).
	// False where the line passes it by.
	bool span(bool across, float line, float &from, float &to) const;
	// Every side moved `by` outward, the corners where the moved sides meet: a rim of one width all round it.
	Outline grown(float by) const;
};

Outline outline_of(const LotElement &e);
Outline outline_of(const Footprint &f);
// The ground a part standing on the lot keeps to itself: the box of its outline, a basin's (EL_POOL) with the deck laid round it,
// which its rim stands on (`pool_deck`: data.h LotRules::pool_deck).
Footprint part_ground(const LotElement &e, float pool_deck);

} // namespace city::b40
