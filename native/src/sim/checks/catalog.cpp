// The road variants' invariant. catalog.variant_room: the catalogue's profiles have room for every variant still to come (the room
// left is at least tuning road_max_variants less the variants made), so a profile never moves and a save with more variants than the
// open city opens. A copy of the catalogue keeps the size, not the room: Catalog::keep_variant_room after the loader and after the
// copy of a city read aside (api/city_core_persist.cpp). O(1) whole or scoped. A record: the count of variants made.
#include "sim/checks/records.h"

#include "sim/state/city_state.h"

#include <cstdint>

namespace city::checks {

namespace {
constexpr const char *FEATURE = "catalog";
constexpr const char *VARIANT_ROOM = "variant_room";
} // namespace

void check_catalog(const CityState &s, const CheckScope &, Broken &broken) {
	const Catalog &c = s.catalog;
	const int64_t room = int64_t(c.profiles.capacity()) - int64_t(c.profiles.size());
	const int64_t still = int64_t(c.tune.road_max_variants) - int64_t(c.variants.size());
	if (room < still) broken.add(FEATURE, VARIANT_ROOM, int(c.variants.size()));
}

} // namespace city::checks
