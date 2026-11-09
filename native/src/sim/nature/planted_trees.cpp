// Planted trees and clearings (see planted_trees.h).
#include "sim/nature/planted_trees.h"

#include "core/math.h"
#include "core/random.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float HASH_CELL_M = 32.f; // about the widest dab the spacing test looks round
constexpr int STREAMS = 8; // a dab's hash makes room for this many streams of draws

template <typename T>
int take_slot(std::vector<T> &list, SlotIds &free) {
	return free.next(int(list.size()));
}
} // namespace

void PlantedTrees::clear() {
	trees.clear();
	clearings.clear();
	free_trees_.clear();
	free_clearings_.clear();
	hash_.clear();
	clear_hash_.clear();
	hash_.cell = clear_hash_.cell = HASH_CELL_M;
	live_trees_ = live_clearings_ = 0;
}

int PlantedTrees::add(const PlantedTree &t) {
	int id = take_slot(trees, free_trees_);
	put(id, t);
	return id;
}

void PlantedTrees::put(int id, const PlantedTree &t) {
	if (id < int(trees.size()) && trees[size_t(id)].alive) remove(id);
	trees.resize(size_t(free_trees_.claim(id, int(trees.size()))));
	trees[size_t(id)] = t;
	trees[size_t(id)].alive = true;
	hash_.insert_aabb(id, t.x, t.z, t.x, t.z);
	live_trees_++;
}

void PlantedTrees::remove(int id) {
	PlantedTree &t = trees[size_t(id)];
	if (!t.alive) return;
	hash_.remove_aabb(id, t.x, t.z, t.x, t.z);
	t.alive = false;
	free_trees_.release(id);
	live_trees_--;
}

int PlantedTrees::add_clearing(const TreeClearing &c) {
	int id = take_slot(clearings, free_clearings_);
	put_clearing(id, c);
	return id;
}

void PlantedTrees::put_clearing(int id, const TreeClearing &c) {
	clearings.resize(size_t(free_clearings_.claim(id, int(clearings.size()))));
	clearings[size_t(id)] = c;
	clearings[size_t(id)].alive = true;
	clear_hash_.insert_aabb(id, c.x - c.radius, c.z - c.radius, c.x + c.radius, c.z + c.radius);
	live_clearings_++;
}

void PlantedTrees::remove_clearing(int id) {
	TreeClearing &c = clearings[size_t(id)];
	if (!c.alive) return;
	clear_hash_.remove_aabb(id, c.x - c.radius, c.z - c.radius, c.x + c.radius, c.z + c.radius);
	c.alive = false;
	free_clearings_.release(id);
	live_clearings_--;
}

void PlantedTrees::keep_slots(int tree_slots, int clearing_slots) {
	for (int k = int(trees.size()); k < tree_slots; k++) free_trees_.release(k);
	if (tree_slots > int(trees.size())) trees.resize(size_t(tree_slots));
	for (int k = int(clearings.size()); k < clearing_slots; k++) free_clearings_.release(k);
	if (clearing_slots > int(clearings.size())) clearings.resize(size_t(clearing_slots));
}

bool PlantedTrees::cleared(float x, float z) const { return cleared(x, z, -1); }

bool PlantedTrees::cleared(float x, float z, int skip) const {
	bool hit = false;
	clear_hash_.query(x, z, x, z, [&](int id) {
		const TreeClearing &c = clearings[size_t(id)];
		hit = hit || (id != skip && c.alive && (c.x - x) * (c.x - x) + (c.z - z) * (c.z - z) < c.radius * c.radius);
	});
	return hit;
}

void PlantedTrees::clearings_near(float minx, float minz, float maxx, float maxz, std::vector<int> &out) const {
	clear_hash_.query(minx, minz, maxx, maxz, [&](int id) { out.push_back(id); });
	std::sort(out.begin(), out.end());
	out.erase(std::unique(out.begin(), out.end()), out.end());
}

float tree_seed(TreeKind kind, float r0, float r1, const TreeBrushDef &d) {
	if (kind == TreeKind::MIXED) return r0 * d.seed_top;
	size_t k = size_t(kind);
	int slot = d.kind_first[k] + std::min(d.kind_count[k] - 1, int(r0 * d.kind_count[k]));
	return (float(slot) + r1 * d.seed_top) / float(d.model_slots);
}

int tree_slot(float seed, const TreeBrushDef &d) { return std::min(d.model_slots - 1, int(seed * d.model_slots)); }

PlantedTree one_tree(TreeKind kind, const TreeBrush &b, float x, float z, uint32_t seed, const TreeBrushDef &d) {
	uint32_t h = hash_combine(hash_u32(seed), hash_combine(uint32_t(int32_t(std::floor(x))), uint32_t(int32_t(std::floor(z)))));
	Rng r(uint64_t(h) * STREAMS + 1);
	PlantedTree t;
	float vary = b.variation * (r.f() * 2.f - 1.f);
	t.x = x;
	t.z = z;
	t.height = b.height * std::max(d.min_height_share, 1.f + vary);
	t.yaw = r.f() * TAU_F;
	t.seed = tree_seed(kind, r.f(), r.f(), d);
	return t;
}

std::vector<PlantedTree> plant_stand(TreeKind kind, const TreeBrush &b, float x, float z, float radius, uint32_t seed,
		const std::vector<PlantedTree> &nearby, const TreeBrushDef &d) {
	Rng r(uint64_t(seed) + 1);
	float hectares = PI_F * radius * radius / d.m2_per_hectare, wanted = b.density * hectares;
	int count = std::min(d.max_per_dab, int(wanted) + (r.f() < wanted - std::floor(wanted) ? 1 : 0));
	std::vector<PlantedTree> out;
	auto free_at = [&](const std::vector<PlantedTree> &list, float px, float pz) {
		return std::all_of(list.begin(), list.end(), [&](const PlantedTree &t) { return std::hypot(t.x - px, t.z - pz) >= b.spacing; });
	};
	for (int i = 0; i < count * d.tries_per_tree && int(out.size()) < count; i++) {
		float rr = radius * std::sqrt(r.f()), a = r.f() * TAU_F;
		float px = x + std::cos(a) * rr, pz = z + std::sin(a) * rr;
		if (!free_at(nearby, px, pz) || !free_at(out, px, pz)) continue;
		out.push_back(one_tree(kind, b, px, pz, r.next(), d));
	}
	return out;
}

} // namespace city
