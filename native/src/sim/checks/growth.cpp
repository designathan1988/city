// The rubble's invariant. growth.nothing_on_rubble: no live building stands over a ruin (a ruin's centre inside the building's lot,
// or the building's centre on the ruin's lot), as a ruin keeps its lot until the player clears it (growth.cpp lot_occupied asks
// DamageState::ruin_under). Whole: every ruin against the buildings whose lot box meets its own, O(ruins x nearby buildings). Scoped
// (after a command, after a batch of ticks): the buildings that came or changed, each against the ruins filed near it. A record:
// the building's id. Nothing allocated: the hashes are walked in place.
#include "sim/checks/records.h"

#include "sim/state/city_state.h"

namespace city::checks {

namespace {
constexpr const char *FEATURE = "growth";
constexpr const char *NOTHING_ON_RUBBLE = "nothing_on_rubble";
} // namespace

void check_growth(const CityState &s, const CheckScope &scope, Broken &broken) {
	const BuildingRecords &bs = s.buildings;
	if (scope.whole) {
		for (const Ruin &r : s.damage.ruins) {
			const Building &fallen = r.record; // the ruin's lot is its fallen record's
			float x0, z0, x1, z1;
			BuildingRecords::bounds(fallen, x0, z0, x1, z1);
			bs.meeting(x0, z0, x1, z1, [&](int id) {
				const Building &b = bs.items[size_t(id)];
				if (BuildingRecords::covers(b, fallen.x, fallen.z, 0.f) || BuildingRecords::covers(fallen, b.x, b.z, 0.f))
					broken.add(FEATURE, NOTHING_ON_RUBBLE, id);
			});
		}
		return;
	}
	if (!scope.buildings) return;
	for (int id : *scope.buildings) {
		if (id < 0 || size_t(id) >= bs.items.size() || !bs.items[size_t(id)].alive) continue;
		const Building &b = bs.items[size_t(id)];
		if (s.damage.ruin_under(b.x, b.z, b.w, b.d, b.rot)) broken.add(FEATURE, NOTHING_ON_RUBBLE, id);
	}
}

} // namespace city::checks
