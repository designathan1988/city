// The refusal's invariant. edit.refusal_inert: after a refused command the city's revisions are those its begin found
// (EditHistory::begin keeps them, end gives them back after the rollback): the rollback is the same city, and a raised revision
// would mark it unsaved and drop the kept furniture of every street (furniture_kept.h). Run after a refused command only, under its
// hold (api/command_hold.h). A record: which revision moved, 0 the map's, 1 the streets', 2 the network's. O(1).
#include "sim/checks/records.h"

#include "sim/state/city_state.h"

namespace city::checks {

namespace {
constexpr const char *FEATURE = "edit";
constexpr const char *REFUSAL_INERT = "refusal_inert";
enum MovedRevision : int { MAP_REVISION = 0, STREET_REVISION, EDIT_REVISION };
} // namespace

void check_edit(const CityState &s, Broken &broken) {
	const EditHistory &h = s.history;
	if (!h.refused()) return;
	const EditHistory::Revisions &was = h.at_begin();
	if (s.revision != was.map) broken.add(FEATURE, REFUSAL_INERT, MAP_REVISION);
	if (s.street_revision != was.streets) broken.add(FEATURE, REFUSAL_INERT, STREET_REVISION);
	if (s.edit_revision != was.edits) broken.add(FEATURE, REFUSAL_INERT, EDIT_REVISION);
}

} // namespace city::checks
