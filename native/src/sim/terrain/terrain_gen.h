// The generated land of a new map, Roadcraft's src/jobs/terrainGen.ts: broad hills, a straight canal 300 m in from
// the north (+z) edge, lakes filling the basins of a 2200 m grid, ranges along the edges. Deterministic from the seed
// (the same map as the TypeScript game). A seed with FLAT_BIT is a level plain at PLAIN_Y without water.
#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace city {

class TerrainGen {
public:
	static constexpr uint32_t FLAT_BIT = 0x80000000U;
	static constexpr double PLAIN_Y = 20;
	static constexpr double OCEAN_Y = 0; // the sea plane; the visual palette reads the same level
	static constexpr double MIN_Y = -64, MAX_Y = 400;
	
	static constexpr float NO_WATER = -1000.f;

	TerrainGen(uint32_t seed, double map_half, double water_spacing);
	bool flat() const { return (seed_ & FLAT_BIT) != 0; }
	// Ground height at (x, z): the land cut by the canal's bed.
	double base_height(double x, double z);
	// Water level at the water-grid sample (gx, gz) (river or lake), or NO_WATER.
	float water_at_sample(int gx, int gz);
	// The nearest water sample's level (the city generator's test).
	float water_level_at(double x, double z);

private:
	struct Lake {
		double level = 0;
		std::unordered_map<int64_t, float> flooded; // key gz * 65536 + gx
	};
	struct River {
		double level = 0, floor = 0, section = 0, d = 0, reach = 0;
	};
	uint32_t seed_;
	double half_, water_spacing_;
	std::vector<double> course_z_, level_, depth_;
	std::unordered_map<int64_t, std::unique_ptr<Lake>> lakes_; // null entry: the square has no lake

	double course_z() const;
	bool basin_of(int i, int j, double &x, double &z) const;
	double land(double x, double z) const;
	void plan_river();
	River river_at(double x, double z) const;
	const Lake *lake_of(int i, int j);
	void flood(Lake &lake, int gx, int gz, double bottom);
};

} // namespace city
