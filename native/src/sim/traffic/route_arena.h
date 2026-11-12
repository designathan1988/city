// Stable size-class route pages. A vehicle/walker explicitly owns one slice; empty pages retire immediately,
// so neither allocation nor reclamation relocates another actor's path or scans the fleet.
#pragma once
#include "core/free_slots.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <utility>

namespace city {
struct RouteRef { int64_t off = -1; int len = 0, cap = 0; uint64_t version = 0, goal_generation = 0; };
// Unpublished writable slice. Published RouteRef values have no element-write API.
class RouteDraft {
public:
	RouteDraft() = default;
	RouteDraft(const RouteDraft &) = delete;
	RouteDraft &operator=(const RouteDraft &) = delete;
	RouteDraft(RouteDraft &&other) noexcept : value_(std::exchange(other.value_, RouteRef{})) {}
	RouteDraft &operator=(RouteDraft &&) = delete;
	int size() const { return value_.len; }
	uint64_t goal_generation() const { return value_.goal_generation; }
	bool empty() const { return value_.off < 0; }
private:
	friend class RoutePool;
	RouteRef value_;
};
class RoutePool {
public:
	RoutePool() { partial_.fill(-1); }
	RoutePool(RoutePool &&other);
	RoutePool &operator=(RoutePool &&other);
	RoutePool(const RoutePool &) = delete;
	RoutePool &operator=(const RoutePool &) = delete;
	int at(const RouteRef &route, int k) const;
	void set(RouteRef &route, const int *source, int count, uint64_t goal_generation = 0);
	void release(RouteRef &route);
	void prepare(RouteDraft &draft, int count);
	void release(RouteDraft &draft) { release(draft.value_); }
	int at(const RouteDraft &draft, int k) const { return at(draft.value_, k); }
	void write(RouteDraft &draft, int at, int value);
	void set_goal(RouteDraft &draft, uint64_t generation) { draft.value_.goal_generation = generation; } // unpublished target identity only
	void adopt(RouteRef &into, RouteDraft &prepared);
	void clear();
private:
	static constexpr int PAGE_INTS = 16384, CLASSES = 32;
	static constexpr unsigned PAGE_SHIFT = 32;
	static constexpr uint64_t OFFSET_MASK = 0xffffffffull;
	struct Page {
		std::unique_ptr<int[]> data;
		int capacity = 0, slots = 0, used = 0, bump = 0, free = -1, kind = 0;
		int previous = -1, next = -1;
	};
	std::deque<Page> pages_;
	FreeSlots free_pages_;
	std::array<int, CLASSES> partial_;
	void link(int page);
	void unlink(int page);
	RouteRef allocate(int count);
	int *data(const RouteRef &route);
};
inline int route_at(const RoutePool &pool, const RouteRef &route, int k) { return pool.at(route, k); }
inline void route_set(RoutePool &pool, RouteRef &route, const int *source, int count, uint64_t goal_generation = 0) { pool.set(route, source, count, goal_generation); }
} // namespace city
