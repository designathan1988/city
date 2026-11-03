// What grows on a lot (Roadcraft src/world/growth.ts): the record of the building for a proposed lot (floors by zone
// and street, one square per 2x2-cell neighbourhood, at most one civic building per cell, the organic height cap)
// and filling every free lot at once for a generated city.
#pragma once

#include "sim/catalog/catalog.h"

namespace city {

struct CityState;
struct Lot;

// Map-relative cell coordinates (Roadcraft cellId: x from the map's west edge) of a world position.
struct MapCell {
	int x = 0, z = 0, id = 0;
};
MapCell map_cell_of(const CityState &s, double x, double z);

int floors_for(const GrowthDef &g, int zone_id, const Profile &street, uint32_t seed);
int civic_for(const GrowthDef &g, int cell_id, int base_zone, bool biggest, float area);
int base_zone(int zone_id);
int zone_density(int zone_id);

// Is a building standing on the lot (any record within 0.8 of the two half sizes)?
bool lot_occupied(const CityState &s, const Lot &l);
// Record for lot i of a cell's lot list (organic: capped by the neighbours' height).
struct Building;
Building record_for(const CityState &s, const std::vector<Lot> &cell_lots, int i, bool organic);
// A building on every free lot (generated city); returns how many were added.
int fill_all(CityState &s);
// City minutes before `now` a generated building was built: spread over one tenth of the decay span.
double generated_age(const GrowthDef &g, uint32_t seed);

} // namespace city
