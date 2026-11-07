// The rooms seen through a building's panes: its interior (sim/buildings/interior) and the rooms worked out so far. A pane
// alike on another floor of the same type is looked at once.
#pragma once

#include "render/buildings/shell.h"
#include "sim/buildings/interior/interior.h"

#include <memory>
#include <mutex>
#include <unordered_map>

namespace city::b40 {

// Shared by every job that draws the body (the cache of building_bodies.h): the floors and the rooms worked out are kept under
// a lock, a room worked out outside it (the same pane gives the same room, whoever asks first). Made from a held body, it holds
// it: its interior reads that body for as long as it lives.
struct PaneRooms {
	PaneRooms(const Data &data, const Body &body, Purpose purpose) : inside_(data, body, purpose) {}
	PaneRooms(const Data &data, std::shared_ptr<const Body> body, Purpose purpose) : body_(std::move(body)), inside_(data, *body_, purpose) {}
	Purpose purpose() const { return inside_.purpose(); }
	const FloorInterior &floor(int level) { // made once, then never changed: read after the lock is let go
		std::lock_guard<std::mutex> lk(mutex_);
		return inside_.at(level);
	}
	template <typename Make>
	PaneRoom room(uint64_t key, Make &&make) {
		{
			std::lock_guard<std::mutex> lk(mutex_);
			auto it = known_.find(key);
			if (it != known_.end()) return it->second;
		}
		const PaneRoom made = make();
		std::lock_guard<std::mutex> lk(mutex_);
		return known_.emplace(key, made).first->second;
	}

private:
	std::shared_ptr<const Body> body_; // first: the interior below reads it
	Interior inside_;
	std::mutex mutex_;
	std::unordered_map<uint64_t, PaneRoom> known_;
};

} // namespace city::b40
