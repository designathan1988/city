// Who stands where on foot (WalkerCells, ped_state.h): the walkers filed by cell in one array in order of cell, made from the walkers
// there are now (none of the cells they stood in before), for those off the lanes asking once a tick and for the walkers being born.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr float WALKER_CELL_M = 8.f;
constexpr int CELL_ROW_BITS = 32;

int64_t cell_key(int64_t i, int64_t j) { return (i << CELL_ROW_BITS) ^ int64_t(uint32_t(int32_t(j))); }
int64_t cell_index(float v) { return int64_t(std::floor(v / WALKER_CELL_M)); }
int64_t cell_of(float x, float z) { return cell_key(cell_index(x), cell_index(z)); }

// The one rule of being near: within `r` of (x, z), as every asker measures it.
bool stands_within(const Pose &q, float x, float z, float r) {
	const float dx = q.x - x, dz = q.z - z;
	return dx * dx + dz * dz < r * r;
}

void file(const TrafficState &t, WalkerCells &c) {
	c.by_cell.clear();
	c.born.clear();
	for (int id = 0; id < int(t.peds.size()); id++)
		if (t.peds[size_t(id)].alive) c.by_cell.emplace_back(cell_of(t.peds[size_t(id)].cur.x, t.peds[size_t(id)].cur.z), id);
	std::sort(c.by_cell.begin(), c.by_cell.end());
}

// Somebody filed in cell `key` stands within `r` of (x, z).
bool cell_within(const TrafficState &t, const WalkerCells &c, int64_t key, float x, float z, float r) {
	auto it = std::lower_bound(c.by_cell.begin(), c.by_cell.end(), std::make_pair(key, -1));
	for (; it != c.by_cell.end() && it->first == key; ++it)
		if (stands_within(t.peds[size_t(it->second)].cur, x, z, r)) return true;
	return false;
}
} // namespace

bool walker_within(CityState &s, float x, float z, float r) {
	TrafficState &t = s.traffic;
	WalkerCells &c = t.walker_cells;
	if (c.tick != t.tick) { // filed at most once a tick, where they stood at the first asking
		c.tick = t.tick;
		file(t, c);
	}
	const int reach = int(std::ceil(r / WALKER_CELL_M));
	for (int j = -reach; j <= reach; j++)
		for (int i = -reach; i <= reach; i++)
			if (cell_within(t, c, cell_of(x + float(i) * WALKER_CELL_M, z + float(j) * WALKER_CELL_M), x, z, r)) return true;
	return false;
}

void file_spawn_cells(TrafficState &t) {
	file(t, t.spawn_cells);
	t.spawn_cells.live = true;
}

void walker_born(TrafficState &t, int id) {
	if (t.spawn_cells.live) t.spawn_cells.born.push_back(id);
}

bool walker_near(const TrafficState &t, const Pose &at, float r) {
	const WalkerCells &c = t.spawn_cells;
	if (!c.live) { // no filing stands: every walker
		for (const Pedestrian &o : t.peds)
			if (o.alive && stands_within(o.cur, at.x, at.z, r)) return true;
		return false;
	}
	// the cells round its own, by index: a walker within `r` stands in one of them (dividing by the cell's power of two is exact)
	const int64_t reach = int64_t(std::ceil(r / WALKER_CELL_M)), ci = cell_index(at.x), cj = cell_index(at.z);
	for (int64_t j = cj - reach; j <= cj + reach; j++)
		for (int64_t i = ci - reach; i <= ci + reach; i++)
			if (cell_within(t, c, cell_key(i, j), at.x, at.z, r)) return true;
	for (int id : c.born)
		if (stands_within(t.peds[size_t(id)].cur, at.x, at.z, r)) return true;
	return false;
}

} // namespace traffic
} // namespace city
