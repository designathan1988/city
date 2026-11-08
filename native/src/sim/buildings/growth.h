// What grows on a lot: the record of the building for a proposed lot (floors by zone
// and street, one square per 2x2-cell neighbourhood, at most one civic building per cell, the organic height cap)
// and filling every free lot at once for a generated city.
#pragma once

#include "sim/buildings/building_store.h"
#include "sim/catalog/catalog.h"

namespace city {

struct CityState;
struct Lot;

// Map-relative cell coordinates (x from the map's west edge) of a world position.
struct MapCell {
	int x = 0, z = 0, id = 0;
};
MapCell map_cell_of(const CityState &s, double x, double z);

// The storeys of a record's body: what its `floors` must say.
int body_floors(const b40::Data &data, const Building &rec);
int civic_for(const GrowthDef &g, int cell_id, int base_zone, bool biggest, float area);
int base_zone(const Catalog &c, int zone_id); // the zone's base kind (zones.json "base", BaseZone)

// Is a building standing on the lot (any record within 0.8 of the two half sizes)?
bool lot_occupied(const CityState &s, const Lot &l);
// Record for lot i of a cell's lot list: a middling block answers its neighbours' height, the body says the storeys, and of the
// lot's candidates the one least like what stands round it is kept (likeness.h). The body is fitted to the lot's ring (the player's
// lot of any shape, else the lot's rectangle): the growth data's fit tries in order (as drawn, set back, narrower, a density lower),
// the first whose ground blocks lie inside the ring kept, the last when none does.
Building record_for(CityState &s, const std::vector<Lot> &cell_lots, int i);
// How a record's body looks, read once (BuildingStore::looks).
BodyLook look_of(CityState &s, const Building &rec);
// A building on every free lot (generated city); returns how many were added.
int fill_all(CityState &s);
// City minutes before `now` a generated building was built: spread over one tenth of the decay span.
double generated_age(const GrowthDef &g, uint32_t seed);

} // namespace city
