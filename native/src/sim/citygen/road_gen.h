// The generated city's streets (Roadcraft src/jobs/cityGen.ts + cityField.ts): one straight grid of 120 x 72 m blocks
// round the map's middle, avenues every 4 lines, collectors between, local streets the rest; water and steep slopes
// cut it, a planar graph is built (crossings, merged nodes), zones are painted by the distance to the centre and the
// street profiles vary by district noise. Deterministic: the same seed gives the TypeScript game's graph.
#pragma once

#include "sim/citygen/terrain_gen.h"

#include <cstdint>
#include <vector>

namespace city {

// Zone ids written on segment sides (Roadcraft src/roads/zones.ts); their look and names are data (zones.json).
enum Zone : int { ZONE_NONE = 0, ZONE_RES = 1, ZONE_COM = 2, ZONE_IND = 3, ZONE_MIXED = 4, ZONE_RES_LOW = 5, ZONE_RES_HIGH = 6,
	ZONE_COM_LOW = 7, ZONE_COM_HIGH = 8, ZONE_COUNT = 9 };

// The profile ids the generator uses, looked up by name in the catalogue (data/catalog/road_profiles.json).
struct GenProfiles {
	int local = 0, avenue = 0, avenue_bike = 0, expressway = 0, collector = 0;
	int wide = 0, urban = 0, cobble = 0, green = 0, concrete = 0, bike = 0, bus_side = 0, boulevard = 0, av4 = 0;
	std::vector<double> asph_right; // per profile id
};

struct GeneratedRoads {
	std::vector<double> nodes; // x, z, y per node
	std::vector<int> segs; // a, b, profile, zone left, zone right per segment
	double centre_r = 0; // the one centre's radius (0.85 R)
	int node_count() const { return int(nodes.size() / 3); }
	int seg_count() const { return int(segs.size() / 5); }
};

constexpr int SEG_FIELDS = 5;

GeneratedRoads generate_roads(uint32_t seed, double map_half, bool medium, TerrainGen &terrain, const GenProfiles &prof);

// Internals shared by the generator's files.
namespace road_gen {
struct Piece {
	double x0, z0, x1, z1;
	int profile;
};
bool land(TerrainGen &t, double x, double z);
GeneratedRoads build_graph(const std::vector<Piece> &pieces, double centre_r, uint32_t seed, TerrainGen &terrain, const GenProfiles &prof);
void tidy_graph(std::vector<double> &nx, std::vector<double> &nz, std::vector<double> &ny, std::vector<int> &segs,
		const std::vector<int> &strength, TerrainGen &terrain, const GenProfiles &prof);
GeneratedRoads dry_crossings(const GeneratedRoads &r, TerrainGen &terrain);
GeneratedRoads without_spurs(const GeneratedRoads &r);
} // namespace road_gen

} // namespace city
