// The lots the player draws (Roadcraft world/lots.ts, lotRecord.ts): land of any shape, its first side the front on the street, its corners in
// the order a building reads them. They are saved and stand beside the lots the streets propose (lot_cache.h), which give way to them.
#pragma once

#include "core/grid.h"
#include "sim/lots/lot_rings.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

struct PlayerLot {
	int id = 0;
	std::vector<LotPoint> corners; // front start, front end, then round: the back on the left of the front
	int zone = 0; // 0: not zoned
};

// The book of the player's lots, in the order they were drawn, with what is derived from it kept by the edits that change it (the
// journal's, undo and redo, a load): each lot's stand, the lots filed by place and by id, the zoned ones by the cell their stand
// stands in. A lot added at the end or changed in place is filed alone (the zone brush adds lots a stroke at a time, and the book
// grows to the thousands); one taken out or put back in the middle moves every index after it, and the book is filed again. A reader finds the lots round a point or a box instead of walking the whole book; every answer comes in the book's
// order, as walking it would give. Edits run under the map's exclusive lock, so the readers under the shared one read a book
// that does not move.
class LotBook {
public:
	int next_id = 1;

	const std::vector<PlayerLot> &all() const { return lots_; }
	bool empty() const { return lots_.empty(); }
	void clear();
	const PlayerLot *find(int id) const;
	int index_of(int id) const; // -1 on none
	// The lot a point is in, the first in the book's order; 0 on none.
	int at(double x, double z) const;
	const LotStand &stand(int index) const { return stands_[size_t(index)]; }
	int zoned_count() const { return zoned_count_; } // lots with a zone (whether the zones show with their tool put down)
	// The indices of the lots whose corners come within a filing pad of the box, ascending (the book's order).
	std::vector<int> near(double x0, double z0, double x1, double z1) const;
	// The zoned lots whose stand's middle is in CHUNK `key`, in the book's order: fn(lot, stand).
	template <typename F>
	void zoned_in(int64_t key, F &&fn) const {
		auto it = zoned_.find(key);
		if (it == zoned_.end()) return;
		for (int i : it->second) fn(lots_[size_t(i)], stands_[size_t(i)]);
	}

	// The edits (sim/commands/journal_ops.cpp, edit_history.cpp, sim/persistence/save_lots.cpp): each files the book again.
	void add(PlayerLot lot);
	void insert(int index, PlayerLot lot); // at `index`, or at the end past it
	void erase(int index); // nothing out of range
	void set(int index, PlayerLot lot); // nothing out of range
	void assign(std::vector<PlayerLot> lots);

private:
	void file();
	void file_one(int index);
	void unfile_one(int index);
	std::vector<PlayerLot> lots_;
	std::vector<LotStand> stands_; // by index in the book
	std::unordered_map<int, int> by_id_; // id -> index
	std::unordered_map<int64_t, std::vector<int>> zoned_; // CHUNK of the stand -> indices of zoned lots, ascending
	SpatialHash hash_; // indices, filed by their corners' bounds
	int zoned_count_ = 0;
};

} // namespace city
