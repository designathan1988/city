// The lots the streets would cut in each cell with every side zoned: what the Zones tool shows along the streets and what its
// brush takes, made once a cell and kept while the cell's streets and ground stay as they were. A cell is cut two ways (lot_gen's
// strips are deeper and wider for works): `works` says which. The lots depend only on the streets and the ground, never on the
// player's lots (a reader leaves out what the player already holds), so only the ChangeLog's street_cells drop a cell
// (LotCache::process_dirty); a new map or an opened city drops them all. Readers under the map's shared lock (the API, the
// overlay's jobs) and commands under the exclusive one both read it; two readers asking at once may both cut a cell, and the
// first kept is the one both read.
#pragma once

#include "sim/citygen/lot_gen.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace city {

class ProposedLots {
public:
	using Cell = std::shared_ptr<const std::vector<Lot>>;

	// The lots of CHUNK `key` cut for works or not: the kept ones, or those `cut()` makes, kept.
	template <typename Cut>
	Cell of(int64_t key, bool works, Cut &&cut) {
		const int64_t slot = slot_of(key, works);
		{
			std::lock_guard<std::mutex> lk(mutex_);
			auto it = cells_.find(slot);
			if (it != cells_.end()) return it->second;
		}
		Cell made = std::make_shared<const std::vector<Lot>>(cut());
		std::lock_guard<std::mutex> lk(mutex_);
		return cells_.emplace(slot, std::move(made)).first->second;
	}
	// The cells whose streets or ground changed, both ways of cutting them.
	void forget(const std::vector<int64_t> &keys) {
		std::lock_guard<std::mutex> lk(mutex_);
		for (int64_t key : keys) {
			cells_.erase(slot_of(key, false));
			cells_.erase(slot_of(key, true));
		}
	}
	void clear() {
		std::lock_guard<std::mutex> lk(mutex_);
		cells_.clear();
	}

private:
	// A CHUNK key (cell_key: the column in the high half) with the way of cutting in bit 62: flipping it moves the column by 2^30
	// either way, where no map's cells reach.
	static int64_t slot_of(int64_t key, bool works) { return works ? key ^ WORKS_BIT : key; }
	static constexpr int64_t WORKS_BIT = int64_t(1) << 62;
	std::mutex mutex_;
	std::unordered_map<int64_t, Cell> cells_;
};

} // namespace city
