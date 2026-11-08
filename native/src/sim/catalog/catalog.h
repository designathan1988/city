// Game definitions. The single source of truth is project/data/catalog/*.json: api/catalog_loader.cpp fills these
// structs once at startup and rejects missing keys. No values are written here.
#pragma once

#include "sim/catalog/blast_params.h"
#include "sim/catalog/furniture_types.h"
#include "sim/catalog/gen_shapes.h"
#include "sim/catalog/nature_types.h"
#include "sim/catalog/profile_types.h"
#include "sim/catalog/scenic.h"
#include "sim/catalog/tuning_fields.h"
#include "sim/catalog/traffic_params.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace city {

struct Tuning {
	CITY_TUNING_FIELDS(CITY_TUNING_DECLARE, CITY_TUNING_DECLARE_COUNT, CITY_TUNING_DECLARE_DOUBLE)
};

// A map offered by the start screen: its size in 1 km tiles, whether its city is planned on flat ground, that city's radius (0: it
// fills the map), and the scenic city laid out on it instead (an index of Catalog::scenic; -1: none).
struct MapDef {
	std::string id, name_key;
	int tiles = 0;
	bool medium = false;
	double city_radius_m = 0;
	int scenic = -1;
};

// A zone id written on segment sides (Roadcraft src/roads/zones.ts).
// What a zone is at bottom (zones.json "base"): the one owner of the base kinds' codes.
enum BaseZone : uint8_t { BASE_NONE, BASE_RESIDENTIAL, BASE_COMMERCIAL, BASE_INDUSTRIAL, BASE_MIXED, BASE_COUNT };
struct ZoneDef {
	std::string id; // its name is the interface's (zones.json name_key, DataTables.zone_name_key)
	int base = BASE_NONE; // BaseZone
	int density = 1; // 0 low, 1 medium, 2 high
	std::string building; // what the generator builds on it (b40 Kind by name: house, flats, tower, shop, office, works)
	uint32_t color = 0; // RGBA8
};
constexpr int ZONE_CODES = 9; // the zones table's rows (zones.json): the codes a road side and a lot carry
constexpr int BASE_ZONE_SLOTS = BASE_COUNT;
constexpr int ZONE_DENSITIES = 3; // low, medium, high
// The zone codes the generator lays, by what each is (its base and density in zones.json), resolved and checked by the loader.
struct GenZones {
	int none = 0, industrial = 0, mixed = 0;
	int commercial[ZONE_DENSITIES] = {}, residential[ZONE_DENSITIES] = {}; // by density
};

// Residents and jobs of a building: floor area = w * d * floor_share * floors (Roadcraft world/world.ts).
struct OccupancyDef {
	float floor_share = 0, m2_per_resident = 0, m2_per_job_commercial = 0, m2_per_job_industrial = 0, m2_per_person_mixed = 0;
};

// Road price multiplier by how a stretch is built (Roadcraft roads/economy.ts).
struct StructureCost {
	float ground = 0, embankment = 0, trench = 0, bridge = 0, tunnel = 0;
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

// The generated city's road roles, each a profile of road_profiles.json (growth.json road_roles): the loader resolves every one
// and refuses a name the catalogue lacks.
#define CITY_ROAD_ROLES(X) X(local) X(avenue) X(avenue_bike) X(expressway) X(collector) X(wide) X(urban) X(cobble) X(green) X(concrete) \
	X(bike) X(bus_side) X(boulevard) X(av4)
struct RoadRoles {
#define X(role) int role = -1;
	CITY_ROAD_ROLES(X)
#undef X
};

// Which lots become civic buildings, how a building's height answers its neighbours', and the generator's road roles
// (data/catalog/growth.json).
// One way a grown building is fitted to its lot (data/catalog/growth.json "fit"): its depth set back from the front by a share of it,
// its width kept to a share of it, its zone dropped so many densities.
struct FitTry {
	float setback_share = 0, width_share = 1;
	int density_drop = 0;
};

struct GrowthDef {
	CityPlanDef plan;
	RoadRoles roads;
	float fit_inset_m = 0; // a body's ground blocks are tested this far inside themselves against the lot's ring
	std::vector<FitTry> fit_tries; // in order; the first whose body fits the lot stands
	float pick_reach_m = 0; // the next lot grown is drawn among the free ones this near the last
	float civic_min_area = 0, civic_chance = 0;
	std::vector<std::pair<int, int>> civic_by_zone[BASE_ZONE_SLOTS]; // (fn, weight) per base zone
	float organic_reach_m = 0;
	int organic_extra_floors = 0;
	float decay_span_min = 0;
	float occupied_share = 0; // a lot is taken when a building stands within this share of their two half sizes
	GenShapes shapes; // every value a city generator file tunes (growth.json "shapes")
};

struct Catalog {
	GrowthDef growth;
	std::vector<MapDef> maps;
	std::vector<Profile> profiles; // room for every variant is reserved at load: a profile never moves
	std::vector<RoadVariant> variants; // the profiles after the catalogue's own, in order (road_variants.cpp)
	RoadPrices prices;
	StructureCost structure;
	ZoneDef zones[ZONE_CODES];
	GenZones gen_zones; // load_zones
	OccupancyDef occupancy;
	Tuning tune;
	TrafficParams traffic; // data/catalog/traffic.json
	StreetFurnitureDef furniture; // data/catalog/street_furniture.json
	BlastParams blast; // data/catalog/blast.json
	NatureDef nature; // data/catalog/nature.json
	std::vector<ScenicCity> scenic; // data/catalog/scenic.json

	bool ready() const { return !profiles.empty(); }
	const Profile &profile(int id) const { return profiles[size_t(id) < profiles.size() ? size_t(id) : 0]; }
	int profile_id(const std::string &id) const {
		for (size_t i = 0; i < profiles.size(); i++)
			if (profiles[i].id == id) return int(i);
		return -1;
	}
	// The scenic city of a map (null: a generated one), the map index read as generate_map reads it.
	const ScenicCity *scenic_of(int map_index) const {
		const MapDef &m = maps[size_t(map_index) < maps.size() ? size_t(map_index) : 0];
		return m.scenic >= 0 && size_t(m.scenic) < scenic.size() ? &scenic[size_t(m.scenic)] : nullptr;
	}
	const ZoneDef &zone(int code) const { return zones[(code >= 0 && code < ZONE_CODES) ? code : 0]; }
	// The code of the zone of `base` at `density`, -1 when the table has none.
	int zone_code(int base, int density) const {
		for (int c = 0; c < ZONE_CODES; c++)
			if (zones[c].base == base && zones[c].density == density) return c;
		return -1;
	}

	// A profile added: the catalogue's own at load, a variant in play (road_variants.cpp). The one way a profile comes, so what is
	// derived from all of them is kept here as each comes, never searched again per call. Returns its id.
	int add_profile(Profile p) {
		const int id = int(profiles.size());
		derive_from(p, id);
		profiles.push_back(std::move(p));
		profiles_stamp_ = next_profiles_stamp();
		return id;
	}
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
