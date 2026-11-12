// Buildings grow over time on free lots (Roadcraft src/world/growth.ts Growth.step): the next one is the free lot
// nearest the last that grew, so a street fills in sequence; `sim_growth_per_s` buildings a simulated second, at most
// `sim_growth_tries` tries a step. Only cells whose lots changed after the city was founded are queued (the generated
// city is filled at once), so the work never grows with the map.
#pragma once

#include "core/bounds_index.h"
#include <cstdint>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

struct CityState;
struct GrowthPick { int64_t key = 0; int lot = -1; double d2 = 0; };

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
	void invalidate_cell(int64_t key);
	void occupancy_changed(float x, float z);
	GrowthPick nearest(CityState &s);
private:
	struct Candidate { int64_t key = 0; int lot = -1; double x = 0, z = 0; Bounds2 point, dependency; bool free = false; };
	struct Cell { std::vector<int> handles; std::set<int> free_lots; };
	std::unordered_map<int64_t, size_t> positions_;
	std::unordered_map<int64_t, Cell> candidates_;
	std::vector<Candidate> lots_;
	FreeSlots free_lots_;
	BoundsIndex free_points_, dependencies_;
	std::set<size_t> empty_;
	std::unordered_set<int64_t> dirty_cells_;
	std::unordered_set<int> dirty_lots_;
	void reset_candidates();
	void refresh_cell(CityState &s, int64_t key);
	void refresh_lot(CityState &s, int handle);
	void retire_lot(int handle);
	void sync_empty(int64_t key);
	void discard_cell(int64_t key);
};

// One simulation step of growth (caller holds map_mutex exclusively).
void grow_step(CityState &s, double dt);

} // namespace city
