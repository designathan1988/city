// A new map (Roadcraft app/cityFlows.ts generatedCity / newMapSession): the land from the seed, then, for a
// generated city, its streets inserted into the network (free of charge), the land shaped under them, the lots cut
// and a building on every lot. An empty map stops after the land.
#pragma once

#include "sim/terrain/terrain_gen.h"

#include <cstdint>
#include <memory>

namespace city {

struct CityState;

struct CityGenReport {
	int nodes = 0, segments = 0, lots = 0, buildings = 0;
	double seconds = 0;
	int squares = 0;
};

// map_index: entry of data/catalog/maps.json; with_city = false makes an empty map.
// The land of a map from its exact seed (flat bit included) with nothing on it: empty roads, buildings, history.
// `generate` false only sizes the land (a file being checked: nothing reads its heights or water). Returns the generator that
// filled the land (null when it was only sized), for what is laid on that land next: its river and lakes are planned once.
std::unique_ptr<TerrainGen> found_land(CityState &s, int map_index, uint32_t exact_seed, bool generate = true, int tiles = 0); // tiles 0: the map's own; a save says what it was made on
// Queues every lot cell of the map for the lot cache (a generated or loaded city).
void mark_all_lot_cells(CityState &s);
CityGenReport generate_map(CityState &s, int map_index, uint32_t seed, bool with_city);

} // namespace city
