// The ground the effects lie on: the street's own surface where there is one (sim/transit/transit_ground.h transit_paved: the
// carriageway, the footway a kerb higher), else the terrain. A road stands over the terrain under it (its deck), so a piece
// of debris, a body, a pool of blood, soot, a crater or a heaved plate laid on the terrain there would lie under the asphalt.
// `at` is read by small cells and kept while the map stands as it is (its revision): a piece in the air reads the ground under
// its corners every step, and a street's surface is a search of the segments near. `laid` is the same for a flat mark over the
// ground: on bare earth the natural ground, the rim of any pit dug in it.
#pragma once

#include "sim/state/city_state.h"

#include <cstdint>
#include <unordered_map>

namespace city {

class GroundCells {
public:
	float at(const CityState &s, float x, float z); // the ground under (x, z), kept by its cell
	static float surface(const CityState &s, float x, float z); // the same, read now
	static float laid(const CityState &s, float x, float z); // where a flat mark lies: the street, else the natural ground
	void forget() { cells_.clear(); }

private:
	std::unordered_map<uint64_t, float> cells_;
	uint64_t revision_ = ~uint64_t(0);
};

} // namespace city
