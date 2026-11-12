// Building records: small and flat; everything drawn is derived from them.
// A record with floors == 0 is an empty lot. Ids come from a free list; queries go through a spatial hash.
#pragma once

#include "core/grid.h"
#include "core/id_table.h"
#include "core/slot_ids.h"
#include "sim/buildings/b40/data.h"
#include "sim/buildings/likeness.h"
#include "sim/state/change_log.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

struct OccupancyDef;
class FurnitureKept;

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
	uint16_t floors = 0; // the storeys its body has (0 = empty lot): what every reader reads
	// What a body made from the seed was made under, apart from what it came out as: the most storeys its neighbours allowed a
	// middling block when it grew (0: no limit). The body is keyed and made again by it (b40/record.h body_key), so the same record
	// always gives the same body, whose top `floors` keeps.
	uint16_t cap = 0;
	uint32_t seed = 0;
	uint16_t style = 0; // 0..127 automatic, 128 + arch = chosen
	uint16_t profile = 0; // profile of the street it faces
	int16_t landmark = -1; // catalogue project (data/catalog/landmarks) or -1
	bool alive = false;
	b40::BodyRef body; // the version the player edited, held by the record (b40::BodyTable); none = made from the seed
	double built = 0; // city minute it was built (decay and renovation)
	// What the lot's edges meet (b40::LotEdges): derived from the streets, settled with the lot's site (sim/commands/lot_sites.h) after
	// every edit, the generation, growth and the load, never saved; a body made from the seed is made to it (b40/record.h body_key).
	b40::LotEdges edges;
	// Its edges were read from the streets (when its body was first made, or settled since), so its storeys stand under them: a change
	// of them is a change of its body (BuildingStore::set_edges). Never saved: a record read from a file has its storeys from the file
	// and its edges still to read.
	bool edges_read = false;
};

// The records and their filing by place: what a reader takes of the buildings, the live city's store or a cut of it (a job's
// copy: sim/state/city_cut.h). The generator's tables are not here: they are the session's, read beside the records.
struct BuildingRecords {
	IdTable<Building> items;
	SpatialHash hash; // by lot centre

	// Does the point stand on the building's lot (its rectangle grown by pad)?
	static bool covers(const Building &b, float x, float z, float pad);
	template <typename F>
	void query(float minx, float minz, float maxx, float maxz, F &&fn) const {
		hash.query(minx, minz, maxx, maxz, [&](int id) {
			const Building &b = items[id];
			if (b.alive && b.x >= minx && b.x <= maxx && b.z >= minz && b.z <= maxz) fn(id);
		});
	}
	// The records filed in the cells the box touches, under their own ids, the copy's hash answering for the box alone.
	BuildingRecords cut(float minx, float minz, float maxx, float maxz) const {
		BuildingRecords c;
		c.items = IdTable<Building>::cut(items.size());
		c.hash = hash.window(minx, minz, maxx, maxz);
		for (int id : c.hash.filed()) c.items.put(size_t(id), items[size_t(id)]);
		return c;
	}
};

class BuildingStore : public BuildingRecords {
public:
	SlotIds free_ids;
	ChangeLog *log = nullptr;
	// The city's kept street furniture (furniture_kept.h): a lot's gates are an input of its street's placement, so a record that
	// changes drops the placements whose lots it may be among (mark).
	FurnitureKept *furniture = nullptr;
	int count = 0;
	// Residents and jobs of the alive records (occupancy), kept by place/remove, the one way a record comes or goes: never
	// counted again over every record, and right after growth, edits and loads alike (the snapshot reads them).
	int residents = 0, jobs = 0;
	b40::Data designs; // the generator's tables (set once at boot, read-only after): every body is made from them, and kept by them
	// The records whose lot the ground is still to be graded to (sim/buildings/lot_sites.h settle_sites takes them): every mark.
	std::vector<int> sites_due;

	void init(ChangeLog *l, const OccupancyDef *occupancy);
	// The same links set again, nothing cleared: the store moved in with a city read aside (CityState::bind), and the city's kept
	// street furniture (init leaves it as bound).
	void attach(ChangeLog *l, const OccupancyDef *occupancy, FurnitureKept *kept) {
		log = l;
		occupancy_ = occupancy;
		furniture = kept;
	}
	void clear();
	int add(const Building &b);
	void remove(int id);
	// Undo / redo / load: the record goes back under the same id. A building that
	// grew there meanwhile (outside the history) moves to a new id; `evict` also knocks down what grew on the lot.
	void put(int id, const Building &b, bool evict);
	// The record changed: its chunks drawn again, the traffic and the render told (the streets it faces lay their furniture off its
	// gates again), its lot due to be graded.
	void mark(int id);
	// The lot's edges settled (sim/commands/lot_sites.h): a body made from the seed is made to them, so the record is drawn again and the
	// traffic and the furniture told, as by `mark`, its ground left to the site that settles it now.
	void set_edges(int id, const b40::LotEdges &edges);
	void mark_look(int id) const; // its chunks drawn again though it did not change: another look (Interior layer)
	// Residents and jobs from the floor area.
	static void occupancy(const Building &b, float floor_share, const float per_m2[4], int &residents, int &jobs);

private:
	const OccupancyDef *occupancy_ = nullptr; // the catalogue's, set by init
	void place(int id, const Building &b);
	void tally(const Building &b, int sign); // the record's residents and jobs into (+1) or out of (-1) the totals
	void changed(int id); // what `mark` tells but the site: its chunks, the traffic, the render's lot boxes, the kept furniture
	static float reach(const Building &b); // how far the lot's turned rectangle reaches from its centre: half its diagonal
	void mark_into(int id, std::vector<int64_t> &out) const; // the chunks the lot's rectangle may touch
};

constexpr float BUILDING_HASH_CELL = 64.f;
constexpr uint16_t BUILDING_AUTO_STYLE_MASK = 127; // style values below 128 are drawn from the seed

} // namespace city
