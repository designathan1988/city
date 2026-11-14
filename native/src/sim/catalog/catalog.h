// Game definitions. The single source of truth is project/data/catalog/*.json: api/catalog_loader.cpp fills these
// structs once at startup and rejects missing keys. No values are written here.
#pragma once

#include "sim/catalog/blast_params.h"
#include "sim/catalog/furniture_types.h"
#include "sim/catalog/nature_types.h"
#include "sim/catalog/profile_types.h"
#include "sim/catalog/tuning_fields.h"
#include "sim/catalog/traffic_params.h"
#include "sim/catalog/zone_codes.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <utility>
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
	double industry_from = 0, industry_half_angle = 0, industry_high_from = 0;
	double park_share = 0, park_cell_m = 0, park_from = 0; // the share of the plan's patches that are parks, a patch's side, and how near a centre one may be
	double square_cell_m = 0, square_touch_m = 0, square_front_m = 0, square_area_low = 0, square_area_high = 0, square_walk_half_m = 0, square_plaza_m = 0;
	double square_tree_grid_m = 0, square_tree_edge_m = 0, square_tree_low_m = 0, square_tree_high_m = 0;
	std::vector<std::string> square_main_roads;
	int centres = 0;
};

// One way a building is tried on a lot something stands partly over, or too small for its plan: set back from the lot's front, in
// from each side, and made of a share of what is left (data/catalog/growth.json "fit").
struct LotTry {
	float front_m = 0, side_m = 0, share = 1;
};

// Which lots become civic buildings and how a building's height answers its neighbours' (data/catalog/growth.json).
struct GrowthDef {
	CityPlanDef plan;
	// "quarters": the side of the cells one street side's character and one quarter's era are drawn in, how far from the map's
	// middle the old core reaches, and the shares of old, middle and new quarters in the core and outside it.
	static constexpr int ERAS = 3;
	float frontage_cell_m = 0, era_cell_m = 0, old_core_m = 0, core_eras[ERAS] = {}, edge_eras[ERAS] = {};
	std::vector<LotTry> tries; // "fit": tried in order, each at the lot's density, then at each lower one
	float least_room_m = 0; // the least side of the room a lot's plan must leave a building's blocks
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
	BlastParams blast; // data/catalog/blast.json
	NatureDef nature; // data/catalog/nature.json

	bool ready() const { return !profiles.empty(); }
	const Profile &profile(int id) const { return profiles[size_t(id) < profiles.size() ? size_t(id) : 0]; }
	int profile_id(const std::string &id) const {
		for (size_t i = 0; i < profiles.size(); i++)
			if (profiles[i].id == id) return int(i);
		return -1;
	}
	const ZoneDef &zone(int code) const { return zones[(code >= 0 && code < ZONE_CODES) ? code : 0]; }

	// A profile added: the catalogue's own at load, a variant in play (road_variants.cpp). The one way a profile comes, so what is
	// derived from all of them is kept here as each comes, never searched again per call. Returns its id.
	int add_profile(Profile p) {
		const int id = int(profiles.size());
		derive_from(p, id);
		profiles.push_back(std::move(p));
		profiles_stamp_ = next_profiles_stamp();
		return id;
	}
	// Room for every variant a city may hold (tuning road_max_variants) past the catalogue's own, so a profile never moves and
	// road_variant refuses only past that limit. A copy of the vector keeps its size, not its room: the loader calls this once the
	// catalogue's own are read, and whoever copies the catalogue (a city read aside: city_core_persist.cpp) calls it after the copy.
	void keep_variant_room() { profiles.reserve(profiles.size() - variants.size() + size_t(tune.road_max_variants)); }
	// The variants dropped (a new city): what is derived is worked out again over the catalogue's own.
	void drop_variants() {
		profiles.resize(profiles.size() - variants.size());
		variants.clear();
		widest_half_ = widest_sidewalk_half_ = 0;
		ring_profile_ = -1;
		for (size_t i = 0; i < profiles.size(); i++) derive_from(profiles[i], int(i));
		profiles_stamp_ = next_profiles_stamp();
	}
	// Which list of profiles this is: it moves with each profile added and each drop, and never comes back, in this catalogue or
	// another (a city opened brings its own), so what a reader kept of the list holds while it stays (the script's road list).
	uint64_t profiles_stamp() const { return profiles_stamp_; }
	// The widest reach either side of a centre line among the profiles.
	float widest_half() const { return widest_half_; }
	// The widest half of a kerbside sidewalk (the first or last strip) among the profiles.
	float widest_sidewalk_half() const { return widest_sidewalk_half_; }
	// The first profile a roundabout's ring is laid with, -1 when the catalogue has none.
	int ring_profile() const { return ring_profile_; }

private:
	void derive_from(const Profile &p, int id) {
		widest_half_ = widest_half_ > p.half() ? widest_half_ : p.half();
		if (!p.edges.empty())
			for (const StripEdge *e : { &p.edges.front(), &p.edges.back() })
				if (e->strip.kind == StripKind::SIDEWALK) {
					const float half = (e->u1 - e->u0) * HALF_OF;
					widest_sidewalk_half_ = widest_sidewalk_half_ > half ? widest_sidewalk_half_ : half;
				}
		if (ring_profile_ < 0 && p.ring) ring_profile_ = id;
	}
	static uint64_t next_profiles_stamp() {
		static std::atomic<uint64_t> next{ 1 };
		return next.fetch_add(1, std::memory_order_relaxed);
	}
	static constexpr float HALF_OF = 0.5f;
	float widest_half_ = 0, widest_sidewalk_half_ = 0;
	int ring_profile_ = -1;
	uint64_t profiles_stamp_ = 0;
};

} // namespace city
