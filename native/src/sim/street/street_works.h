// What the player builds along the streets (Roadcraft doc.ts poles, spans, barriers and landscape): items on the
// footways, utility poles and the spans between them, barriers, and the cuts that take a piece of a street's own kit
// away. Everything else on a street is derived by rule (sim/roads/street_furniture.h); traffic::street_furniture_of reads
// both as one list. Records keep their ids through undo (sim/commands/edit_journal.h) and the save file.
#pragma once

#include "core/grid.h"
#include "core/id_table.h"
#include "core/slot_ids.h"
#include "sim/catalog/furniture_types.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace city {

enum class ItemKind : uint8_t { TREE = 0, SHRUB, BENCH, BIN, LAMP, HYDRANT, POSTBOX, PHONE, DRAIN, MEADOW, SIGN, STREETNAME, COUNT };
// A piece of a street's furniture, which goes with its road when the road goes and moves with it when it changes (Roadcraft
// roads/furnitureCatalog.ts, streetKit.ts carried); a sign, a name plate, a phone or long grass stays where it was put.
constexpr bool street_kit_piece(ItemKind k) {
	return k == ItemKind::TREE || k == ItemKind::SHRUB || k == ItemKind::BENCH || k == ItemKind::BIN || k == ItemKind::LAMP ||
			k == ItemKind::HYDRANT || k == ItemKind::POSTBOX || k == ItemKind::DRAIN;
}
constexpr int ITEM_KINDS = int(ItemKind::COUNT);

struct StreetItem {
	bool alive = false;
	ItemKind kind = ItemKind::BENCH;
	SignType sign = SignType::STOP; // a SIGN
	float x = 0, z = 0;
	std::string text; // a SIGN's words, a STREETNAME's name
	float box(int axis) const { return axis == 0 ? x : z; }
};

// What a pole is built as (Roadcraft 4.0 world/utilities.ts POLE_KINDS, after CELESC E-313.0002): URBAN, the cross-arm held out
// over the street from the mast (structure B1) with the secondary rack under it (S1); RURAL, the cross-arm across the crown (N1).
enum class PoleKind : uint8_t { URBAN = 0, RURAL = 1 };

struct UtilityPole {
	bool alive = false;
	float x = 0, z = 0;
	bool lamp = false;
	PoleKind kind = PoleKind::URBAN;
	std::vector<int> spans; // reaching it, in the order they were strung
};

struct UtilitySpan {
	bool alive = false;
	int a = -1, b = -1;
};

struct Barrier {
	bool alive = false;
	BarrierKind kind = BarrierKind::FENCE;
	std::vector<float> pts; // x, z of each point of the path: two points or more
};

// A plate standing: where on the road's level, what it says, which way its face looks.
struct SignStand {
	float x = 0, y = 0, z = 0;
	SignType type = SignType::STOP;
	std::string text;
	float face_x = 0, face_z = 1;
};

// The streets' name plates (sim/street/street_names.h settle_name_plates keeps it): by name item, the segment it names,
// its street and the plates at its corners; by chunk, the name items with a plate there, in id order. Made again only for
// the names an edit reached, so a chunk's plates are read without walking every name and its street.
struct NamePlates {
	struct Entry {
		int seg = -1;
		std::vector<int> chain;
		std::vector<SignStand> plates;
		std::vector<int64_t> chunks; // of its plates, each once
	};
	std::unordered_map<int, Entry> by_item;
	std::unordered_map<int64_t, std::vector<int>> by_chunk; // name items, ascending
	std::unordered_map<int, std::vector<int>> by_seg; // name items whose street holds the segment
	std::vector<int> dirty; // name items added, put back or removed since the last settle
	// The plates standing in CHUNK `key`, in the order the names were made and their streets walked.
	template <typename F>
	void in_chunk(int64_t key, F &&fn) const {
		auto it = by_chunk.find(key);
		if (it == by_chunk.end()) return;
		for (int id : it->second)
			for (const SignStand &p : by_item.at(id).plates)
				if (chunk_of(p.x, p.z) == key) fn(p);
	}
};

