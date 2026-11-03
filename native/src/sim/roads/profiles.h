// Road profiles (cross-sections), Roadcraft src/roads/profiles.ts. Strips are listed from the right edge (negative u)
// to the left edge looking from node a to node b; right-hand traffic, lanes with dir +1 go a -> b on the -u side.
// The definitions live in project/data/catalog/road_profiles.json; derive_profile() computes what every system reads.
#pragma once

#include "sim/catalog/profile_types.h"

namespace city {

constexpr int MAX_PROFILE_LINES = 8; // painted lines and lanes per profile

bool is_asphalt(StripKind k);
void derive_profile(Profile &p, const RoadPrices &prices);

} // namespace city
