// Buildings grow over time on free lots: the next one stands on the open lot nearest the last that grew when one is within
// `sim_growth_reach_m` (a street fills along), else on an open lot drawn from the whole queue (other zoned land starts too);
// `sim_growth_per_s` buildings a simulated second, at most `sim_growth_tries` tries a step. Only cells whose lots changed after the
// city was founded are queued (the generated city is filled at once), and each queued cell's open lots are found once and kept, so a
// building costs a look at the cells within its reach, never a walk of the queue's lots.
#pragma once

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

struct CityState;

struct GrowthQueue {
	std::vector<int64_t> cells; // CHUNK keys with lots that may be free
	std::unordered_set<int64_t> members; // same keys, O(1) membership
	// A queued cell's open lots (indices into the cell's lots, LotCache::lots_of), found once by a look of growth and kept until the
	// cell is queued again (its lots were cut again, or something left one of them): never saved, a cell read from a save has none yet.
	std::unordered_map<int64_t, std::vector<int>> open;
	double acc = 0; // buildings owed: the simulation thread's alone (neither saved nor read elsewhere)
	bool has_last = false;
	double last_x = 0, last_z = 0;
	int grown = 0; // buildings grown since the map began (probe for tests and the UI)

	void clear() { *this = GrowthQueue(); }
	void push(int64_t key); // queued, its open lots to be found again
	void drop(int64_t key); // out of the queue: nothing is open there, or its lots are gone
	void assign(std::vector<int64_t> keys); // the queue as saved: cells and their members (each key once)
	void file(int64_t key, std::vector<int> lots); // a queued cell's open lots, as a look found them
	void shut(int64_t key, int lot); // one lot of a filed cell is open no more; a cell left with none leaves the queue
};

// One simulation step of growth. It takes the map itself (the caller holds nothing): shared while it decides each building (the
// open lots of the cells not yet looked at, the lot, its neighbours' looks, the candidate bodies), beside the mesh jobs and the frame,
// and exclusively only to write what it decided, a building at a time, so each is decided on the city the last one already stands in.
// What was decided on a map a command edited meanwhile (CityState::revision) is not written: it stays owed to the next step.
void grow_step(CityState &s, double dt);

} // namespace city
