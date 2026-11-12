// Free-growth candidates follow lot and building mutations, independent of render logs. Nearest selection uses a point tree;
// empty-cell swap removal and queue/lot tie order reproduce the original growth sequence.
#include "sim/simulation/city_growth.h"
#include "sim/buildings/growth.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

void GrowthQueue::reset_candidates() {
	positions_.clear(); candidates_.clear(); lots_.clear(); free_lots_.clear();
	free_points_.clear(); dependencies_.clear(); empty_.clear(); dirty_cells_.clear(); dirty_lots_.clear();
}
void GrowthQueue::push(int64_t key) {
	if (members.insert(key).second) {
		positions_[key] = cells.size(); empty_.insert(cells.size()); cells.push_back(key); candidates_.emplace(key, Cell{});
	}
	dirty_cells_.insert(key);
}
void GrowthQueue::assign(std::vector<int64_t> keys) {
	cells.clear(); members.clear(); reset_candidates();
	for (int64_t key : keys) push(key); // acc/last/grown retain their saved values
}
void GrowthQueue::invalidate_cell(int64_t key) {
	if (members.count(key)) dirty_cells_.insert(key);
}
void GrowthQueue::occupancy_changed(float x, float z) {
	dependencies_.query({x, z, x, z}, [&](int handle) { dirty_lots_.insert(handle); });
}
void GrowthQueue::sync_empty(int64_t key) {
	const size_t at = positions_.at(key);
	if (candidates_.at(key).free_lots.empty()) empty_.insert(at);
	else empty_.erase(at);
}
void GrowthQueue::retire_lot(int handle) {
	Candidate &c = lots_[size_t(handle)];
	if (c.free) free_points_.erase(handle, c.point);
	dependencies_.erase(handle, c.dependency); dirty_lots_.erase(handle);
	c.free = false; free_lots_.push_back(handle);
}
void GrowthQueue::refresh_lot(CityState &s, int handle) {
	Candidate &c = lots_[size_t(handle)];
	const auto *source = s.lots.lots_of(c.key);
	const bool available = source && c.lot >= 0 && size_t(c.lot) < source->size() && !lot_occupied(s, (*source)[size_t(c.lot)]);
	if (available != c.free) {
		Cell &cell = candidates_.at(c.key);
		if (available) { free_points_.insert(handle, c.point); cell.free_lots.insert(c.lot); }
		else { free_points_.erase(handle, c.point); cell.free_lots.erase(c.lot); }
		c.free = available;
	}
	sync_empty(c.key);
}
void GrowthQueue::refresh_cell(CityState &s, int64_t key) {
	Cell &cell = candidates_.at(key);
	for (int handle : cell.handles) retire_lot(handle);
	cell.handles.clear(); cell.free_lots.clear();
	const auto *source = s.lots.lots_of(key);
	for (size_t i = 0; source && i < source->size(); i++) {
		const Lot &lot = (*source)[i];
		int handle;
		if (free_lots_.empty()) { handle = int(lots_.size()); lots_.emplace_back(); }
		else { handle = free_lots_.back(); free_lots_.pop_back(); }
		// These are exactly lot_occupied's float query windows, including its larger square-centre search.
		const float x = float(lot.x), z = float(lot.z), radius = float(std::max(lot.w, lot.d));
		const float wide = radius + std::sqrt(float(s.catalog.growth.plan.square_area_high));
		Candidate &c = lots_[size_t(handle)];
		c = {key, int(i), lot.x, lot.z, {lot.x, lot.z, lot.x, lot.z}, {x - wide, z - wide, x + wide, z + wide}, false};
		cell.handles.push_back(handle); dependencies_.insert(handle, c.dependency);
		refresh_lot(s, handle);
	}
	sync_empty(key);
}
void GrowthQueue::discard_cell(int64_t key) {
	const size_t at = positions_.at(key), last = cells.size() - 1;
	Cell &cell = candidates_.at(key);
	for (int handle : cell.handles) retire_lot(handle);
	candidates_.erase(key); dirty_cells_.erase(key); empty_.erase(at); empty_.erase(last);
	if (at != last) {
		const int64_t moved = cells[last]; cells[at] = moved; positions_[moved] = at;
		if (candidates_.at(moved).free_lots.empty()) empty_.insert(at);
	}
	cells.pop_back(); positions_.erase(key); members.erase(key);
}

GrowthPick GrowthQueue::nearest(CityState &s) {
	// Each source change is consumed once. A building grown below invalidates just dependent lots for the next pick.
	std::unordered_set<int64_t> cells_dirty; cells_dirty.swap(dirty_cells_);
	for (int64_t key : cells_dirty) if (members.count(key)) refresh_cell(s, key);
	std::unordered_set<int> lots_dirty; lots_dirty.swap(dirty_lots_);
	for (int handle : lots_dirty) refresh_lot(s, handle);
	// Removing the lowest empty index, then swapping the last, is the original scan's exact compaction order.
	while (!empty_.empty()) discard_cell(cells[*empty_.begin()]);
	GrowthPick best;
	if (cells.empty()) return best;
	if (!has_last) return {cells.front(), *candidates_.at(cells.front()).free_lots.begin(), 0};
	const auto lower_bound = [&](const Bounds2 &b) {
		const double dx = last_x < b.minx ? b.minx - last_x : (last_x > b.maxx ? last_x - b.maxx : 0);
		const double dz = last_z < b.minz ? b.minz - last_z : (last_z > b.maxz ? last_z - b.maxz : 0);
		return dx * dx + dz * dz;
	};
	free_points_.query_if([&](const Bounds2 &b) { return best.lot < 0 || lower_bound(b) <= best.d2; }, [&](int handle) {
		const Candidate &c = lots_[size_t(handle)];
		const double d2 = (c.x - last_x) * (c.x - last_x) + (c.z - last_z) * (c.z - last_z);
		const bool first = best.lot >= 0 && (positions_.at(c.key) < positions_.at(best.key)
			|| (c.key == best.key && c.lot < best.lot));
		if (best.lot < 0 || d2 < best.d2 || (d2 == best.d2 && first)) best = {c.key, c.lot, d2};
	});
	return best;
}
} // namespace city
