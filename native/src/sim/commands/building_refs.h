// What names a building by id outside its record, followed where the record goes: the one place every building dependent is
// dropped or carried (the Interior view's target, the fires). Ids are reused last freed first (core/slot_ids.h), so a reference
// left on a dead id passes to the next building that takes it. The journal's removal (j_remove_building) and the history's own
// (edit_history.cpp exec, and the records BuildingStore::put moves or knocks down) call these. map_mutex held exclusively.
#pragma once

#include "sim/state/city_state.h"

namespace city {

// Building `id` is gone (removed, undone, knocked down by a put): the view on it closes and its fires go out. Neither is undone
// (the view is not part of the city; fires are not saved), so an undo that brings the building back leaves both as they are.
void building_gone(CityState &s, int id);

// The live record at `from` moved to `to` (BuildingStore::put): the view and the fires follow it.
void building_moved(CityState &s, int from, int to);

} // namespace city
