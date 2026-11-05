// A new map (Roadcraft app/cityFlows.ts generatedCity / newMapSession): the land from the seed, then, for a
// generated city, its streets inserted into the network (free of charge), the land shaped under them, the lots cut
// and a building on every lot. An empty map stops after the land.
#pragma once

#include <cstdint>

namespace city {

struct CityState;

struct CityGenReport {
	int nodes = 0, segments = 0, lots = 0, buildings = 0;
	double seconds = 0;
	int squares = 0;
};

// map_index: entry of data/catalog/maps.json; with_city = false makes an empty map.
// The land of a map from its exact seed (flat bit included) with nothing on it: empty roads, buildings, history.
void found_land(CityState &s, int map_index, uint32_t exact_seed);
// Queues every lot cell of the map for the lot cache (a generated or loaded city).
void mark_all_lot_cells(CityState &s);
CityGenReport generate_map(CityState &s, int map_index, uint32_t seed, bool with_city);

} // namespace city
