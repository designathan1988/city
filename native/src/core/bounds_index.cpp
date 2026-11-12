// AVL insertion/removal and aggregate bounds for BoundsIndex. Nodes use recyclable slots; queries never allocate nodes.
#include "core/bounds_index.h"

#include <cassert>

namespace city {

void BoundsIndex::refresh(int at) {
	Node &n = nodes_[size_t(at)];
	n.height = 1 + std::max(height(n.left), height(n.right));
	n.count = 1 + count(n.left) + count(n.right);
	n.subtree = n.box;
	n.min_id = n.id;
	if (n.left != NONE) n.min_id = std::min(n.min_id, nodes_[size_t(n.left)].min_id);
	if (n.right != NONE) n.min_id = std::min(n.min_id, nodes_[size_t(n.right)].min_id);
	if (n.left != NONE) n.subtree = Bounds2::united(n.subtree, nodes_[size_t(n.left)].subtree);
	if (n.right != NONE) n.subtree = Bounds2::united(n.subtree, nodes_[size_t(n.right)].subtree);
}

int BoundsIndex::rotate_left(int at) {
	Node &a = nodes_[size_t(at)];
	const int next = a.right;
	Node &b = nodes_[size_t(next)];
	a.right = b.left; b.left = at;
	refresh(at); refresh(next);
	return next;
}

int BoundsIndex::rotate_right(int at) {
	Node &a = nodes_[size_t(at)];
	const int next = a.left;
	Node &b = nodes_[size_t(next)];
	a.left = b.right; b.right = at;
	refresh(at); refresh(next);
	return next;
}

int BoundsIndex::balance(int at) {
	refresh(at);
	Node &n = nodes_[size_t(at)];
	if (height(n.left) - height(n.right) > MAX_BALANCE) {
		const Node &left = nodes_[size_t(n.left)];
		if (height(left.left) < height(left.right)) n.left = rotate_left(n.left);
		return rotate_right(at);
	}
	if (height(n.right) - height(n.left) > MAX_BALANCE) {
		const Node &right = nodes_[size_t(n.right)];
		if (height(right.right) < height(right.left)) n.right = rotate_right(n.right);
		return rotate_left(at);
	}
	return at;
}

int BoundsIndex::insert_at(int at, int id, const Bounds2 &box) {
	if (at == NONE) {
		const int slot = free_.empty() ? int(nodes_.size()) : free_.back();
		if (free_.empty()) nodes_.emplace_back(); else free_.pop_back();
		nodes_[size_t(slot)] = Node{ box, box, id };
		nodes_[size_t(slot)].min_id = id;
		return slot;
	}
	assert(nodes_[size_t(at)].id != id); // owner removes the old bounds before replacing a record
	if (before(id, box, nodes_[size_t(at)])) {
		const int child = insert_at(nodes_[size_t(at)].left, id, box);
		nodes_[size_t(at)].left = child; // recursion may have reallocated nodes_: hold indices, never references
	} else {
		const int child = insert_at(nodes_[size_t(at)].right, id, box);
		nodes_[size_t(at)].right = child;
	}
	return balance(at);
}

int BoundsIndex::erase_at(int at, int id, const Bounds2 &box) {
	if (at == NONE) return NONE;
	Node &n = nodes_[size_t(at)];
	if (before(id, box, n)) n.left = erase_at(n.left, id, box);
	else if (before(n.id, n.box, Node{ box, box, id })) n.right = erase_at(n.right, id, box);
	else {
		if (n.left == NONE || n.right == NONE) {
			const int child = n.left != NONE ? n.left : n.right;
			n = Node(); free_.push_back(at);
			return child;
		}
		int successor = n.right;
		while (nodes_[size_t(successor)].left != NONE) successor = nodes_[size_t(successor)].left;
		const int next_id = nodes_[size_t(successor)].id;
		const Bounds2 next_box = nodes_[size_t(successor)].box;
		n.id = next_id; n.box = next_box;
		n.right = erase_at(n.right, next_id, next_box);
	}
	return balance(at);
}

} // namespace city
