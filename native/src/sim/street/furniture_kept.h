// The street furniture of each segment, placed once and kept while the city stays as it was (traffic::street_furniture_of is its
// front): every reader of a segment's kit, what the player put up and cut, and what a blow left standing reads one placement
// instead of placing it again. The entries are made for the city's revision (CityData::revision, which every change to the map
// moves: touch(), a new map, a load) and dropped together when it moves. Only a reader under the map's shared lock reads kept
// furniture (the render's jobs, the walkers, previews and probes): nothing changes while it reads, so what it keeps holds. An edit,
// which may read a segment before and after changing it, has it placed anew (FurnitureRead::PLACED).
#pragma once

#include "sim/roads/street_furniture.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace city {

// Who asks for a segment's furniture: a reader under the map's shared lock reads the placement kept for the city's revision; an
// edit, which may read a segment before and after changing it, has it placed anew.
enum class FurnitureRead : uint8_t { KEPT, PLACED };

class FurnitureKept {
public:
	using Spots = std::shared_ptr<const std::vector<FurnitureSpot>>;

	// Segment `seg`'s furniture of every kind at `revision`: the kept placement, or the one `place(out)` makes, kept. Two readers
	// asking at once may both place it; the first kept is the one both read.
	template <typename Place>
	Spots of(uint64_t revision, int seg, Place &&place) {
		{
			std::lock_guard<std::mutex> lk(mutex_);
			forget_older(revision);
			auto it = segs_.find(seg);
			if (it != segs_.end()) return it->second;
		}
		auto made = std::make_shared<std::vector<FurnitureSpot>>();
		place(*made);
		std::lock_guard<std::mutex> lk(mutex_);
		forget_older(revision);
		return segs_.emplace(seg, Spots(std::move(made))).first->second;
	}

private:
	void forget_older(uint64_t revision) {
		if (revision == revision_) return;
		segs_.clear();
		revision_ = revision;
	}
	std::mutex mutex_;
	uint64_t revision_ = 0;
	std::unordered_map<int, Spots> segs_;
};

} // namespace city
