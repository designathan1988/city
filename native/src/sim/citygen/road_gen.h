// The generated city's streets (the lab's world/cityGen/plan.ts blockGrid, editor/cityGenerator.ts layCity): one straight grid of
// blocks round the map's middle (growth.json shapes.roads), avenues every few lines, collectors between, local streets the rest,
// each laid as its level's profile (growth.json road_roles); water and steep slopes cut it, a planar graph is built (crossings,
// merged nodes) and zones are painted by the city's plan (city_plan.h). Deterministic: the same seed gives the same graph.
#pragma once

#include "sim/catalog/gen_shapes.h"
#include "sim/citygen/city_plan.h"
#include "sim/terrain/terrain_gen.h"

#include <cstdint>
#include <vector>

namespace city {

// The profile ids the generator uses, looked up by name in the catalogue (data/catalog/road_profiles.json).
// The road roles (growth.json road_roles, by profile id) and each profile's asphalt to the right of its centre.
struct GenProfiles : RoadRoles {
	std::vector<double> asph_right; // per profile id
};

constexpr int NODE_FIELDS = 3, SEG_FIELDS = 5; // a node's numbers (x, z, y) and a segment's (a, b, profile, zone left, zone right)

struct GeneratedRoads {
	std::vector<double> nodes; // x, z, y per node
	std::vector<int> segs; // a, b, profile, zone left, zone right per segment
	double centre_r = 0; // the one centre's radius (0.85 R)
	int node_count() const { return int(nodes.size() / NODE_FIELDS); }
	int seg_count() const { return int(segs.size() / SEG_FIELDS); }
};

// `radius`: the city's, 0 for one that fills the map.
GeneratedRoads generate_roads(uint32_t seed, double map_half, double radius, TerrainGen &terrain, const GenProfiles &prof, const CityPlanDef &plan,
	const GenZones &zones, const GenShapes &S);

// Internals shared by the generator's files (and the scenic city's, sim/citygen/scenic_roads.h).
namespace road_gen {
struct Piece {
	double x0, z0, x1, z1;
	int profile;
};

// What the graph builder asks of the city it builds: which of two roads keeps an edge or a sharp node (the higher rank),
// which street goes when it hugs a stronger one, what the land beside a street of a profile is zoned, and the section a
// street finally takes (its zone, its distance share of the centre and its middle given).
class GraphRules {
public:
	virtual ~GraphRules() = default;
	virtual int rank(int profile) const = 0;
	virtual bool yields(int profile) const = 0;
	virtual int zone_beside(int profile, double x, double z) const = 0;
	virtual int section(int profile, int zone, double centre_share, double x, double z) const = 0;
};

bool land(const GenShapes &S, TerrainGen &t, double x, double z);
// `asph_right`: each profile's half carriageway (the tidy pass's reach onto a road).
GeneratedRoads build_graph(const GenShapes &S, const std::vector<Piece> &pieces, double centre_r, TerrainGen &terrain, const std::vector<double> &asph_right,
	const GraphRules &rules);
void tidy_graph(const GenShapes &S, std::vector<double> &nx, std::vector<double> &nz, std::vector<double> &ny, std::vector<int> &segs,
		const std::vector<int> &strength, TerrainGen &terrain, const std::vector<double> &asph_right);
GeneratedRoads dry_crossings(const GenShapes &S, const GeneratedRoads &r, TerrainGen &terrain);
GeneratedRoads without_spurs(const GeneratedRoads &r);
} // namespace road_gen

} // namespace city
