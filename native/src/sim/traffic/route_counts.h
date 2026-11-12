// Immutable exact micro-vehicle counts for one route input; lane-list writers publish changes with their membership.
#pragma once
#include "core/paged_array.h"

namespace city {
class RouteCounts {
	using Storage = PagedArray<int, 256>; // small hot count pages; a retained request does not copy a whole lane grid
public:
	using Reader = Storage::Reader;
	struct Input { Storage::Snapshot values; bool ready = false; };
	void reset(size_t lanes) { values_.assign(lanes, 0); ready_ = true; }
	void grow(size_t lanes) { if (ready_) values_.grow(lanes); }
	void changed(size_t lane, size_t count) { if (ready_ && lane < values_.size()) values_.set(lane, int(count)); }
	Input capture(size_t lanes) const {
		if (!ready_ || values_.size() < lanes) return {}; // unprepared low-level graphs keep their existing synchronous reader
		return {values_.snapshot(), true};
	}
private:
	Storage values_;
	bool ready_ = false;
};
} // namespace city
