// Buildings grow over time on free lots (Roadcraft src/world/growth.ts Growth.step): the next one is drawn, by a seeded pick,
// among the free lots within the growth data's `pick_reach_m` of the last that grew (the nearest when none is), so a
// neighbourhood fills round where it began without running down a street in order; `sim_growth_per_s` buildings a simulated second, at most
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
	double acc = 0; // buildings owed
	bool has_last = false;
	double last_x = 0, last_z = 0;
	int grown = 0; // buildings grown since the map began (probe for tests and the UI)

	void clear() { *this = GrowthQueue(); }
	void push(int64_t key);
	void assign(std::vector<int64_t> keys); // the queue as saved: cells and their members (each key once)
};

// One simulation step of growth (caller holds map_mutex exclusively).
void grow_step(CityState &s, double dt);

} // namespace city
