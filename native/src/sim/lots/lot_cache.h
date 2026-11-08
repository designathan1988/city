// The zoned lots per 256 m cell (the streets' zoned sides cut into lots, giving way to the player's zoned lots), re-derived whenever
// the ChangeLog lists the cell in lot_cells (only the dirty cells): these are never saved, buildings and the player's lots are. And
// the proposal: what the streets would cut with every side zoned (proposed_lots.h), read through `proposed`.
#pragma once

#include "sim/citygen/lot_gen.h"
#include "sim/lots/lot_book.h"
#include "sim/lots/proposed_lots.h"

#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

struct CityState;
struct Catalog;

// A lot of the streets' as a ring on the ground: its front (the side on the street, local +z) first, as the player's lots hold it.
std::vector<XZ> lot_ring(const Lot &l);
// Whether the lots of `zone` are cut as works: its base is industrial (lot_gen cuts works' strips deeper and wider).
bool cut_for_works(const Catalog &c, int zone);
// Whether p lies on a lot of the streets' (its rectangle, in its own frame).
bool on_lot(const Lot &l, const XZ &p);
// The CHUNK cells whose lots may hold a point: its own and the eight round it (a lot reaches past the cell its middle is in).
template <typename F>
void each_cell_round(const XZ &p, F &&fn) {
	const int ci = int(std::floor(p.x / CHUNK)), cj = int(std::floor(p.z / CHUNK));
	for (int j = cj - 1; j <= cj + 1; j++)
		for (int i = ci - 1; i <= ci + 1; i++) fn(cell_key(i, j));
}

class LotCache {
public:
	std::unordered_map<int64_t, std::vector<Lot>> by_cell; // CHUNK key -> lots whose centre is in the cell

	void clear() { by_cell.clear(); }
	// Recomputes every cell listed in the log (threads: one cell per task) and drops the proposal of every cell whose streets or
	// ground changed; clears both lists.
	void process_dirty(CityState &s);
	// The lots of one cell computed from the current streets, the player's lots read through their book's own filing.
	static std::vector<Lot> compute(const CityState &s, int64_t key);
	// What the streets would cut there, every side taken as zoned for works or for any other use (lot_gen cuts works deeper and
	// wider), whatever the player holds: what the Zones tool shows and its brush takes. Read it through `proposed`.
	static std::vector<Lot> proposal(const CityState &s, int64_t key, bool works);
	// The proposal of a cell, cut once and kept while its streets stay (CityState::proposed).
	static ProposedLots::Cell proposed(const CityState &s, int64_t key, bool works);
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

// Whether `fn` holds for one of the zoned lots of the streets' round p that the player does not hold (the land a lot of theirs
// takes, or must not run into); it is asked of each such lot until it holds.
template <typename F>
bool any_street_lot(const LotCache &lots, const LotBook &book, const XZ &p, F &&fn) {
	bool hit = false;
	each_cell_round(p, [&](int64_t key) {
		const std::vector<Lot> *cell = lots.lots_of(key);
		for (size_t k = 0; cell && !hit && k < cell->size(); k++) {
			const Lot &l = (*cell)[k];
			hit = l.zone != 0 && book.at(l.x, l.z) == 0 && fn(l);
		}
	});
	return hit;
}

} // namespace city