// A piece of a street's own kit the player took away, by its place.
struct StreetCut {
	bool alive = false;
	float x = 0, z = 0;
};

// Records under stable ids: a freed id is used again, and `put` brings a record back under its own.
template <typename T>
struct StreetTable {
	IdTable<T> items;
	SlotIds free_ids;
	int alive = 0;

	void clear() {
		items.clear();
		free_ids.clear();
		alive = 0;
	}
	int add(const T &rec) {
		const int id = free_ids.next(int(items.size()));
		put(id, rec);
		return id;
	}
	void put(int id, const T &rec) {
		items.resize(size_t(free_ids.claim(id, int(items.size()))));
		items[size_t(id)] = rec;
		items[size_t(id)].alive = true;
		alive++;
	}
	void drop(int id) {
		items[size_t(id)] = T();
		free_ids.release(id);
		alive--;
	}
};

class StreetWorks {
public:
	StreetTable<StreetItem> items;
	StreetTable<UtilityPole> poles;
	StreetTable<UtilitySpan> spans;
	StreetTable<Barrier> barriers;
	StreetTable<StreetCut> cuts;
	NamePlates names; // derived from the name items and the roads: never saved

	void clear();
	bool empty() const { return items.alive + poles.alive + barriers.alive + cuts.alive == 0; }
	// What a reader of the box needs of the works, copied under their own ids (sim/state/city_cut.h): the items, poles, barriers
	// and cuts filed in the cells the box touches (the copy's hashes answer for the box alone), the poles up to two spans from
	// those poles with the spans between (a pole's arms read the poles a span away, and a span drawn reads both its poles'
	// arms), and the name plates of the chunks the box touches.
	StreetWorks cut(float minx, float minz, float maxx, float maxz) const;

	int add_item(const StreetItem &r);
	void put_item(int id, const StreetItem &r);
	void remove_item(int id);
	int add_pole(const UtilityPole &r);
	void put_pole(int id, const UtilityPole &r);
	void remove_pole(int id); // its spans first: the caller takes them (sim/commands j_remove_pole)
	int add_span(const UtilitySpan &r);
	void put_span(int id, const UtilitySpan &r);
	void remove_span(int id);
	int span_between(int a, int b) const; // the span joining two poles, or -1
	int add_barrier(const Barrier &r);
	void put_barrier(int id, const Barrier &r);
	void remove_barrier(int id);
	int add_cut(const StreetCut &r);
	void put_cut(int id, const StreetCut &r);
	void remove_cut(int id);

	// Each id once, those whose place lies in the box (a barrier: whose path's bounds meet it).
	template <typename F>
	void items_in(float x0, float z0, float x1, float z1, F &&fn) const { unique(item_hash_, x0, z0, x1, z1, fn); }
	template <typename F>
	void poles_in(float x0, float z0, float x1, float z1, F &&fn) const { unique(pole_hash_, x0, z0, x1, z1, fn); }
	template <typename F>
	void barriers_in(float x0, float z0, float x1, float z1, F &&fn) const { unique(barrier_hash_, x0, z0, x1, z1, fn); }
	template <typename F>
	void cuts_in(float x0, float z0, float x1, float z1, F &&fn) const { unique(cut_hash_, x0, z0, x1, z1, fn); }
	// The nearest of each within `reach` of (x, z), or -1.
	int item_near(float x, float z, float reach) const;
	int pole_near(float x, float z, float reach) const;
	int barrier_near(float x, float z, float reach) const;

private:
	static constexpr float WORKS_CELL_M = 64.f; // a few pole spacings (Roadcraft poleIndex.ts)
	SpatialHash item_hash_{ WORKS_CELL_M, {} }, pole_hash_{ WORKS_CELL_M, {} }, barrier_hash_{ WORKS_CELL_M, {} }, cut_hash_{ WORKS_CELL_M, {} };

	template <typename F>
	static void unique(const SpatialHash &h, float x0, float z0, float x1, float z1, F &&fn) { h.query_unique(x0, z0, x1, z1, fn); }
	void file_barrier(int id, const Barrier &b, bool in);
};

// Distance from a point to a barrier's path.
float barrier_distance(const Barrier &b, float x, float z);

} // namespace city
