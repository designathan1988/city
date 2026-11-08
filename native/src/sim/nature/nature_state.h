// The landscape's state in the CityState: the weather, the placed clouds, the fog banks painted, the painted ground,
// the planted trees and their clearings, and the map's ecosystem. Commands change it (sim/commands/nature_commands.cpp),
// the save writes it (sim/persistence/save_nature.cpp), the render reads it.
#pragma once

#include "sim/nature/ecology.h"
#include "sim/nature/elements.h"
#include "sim/nature/ground_paint.h"
#include "sim/nature/planted_trees.h"
#include "sim/nature/sky.h"

#include <cstdint>
#include <vector>

namespace city {

// One dab of painted fog: how far it takes the fog towards full at its centre, how high the bank stands and how fast
// the wind carries it through.
struct FogDab {
	float x = 0, z = 0, radius = 0, strength = 0, height = 0, speed = 0;
	uint8_t erase = 0;
};

struct FogSample {
	float density = 0, height = 0, speed = 0;
};
// The fog the dabs listed by `which` (indices into `dabs`, in the order they were laid) leave at a point: each in turn, a smooth
// falloff from its centre to its rim, adding towards one (or, erasing, taking away); height and speed follow the dabs by what each
// laid.
void fog_at(const std::vector<FogDab> &dabs, const std::vector<int> &which, float x, float z, const FogDef &d, FogSample &out);

struct NatureState {
	Weather weather;
	std::vector<PlacedCloud> clouds;
	int32_t next_cloud = 1;
	std::vector<FogDab> fog;
	GroundPaint ground;
	PlantedTrees planted;
	Elements elements;
	NatureSettings nature;
	uint64_t sky_revision = 0; // weather, clouds or fog changed: the render reads them again
	uint64_t fog_revision = 0;
	void reset(const Weather &w, int region, uint32_t seed); // a new map's
};

} // namespace city
