// The bodies of the building records (sim/buildings/b40/record.h), made once and kept: one cache for every job of the
// building renderer (it owns it and empties it with itself), so every level of a chunk and every worker draws the same body
// made once. Keyed exactly: by the record's body key (an edited version's stamp, or all a seed body is made from) and the
// generation of the tables it was made from. Entries are shared: a placement holds what it draws, so an eviction never frees
// a body a job is still drawing. Also the placement of a record in the world (lot basis, floor on the highest ground +
// 0.15 m, building shade, ground sampler).
#pragma once

#include "render/buildings/facade.h"
#include "render/buildings/pane_rooms.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/interior/cut.h"
#include "sim/buildings/building_store.h"
#include "sim/terrain/height_patch.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace city {

class BuildingBodies {
public:
	// The record's body as it stands (record_mass), shared: what holds it keeps it past an eviction.
	std::shared_ptr<const b40::Body> body(const b40::Data &data, const Building &rec);
	// Fills `pl` for the record; pl.hold keeps its bodies and rooms alive while it is used. `ground` is the job's copy of the
	// terrain under its lots (pl.ground samples it: it must outlive pl). `cut`: the building cut open at a floor (the Interior
	// layer): its body is the cut one and its panes show no room behind them.
	void place(const b40::Data &data, const b40::BuildingLook &look, const HeightPatch &ground, const Building &rec, b40::Placed &pl,
			const b40::CutView *cut = nullptr);
	void clear();

private:
	struct Key {
		b40::BodyKey body;
		uint64_t generation = 0; // of the tables (b40::Data::generation)
		bool operator==(const Key &o) const { return body == o.body && generation == o.generation; }
	};
	// The rooms behind the panes depend on the body and on what the record's function and seed make of it.
	struct RoomsKey {
		Key body;
		uint32_t seed = 0;
		bool operator==(const RoomsKey &o) const { return body == o.body && seed == o.seed; }
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
		size_t operator()(const RoomsKey &k) const;
		size_t operator()(const CutKey &k) const;
	};
	std::mutex mutex_; // the jobs share the cache: lookups and insertions under it, the making of a body outside it
	std::unordered_map<Key, std::shared_ptr<const b40::Body>, KeyHash> cache_;
	std::unordered_map<RoomsKey, std::shared_ptr<b40::PaneRooms>, KeyHash> rooms_;
	std::unordered_map<CutKey, std::shared_ptr<const b40::Body>, KeyHash> cuts_;
	static Key key_of(const b40::Data &data, const Building &rec) { return Key{ b40::body_key(rec), data.generation }; }
	std::shared_ptr<b40::PaneRooms> rooms(const b40::Data &data, const Key &k, const std::shared_ptr<const b40::Body> &whole, const Building &rec);
	std::shared_ptr<const b40::Body> cut_body(const Key &k, const b40::Body &whole, const Building &rec, const b40::LotFrame &f, const b40::CutView &cut);
};

} // namespace city
