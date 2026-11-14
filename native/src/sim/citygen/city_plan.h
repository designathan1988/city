// What the land of a generated city is for: its centres from the seed, and the bid-rent curve (dense where the heat is, commerce along the
// main roads, works in a sector of the edge, heavier towards the edge), and its parks: whole patches drawn by the seed, away from the
// centres. Deterministic: the seed gives the same centres as the TypeScript game's plan.
#pragma once

#include "sim/catalog/catalog.h"

#include <array>
#include <cstdint>
#include <vector>

namespace city {

struct CityPlan {
	CityPlanDef def;
	double radius = 0, industry_angle = 0;
	uint32_t seed = 0; // what its parks are drawn by
	std::vector<std::array<double, 2>> centres; // x, y of the map (its north is -z); the first is the main one
};

CityPlan plan_city(uint32_t seed, double radius, const CityPlanDef &def);
// The zone code of a lot at world (x, z); `on_main`: on an avenue or a collector.
int zone_at(const CityPlan &plan, double x, double z, bool on_main);
// Whether the land at world (x, z) is one of the plan's parks: the patches are cells of `park_cell_m`, `park_share` of them drawn by
// the seed, none nearer a centre than `park_from` of the radius. A plan never made (no radius) has none.
bool park_at(const CityPlan &plan, double x, double z);

} // namespace city
