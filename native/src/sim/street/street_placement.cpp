// Reuses one complete procedural/authored placement across trees, props and pedestrian footprint projections.
#include "sim/street/street_placement.h"

#include "sim/state/city_state.h"
#include "sim/street/street_spots.h"
#include "core/units.h"

namespace city {

std::shared_ptr<const std::vector<FurnitureSpot>> StreetPlacement::get(const CityState &s, int seg, const bool signalled[2]) const {
	const uint8_t signals = uint8_t(signalled[0]) | uint8_t(signalled[1] << 1);
	const size_t limit = size_t(std::max(0.f, s.catalog.tune.render_placement_cache_mb) * BYTES_PER_MIB);
	Roots retired;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		while (bytes_ > limit && !recent_.empty()) retire(entries_.find(recent_.back()), retired);
		const auto found = entries_.find(seg);
		if (found != entries_.end() && found->second.network == s.roads.version && found->second.signals == signals) {
			recent_.splice(recent_.begin(), recent_, found->second.age);
			return found->second.spots;
		}
	}
	// Source is held shared by the caller. Readers never wait for another reader's geometry/preparation. Simultaneous
	// cold misses may compute independently; publication keeps one immutable result and releases the redundant copy.
	auto spots = std::make_shared<std::vector<FurnitureSpot>>();
	segment_furniture(s.roads, seg, signalled, FURNITURE_ALL, *spots);
	merge_street(s.street, s.roads, seg, FURNITURE_ALL, *spots, 0);
	// Limit retained placement storage, never the pieces produced. Oversized results serve this caller uncached.
	constexpr size_t ENTRY_POINTER_WORDS = 10; // tree links/color, list links, shared control metadata
	const size_t bytes = spots->capacity() * sizeof(FurnitureSpot) + sizeof(std::vector<FurnitureSpot>)
		+ sizeof(std::pair<const int, Entry>) + sizeof(int) + ENTRY_POINTER_WORDS * sizeof(void *);
	std::lock_guard<std::mutex> lock(mutex_);
	if (!JobPool::cancelled()) {
		const auto found = entries_.find(seg);
		if (found != entries_.end()) {
			if (found->second.network == s.roads.version && found->second.signals == signals) {
				recent_.splice(recent_.begin(), recent_, found->second.age);
				return found->second.spots;
			}
			retire(found, retired);
		}
		if (bytes <= limit) {
			while (bytes_ > limit - bytes && !recent_.empty()) retire(entries_.find(recent_.back()), retired);
			recent_.push_front(seg);
			entries_.emplace(seg, Entry{ s.roads.version, signals, spots, bytes, recent_.begin() });
			bytes_ += bytes;
		}
	}
	return spots;
}

void StreetPlacement::retire(Entries::iterator found, Roots &retired) const {
	bytes_ -= found->second.bytes;
	recent_.erase(found->second.age);
	retired.push_back(std::move(found->second.spots));
	entries_.erase(found);
}

void StreetPlacement::invalidate(int seg) {
	std::shared_ptr<const std::vector<FurnitureSpot>> retired;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		const auto found = entries_.find(seg);
		if (found != entries_.end()) {
			bytes_ -= found->second.bytes;
			recent_.erase(found->second.age);
			retired = std::move(found->second.spots); entries_.erase(found);
		}
	}
}

void StreetPlacement::clear() {
	decltype(entries_) retired;
	decltype(recent_) old_order;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		retired.swap(entries_);
		old_order.swap(recent_);
		bytes_ = 0;
	}
}

} // namespace city
