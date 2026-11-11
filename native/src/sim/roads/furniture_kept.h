// The street furniture of each segment, placed once and kept while the streets stay as they were (traffic::street_furniture_of is
// its front): every reader of a segment's kit, what the player put up and cut, and what a blow left standing reads one placement
// instead of placing it again. The entries are made for the streets' revision (CityData::street_revision, which every change to
// the map moves but a building that grew: touch(), a new map, a load) and dropped together when it moves. The city grows many a
// second: keyed by the city's whole revision, the furniture of every street in view was placed again all the time. A lot's gates
// are an input of its street's placement (the pieces keep off them), so a lot that comes, goes or changes drops only the entries
// whose reach meets it (forget_meeting, from BuildingStore::mark under the map's exclusive lock).
// Only a reader under the map's shared lock reads kept furniture (the render's jobs, the walkers, previews and probes): nothing
// changes while it reads, so what it keeps holds. An edit, which may read a segment before and after changing it, has it placed
// anew (FurnitureRead::PLACED).
#pragma once

#include "sim/roads/street_furniture.h"

#include <cstdint>
#include <iterator>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace city {

// Who asks for a segment's furniture: a reader under the map's shared lock reads the placement kept for the streets' revision; an
// edit, which may read a segment before and after changing it, has it placed anew.
enum class FurnitureRead : uint8_t { KEPT, PLACED };

class FurnitureKept {
public:
	using Spots = std::shared_ptr<const std::vector<FurnitureSpot>>;
	// Ground in the world's x and z: how far the lots whose gates a segment's placement reads may stand from it.
	struct Reach {
		float x0 = 0, z0 = 0, x1 = 0, z1 = 0;
		bool meets(const Reach &o) const { return x0 <= o.x1 && o.x0 <= x1 && z0 <= o.z1 && o.z0 <= z1; }
	};

	// Segment `seg`'s furniture of every kind at `revision`: the kept placement, or the one `place(out)` makes, kept with the
	// ground its lots stand on (`reach`). Two readers asking at once may both place it; the first kept is the one both read.
	template <typename Place>
	Spots of(uint64_t revision, int seg, const Reach &reach, Place &&place) {
		{
			std::lock_guard<std::mutex> lk(mutex_);
			forget_older(revision);
			auto it = segs_.find(seg);
			if (it != segs_.end()) return it->second.spots;
		}
		auto made = std::make_shared<std::vector<FurnitureSpot>>();
		place(*made);
		std::lock_guard<std::mutex> lk(mutex_);
		forget_older(revision);
		return segs_.emplace(seg, Entry{ Spots(std::move(made)), reach }).first->second.spots;
	}

	// A lot came, went or changed on `lot`: the placements whose lots it may be among are made again when next read.
	void forget_meeting(const Reach &lot) {
		std::lock_guard<std::mutex> lk(mutex_);
		for (auto it = segs_.begin(); it != segs_.end();) it = it->second.reach.meets(lot) ? segs_.erase(it) : std::next(it);
	}

private:
	struct Entry {
		Spots spots;
		Reach reach;
	};
	void forget_older(uint64_t revision) {
		if (revision == revision_) return;
		segs_.clear();
		revision_ = revision;
	}
	std::mutex mutex_;
	uint64_t revision_ = 0;
	std::unordered_map<int, Entry> segs_;
};

} // namespace city
