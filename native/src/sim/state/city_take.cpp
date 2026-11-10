// A city read aside taken into the live state (city_state.h take_city), and the data's links to the channels (CityState::bind).
#include "sim/state/city_state.h"

#include <utility>

namespace city {

void CityState::bind() {
	roads.cat = &catalog;
	roads.log = &log;
	buildings.attach(&log, &catalog.occupancy);
	terrain.log = &log;
	terrain.tune = &catalog.tune;
	terrain.jobs = jobs.get();
	nature.ground.bind(&catalog.nature.paint);
}

void take_city(CityState &live, CityState &read) {
	// What only rises: decoded in place, each would have gone on from the live city's value by what the reading added.
	const uint64_t revision = live.revision, edit_revision = live.edit_revision, transit_revision = live.transit_revision;
	const uint64_t sky_revision = live.nature.sky_revision, fog_revision = live.nature.fog_revision;
	const int roads_version = live.roads.version;
	std::swap(static_cast<CityData &>(live), static_cast<CityData &>(read));
	std::swap(live.buildings.designs, read.buildings.designs); // the boot tables are the session's, not a city's
	live.revision += revision;
	live.saved_revision = live.revision; // what was just opened is what the file holds
	live.edit_revision += edit_revision;
	live.transit_revision += transit_revision;
	live.nature.sky_revision += sky_revision;
	live.nature.fog_revision += fog_revision;
	live.roads.version += roads_version;
	live.damage.continue_after(read.damage); // `read` holds the old city now
	live.bind();
	live.log.absorb(read.log);
}

} // namespace city
