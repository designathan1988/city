// What the Interior layer shows: which buildings are drawn cut open, at which floor, for a camera looking which way. A view of
// the city, not part of it: not saved, not undone. The renderer's jobs copy it with the building records; the command that
// sets it (sim/commands/interior_commands.cpp) marks the chunks to draw again; it closes when its building goes and follows the
// record when it moves (sim/commands/building_refs.h).
#pragma once

#include "sim/buildings/building_store.h"
#include "sim/buildings/interior/cut.h"

#include <cstdint>

namespace city {

struct InteriorView {
	bool on = false;
	int target = -1; // the one building: the one in the middle of the view, or double-clicked (the 4.0 opens one, never a radius)
	// The target's record seed when the view opened: ids are reused, a seed names the building across edits (damage_state.h Hurt),
	// so the invariant interior.target_same_building (sim/checks/buildings.cpp) tells the building it opened from one on its id.
	uint32_t seed = 0;
	b40::CutView cut; // the floor and the camera direction
	uint32_t revision = 0; // moves with every change

	// Whether building `id` is drawn cut open.
	bool cuts(int id) const { return on && id == target; }
};

} // namespace city
