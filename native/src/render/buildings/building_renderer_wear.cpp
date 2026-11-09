// A building's look moves on with its age (wear.h): a chunk built before the next step of one of its buildings is meshed again as
// a changed one is, its meshes kept on screen until the new ones land, and the far mesh over it on its own clock. The step is
// worked out of what the chunk's job read (the records' build minutes and the clock: building_renderer_build.cpp), so it needs no
// mark of the simulation's: it is the chunk's own data run on, as a far mesh's rebuild clock is.
#include "render/buildings/building_renderer.h"

#include <limits>

namespace city {

void BuildingRenderer::wear_on(double minutes) {
	constexpr double NOT_DUE = std::numeric_limits<double>::infinity(); // the new mesh brings its next step
	for (auto &[key, ch] : chunks_) {
		if (minutes < ch.wear_due) continue;
		ch.wear_due = NOT_DUE;
		ch.version++;
		for (int l = 0; l < LEVELS; l++) ch.stale[l] = ch.built[l];
	}
	for (auto &[key, sp] : supers_) {
		if (minutes < sp.wear_due) continue;
		sp.wear_due = NOT_DUE;
		sp.dirty = true; // made again on its own clock (update_view: not within far_rebuild_interval_s of the last)
	}
}

} // namespace city
