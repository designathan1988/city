// What grows on a lot: whether a lot is free, the record of the building for it (tried set back, narrower and a density lower
// until it fits; at most one civic building per cell; the organic height cap; the quarter's era), how run-down a building is, and
// filling every free lot at once for a generated city.
#pragma once

#include "sim/buildings/building_store.h"
#include "sim/catalog/catalog.h"

#include <optional>
#include <vector>

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
int civic_for(const GrowthDef &g, int cell_id, int use, bool biggest, float area); // `use`: road_gen.h zone_use of the lot's zone

// Is a building standing on the lot? One whose lot holds the lot's middle, or whose middle the lot holds (a ruin and a square too):
// by the rectangles, so a cell cut again by an edit never opens a lot under a standing building, however the new cut falls.
bool lot_occupied(const CityState &s, const Lot &l);
// What growth reads beside the store while a city is filled at once (growth_fill.cpp), so that cells are filled side by side with
// nothing written: the buildings its cell made in the pass under way, their looks by the same index, and the looks of the records
// already in the store by id (a look whose `form` is 0 is not known: the look cache is asked).
struct GrowthBeside {
	const std::vector<Building> *made = nullptr;
	const std::vector<BodyLook> *made_looks = nullptr;
	const std::vector<BodyLook> *looks = nullptr;
};
// The record for lot i of a cell's lot list, or none when nothing fits it. The lot is tried whole, then set back and narrower
// (GrowthDef::tries), at its density and then at each lower one: a try stands when no standing lot lies over it and its plan leaves
// its blocks room. A middling block answers its neighbours' height, the blocks say the storeys, the quarter's era and the street
// side's character are written in the seed (quarter_seed), and of the lot's candidates the one least like what stands round it is
// kept (likeness.h); `look`, when given, takes its look. It only reads the city: the caller holds the map shared.
std::optional<Building> record_for(const CityState &s, const std::vector<Lot> &cell_lots, int i, const GrowthBeside *beside = nullptr, BodyLook *look = nullptr);
// A seed for a building at (x, z) facing `rot`, from `draw`: the character of its street's side in the bits the body reads it in
// (one for the lots of a side within GrowthDef::frontage_cell_m) and its quarter's era (b40/gen.h seed_character, seed_era).
uint32_t quarter_seed(const GrowthDef &g, float x, float z, float rot, uint32_t draw);
// How a record's standing body looks, kept with the body (b40/body_cache.h).
BodyLook look_of(const CityState &s, const Building &rec);
// A building on every free lot (generated city); returns how many were added.
int fill_all(CityState &s);
// City minutes before `now` a generated building was built: spread over one tenth of the decay span.
double generated_age(const GrowthDef &g, uint32_t seed);
// How run-down a building built at city minute `built` is at `now`: 0 as new through the grace, then up to 1 over the span. The one
// reading of a record's age: what draws it (render/buildings) and what tells the player (the inspector) ask this.
float decay_of(const GrowthDef &g, double built, double now);

} // namespace city
