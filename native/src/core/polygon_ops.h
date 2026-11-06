// Boolean operations on plans, in whole millimetres: Clipper2 works in integers, so a cut comes out the same on every machine.
#pragma once

#include "core/math.h"

#include <vector>

namespace city {

using PlanRing = std::vector<V2>;

// What of `a` lies inside `b` and what lies outside it, each side the union of its rings. The answer is simple rings, counter-clockwise; a piece with a hole comes
// split in two through the hole, because whoever asks keeps plans that have none.
std::vector<PlanRing> plan_intersection(const std::vector<PlanRing> &a, const std::vector<PlanRing> &b);
std::vector<PlanRing> plan_difference(const std::vector<PlanRing> &a, const std::vector<PlanRing> &b);

} // namespace city
