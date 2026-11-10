// The trees the player plants and the clearings they cut, the woods' own trees included: kept in slots (a removed tree
// leaves its slot, so undo puts it back under the same id) and filed in a spatial hash, so a dab and a chunk read only
// their neighbours.
#pragma once

#include "core/grid.h"
#include "core/id_table.h"
#include "core/slot_ids.h"
#include "sim/catalog/nature_types.h"

#include <cstdint>
#include <vector>

namespace city {

enum class TreeKind : int { MIXED = 0, OAK, CYPRESS, PALM, COUNT };

// The save writes these two as their object's bytes (save_nature.cpp): the bytes between `alive` and the floats are a field
// of their own, zero, so every byte written is the record's (core/bytes.h ByteWriter::whole).
constexpr int TREE_RECORD_UNUSED = 3; // the bytes from a bool to the next float

struct PlantedTree {
	bool alive = false;
	uint8_t unused[TREE_RECORD_UNUSED] = {};
	float x = 0, z = 0, height = 0, yaw = 0;
	float seed = 0; // 0..1: which model, its tint and lean
};

struct TreeClearing {
	bool alive = false;
	uint8_t unused[TREE_RECORD_UNUSED] = {};
	float x = 0, z = 0, radius = 0;
};

// The tree brush in hand: trees per hectare, a tree's height in metres, how far heights vary (0..1), least distance
// between two trees in metres.
struct TreeBrush {
	float density = 0, height = 0, variation = 0, spacing = 0;
};

class PlantedTrees {
public:
	IdTable<PlantedTree> trees;
	IdTable<TreeClearing> clearings;
	void clear();
	// What a reader of the box needs (sim/state/city_cut.h): the trees and the clearings filed in the cells the box touches,
	// under their own slots, the copy's hashes answering for the box alone.
	PlantedTrees cut(float minx, float minz, float maxx, float maxz) const;
	int live_trees() const { return live_trees_; }
	int live_clearings() const { return live_clearings_; }
	int add(const PlantedTree &t); // the slot it took
	void put(int id, const PlantedTree &t); // undo, load: the same slot again
	void remove(int id);
	int add_clearing(const TreeClearing &c);
	void put_clearing(int id, const TreeClearing &c);
	void remove_clearing(int id);
	// Load: the saved slot counts, so slots past the last live one stay free slots as they were before the save.
	void keep_slots(int tree_slots, int clearing_slots);
	// Calls fn(id) for the trees whose foot is in the box.
	template <typename F>
	void for_trees(float minx, float minz, float maxx, float maxz, F fn) const {
		hash_.query(minx, minz, maxx, maxz, [&](int id) {
			const PlantedTree &t = trees[size_t(id)];
			if (t.alive && t.x >= minx && t.x <= maxx && t.z >= minz && t.z <= maxz) fn(id);
		});
	}
	// True when a clearing covers the point.
	bool cleared(float x, float z) const;
	// The clearings near the box, for a whole chunk's candidates to test (`cleared` looks them up one by one).
	void clearings_near(float minx, float minz, float maxx, float maxz, std::vector<int> &out) const;

private:
	SpatialHash hash_, clear_hash_;
	SlotIds free_trees_, free_clearings_;
	int live_trees_ = 0, live_clearings_ = 0;
};

// A seed in a kind's share of the models' range (any model for MIXED).
float tree_seed(TreeKind kind, float r0, float r1, const TreeBrushDef &d);
// The tree a seed grows as: the model slot among `model_slots`.
int tree_slot(float seed, const TreeBrushDef &d);
// One tree at a point, of the brush's height varied.
PlantedTree one_tree(TreeKind kind, const TreeBrush &b, float x, float z, uint32_t seed, const TreeBrushDef &d);
// The stand one dab of the brush plants in a circle: as many as the density asks for its area, none nearer than the
// spacing to `nearby` or to one another.
std::vector<PlantedTree> plant_stand(TreeKind kind, const TreeBrush &b, float x, float z, float radius, uint32_t seed,
		const std::vector<PlantedTree> &nearby, const TreeBrushDef &d);

} // namespace city
