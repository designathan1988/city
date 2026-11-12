// Stable LIFO free ids with constant-time restoration of a specific id, preserving every other id's reuse order.
#pragma once

#include <cassert>
#include <cstddef>
#include <vector>

namespace city {

class FreeSlots {
public:
	bool empty() const { return last_ < 0; }
	size_t size() const { return size_; }
	size_t retained_bytes() const { return links_.capacity() * sizeof(Link); }
	int back() const { assert(!empty()); return last_; }
	void pop_back() { erase(back()); }
	void push_back(int id) {
		assert(id >= 0);
		if (size_t(id) >= links_.size()) links_.resize(size_t(id) + 1);
		Link &link = links_[size_t(id)];
		if (link.previous != UNLISTED) return;
		link.previous = last_;
		link.next = NONE;
		if (last_ >= 0) links_[size_t(last_)].next = id;
		else first_ = id;
		last_ = id;
		size_++;
	}
	void erase(int id) {
		if (id < 0 || size_t(id) >= links_.size()) return;
		Link &link = links_[size_t(id)];
		if (link.previous == UNLISTED) return;
		if (link.previous >= 0) links_[size_t(link.previous)].next = link.next;
		else first_ = link.next;
		if (link.next >= 0) links_[size_t(link.next)].previous = link.previous;
		else last_ = link.previous;
		link = Link{};
		size_--;
	}
	void clear() {
		links_.clear();
		first_ = last_ = NONE;
		size_ = 0;
	}

private:
	static constexpr int NONE = -1, UNLISTED = -2;
	struct Link { int previous = UNLISTED, next = NONE; };
	std::vector<Link> links_;
	int first_ = NONE, last_ = NONE;
	size_t size_ = 0;
};

} // namespace city
