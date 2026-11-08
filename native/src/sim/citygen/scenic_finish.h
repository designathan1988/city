// What a scenic city takes once its streets, lots and buildings stand: the trees of its parks (a shaken lattice in groves,
// off the roads, the water and the buildings) and the names of its named roads, each on the street its line runs on, before
// the rest of the streets take names from the style's list.
#pragma once

#include <cstdint>

namespace city {

struct CityState;
struct ScenicCity;

// Plants the parks' trees; returns how many. Never past the planted trees' cap (data/catalog/nature.json).
int plant_parks(CityState &s, const ScenicCity &city, uint32_t seed);
// Puts a name item on the street of each named road; returns how many.
int name_scenic_roads(CityState &s, const ScenicCity &city);

} // namespace city
