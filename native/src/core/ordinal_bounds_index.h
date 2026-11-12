// Rectangle index for dense size_t ordinals. Shards reuse BoundsIndex's one AVL implementation, keeping both
// internal node indices and local ids representable without capping a caller's total input or changing id order.
#pragma once
#include "core/bounds_index.h"
#include <limits>
#include <memory>
#include <vector>

namespace city {
class OrdinalBoundsIndex {
public:
	void insert(size_t id, const Bounds2 &box) {
		const size_t shard = id / IDS_PER_SHARD;
		while (shards_.size() <= shard) shards_.push_back(std::make_unique<BoundsIndex>());
		shards_[shard]->insert(int(id % IDS_PER_SHARD), box);
	}
	void erase(size_t id, const Bounds2 &box) { shards_[id / IDS_PER_SHARD]->erase(int(id % IDS_PER_SHARD), box); }
	template<class Predicate, class F>
	void query_ordered(Predicate may_overlap, F fn, std::vector<BoundsIndex::OrderedVisit> &scratch) const {
		bool proceed = true;
		for (size_t shard = 0; shard < shards_.size() && proceed; shard++)
			shards_[shard]->query_ordered(may_overlap, [&](int id) {
				proceed = fn(shard * IDS_PER_SHARD + size_t(id)); return proceed;
			}, scratch);
	}
	template<class Predicate, class F> void query_if(Predicate may_overlap, F fn) const {
		for (size_t shard = 0; shard < shards_.size(); shard++)
			shards_[shard]->query_if(may_overlap, [&](int id) { fn(shard * IDS_PER_SHARD + size_t(id)); });
	}
private:
	static constexpr size_t IDS_PER_SHARD = size_t(std::numeric_limits<int>::max()) / 2;
	std::vector<std::unique_ptr<BoundsIndex>> shards_;
};
} // namespace city
