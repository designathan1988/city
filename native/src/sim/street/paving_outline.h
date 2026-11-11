// The paving as it is drawn, round a box, as polygons in whole millimetres (Clipper2 works in integers, so a cut is the same on
// every machine): each segment's band between its mouths, out to its footways' outer edges; each plated node's ring through its
// corners' footway backs (sim/roads/node_outline.h, the owner the road surfaces draw from); each turning bulb's ring. Read only round
// the box asked: the whole map's outline is never made. What a lot is cut by (sim/citygen lot_land.cpp, the lot cells) and what the
// probes hold the lots against.
#pragma once

#include <iterator> // before Clipper2: its header needs it and does not say so

#include <clipper.h>

namespace city {

class RoadNetwork;

constexpr double PAVING_UNITS_PER_M = 1000;

// A point of the plan in the paving's units.
Clipper2Lib::Point64 paving_point(double x, double z);
// `path` added to `out` wound one way for every piece: under the non-zero rule two pieces wound against each other cancel where
// they overlap.
void add_positive(Clipper2Lib::Paths64 &out, Clipper2Lib::Path64 path);
// The paving round the box [x0, x1] x [z0, z1], whole: its outline and, wound the other way, the blocks it closes; empty with no
// road near.
Clipper2Lib::Paths64 paving_near(const RoadNetwork &net, double x0, double z0, double x1, double z1);

} // namespace city
