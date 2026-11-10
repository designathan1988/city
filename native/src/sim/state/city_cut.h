// A cut of the city (CityCut): what a mesh job reads of its place, copied while the map is held shared for the copy alone
// (cut_city), so the build runs with the lock let go and a command, a grown building or a save never waits for a mesh. The cut
// holds the same stores under the same ids (core/id_table.h, SpatialHash::window), only the records of its box; the stores it
// was not asked for hold nothing, and reading them, an id the cut does not hold or a place past its box stops the tests' DLL
// (core/checks.h). The catalogue and the generator's tables are the session's, read by reference: the job pool is paused while
// a city is taken in (core/job_pool.h pause).
#pragma once

#include "sim/state/city_view.h"

#include <algorithm>

namespace city {

// The box of the map a cut copies, in metres.
struct CutBox {
	float minx = 0, minz = 0, maxx = 0, maxz = 0;
	void take(float x0, float z0, float x1, float z1) {
		minx = std::min(minx, x0);
		minz = std::min(minz, z0);
		maxx = std::max(maxx, x1);
		maxz = std::max(maxz, z1);
	}
	CutBox grown(float by) const { return CutBox{ minx - by, minz - by, maxx + by, maxz + by }; }
};

// What a cut copies besides the final ground of its box.
enum CutParts : unsigned { CUT_ROADS = 1, CUT_BUILDINGS = 2, CUT_GROUND_BASE = 4, CUT_WATER = 8 };

struct CityCut {
	const Catalog *catalog = nullptr;
	const b40::Data *designs = nullptr;
	GroundPatch ground;
	RoadNetwork roads;
	BuildingRecords buildings;
	CarriedAtomic<int> marking_style;
	uint64_t revision = 0, street_revision = 0;
};

inline CityView::CityView(const CityCut &cut) :
		catalog(*cut.catalog), terrain(cut.ground), roads(cut.roads), buildings(cut.buildings), designs(*cut.designs),
		marking_style(cut.marking_style), revision(cut.revision), street_revision(cut.street_revision), live(nullptr) {}

// The cut of `box` with `parts`, made by a thread that holds the map.
CityCut cut_held(const CityState &s, const CutBox &box, unsigned parts);

// The cut a job makes: the map taken shared here, for the copy alone; `box_of(s)` says the box once the map is held (a job's
// box may depend on what stands in its place).
template <typename BoxOf>
CityCut cut_city(const CityState &s, unsigned parts, BoxOf &&box_of) {
	MapRead map(s);
	return cut_held(s, box_of(s), parts);
}

} // namespace city
