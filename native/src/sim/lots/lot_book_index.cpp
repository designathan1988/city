// Player-lot mutation boundary: prepare only changed rings, maintain point/stand indices and preserve record-order ties.
#include "sim/lots/lot_book.h"

#include "core/grid.h"

#include <cassert>

namespace city {

void LotBook::clear() {
	lots_ = {}; positions_.clear(); geometry_.clear(); stand_cells_.clear(); bounds_.clear();
	next_id = 1;
}

const PlayerLot *LotBook::find(int id) const {
	const auto found = positions_.find(id);
	return found == positions_.end() ? nullptr : &lots_[found->second];
}

const LotGeometry *LotBook::geometry(int id) const {
	const auto found = geometry_.find(id);
	return found == geometry_.end() ? nullptr : &found->second;
}

void LotBook::prepare(const PlayerLot &lot) {
	assert(!geometry_.count(lot.id) && lot.corners.size() >= 3);
	LotGeometry shape;
	const LotPoint &first = lot.corners.front();
	shape.bounds = { first.x, first.z, first.x, first.z };
	for (const LotPoint &p : lot.corners) shape.bounds = Bounds2::united(shape.bounds, { p.x, p.z, p.x, p.z });
	shape.stand = lot_stand(lot.corners);
	shape.triangles = lot_triangles(lot.corners);
	bounds_.insert(lot.id, shape.bounds);
	stand_cells_[chunk_of(float(shape.stand.x), float(shape.stand.z))].push_back(lot.id);
	geometry_.emplace(lot.id, std::move(shape));
}

void LotBook::forget(int id) {
	const auto shape = geometry_.find(id);
	if (shape == geometry_.end()) return;
	bounds_.erase(id, shape->second.bounds);
	const auto cell = stand_cells_.find(chunk_of(float(shape->second.stand.x), float(shape->second.stand.z)));
	assert(cell != stand_cells_.end());
	auto &ids = cell->second;
	ids.erase(std::remove(ids.begin(), ids.end(), id), ids.end());
	if (ids.empty()) stand_cells_.erase(cell);
	geometry_.erase(shape);
}

void LotBook::restore(std::vector<PlayerLot> lots, int next) {
	clear();
	lots_ = std::move(lots); next_id = next;
	for (size_t i = 0; i < lots_.size(); i++) {
		const PlayerLot &lot = lots_[i];
		const bool unique = positions_.emplace(lot.id, i).second;
		assert(unique); (void)unique;
		prepare(lot);
	}
}

void LotBook::apply(const TableDelta<PlayerLot> &change, bool forward) {
	change.apply(lots_, forward);
	bool reordered = false;
	for (const auto &edit : change.edits) {
		const auto &from = forward ? edit.before : edit.after, &to = forward ? edit.after : edit.before;
		if (!(from && to)) reordered = true;
		if (from && to && from->id == to->id && from->corners == to->corners) continue; // zone-only: no geometry work
		if (from) { forget(from->id); positions_.erase(from->id); }
		if (to) { prepare(*to); positions_[to->id] = edit.index; }
	}
	if (reordered) {
		positions_.clear();
		for (size_t i = 0; i < lots_.size(); i++) positions_.emplace(lots_[i].id, i);
	}
}

void LotBook::order(std::vector<int> &ids) const {
	std::sort(ids.begin(), ids.end(), [&](int a, int b) { return positions_.at(a) < positions_.at(b); });
}

std::vector<int> LotBook::overlapping(const Bounds2 &bounds) const {
	std::vector<int> ids;
	bounds_.query(bounds, [&](int id) { ids.push_back(id); });
	order(ids);
	return ids;
}

std::vector<int> LotBook::stands_in(int64_t cell) const {
	const auto found = stand_cells_.find(cell);
	if (found == stand_cells_.end()) return {};
	std::vector<int> ids = found->second;
	order(ids);
	return ids;
}

int LotBook::at(double x, double z) const {
	for (int id : overlapping({ x, z, x, z }))
		if (inside_lot({ x, z }, find(id)->corners)) return id;
	return 0;
}

} // namespace city
