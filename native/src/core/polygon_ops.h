// Operations on plans: boolean ones in whole millimetres (Clipper2 works in integers, so a cut comes out the same on every machine),
// and a simple ring cut into triangles.
#pragma once

#include "core/math.h"

#include <vector>

namespace city {

using PlanRing = std::vector<V2>;

// What of `a` lies inside `b` and what lies outside it, each side the union of its rings. The answer is simple rings, counter-clockwise; a piece with a hole comes
// split in two through the hole, because whoever asks keeps plans that have none.
std::vector<PlanRing> plan_intersection(const std::vector<PlanRing> &a, const std::vector<PlanRing> &b);
std::vector<PlanRing> plan_difference(const std::vector<PlanRing> &a, const std::vector<PlanRing> &b);

// A simple ring (x[i], y[i]), either winding, concave or not, in triangles: three indices each, counter-clockwise in (x, y). An ear is
// a convex corner whose triangle holds no corner of the ring that turns the other way or runs straight (only such a corner can stand in
// it); ears are cut until three corners are left. A ring that is not simple, or what is left in a line, ends the cut short: the
// triangles found so far.
std::vector<int> ring_triangles(const std::vector<double> &x, const std::vector<double> &y);

} // namespace city
