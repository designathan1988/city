// The book of the player's lots and what it files as its edits change it (lot_book.h).
#include "sim/lots/lot_book.h"

#include <algorithm>
#include <limits>

namespace city {

namespace {
constexpr float INDEX_CELL_M = 64.f; // the book's spatial filing: a few lots a cell
constexpr double INDEX_PAD_M = 1; // a lot is filed this far past its corners: the inside test decides, the index must not miss it
} // namespace

void LotBook::clear() {
	lots_.clear();
	next_id = 1;
	file();
}

const PlayerLot *LotBook::find(int id) const {
	const int i = index_of(id);
	return i < 0 ? nullptr : &lots_[size_t(i)];
}

int LotBook::index_of(int id) const {
	auto it = by_id_.find(id);
	return it == by_id_.end() ? -1 : it->second;
}

int LotBook::at(double x, double z) const {
	int first = -1;
	hash_.query(float(x), float(z), float(x), float(z), [&](int i) {
		if ((first < 0 || i < first) && inside_lot({ x, z }, lots_[size_t(i)].corners)) first = i;
	});
	return first < 0 ? 0 : lots_[size_t(first)].id;
}

std::vector<int> LotBook::near(double x0, double z0, double x1, double z1) const {
	std::vector<int> out;
	hash_.query(float(x0), float(z0), float(x1), float(z1), [&](int i) { out.push_back(i); });
	std::sort(out.begin(), out.end());
	out.erase(std::unique(out.begin(), out.end()), out.end());
	return out;
}

void LotBook::add(PlayerLot lot) {
	lots_.push_back(std::move(lot));
	file();
}

void LotBook::insert(int index, PlayerLot lot) {
	lots_.insert(lots_.begin() + std::clamp<std::ptrdiff_t>(index, 0, std::ptrdiff_t(lots_.size())), std::move(lot));
	file();
}

void LotBook::erase(int index) {
	if (index < 0 || index >= int(lots_.size())) return;
	lots_.erase(lots_.begin() + index);
	file();
}

void LotBook::set(int index, PlayerLot lot) {
	if (index < 0 || index >= int(lots_.size())) return;
	lots_[size_t(index)] = std::move(lot);
	file();
}

void LotBook::assign(std::vector<PlayerLot> lots) {
	lots_ = std::move(lots);
	file();
}

// What is derived from the book filed again whole: an edit is the player's, a few hundred lots at most.
void LotBook::file() {
	stands_.clear();
	by_id_.clear();
	zoned_.clear();
	hash_.clear();
	hash_.cell = INDEX_CELL_M;
	stands_.reserve(lots_.size());
	for (size_t i = 0; i < lots_.size(); i++) {
		const PlayerLot &l = lots_[i];
		stands_.push_back(lot_stand(l.corners));
		by_id_.emplace(l.id, int(i));
		if (l.zone != 0) zoned_[chunk_of(float(stands_.back().x), float(stands_.back().z))].push_back(int(i));
		if (l.corners.empty()) continue;
		double x0 = std::numeric_limits<double>::max(), z0 = x0, x1 = std::numeric_limits<double>::lowest(), z1 = x1;
		for (const XZ &p : l.corners) {
			x0 = std::min(x0, p.x); z0 = std::min(z0, p.z);
			x1 = std::max(x1, p.x); z1 = std::max(z1, p.z);
		}
		hash_.insert_aabb(int(i), float(x0 - INDEX_PAD_M), float(z0 - INDEX_PAD_M), float(x1 + INDEX_PAD_M), float(z1 + INDEX_PAD_M));
	}
}

} // namespace city
