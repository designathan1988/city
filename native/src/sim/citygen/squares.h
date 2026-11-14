// The squares and parks of a generated city: in each neighbourhood the block on a main road nearest its middle, and every whole
// block in one of the plan's park patches (city_plan.h park_at), stops being lots and becomes a lawn with paved walks and trees.
// Runs once, on the lots just cut and before anything grows on them.
#pragma once

#include "sim/citygen/city_plan.h"

#include <cstdint>
#include <vector>

namespace city {

struct CityState;
struct Lot;
namespace b40 {
struct Body;
struct Data;
} // namespace b40

// Sets the squares and the parks of `plan` down as records and answers how many.
int lay_greens(CityState &s, uint32_t seed, const CityPlan &plan);

// The body of a square on one block of lots, as lay_greens lays it on the block it chose, with the draws of `seed`: false when the
// block cannot be a square. No city is read: the tests lay blocks by hand.
bool square_on(const CityPlanDef &plan, const b40::Data &data, const std::vector<Lot> &block, uint32_t seed, b40::Body &body);

} // namespace city
