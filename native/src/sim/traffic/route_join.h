// Cooperative unpublished path joining; query/prefix/old-tail order stays explicit and cancellation releases its one draft.
#pragma once
#include "core/work_budget.h"
#include "sim/traffic/route_arena.h"
#include <cassert>
#include <cstdlib>
#include <limits>
namespace city::traffic {
class RouteJoin {
public:
	bool prepare(RoutePool &pool, const RouteRef &old, int from, const RouteDraft &path, int skip, bool prefix, int tail, WorkBudget &work, int suffix = -1, uint64_t suffix_generation = 0) {
		assert(skip >= 0 && skip <= path.size() && tail >= 0 && tail <= old.len);
		if (!allocated_) {
			if (!work.take()) return false;
			const int64_t count = int64_t(prefix) + path.size() - skip + old.len - tail + int(suffix >= 0);
			if (count > std::numeric_limits<int>::max()) std::abort(); // route handles cannot represent a larger result
			size_ = int(count); pool.prepare(draft_, size_);
			pool.set_goal(draft_, suffix >= 0 ? suffix_generation : tail < old.len ? old.goal_generation : path.goal_generation());
			same_ = size_ == old.len && draft_.goal_generation() == old.goal_generation; allocated_ = true;
		}
		const int head = int(prefix), path_end = head + path.size() - skip;
		while (copied_ < size_ && work.take()) {
			const int tail_end = path_end + old.len - tail;
			const int value = copied_ < head ? from : copied_ < path_end ? pool.at(path, skip + copied_ - head) : copied_ < tail_end ? pool.at(old, tail + copied_ - path_end) : suffix;
			if (same_) same_ = pool.at(old, copied_) == value;
			pool.write(draft_, copied_++, value);
		}
		return copied_ == size_;
	}
	void adopt(RoutePool &pool, RouteRef &into) { if (same_) pool.release(draft_); else pool.adopt(into, draft_); }
	void retire(RoutePool &pool) { pool.release(draft_); allocated_ = same_ = false; copied_ = size_ = 0; }
private:
	RouteDraft draft_;
	bool allocated_ = false, same_ = false;
	int copied_ = 0, size_ = 0;
};
} // namespace city::traffic
