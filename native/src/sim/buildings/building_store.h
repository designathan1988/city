// Building records: small and flat; everything drawn is derived from them.
// A record with floors == 0 is an empty lot. Ids come from a free list; queries go through a spatial hash.
#pragma once

#include "core/grid.h"
#include "core/slot_ids.h"
#include "sim/buildings/b40/data.h"
#include "sim/buildings/likeness.h"
#include "sim/state/change_log.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

struct Catalog;

// What a building is for (the ids saved with a record, new ones at the end). FN_NONE: its kind and seed say.
enum BuildingFn : uint8_t { FN_NONE = 0, FN_SCHOOL, FN_CHURCH, FN_HOSPITAL, FN_CLINIC, FN_SQUARE, FN_PARK, FN_GAS_STATION,
	FN_SUPERMARKET, FN_CITY_HALL, FN_POLICE, FN_FIRE_STATION, FN_MALL, FN_SPORTS_COURT, FN_COUNCIL, FN_COURTHOUSE, FN_POST_OFFICE,
	FN_UNIVERSITY, FN_LIBRARY, FN_MUSEUM, FN_PRISON, FN_CEMETERY, FN_BUS_STATION, FN_SHOP, FN_BANK, FN_PHARMACY, FN_BAKERY,
	FN_RESTAURANT, FN_SNACK_BAR, FN_BAR, FN_NIGHTCLUB, FN_CINEMA, FN_HOTEL, FN_GYM, FN_CLUB, FN_OFFICE, FN_FACTORY, FN_WAREHOUSE,
	FN_PLAYGROUND, FN_COUNT };

struct Building {
	float x = 0, z = 0, w = 0, d = 0, rot = 0; // lot centre, frontage, depth; local +z faces the street
	uint8_t zone = 0; // base zone (1..4) or a density zone (5..8)
	uint8_t fn = 0; // BuildingFn
	uint16_t floors = 0; // 0 = empty lot
	uint32_t seed = 0;
	uint16_t style = 0; // 0..127 automatic, 128 + arch = chosen
	uint16_t profile = 0; // profile of the street it faces
	int16_t landmark = -1; // catalogue project (data/catalog/landmarks) or -1
	bool alive = false;
	b40::BodyRef body; // the version the player edited, held by the record (b40::BodyTable); none = made from the seed
	double built = 0; // city minute it was built (decay and renovation)
};

class BuildingStore {
public:
	std::vector<Building> items;
	SlotIds free_ids;
	SpatialHash hash; // by lot centre
	ChangeLog *log = nullptr;
	int count = 0;
	// Residents and jobs of the alive records (occupancy), kept by place/remove, the one way a record comes or goes: never
	// counted again over every record, and right after growth, edits and loads alike (the snapshot reads them).
	int residents = 0, jobs = 0;
	b40::Data designs; // the generator's tables (set once at boot, read-only after): every body is made from them
	// How each body looks (likeness.h), by b40::body_key: derived, never saved. Only growth writes it, under the map's exclusive lock.
	std::unordered_map<b40::BodyKey, BodyLook, b40::BodyKeyHash> looks;

	void init(ChangeLog *l, const Catalog *catalog);
	// The same links set again, nothing cleared: the store moved in with a city read aside (CityState::bind).
	void attach(ChangeLog *l, const Catalog *catalog) {
		log = l;
		catalog_ = catalog;
	}
	void clear();
	int add(const Building &b);
	void remove(int id);
	// Undo / redo / load: the record goes back under the same id. A building that
	// grew there meanwhile (outside the history) moves to a new id; `evict` also knocks down what grew on the lot.
	void put(int id, const Building &b, bool evict);
	// Does the point stand on the building's lot (its rectangle grown by pad)?
	static bool covers(const Building &b, float x, float z, float pad);
	void mark(int id) const;
	void mark_look(int id) const; // its chunks drawn again though it did not change: another look (Interior layer)
	template <typename F>
	void query(float minx, float minz, float maxx, float maxz, F &&fn) const {
		hash.query(minx, minz, maxx, maxz, [&](int id) {
			const Building &b = items[id];
			if (b.alive && b.x >= minx && b.x <= maxx && b.z >= minz && b.z <= maxz) fn(id);
		});
	}
	// Residents and jobs from the floor area.
	// `base`: the zone's base kind (the catalogue's, BaseZone), which says who lives or works there.
	static void occupancy(const Building &b, int base, float floor_share, const float per_m2[4], int &residents, int &jobs);

private:
	const Catalog *catalog_ = nullptr; // its occupancy and zones, set by init
	void place(int id, const Building &b);
	void tally(const Building &b, int sign); // the record's residents and jobs into (+1) or out of (-1) the totals
	void mark_into(int id, std::vector<int64_t> &out) const;
};

constexpr float BUILDING_HASH_CELL = 64.f;
constexpr uint16_t BUILDING_AUTO_STYLE_MASK = 127; // style values below 128 are drawn from the seed

} // namespace city
