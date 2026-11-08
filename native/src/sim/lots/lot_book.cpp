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
	file_one(int(lots_.size()) - 1); // appended: no other lot's index moves
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
	unfile_one(index);
	lots_[size_t(index)] = std::move(lot);
	file_one(index);
}

void LotBook::assign(std::vector<PlayerLot> lots) {
	lots_ = std::move(lots);
	file();
}

// What is derived from the book filed again whole: a lot taken out or put back in the middle moves every index after it.
void LotBook::file() {
	stands_.clear();
	by_id_.clear();
	zoned_.clear();
	zoned_count_ = 0;
	hash_.clear();
	hash_.cell = INDEX_CELL_M;
	stands_.reserve(lots_.size());
	for (size_t i = 0; i < lots_.size(); i++) file_one(int(i));
}

namespace {
// A ring's box on the ground, padded as the book files it; false for a ring with no corner.
bool filed_box(const std::vector<LotPoint> &q, float box[4]) {
	if (q.empty()) return false;
	double x0 = std::numeric_limits<double>::max(), z0 = x0, x1 = std::numeric_limits<double>::lowest(), z1 = x1;
	for (const LotPoint &p : q) {
		x0 = std::min(x0, p.x); z0 = std::min(z0, p.z);
		x1 = std::max(x1, p.x); z1 = std::max(z1, p.z);
	}
	box[0] = float(x0 - INDEX_PAD_M), box[1] = float(z0 - INDEX_PAD_M), box[2] = float(x1 + INDEX_PAD_M), box[3] = float(z1 + INDEX_PAD_M);
	return true;
}
} // namespace

// One lot filed at its index: its stand, its id, its zoned cell (the cell's indices kept ascending) and its box.
void LotBook::file_one(int i) {
	const PlayerLot &l = lots_[size_t(i)];
	if (stands_.size() <= size_t(i)) stands_.resize(size_t(i) + 1);
	stands_[size_t(i)] = lot_stand(l.corners);
	by_id_[l.id] = i;
	if (l.zone != 0) {
		std::vector<int> &cell = zoned_[chunk_of(float(stands_[size_t(i)].x), float(stands_[size_t(i)].z))];
		cell.insert(std::lower_bound(cell.begin(), cell.end(), i), i);
		zoned_count_++;
	}
	float box[4];
	if (filed_box(l.corners, box)) hash_.insert_aabb(i, box[0], box[1], box[2], box[3]);
}

// What file_one filed for the lot now at index i, taken out before it changes.
void LotBook::unfile_one(int i) {
	const PlayerLot &l = lots_[size_t(i)];
	by_id_.erase(l.id);
	if (l.zone != 0) {
		auto it = zoned_.find(chunk_of(float(stands_[size_t(i)].x), float(stands_[size_t(i)].z)));
		if (it != zoned_.end()) {
			it->second.erase(std::remove(it->second.begin(), it->second.end(), i), it->second.end());
			if (it->second.empty()) zoned_.erase(it);
		}
		zoned_count_--;
	}
	float box[4];
	if (filed_box(l.corners, box)) hash_.remove_aabb(i, box[0], box[1], box[2], box[3]);
}

} // namespace city
