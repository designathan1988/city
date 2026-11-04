// Proposed lots per 256 m cell, re-derived from the streets whenever the ChangeLog lists the cell in lot_cells
// (only the dirty cells): lots are never saved, buildings are.
#pragma once

#include "sim/citygen/lot_gen.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

struct CityState;

class LotCache {
public:
	std::unordered_map<int64_t, std::vector<Lot>> by_cell; // CHUNK key -> lots whose centre is in the cell

	void clear() { by_cell.clear(); }
	// Recomputes every cell listed in the log (threads: one cell per task); clears the list.
	void process_dirty(CityState &s);
	// The lots of one cell computed from the current streets.
	static std::vector<Lot> compute(const CityState &s, int64_t key);
	const std::vector<Lot> *lots_of(int64_t key) const {
		auto it = by_cell.find(key);
		return it == by_cell.end() ? nullptr : &it->second;
	}
	size_t total() const {
		size_t n = 0;
		for (const auto &kv : by_cell) n += kv.second.size();
		return n;
	}
};

} // namespace city
