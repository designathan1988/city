// The bodies of the building records (sim/buildings/b40/record.h): made once per (seed, zone, floors, size)
// and kept, so every level of a chunk draws the same building; and the placement of a record in the world (lot
// basis, floor on the highest ground + 0.15 m, building shade, ground sampler).
#pragma once

#include "render/buildings/facade.h"
#include "render/buildings/pane_rooms.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/interior/cut.h"
#include "sim/buildings/building_store.h"
#include "sim/terrain/height_patch.h"

#include <memory>
#include <unordered_map>

namespace city {

class BuildingBodies {
public:
	const b40::Body &body(const b40::Data &data, const Building &rec);
	// Fills `pl` for the record (pl.b points into the cache: valid until the next body() call that evicts). `ground`
	// is the job's copy of the terrain under its lots (pl.ground samples it: it must outlive pl). `cut`: the building
	// cut open at a floor (the Interior layer): its body is the cut one and its panes show no room behind them.
	void place(const b40::Data &data, const b40::BuildingLook &look, const HeightPatch &ground, const Building &rec, b40::Placed &pl,
			const b40::CutView *cut = nullptr);
	void clear() {
		rooms_.clear();
		cache_.clear();
		cuts_.clear();
	}

private:
	std::unordered_map<uint64_t, std::unique_ptr<b40::Body>> cache_;
	std::unordered_map<uint64_t, std::unique_ptr<b40::PaneRooms>> rooms_; // of the bodies above, by the same key
	std::unordered_map<uint64_t, std::unique_ptr<b40::Body>> cuts_; // the bodies cut open: by body, floor, step and turn of the lot
	const b40::Body &cut_body(const b40::Body &whole, const Building &rec, const b40::LotFrame &f, const b40::CutView &cut);
};

} // namespace city
