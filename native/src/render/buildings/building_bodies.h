// The bodies of the building records (sim/buildings/b40/record.h): made once per (seed, zone, floors, size)
// and kept, so every level of a chunk draws the same building; and the placement of a record in the world (lot
// basis, floor on the highest ground + 0.15 m, building shade, ground sampler).
#pragma once

#include "render/buildings/facade.h"
#include "render/buildings/pane_rooms.h"
#include "render/buildings/body_cache.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/interior/cut.h"
#include "sim/buildings/building_store.h"
#include "sim/terrain/height_patch.h"

#include <memory>
#include <unordered_map>

namespace city {

class BuildingBodies {
public:
	explicit BuildingBodies(std::shared_ptr<BodyCache> cache = {}) : cache_(cache ? std::move(cache) : std::make_shared<BodyCache>(0)) {}
	const b40::Body &body(const b40::Data &data, const Building &rec, const b40::BodyInput *input = nullptr);
	// Fills `pl` for the record (this workspace pins pl.b until a different body() call or clear). `ground`
	// is the job's copy of the terrain under its lots (pl.ground samples it: it must outlive pl). `cut`: the building
	// cut open at a floor (the Interior layer): its body is the cut one and its panes show no room behind them.
	void place(const b40::Data &data, const b40::BuildingLook &look, const HeightPatch &ground, const Building &rec, b40::Placed &pl,
			const b40::CutView *cut = nullptr, const b40::BodyInput *input = nullptr);
	void clear() {
		rooms_.reset(); cut_.reset(); current_.reset(); source_ = {};
	}

private:
	std::shared_ptr<BodyCache> cache_;
	BodyIdentity identity_{};
	b40::BodyInput source_; // this job's active authored-version lease
	std::shared_ptr<const b40::Body> current_;
	std::unique_ptr<b40::PaneRooms> rooms_;
	std::unique_ptr<b40::Body> cut_;
	int cut_level_ = 0, cut_dir_ = 0;
	uint32_t cut_turn_ = 0;
	const b40::Body &cut_body(const b40::Body &whole, const Building &rec, const b40::LotFrame &f, const b40::CutView &cut);
};

} // namespace city
