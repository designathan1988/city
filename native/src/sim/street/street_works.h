// What the player builds along the streets (Roadcraft doc.ts poles, spans, barriers and landscape): items on the
// footways, utility poles and the spans between them, barriers, and the cuts that take a piece of a street's own kit
// away. Everything else on a street is derived by rule (sim/roads/street_furniture.h); traffic::street_furniture_of reads
// both as one list. Records keep their ids through undo (sim/commands/edit_journal.h) and the save file.
#pragma once

#include "core/grid.h"
#include "sim/catalog/furniture_types.h"

#include <algorithm>
#include <string>
#include <vector>

namespace city {

enum class ItemKind : uint8_t { TREE = 0, SHRUB, BENCH, BIN, LAMP, HYDRANT, POSTBOX, PHONE, DRAIN, MEADOW, SIGN, STREETNAME, COUNT };
constexpr int ITEM_KINDS = int(ItemKind::COUNT);

struct StreetItem {
	bool alive = false;
	ItemKind kind = ItemKind::BENCH;
	SignType sign = SignType::STOP; // a SIGN
	float x = 0, z = 0;
	std::string text; // a SIGN's words, a STREETNAME's name
	float box(int axis) const { return axis == 0 ? x : z; }
};

struct UtilityPole {
	bool alive = false;
	float x = 0, z = 0;
	bool lamp = false;
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

// A piece of a street's own kit the player took away, by its place.
struct StreetCut {
	bool alive = false;
	float x = 0, z = 0;
};

// Records under stable ids: a freed id is used again, and `put` brings a record back under its own.
template <typename T>
struct StreetTable {
	std::vector<T> items;
	std::vector<int> free_ids;
	int alive = 0;

	void clear() {
		items.clear();
		free_ids.clear();
		alive = 0;
	}
	int add(const T &rec) {
		const int id = free_ids.empty() ? int(items.size()) : free_ids.back();
		put(id, rec);
		return id;
	}
	void put(int id, const T &rec) {
		for (int k = int(items.size()); k < id; k++) free_ids.push_back(k);
		if (id >= int(items.size())) items.resize(size_t(id) + 1);
		free_ids.erase(std::remove(free_ids.begin(), free_ids.end(), id), free_ids.end());
		items[size_t(id)] = rec;
		items[size_t(id)].alive = true;
		alive++;
	}
	void drop(int id) {
		items[size_t(id)] = T();
		free_ids.push_back(id);
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

	void clear();
	bool empty() const { return items.alive + poles.alive + barriers.alive + cuts.alive == 0; }

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
	static constexpr float CELL_M = 64.f; // a few pole spacings (Roadcraft poleIndex.ts)
	SpatialHash item_hash_{ CELL_M, {} }, pole_hash_{ CELL_M, {} }, barrier_hash_{ CELL_M, {} }, cut_hash_{ CELL_M, {} };

	template <typename F>
	static void unique(const SpatialHash &h, float x0, float z0, float x1, float z1, F &&fn) {
		std::vector<int> seen;
		h.query(x0, z0, x1, z1, [&](int id) {
			if (std::find(seen.begin(), seen.end(), id) != seen.end()) return;
			seen.push_back(id);
			fn(id);
		});
	}
	void file_barrier(int id, const Barrier &b, bool in);
};

// Distance from a point to a barrier's path.
float barrier_distance(const Barrier &b, float x, float z);

} // namespace city
