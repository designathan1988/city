// One external command's derived-state commit. Geometry needed by intermediate road validation remains current;
// lot regeneration, growth admission and public edit revision are consolidated at the outer boundary.
#pragma once

#include "sim/commands/road_edit.h"
#include "sim/state/city_state.h"

namespace city {
class EditScope {
public:
	explicit EditScope(CityState &state) : state_(state) { state_.edit_depth++; }
	EditScope(const EditScope &) = delete;
	EditScope &operator=(const EditScope &) = delete;
	~EditScope() { close(); }
	void close() {
		if (!open_) return;
		open_ = false;
		if (--state_.edit_depth == 0 && state_.edit_pending) {
			state_.edit_pending = false;
			finish_edit(state_, 0, 0, 0, 0);
		}
	}

private:
	CityState &state_;
	bool open_ = true;
};
} // namespace city
