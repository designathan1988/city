// Mutable rectangle index with one node per record. AVL balance bounds update depth; subtree bounds prune queries.
// Large rectangles consume one node, independent of how many grid cells their area covers.
#pragma once

#include "core/free_slots.h"

#include <algorithm>
#include <vector>

namespace city {

struct Bounds2 {
	double minx = 0, minz = 0, maxx = 0, maxz = 0;
	bool overlaps(const Bounds2 &b) const { return minx <= b.maxx && maxx >= b.minx && minz <= b.maxz && maxz >= b.minz; }
	static Bounds2 united(const Bounds2 &a, const Bounds2 &b) {
		return { std::min(a.minx, b.minx), std::min(a.minz, b.minz), std::max(a.maxx, b.maxx), std::max(a.maxz, b.maxz) };
	}
};

class BoundsIndex {
public:
	size_t size() const { return count(root_); }
	// Select by the tree's (minx, id) order, without materializing a query result.
	int select(size_t ordinal) const {
		int at = root_;
		while (at != NONE) {
			const Node &node = nodes_[size_t(at)]; const size_t left = count(node.left);
			if (ordinal < left) at = node.left;
			else if (ordinal == left) return node.id;
			else { ordinal -= left + 1; at = node.right; }
		}
		return -1;
	}
	size_t retained_bytes() const { return nodes_.capacity() * sizeof(Node) + free_.retained_bytes(); }
	void clear() { decltype(nodes_)().swap(nodes_); free_ = FreeSlots(); root_ = NONE; }
	void reset() { nodes_.clear(); free_ = FreeSlots(); root_ = NONE; } // transient rebuild, retain node capacity
	struct OrderedVisit { int id = -1, at = -1; bool value = false; };
	// Spatial rejection with ascending record IDs and early stop; callers reuse scratch between queries.
	template <typename Predicate, typename F>
	void query_ordered(Predicate &&may_overlap, F &&fn, std::vector<OrderedVisit> &heap) const {
		heap.clear();
		const auto later = [](const OrderedVisit &a, const OrderedVisit &b) { return a.id > b.id; };
		const auto subtree = [&](int at) {
			if (at == NONE || !may_overlap(nodes_[size_t(at)].subtree)) return;
			heap.push_back({nodes_[size_t(at)].min_id, at, false});
			std::push_heap(heap.begin(), heap.end(), later);
		};
		subtree(root_);
		while (!heap.empty()) {
			std::pop_heap(heap.begin(), heap.end(), later);
			const OrderedVisit visit = heap.back(); heap.pop_back();
			const Node &n = nodes_[size_t(visit.at)];
			if (visit.value) { if (!fn(n.id)) return; continue; }
			subtree(n.left); subtree(n.right);
			if (may_overlap(n.box)) {
				heap.push_back({n.id, visit.at, true});
				std::push_heap(heap.begin(), heap.end(), later);
			}
		}
	}
	void insert(int id, const Bounds2 &box) { root_ = insert_at(root_, id, box); }
	void erase(int id, const Bounds2 &box) { root_ = erase_at(root_, id, box); }
	template <typename F> void query(const Bounds2 &box, F &&fn) const { query_at(root_, box, fn); }
	// A conservative predicate over a subtree's rectangle, e.g. an extruded rectangle against screen side planes.
	template <typename Predicate, typename F> void query_if(Predicate &&may_overlap, F &&fn) const { query_if_at(root_, may_overlap, fn); }

private:
	static constexpr int NONE = -1, MAX_BALANCE = 1;
	struct Node { Bounds2 box, subtree; int id = -1, left = NONE, right = NONE, height = 1, min_id = -1; size_t count = 1; };
	std::vector<Node> nodes_;
	FreeSlots free_;
	int root_ = NONE;
	size_t count(int at) const { return at == NONE ? 0 : nodes_[size_t(at)].count; }
	int height(int at) const { return at == NONE ? 0 : nodes_[size_t(at)].height; }
	void refresh(int at);
	int rotate_left(int at), rotate_right(int at), balance(int at);
	int insert_at(int at, int id, const Bounds2 &box), erase_at(int at, int id, const Bounds2 &box);
	static bool before(int id, const Bounds2 &box, const Node &node) {
		return box.minx < node.box.minx || (box.minx == node.box.minx && id < node.id);
	}
	template <typename F> void query_at(int at, const Bounds2 &box, F &fn) const {
		if (at == NONE) return;
		const Node &node = nodes_[size_t(at)];
		if (!node.subtree.overlaps(box)) return;
		query_at(node.left, box, fn);
		if (node.box.overlaps(box)) fn(node.id);
		query_at(node.right, box, fn);
	}
	template <typename Predicate, typename F> void query_if_at(int at, Predicate &may_overlap, F &fn) const {
		if (at == NONE) return;
		const Node &node = nodes_[size_t(at)];
		if (!may_overlap(node.subtree)) return;
		query_if_at(node.left, may_overlap, fn);
		if (may_overlap(node.box)) fn(node.id);
		query_if_at(node.right, may_overlap, fn);
	}
};

} // namespace city
