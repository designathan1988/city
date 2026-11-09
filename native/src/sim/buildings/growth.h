// What grows on a lot: the record of the building for a proposed lot (floors by zone
// and street, one square per 2x2-cell neighbourhood, at most one civic building per cell, the organic height cap)
// and filling every free lot at once for a generated city.
#pragma once

#include "sim/buildings/building_store.h"
#include "sim/buildings/likeness.h"
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

// The side of a lot with a street just past it over most of its depth (Roadcraft zoning.ts cornerOf): a b40::LotCorner.
uint8_t corner_of(const CityState &s, const Lot &l);
// Is a building standing on the lot (any record within 0.8 of the two half sizes)?
bool lot_occupied(const CityState &s, const Lot &l);
// Record for lot i of a cell's lot list: a middling block answers its neighbours' height, the body says the storeys, and of the
// lot's candidates the one least like what stands round it is kept (likeness.h).
Building record_for(CityState &s, const std::vector<Lot> &cell_lots, int i);
// How a record's standing body looks, kept with the body (b40/body_cache.h).
BodyLook look_of(const CityState &s, const Building &rec);
// A building on every free lot (generated city); returns how many were added.
int fill_all(CityState &s);
// City minutes before `now` a generated building was built: spread over one tenth of the decay span.
double generated_age(const GrowthDef &g, uint32_t seed);

} // namespace city
