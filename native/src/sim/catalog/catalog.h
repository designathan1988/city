// Game definitions. The single source of truth is project/data/catalog/*.json: api/catalog_loader.cpp fills these
// structs once at startup and rejects missing keys. No values are written here.
#pragma once

#include "sim/catalog/blast_params.h"
#include "sim/catalog/furniture_types.h"
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

// A map offered by the start screen: its size in 1 km tiles and whether the generated city is the medium town.
struct MapDef {
	std::string id, name_key;
	int tiles = 0;
	bool medium = false;
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

// How many floors a grown building gets and which lots become civic buildings (data/catalog/growth.json).
struct GrowthDef {
	int avenue_priority = 0;
	IntRange low_res, low_other, high_res, high_res_avenue, high_other, high_other_avenue, res_avenue;
	float res_split[2] = {};
	IntRange res_bands[3], commercial, commercial_avenue, industrial, mixed, mixed_avenue;
	float civic_min_area = 0, civic_chance = 0, square_min_area = 0;
	std::vector<std::pair<int, int>> civic_by_zone[BASE_ZONE_SLOTS]; // (fn, weight) per base zone
	float organic_reach_m = 0;
	int organic_extra_floors = 0;
	float decay_grace_min = 0, decay_span_min = 0;
};

struct Catalog {
	GrowthDef growth;
	std::vector<MapDef> maps;
	std::vector<Profile> profiles;
	RoadPrices prices;
	StructureCost structure;
	ZoneDef zones[ZONE_CODES];
	OccupancyDef occupancy;
	Tuning tune;
	TrafficParams traffic; // data/catalog/traffic.json
	StreetFurnitureDef furniture; // data/catalog/street_furniture.json
	BlastParams blast; // data/catalog/blast.json

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
