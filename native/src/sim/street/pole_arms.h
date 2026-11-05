// Which way a pole's cross-arms lie (Roadcraft utilities.ts poleArms, as line construction standards frame a pole): one
// arm square to a straight line or on the bisector of a gentle bend, and at a corner (poles.corner_deflection_deg or more)
// or a branching an arm for each line leaving it, two lines straight through each other sharing theirs.
#pragma once

#include "sim/street/street_works.h"

#include <vector>

namespace city {

struct ArmDir {
	float x = 0, z = 1;
};

struct PoleArms {
	std::vector<ArmDir> arms; // one per physical arm
	std::vector<ArmDir> by_span; // per span of the pole, in its order: the arm it hangs from, on the left of the span's a -> b
};

// `idle`: the arm of a pole with no wire yet, square to its street.
PoleArms pole_arms(const StreetWorks &w, const PoleDef &def, int pole, ArmDir idle);

} // namespace city
