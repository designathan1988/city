// The squares of a generated city: in each neighbourhood the block on a main road nearest its middle stops being lots and becomes a
// lawn with paved walks and trees. Runs once, on the lots just cut and before anything grows on them.
#pragma once

#include <cstdint>
#include <vector>

namespace city {

struct CityPlanDef;
struct CityState;
struct Lot;
namespace b40 {
struct Body;
struct Data;
} // namespace b40

// Sets the squares down as records and answers how many.
int plan_squares(CityState &s, uint32_t seed);

// The body of a square on one block of lots, as plan_squares lays it on the block it chose, with the draws of `seed`: false when the
// block cannot be a square. No city is read: the tests lay blocks by hand.
bool square_on(const CityPlanDef &plan, const b40::Data &data, const std::vector<Lot> &block, uint32_t seed, b40::Body &body);

} // namespace city
