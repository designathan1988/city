// The paving as polygons in whole millimetres (Clipper2 works in integers, so a cut is the same on every machine): each road's ribbon
// between its mouths, each junction's plate out to the point where two footways meet, each turning bulb. Read only round the box
// asked: the whole map's outline is never made. What a lot against the streets is cut by (lot_land.cpp).
#pragma once

#include <iterator> // before Clipper2: its header needs it and does not say so

#include <clipper.h>
#include "sim/lots/lot_book.h"

#include <vector>

namespace city {

class RoadNetwork;

constexpr double PAVING_UNITS_PER_M = 1000; // a paving unit is a millimetre
constexpr double PAVING_MARGIN_M = 2; // past the box, so the cut inside it is the whole map's

// The paving round a box, whole: its outline and, wound the other way, the blocks it closes.
Clipper2Lib::Paths64 paving_near(const RoadNetwork &net, double x0, double z0, double x1, double z1);
// The paving round a ring's box, the margin past it.
Clipper2Lib::Paths64 paving_round(const RoadNetwork &net, const std::vector<LotPoint> &ring);
// What of a ring stands off the paving: the outlines, holes left out as a lot has none.
Clipper2Lib::Paths64 land_of(const std::vector<LotPoint> &ring, const Clipper2Lib::Paths64 &paving);
// A paving path as a ring in metres.
std::vector<LotPoint> paving_ring(const Clipper2Lib::Path64 &path);

} // namespace city
