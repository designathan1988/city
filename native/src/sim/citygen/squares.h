// The squares of a generated city: in each neighbourhood the block on a main road nearest its middle stops being lots and becomes a
// lawn with paved walks and trees. Runs once, on the lots just cut and before anything grows on them.
#pragma once

#include <cstdint>

namespace city {

struct CityState;

// Sets the squares down as records and answers how many.
int plan_squares(CityState &s, uint32_t seed);

} // namespace city
