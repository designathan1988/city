// The placement of a building record in the world for the building renderer's jobs (lot basis, floor on the highest ground +
// 0.15 m, building shade, ground sampler), and what the render alone derives from a record's body: the rooms behind its panes
// and the body cut open at a floor. The body itself is the simulation's (sim/buildings/b40/body_cache.h, through record_mass),
// made once for every reader. Rooms and cuts are kept exactly (the body's key and the tables' generation; a cut also by its
// floor, the camera's step and the lot's turn), the least recently drawn pushed out first; entries are shared, so a placement
// holds what it draws past an eviction.
#pragma once

#include "core/lru_cache.h"
#include "render/buildings/facade.h"
#include "render/buildings/pane_rooms.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/interior/cut.h"
#include "sim/buildings/building_store.h"
#include "sim/terrain/height_patch.h"

#include <cstdint>
#include <memory>

namespace city {

class BuildingBodies {
public:
	BuildingBodies();
	// Fills `pl` for the record; pl.hold keeps its bodies and rooms alive while it is used. `ground` is the job's copy of the
	// terrain under its lots (pl.ground samples it: it must outlive pl). `cut`: the building cut open at a floor (the Interior
	// layer): its body is the cut one and its panes show no room behind them.
	void place(const b40::Data &data, const b40::BuildingLook &look, const HeightPatch &ground, const Building &rec, b40::Placed &pl,
			const b40::CutView *cut = nullptr);
	void clear();

private:
	// The body's key (the record's function and seed, which the rooms read, are in it) and the tables' generation.
	struct Key {
		b40::BodyKey body;
		uint64_t generation = 0; // of the tables (b40::Data::generation)
		bool operator==(const Key &o) const { return body == o.body && generation == o.generation; }
	};
	// A cut body: the whole one, the floor, the camera's step and the lot's turn (its bits).
	struct CutKey {
		Key body;
		int level = 0, dir = 0;
		uint32_t turn = 0;
		bool operator==(const CutKey &o) const { return body == o.body && level == o.level && dir == o.dir && turn == o.turn; }
	};
	struct KeyHash {
		size_t operator()(const Key &k) const;
		size_t operator()(const CutKey &k) const;
	};
	LruCache<Key, b40::PaneRooms, KeyHash> rooms_;
	LruCache<CutKey, const b40::Body, KeyHash> cuts_;
	static Key key_of(const b40::Data &data, const Building &rec) { return Key{ b40::body_key(rec), data.generation }; }
	std::shared_ptr<b40::PaneRooms> rooms(const b40::Data &data, const Key &k, const std::shared_ptr<const b40::Body> &whole, const Building &rec);
	std::shared_ptr<const b40::Body> cut_body(const Key &k, const b40::Body &whole, const Building &rec, const b40::LotFrame &f, const b40::CutView &cut);
};

} // namespace city
