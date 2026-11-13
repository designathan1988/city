// The Interior view's invariant. interior.target_same_building: the view is off, or its target is a live building with the seed
// the view opened on (InteriorView::seed). Ids are reused last freed first, so a view left on a demolished or undone building would
// open the next building that takes the id; building_gone closes it and building_moved carries it (sim/commands/building_refs.h).
// One view, so the same O(1) test whole or scoped. A record: the target's id.
#include "sim/checks/records.h"

#include "sim/state/city_state.h"

namespace city::checks {

namespace {
constexpr const char *FEATURE = "interior";
constexpr const char *SAME_BUILDING = "target_same_building";
} // namespace

void check_buildings(const CityState &s, const CheckScope &, Broken &broken) {
	const InteriorView &v = s.interior;
	if (!v.on) return;
	const bool live = v.target >= 0 && size_t(v.target) < s.buildings.items.size() && s.buildings.items[size_t(v.target)].alive;
	if (!live || s.buildings.items[size_t(v.target)].seed != v.seed) broken.add(FEATURE, SAME_BUILDING, v.target);
}

} // namespace city::checks
