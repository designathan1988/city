// The ecosystem: what grows where, read from the land (the fitofisionomias of each Brazilian biome). A site is measured
// at every corner of a grid (distance to water, valley or ridge against the ground round it, slope, height over the
// sea, two seeded patches); each biome over the corner, the painted ones and the map's own the rest, mixes its plant
// communities by the site; the result is the density of every stratum. A chunk's answer is worked out where it is
// read (render/trees, render/cover) from the heights and the paint: nothing of it is stored or saved.
#pragma once

#include "sim/catalog/nature_types.h"
#include "sim/nature/ground_paint.h"
#include "sim/terrain/heightfield.h"

#include <cstdint>
#include <vector>

namespace city {

// A map's ecosystem: its biome (NONE: no ecosystem, only the woods of the patches) and the seed of its patches.
struct NatureSettings {
	int region = int(Biome::NONE);
	uint32_t seed = 0;
};

// What grows over a square of the map: `side` corners a side, `cell` apart, the first at (x0, z0).
struct EcologyField {
	int side = 0;
	float x0 = 0, z0 = 0, cell = 1;
	std::vector<Community> at;
	const Community &near(float x, float z) const;
};

// The ecosystem over the square from (x0, z0), `span` metres a side, in cells of `def.cell_m` (a margin of
// `def.mean_cells` is read round it so valleys and ridges see past the edge).
EcologyField compute_ecology(const Heightfield &hf, const GroundPaint &paint, const NatureSettings &nature, const EcologyDef &def,
		float x0, float z0, float span);

// How much a corner makes a wood: the strata that stand as trees.
float wood_share(const Community &c, const EcologyDef &def);

} // namespace city
