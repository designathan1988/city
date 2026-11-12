// A cut of the city (CityCut): what a mesh job reads of its place, copied while the map is held shared for the copy alone
// (cut_city), so the build runs with the lock let go and a command, a grown building or a save never waits for a mesh. The cut
// holds the same stores under the same ids (core/id_table.h, SpatialHash::window), only the records of its box; the stores it
// was not asked for hold nothing, and reading them, an id the cut does not hold or a place past its box stops the tests' DLL
// (core/checks.h). The catalogue and the generator's tables are the session's, read by reference: the job pool is paused while
// a city is taken in (core/job_pool.h pause).
// What a job cannot derive from a box of records (a segment's furniture reads the hydrant squares round its ends; a junction's
// control and plates are the traffic's rules) the cut carries as facts, read from the live city while the map is held: a job
// writes none of the caches the simulation reads.
#pragma once

#include "sim/roads/parking_layout.h"
#include "sim/roads/street_furniture.h"
#include "sim/state/city_view.h"

#include <algorithm>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

// The box of the map a cut copies, in metres.
struct CutBox {
	float minx = 0, minz = 0, maxx = 0, maxz = 0;
	void take(float x0, float z0, float x1, float z1) {
		minx = std::min(minx, x0);
		minz = std::min(minz, z0);
		maxx = std::max(maxx, x1);
		maxz = std::max(maxz, z1);
	}
	CutBox grown(float by) const { return CutBox{ minx - by, minz - by, maxx + by, maxz + by }; }
};

// What a cut copies besides the final ground of its box. CUT_GATES: the car gates facing the segments whose kerbs the job lowers,
// kept as facts (keep_gates), for a junction's corner reads them along an arm whose lots stand past the box.
enum CutParts : unsigned { CUT_ROADS = 1, CUT_BUILDINGS = 2, CUT_GROUND_BASE = 4, CUT_WATER = 8, CUT_STREET = 16, CUT_NATURE = 32, CUT_GATES = 64 };

struct CutFacts {
	std::unordered_map<int, std::vector<FurnitureSpot>> furniture; // by segment: its pieces of the kinds the cut keeps
	uint32_t kinds_kept = FURNITURE_ALL; // those kinds: every one (keep_furniture), or the lamps alone (keep_lamps)
	std::unordered_map<int, std::vector<SignStand>> signs; // by node: the plates its rules put up
	std::unordered_set<int> signalled; // of those nodes, the ones whose junction runs a signal
	std::unordered_map<int, std::array<std::vector<ArcSpan>, 2>> gates; // by segment: the car gates facing each side, no margin
};

struct CityCut {
	const Catalog *catalog = nullptr;
	const b40::Data *designs = nullptr;
	GroundPatch ground;
	RoadNetwork roads;
	StreetWorks street;
	BuildingRecords buildings;
	NatureState nature;
	CarriedAtomic<int> marking_style;
	uint64_t revision = 0;
	uint32_t seed = 0;
	CutFacts facts;
};

inline CityView::CityView(const CityCut &cut) :
		catalog(*cut.catalog), terrain(cut.ground), roads(cut.roads), street(cut.street), buildings(cut.buildings), designs(*cut.designs),
		nature(cut.nature), marking_style(cut.marking_style), revision(cut.revision), seed(cut.seed), live(nullptr), facts(&cut.facts) {}

// The cut of `box` with `parts`, made by a thread that holds the map.
CityCut cut_held(const CityState &s, const CutBox &box, unsigned parts);
// `box` grown to hold every pole a span reaches from the player's poles standing in [x0, x1) x [z0, z1) (a pole's arms turn by its
// spans and a span hangs from both its ends): the one reach of the street works a job's box takes, read under the map held.
void take_pole_reach(CutBox &box, const StreetWorks &street, float x0, float z0, float x1, float z1);
// Facts taken into a cut while the map is held: segment `sid`'s furniture; node `node`'s control and plates; the car gates facing
// segment `sid`. keep_lamps: its lamps alone (LIT_FURNITURE), placed anew and kept nowhere else (a light reads the lamps of streets as
// far as the eye sees: their every piece kept would be the whole map's); a cut keeps one or the other.
void keep_furniture(CityCut &cut, const CityState &live, int sid);
void keep_lamps(CityCut &cut, const CityState &live, int sid);
void keep_node(CityCut &cut, const CityState &live, int node);
void keep_gates(CityCut &cut, const CityState &live, int sid);

// The cut a job makes: the map taken shared here, for the copy alone; `box_of(s)` says the box once the map is held (a job's
// box may depend on what stands in its place), and `keep(cut, s)` takes the facts the job reads.
template <typename BoxOf, typename Keep>
CityCut cut_city(const CityState &s, unsigned parts, BoxOf &&box_of, Keep &&keep) {
	MapRead map(s);
	CityCut cut = cut_held(s, box_of(s), parts);
	keep(cut, s);
	return cut;
}
template <typename BoxOf>
CityCut cut_city(const CityState &s, unsigned parts, BoxOf &&box_of) {
	return cut_city(s, parts, box_of, [](CityCut &, const CityState &) {});
}

// The pieces of segment `sid` of the kinds in `kinds`, appended to `out`: the live city's kept placement
// (traffic::street_furniture_of), or the one the cut carries.
void furniture_of(const CityView &s, int sid, uint32_t kinds, std::vector<FurnitureSpot> &out);
// Whether node `node`'s junction runs a signal, and the plates its rules put up, appended to `out`.
bool node_signalled(const CityView &s, int node);
void node_signs(const CityView &s, int node, std::vector<SignStand> &out);
// The car gates of the lots facing each side of segment `sid` (STREET_RIGHT, STREET_LEFT), the gates themselves with no margin, where
// its kerbs drop: the live city's (sim/buildings/lot_access.h street_gates), or the ones the cut carries.
void kerb_gates_of(const CityView &s, int sid, std::vector<ArcSpan> out[2]);

} // namespace city
