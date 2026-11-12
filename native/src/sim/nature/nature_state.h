// The landscape's state in the CityState: the weather, the placed clouds, the fog banks painted, the painted ground,
// the planted trees and their clearings, and the map's ecosystem. Commands change it (sim/commands/nature_commands.cpp),
// the save writes it (sim/persistence/save_nature.cpp), the render reads it.
#pragma once

#include "sim/nature/ecology.h"
#include "sim/nature/elements.h"
#include "sim/nature/fog_paint.h"
#include "sim/nature/ground_paint.h"
#include "sim/nature/planted_trees.h"
#include "sim/nature/sky.h"

#include <cstdint>
#include <vector>

namespace city {

struct NatureState {
	Weather weather;
	std::vector<PlacedCloud> clouds;
	int32_t next_cloud = 1;
	FogPaint fog;
	GroundPaint ground;
	PlantedTrees planted;
	Elements elements;
	NatureSettings nature;
	uint64_t sky_revision = 0; // weather, clouds or fog changed: the render reads them again
	uint64_t fog_revision = 0;
	void reset(const Weather &w, int region, uint32_t seed); // a new map's
};

} // namespace city
