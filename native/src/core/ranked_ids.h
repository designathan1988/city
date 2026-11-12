// Ordered live ids with logarithmic rank selection. Reuses the one AVL owner, BoundsIndex, with id-coordinate keys;
// changing the set never changes the rank of another id relative to its neighbours.
#pragma once
#include "core/bounds_index.h"
namespace city {
class RankedIds {
public:
	void set(int id, bool included) {
		const Bounds2 key{double(id), 0, double(id), 0};
		bool present = false; index_.query(key, [&](int found) { if (found == id) present = true; });
		if (included && !present) index_.insert(id, key);
		else if (!included && present) index_.erase(id, key);
	}
	size_t size() const { return index_.size(); }
	bool empty() const { return size() == 0; }
	int select(size_t ordinal) const { return index_.select(ordinal); }
	void clear() { index_.clear(); }
private:
	BoundsIndex index_;
};
} // namespace city
