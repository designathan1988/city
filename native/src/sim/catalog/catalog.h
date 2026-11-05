// Game definitions. The single source of truth is project/data/catalog/*.json: api/catalog_loader.cpp fills these
// structs once at startup and rejects missing keys. No values are written here.
#pragma once

#include "sim/catalog/furniture_types.h"
#include "sim/catalog/nature_types.h"
#include "sim/catalog/profile_types.h"
#include "sim/catalog/tuning_fields.h"
#include "sim/catalog/traffic_params.h"

#include <cstdint>
#include <string>
#include <vector>

namespace city {

struct Tuning {
	CITY_TUNING_FIELDS(CITY_TUNING_DECLARE)
};

// A map offered by the start screen: its size in 1 km tiles, whether its city is planned on flat ground, and that city's radius (0: it
// fills the map).
struct MapDef {
	std::string id, name_key;
	int tiles = 0;
	bool medium = false;
	double city_radius_m = 0;
};

// A zone id written on segment sides (Roadcraft src/roads/zones.ts).
struct ZoneDef {
	std::string id, name_key;
	int base = 0; // 0 none, 1 residential, 2 commercial, 3 industrial, 4 mixed
	int density = 1; // 0 low, 1 medium, 2 high
	uint32_t color = 0; // RGBA8
};
constexpr int ZONE_CODES = 9;
constexpr int BASE_ZONE_SLOTS = 5; // none, residential, commercial, industrial, mixed

// Residents and jobs of a building: floor area = w * d * floor_share * floors (Roadcraft world/world.ts).
struct OccupancyDef {
	float floor_share = 0, m2_per_resident = 0, m2_per_job_commercial = 0, m2_per_job_industrial = 0, m2_per_person_mixed = 0;
};

// Road price multiplier by how a stretch is built (Roadcraft roads/economy.ts).
struct StructureCost {
	float ground = 0, embankment = 0, trench = 0, bridge = 0, tunnel = 0;
};

// A drawn integer range [lo, hi) (Roadcraft Rng.int is exclusive of hi).
struct IntRange {
	int lo = 0, hi = 0;
};

// The plan of a generated city (data/catalog/growth.json "plan", sim/citygen/city_plan.h).
struct CityPlanDef {
	double main_offset = 0, centre_turn = 0, second_from = 0, second_to = 0, second_pull = 0;
	double density = 0, density_pull = 0, noise_cell_m = 0, noise_heat = 0, high_heat = 0, medium_heat = 0, commerce_heat = 0, main_commerce_heat = 0;
	double industry_from = 0, industry_half_angle = 0;
	double square_cell_m = 0, square_touch_m = 0, square_front_m = 0, square_area_low = 0, square_area_high = 0, square_walk_half_m = 0, square_plaza_m = 0;
	double square_tree_grid_m = 0, square_tree_edge_m = 0, square_tree_low_m = 0, square_tree_high_m = 0;
	std::vector<std::string> square_main_roads;
	int centres = 0;
};

// Which lots become civic buildings and how a building's height answers its neighbours' (data/catalog/growth.json).
struct GrowthDef {
	CityPlanDef plan;
	float civic_min_area = 0, civic_chance = 0;
	std::vector<std::pair<int, int>> civic_by_zone[BASE_ZONE_SLOTS]; // (fn, weight) per base zone
	float organic_reach_m = 0;
	int organic_extra_floors = 0;
	float decay_grace_min = 0, decay_span_min = 0;
};

struct Catalog {
	GrowthDef growth;
	std::vector<MapDef> maps;
	std::vector<Profile> profiles; // room for every variant is reserved at load: a profile never moves
	std::vector<RoadVariant> variants; // the profiles after the catalogue's own, in order (road_variants.cpp)
	RoadPrices prices;
	StructureCost structure;
	ZoneDef zones[ZONE_CODES];
	OccupancyDef occupancy;
	Tuning tune;
	TrafficParams traffic; // data/catalog/traffic.json
	StreetFurnitureDef furniture; // data/catalog/street_furniture.json
	NatureDef nature; // data/catalog/nature.json

	bool ready() const { return !profiles.empty(); }
	const Profile &profile(int id) const { return profiles[size_t(id) < profiles.size() ? size_t(id) : 0]; }
	int profile_id(const std::string &id) const {
		for (size_t i = 0; i < profiles.size(); i++)
			if (profiles[i].id == id) return int(i);
		return -1;
	}
	const ZoneDef &zone(int code) const { return zones[(code >= 0 && code < ZONE_CODES) ? code : 0]; }
	float widest_half() const {
		float w = 0;
		for (const Profile &p : profiles) w = w > p.half() ? w : p.half();
		return w;
	}
};

} // namespace city
