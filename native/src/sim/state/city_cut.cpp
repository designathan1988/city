// The parts of the city copied into a cut (city_cut.h).
#include "sim/state/city_cut.h"

namespace city {

namespace {
// A box that holds nothing: the cut of a store over it answers for no place and holds no record (a part the job did not ask).
constexpr CutBox NOWHERE{ 1.f, 1.f, -1.f, -1.f };
} // namespace

CityCut cut_held(const CityState &s, const CutBox &box, unsigned parts) {
	if (CHECKS && map_hold::mine().depth == 0) city_stop("a cut of the city made without the map held");
	CityCut c;
	c.catalog = &s.catalog;
	c.designs = &s.buildings.designs;
	c.marking_style = s.marking_style;
	c.revision = s.revision;
	c.street_revision = s.street_revision;
	unsigned ground = GROUND_FINAL;
	if (parts & CUT_GROUND_BASE) ground |= GROUND_BASE;
	if (parts & CUT_WATER) ground |= GROUND_WATER;
	c.ground = s.terrain.ground_patch(box.minx, box.minz, box.maxx, box.maxz, ground);
	const CutBox roads = parts & CUT_ROADS ? box : NOWHERE, buildings = parts & CUT_BUILDINGS ? box : NOWHERE;
	c.roads = s.roads.cut(roads.minx, roads.minz, roads.maxx, roads.maxz);
	c.buildings = s.buildings.cut(buildings.minx, buildings.minz, buildings.maxx, buildings.maxz);
	return c;
}

} // namespace city
