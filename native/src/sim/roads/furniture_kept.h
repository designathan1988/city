// The street furniture of each segment, placed once and kept while the streets stay as they were (traffic::street_furniture_of is
// its front): every reader of a segment's kit, what the player put up and cut, and what a blow left standing reads one placement
// instead of placing it again. The entries are made for the streets' revision (CityData::street_revision, which every change to
// the map moves but a building that grew: touch(), a new map, a load) and dropped together when it moves. The city grows many a
// second: keyed by the city's whole revision, the furniture of every street in view was placed again all the time. A lot's gates
// are an input of its street's placement (the pieces keep off them), so a lot that comes, goes or changes drops only the entries
// whose reach meets it (forget_meeting, from BuildingStore::mark under the map's exclusive lock). A junction's control is an input
// too (its signal posts): an entry keeps the ends it was placed signalled at, and a control the traffic chose (RoadNode::auto_signal,
// derived, never moving the revision) drops the placements of that junction's arms (forget_seg, from traffic::apply_flow_control).
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
// edit, which may read a segment before and after changing it, has it placed anew, and so does a reader of a few kinds over streets as
// far as the eye sees (the lamps' pools, city_cut.h keep_lamps): kept, every piece of all those streets would be held.
enum class FurnitureRead : uint8_t { KEPT, PLACED };

class FurnitureKept {
public:
	using Spots = std::shared_ptr<const std::vector<FurnitureSpot>>;
	// Ground in the world's x and z: how far the lots whose gates a segment's placement reads may stand from it.
	struct Reach {
		float x0 = 0, z0 = 0, x1 = 0, z1 = 0;
		bool meets(const Reach &o) const { return x0 <= o.x1 && o.x0 <= x1 && z0 <= o.z1 && o.z0 <= z1; }
	};

	// Which of a segment's two ends a placement stood signalled at: bit 0 its end a, bit 1 its end b.
	using Ends = uint8_t;
	static constexpr Ends END_A = 1, END_B = 2;
	static constexpr Ends ends(bool a_signalled, bool b_signalled) { return Ends((a_signalled ? END_A : 0) | (b_signalled ? END_B : 0)); }

	// Segment `seg`'s furniture of every kind at `revision`: the kept placement, or the one `place(out)` makes, kept with the
	// ground its lots stand on (`reach`) and the ends `place` returns it placed signalled. Two readers asking at once may both place
	// it; the first kept is the one both read.
	template <typename Place>
	Spots of(uint64_t revision, int seg, const Reach &reach, Place &&place) {
		{
			std::lock_guard<std::mutex> lk(mutex_);
			forget_older(revision);
			auto it = segs_.find(seg);
			if (it != segs_.end()) return it->second.spots;
		}
		auto made = std::make_shared<std::vector<FurnitureSpot>>();
		const Ends signalled = place(*made);
		std::lock_guard<std::mutex> lk(mutex_);
		forget_older(revision);
		return segs_.emplace(seg, Entry{ Spots(std::move(made)), reach, signalled }).first->second.spots;
	}

	// Segment `seg`'s placement is made again when next read: the control of one of its ends changed (traffic::apply_flow_control,
	// under the map's exclusive lock). O(1).
	void forget_seg(int seg) {
		std::lock_guard<std::mutex> lk(mutex_);
		segs_.erase(seg);
	}

	// `fn(seg, ends)` for each placement kept for `revision`, with the ends it was placed signalled at; none when the entries are
	// another revision's (they go at their next read). The invariant's reader (sim/checks/furniture.cpp): nothing allocated.
	template <typename F>
	void each_kept(uint64_t revision, F &&fn) {
		std::lock_guard<std::mutex> lk(mutex_);
		if (revision != revision_) return;
		for (const auto &kept : segs_) fn(kept.first, kept.second.signalled);
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
		Ends signalled = 0;
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
