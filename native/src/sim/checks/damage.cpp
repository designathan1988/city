// The fires' invariant. damage.fire_on_its_building: every fire burns a live building standing where it was set alight (the
// record's x z are the fire's, set at ignition: strike_buildings.cpp). A fire left on a bulldozed or undone building would pass to
// the next building that takes the id; building_gone puts it out and building_moved carries it (sim/commands/building_refs.h).
// Every fire whole or scoped: at most strike.fire_max. A record: the fire's building id. Nothing allocated.
#include "sim/checks/records.h"

#include "sim/state/city_state.h"

namespace city::checks {

namespace {
constexpr const char *FEATURE = "damage";
constexpr const char *ON_ITS_BUILDING = "fire_on_its_building";
} // namespace

void check_damage(const CityState &s, const CheckScope &, Broken &broken) {
	const auto &items = s.buildings.items;
	for (const Fire &f : s.damage.fires) {
		const bool live = f.building >= 0 && size_t(f.building) < items.size() && items[size_t(f.building)].alive;
		if (!live || items[size_t(f.building)].x != f.x || items[size_t(f.building)].z != f.z) broken.add(FEATURE, ON_ITS_BUILDING, f.building);
	}
}

} // namespace city::checks
