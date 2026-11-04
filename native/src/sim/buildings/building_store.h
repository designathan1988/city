// Building records: small and flat; everything drawn is derived from them.
// A record with floors == 0 is an empty lot. Ids come from a free list; queries go through a spatial hash.
#pragma once

#include "core/grid.h"
#include "sim/buildings/b40/data.h"
#include "sim/state/change_log.h"

#include <cstdint>
#include <vector>

namespace city {

// Civic functions (the ids saved with a record).
constexpr uint8_t FN_COUNT_VALUE = 35; // the catalogue keeps unused function ids reserved for future civic kinds
enum BuildingFn : uint8_t { FN_NONE = 0, FN_SCHOOL, FN_CHURCH, FN_HOSPITAL, FN_CLINIC, FN_SQUARE, FN_PARK, FN_GAS_STATION,
	FN_SUPERMARKET, FN_CITY_HALL, FN_POLICE, FN_FIRE_STATION, FN_MALL, FN_SPORTS_COURT, FN_COUNT = FN_COUNT_VALUE };

struct Building {
	bool alive = false;
	float x = 0, z = 0, w = 0, d = 0, rot = 0; // lot centre, frontage, depth; local +z faces the street
	uint8_t zone = 0; // base zone (1..4) or a density zone (5..8)
	uint8_t fn = 0; // BuildingFn
	uint16_t floors = 0; // 0 = empty lot
	uint32_t seed = 0;
	uint16_t style = 0; // 0..127 automatic, 128 + arch = chosen
	uint16_t profile = 0; // profile of the street it faces
	int16_t landmark = -1; // catalogue project (data/catalog/landmarks) or -1
	double built = 0; // city minute it was built (decay and renovation)
};

class BuildingStore {
public:
	std::vector<Building> items;
	std::vector<int> free_ids;
	SpatialHash hash; // by lot centre
	ChangeLog *log = nullptr;
	int count = 0;
	b40::Data designs; // the generator's tables (set once at boot, read-only after): every body is made from them

	void init(ChangeLog *l);
	void clear();
	int add(const Building &b);
	void remove(int id);
	// Undo / redo / load: the record goes back under the same id. A building that
	// grew there meanwhile (outside the history) moves to a new id; `evict` also knocks down what grew on the lot.
	void put(int id, const Building &b, bool evict);
	// Does the point stand on the building's lot (its rectangle grown by pad)?
	static bool covers(const Building &b, float x, float z, float pad);
	void mark(int id) const;
	template <typename F>
	void query(float minx, float minz, float maxx, float maxz, F &&fn) const {
		hash.query(minx, minz, maxx, maxz, [&](int id) {
			const Building &b = items[id];
			if (b.alive && b.x >= minx && b.x <= maxx && b.z >= minz && b.z <= maxz) fn(id);
		});
	}
	// Residents and jobs from the floor area.
	static void occupancy(const Building &b, float floor_share, const float per_m2[4], int &residents, int &jobs);

private:
	void place(int id, const Building &b);
};

constexpr float BUILDING_HASH_CELL = 64.f;
constexpr uint16_t BUILDING_AUTO_STYLE_MASK = 127; // style values below 128 are drawn from the seed

} // namespace city
