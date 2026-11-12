// Route page allocation/reclamation: local size-class work and stable handles replace append-only pool compaction.
#include "sim/traffic/route_arena.h"
#include "core/generation.h"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <utility>

namespace city {
RoutePool::RoutePool(RoutePool &&other) : RoutePool() { *this = std::move(other); }
RoutePool &RoutePool::operator=(RoutePool &&other) {
	if (this == &other) return *this;
	pages_ = std::move(other.pages_); free_pages_ = std::move(other.free_pages_); partial_ = other.partial_;
	other.clear(); return *this;
}
void RoutePool::link(int index) {
	Page &page = pages_[size_t(index)];
	page.previous = -1; page.next = partial_[size_t(page.kind)];
	if (page.next >= 0) pages_[size_t(page.next)].previous = index;
	partial_[size_t(page.kind)] = index;
}
void RoutePool::unlink(int index) {
	Page &page = pages_[size_t(index)];
	if (page.previous >= 0) pages_[size_t(page.previous)].next = page.next;
	else partial_[size_t(page.kind)] = page.next;
	if (page.next >= 0) pages_[size_t(page.next)].previous = page.previous;
	page.previous = page.next = -1;
}
RouteRef RoutePool::allocate(int count) {
	int kind = 0, capacity = 1;
	while (capacity < count && capacity <= std::numeric_limits<int>::max() / 2) { capacity *= 2; kind++; }
	if (capacity < count) { capacity = count; kind++; } // one large slice, no rounded signed overflow
	int index = partial_[size_t(kind)];
	if (index < 0) {
		if (free_pages_.empty()) {
			if (pages_.size() >= size_t(std::numeric_limits<int>::max())) std::abort();
			index = int(pages_.size()); pages_.emplace_back();
		} else { index = free_pages_.back(); free_pages_.pop_back(); }
		Page &page = pages_[size_t(index)];
		page.capacity = capacity; page.kind = kind;
		const int elements = std::max(PAGE_INTS, capacity);
		page.slots = elements / capacity;
		page.data.reset(new int[size_t(elements)]); // reserve raw ints; only a written route or free link is read
		link(index);
	}
	Page &page = pages_[size_t(index)];
	assert(page.capacity == capacity && page.used < page.slots);
	int offset;
	if (page.free >= 0) { offset = page.free; page.free = page.data[size_t(offset)]; }
	else { offset = page.bump; page.bump += page.capacity; }
	if (++page.used == page.slots) unlink(index);
	const int64_t handle = int64_t((uint64_t(index) << PAGE_SHIFT) | uint32_t(offset));
	return {handle, count, capacity, next_generation()};
}
int *RoutePool::data(const RouteRef &route) {
	const size_t page = size_t(uint64_t(route.off) >> PAGE_SHIFT), offset = size_t(uint64_t(route.off) & OFFSET_MASK);
	assert(route.off >= 0 && page < pages_.size() && pages_[page].data && pages_[page].capacity == route.cap);
	return pages_[page].data.get() + offset;
}
int RoutePool::at(const RouteRef &route, int k) const {
	if (k < 0 || k >= route.len) return -1;
	const size_t page = size_t(uint64_t(route.off) >> PAGE_SHIFT), offset = size_t(uint64_t(route.off) & OFFSET_MASK);
	assert(route.off >= 0 && page < pages_.size() && pages_[page].data && pages_[page].capacity == route.cap);
	return pages_[page].data[offset + size_t(k)];
}
void RoutePool::set(RouteRef &route, const int *source, int count, uint64_t goal_generation) {
	if (count <= 0) { release(route); return; }
	if (route.off >= 0 && route.len == count && route.goal_generation == goal_generation && std::memcmp(data(route), source, size_t(count) * sizeof(int)) == 0) return;
	if (route.off >= 0 && count <= route.cap) {
		std::memmove(data(route), source, size_t(count) * sizeof(int)); route.len = count; route.goal_generation = goal_generation; route.version = next_generation(); return;
	}
	RouteRef next = allocate(count); next.goal_generation = goal_generation;
	std::memmove(data(next), source, size_t(count) * sizeof(int));
	release(route); route = next;
}
void RoutePool::prepare(RouteDraft &draft, int count) {
	assert(draft.empty()); if (count > 0) draft.value_ = allocate(count);
}
void RoutePool::write(RouteDraft &draft, int at, int value) {
	assert(at >= 0 && at < draft.value_.len); data(draft.value_)[size_t(at)] = value;
}
void RoutePool::adopt(RouteRef &into, RouteDraft &prepared) {
	release(into); into = std::exchange(prepared.value_, RouteRef{});
}
void RoutePool::release(RouteRef &route) {
	if (route.off < 0) { route = {}; return; }
	const int index = int(uint64_t(route.off) >> PAGE_SHIFT), offset = int(uint64_t(route.off) & OFFSET_MASK);
	Page &page = pages_[size_t(index)];
	assert(page.data && page.used > 0 && page.capacity == route.cap);
	if (page.used == page.slots) link(index);
	page.data[size_t(offset)] = page.free; page.free = offset;
	if (--page.used == 0) {
		unlink(index); page = Page(); free_pages_.push_back(index);
	}
	route = {};
}
void RoutePool::clear() {
	pages_.clear(); free_pages_ = FreeSlots(); partial_.fill(-1);
}
} // namespace city
