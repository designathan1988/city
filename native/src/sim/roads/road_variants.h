// Road variants: a class road with the lanes, width and kerb bands the road tool's options chose. Each is a profile of its
// own, appended to the catalogue once, so everything that reads a road's profile by index reads it unchanged.
#pragma once

#include "sim/catalog/catalog.h"

namespace city {

// The profile for `v`: the base itself when nothing differs or the base takes no options, -1 when the catalogue is full.
int road_variant(Catalog &c, const RoadVariant &v);
// Back to the catalogue's own profiles (before a save's variants are made again).
void clear_variants(Catalog &c);

} // namespace city
