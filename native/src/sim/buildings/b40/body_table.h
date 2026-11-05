// The bodies the player edited, each version kept whole and never changed. A record names one by `Building::body`: undo
// and redo swap that number in the record, and a job holds a version while the map moves on. A body made from a seed is
// not here, so a building nobody edited costs nothing.
#pragma once

#include "sim/buildings/b40/types.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <vector>

namespace city::b40 {

using BodyRef = std::shared_ptr<const Body>;

class BodyTable {
public:
	// The version's number, from 1: 0 in a record is a body made from its seed.
	uint32_t add(Body body) {
		std::unique_lock<std::shared_mutex> lk(mutex_);
		items_.push_back(std::make_shared<const Body>(std::move(body)));
		return uint32_t(items_.size());
	}
	BodyRef get(uint32_t id) const {
		std::shared_lock<std::shared_mutex> lk(mutex_);
		return id >= 1 && id <= items_.size() ? items_[id - 1] : nullptr;
	}
	void clear() {
		std::unique_lock<std::shared_mutex> lk(mutex_);
		items_.clear();
	}

private:
	mutable std::shared_mutex mutex_;
	std::vector<BodyRef> items_;
};

} // namespace city::b40
