// One A* for the traffic searches (cars over directed segments, walkers over nodes): binary heap with generation
// stamps (no clearing per search), a scratch per caller and thread, stale heap entries skipped.
#pragma once

#include "core/generation.h"
#include "core/paged_array.h"
#include "core/stable_chunks.h"

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace city {
namespace traffic {

enum class SearchStatus : uint8_t { PENDING, FOUND, NO_PATH, LIMIT, INVALIDATED, UNPREPARED, SKIPPED, SOURCE_RETIRED, TARGET_RETIRED };
enum class EdgeStep : uint8_t { EDGE, SKIP, END };
struct AStarCursor { size_t at = 0; };

template<class Cursor = AStarCursor> struct AStarScratch {
	struct Entry { float f = 0, g = 0; int k = -1; };
	struct Node { uint64_t stamp = 0; float g = 0; int parent = -1; };
	PagedArray<Node, 1024> nodes;
	StableChunks<Entry> heap;
	StableChunks<int> path; // goal-to-source; at() presents forward order without an unbudgeted whole reverse
	uint64_t cur = 0;

	void begin(size_t n, int from, int goal, int limit) {
		n_ = n; from_ = from; goal_ = goal; limit_ = limit; expanded_ = 0;
		heap.clear(); path.clear(); phase_ = START; status_ = SearchStatus::PENDING;
	}
	size_t length() const { return path.size(); }
	int at(size_t i) const { return path[path.size() - 1 - i]; }

	// A unit is one bounded start/pop, next-neighbour/relaxation, or parent output step. Take is shared by the caller.
	// H/Next must keep the same source/cost input between calls; the route owner guards topology before resuming.
	template<class Take, class H, class Next> SearchStatus advance(Take &&take, H &&h, Next &&next) {
		constexpr float STALE_EPS = 1e-3f;
		auto cmp = [](const Entry &a, const Entry &b) { return a.f > b.f; };
		while (status_ == SearchStatus::PENDING) {
			if (phase_ == POP && (heap.empty() || expanded_ >= limit_)) {
				status_ = heap.empty() ? SearchStatus::NO_PATH : SearchStatus::LIMIT; break;
			}
			if (!take()) break;
			if (phase_ == START) {
				nodes.grow(n_); cur = next_generation();
				nodes.set(size_t(from_), Node{cur, 0.f, -1});
				heap.push_back({h(from_), 0.f, from_}); phase_ = POP;
			} else if (phase_ == POP) {
				std::pop_heap(heap.begin(), heap.end(), cmp);
				const Entry e = heap.back(); heap.pop_back();
				if (e.g > nodes[size_t(e.k)].g + STALE_EPS) continue;
				++expanded_;
				if (e.k == goal_) { trace_ = goal_; phase_ = PATH; continue; }
				active_ = e.k; gk_ = nodes[size_t(active_)].g; cursor_ = {}; phase_ = EDGES;
			} else if (phase_ == EDGES) {
				int to = -1; float cost = 0;
				const EdgeStep step = next(active_, cursor_, to, cost);
				if (step == EdgeStep::END) { phase_ = POP; continue; }
				if (step == EdgeStep::SKIP) continue;
				const float ng = gk_ + cost;
				const Node old = nodes[size_t(to)];
				if (old.stamp == cur && ng >= old.g) continue;
				nodes.set(size_t(to), Node{cur, ng, active_});
				heap.push_back({ng + h(to), ng, to}); std::push_heap(heap.begin(), heap.end(), cmp);
			} else {
				path.push_back(trace_);
				if (trace_ == from_) { status_ = SearchStatus::FOUND; continue; }
				trace_ = nodes[size_t(trace_)].parent;
				if (trace_ < 0) status_ = SearchStatus::FOUND;
			}
		}
		return status_;
	}
private:
	enum Phase : uint8_t { START, POP, EDGES, PATH } phase_ = START;
	SearchStatus status_ = SearchStatus::PENDING;
	size_t n_ = 0;
	int from_ = -1, goal_ = -1, limit_ = 0, expanded_ = 0, active_ = -1, trace_ = -1;
	float gk_ = 0;
	Cursor cursor_;
};

// Compatibility boundary for existing synchronous callers. All search work still uses the same resumable owner.
// Next emits one neighbour (or skip/end), preserving source adjacency order. The route owner must migrate callers
// to aggregate Take/PENDING before these synchronous entry points can be considered admitted.
template <typename Cursor, typename H, typename Next>
bool astar(AStarScratch<Cursor> &sc, size_t n, int from, int goal, int limit, H h, Next next, std::vector<int> &out) {
	out.clear(); sc.begin(n, from, goal, limit);
	if (sc.advance([] { return true; }, h, next) != SearchStatus::FOUND) return false;
	out.reserve(sc.length());
	for (size_t i = 0; i < sc.length(); ++i) out.push_back(sc.at(i));
	return true;
}

} // namespace traffic
} // namespace city
