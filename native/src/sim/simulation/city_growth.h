// Buildings grow over time on free lots (Roadcraft src/world/growth.ts Growth.step, growth/zoning.ts pickLot): the next one is
// the free lot nearest the last that grew when one is within sim.grow_on_reach_m, so a street fills in sequence, else a free
// lot drawn by the city's seed; `sim_growth_per_s` buildings a simulated second, at most
// `sim_growth_tries` tries a step. Only cells whose lots changed after the city was founded are queued (the generated
// city is filled at once), so the work never grows with the map.
#pragma once

#include "sim/buildings/building_store.h"

#include <cstdint>
#include <unordered_set>
#include <vector>

namespace city {

struct CityState;

struct GrowthQueue {
	std::vector<int64_t> cells; // CHUNK keys with lots that may be free
	std::unordered_set<int64_t> members; // same keys, O(1) membership
	double acc = 0; // buildings owed: the simulation thread's alone (neither saved nor read elsewhere), moved by the plan
	bool has_last = false;
	double last_x = 0, last_z = 0;
	int grown = 0; // buildings grown since the map began (probe for tests and the UI)

	void clear() { *this = GrowthQueue(); }
	void push(int64_t key);
	void assign(std::vector<int64_t> keys); // the queue as saved: cells and their members (each key once)
};

// What one simulation step grows, decided by reading the map: the buildings owed, each on its free lot in the order the step adds them,
// and the queued cells found with no free lot. Deciding (a scan of the queued lots, the candidate bodies) takes the map shared, beside
// the mesh jobs and the frame; only adding what was decided takes it exclusively, and only when there is something to add.
struct GrowthPlan {
	std::vector<Building> add;
	std::vector<int64_t> drop; // cells left with no free lot
	bool none_free = false; // no free lot in the queue: what is owed is cleared
	uint64_t revision = 0; // the map's when the plan was made: an edit since makes it void
	bool changes() const { return !add.empty() || !drop.empty() || none_free; }
};

// The plan of one step of `dt` (caller holds map_mutex shared; it moves the growth's debt, GrowthQueue::acc, and nothing else).
GrowthPlan plan_growth(CityState &s, double dt);
// Adds what `plan` decided (caller holds map_mutex exclusively); nothing when the map was edited since the plan was made, and the
// buildings owed wait for the next step's plan.
void apply_growth(CityState &s, const GrowthPlan &plan);

} // namespace city
