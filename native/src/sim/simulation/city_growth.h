// Buildings grow over time on free lots (Roadcraft src/world/growth.ts Growth.step): the next one is the free lot
// nearest the last that grew, so a street fills in sequence; `sim_growth_per_s` buildings a simulated second, at most
// `sim_growth_tries` tries a step. Only cells whose lots changed after the city was founded are queued (the generated
// city is filled at once), so the work never grows with the map.
#pragma once

#include <cstdint>
#include <unordered_set>
#include <vector>

namespace city {

struct CityState;

struct GrowthQueue {
	std::vector<int64_t> cells; // CHUNK keys with lots that may be free
	std::unordered_set<int64_t> members; // same keys, O(1) membership
	double acc = 0; // buildings owed: the simulation thread's alone (neither saved nor read elsewhere)
	bool has_last = false;
	double last_x = 0, last_z = 0;
	int grown = 0; // buildings grown since the map began (probe for tests and the UI)

	void clear() { *this = GrowthQueue(); }
	void push(int64_t key);
	void assign(std::vector<int64_t> keys); // the queue as saved: cells and their members (each key once)
};

// One simulation step of growth. It takes the map itself (the caller holds nothing): shared while it decides each building (the
// scan of the queued lots, its neighbours' looks, the candidate bodies), beside the mesh jobs and the frame, and exclusively only
// to add what it decided, a building at a time, so each is decided on the city the last one already stands in. A building decided
// on a map a command edited meanwhile (CityState::revision) is not added: it stays owed to the next step.
void grow_step(CityState &s, double dt);

} // namespace city
