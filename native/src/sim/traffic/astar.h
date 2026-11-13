// One A* for the traffic searches (cars over directed segments, walkers over nodes): binary heap with generation
// stamps (no clearing per search), a scratch per caller and thread, stale heap entries skipped.
#pragma once

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace city {
namespace traffic {

struct AStarScratch {
	// f and g travel in the heap entry: the pop no longer has to recompute the heuristic to recover g.
	struct Entry {
		float f = 0, g = 0;
		int k = -1;
	};
	std::vector<float> g;
	std::vector<int> parent;
	std::vector<uint32_t> stamp;
	uint32_t cur = 0;
	std::vector<Entry> heap;
};

// States 0..n-1. `h(k)` = heuristic; `expand(k, relax)` calls relax(next, step_cost) for each neighbour; `done(k)` says a
// state ends the search. Writes the states from `from` to the one that ended it into `out` and returns true when one was reached
// within `limit` expansions.
template <typename Done, typename H, typename Expand>
bool astar_to(AStarScratch &sc, size_t n, int from, Done done, int limit, H h, Expand expand, std::vector<int> &out) {
	constexpr float STALE_EPS = 1e-3f;
	out.clear();
	if (sc.g.size() < n) {
		sc.g.assign(n, 0);
		sc.parent.assign(n, -1);
		sc.stamp.assign(n, 0);
	}
	if (++sc.cur == 0) {
		std::fill(sc.stamp.begin(), sc.stamp.end(), 0);
		sc.cur = 1;
	}
	auto cmp = [](const AStarScratch::Entry &a, const AStarScratch::Entry &b) { return a.f > b.f; };
	sc.heap.clear();
	sc.g[from] = 0;
	sc.parent[from] = -1;
	sc.stamp[from] = sc.cur;
	sc.heap.push_back({ h(from), 0.f, from });
	int expanded = 0, last = -1;
	while (!sc.heap.empty() && expanded < limit) {
		std::pop_heap(sc.heap.begin(), sc.heap.end(), cmp);
		const AStarScratch::Entry e = sc.heap.back();
		sc.heap.pop_back();
		if (e.g > sc.g[e.k] + STALE_EPS) continue;
		const int k = e.k;
		expanded++;
		if (done(k)) { last = k; break; }
		const float gk = sc.g[k];
		expand(k, [&](int o, float cost) {
			float ng = gk + cost;
			if (sc.stamp[o] == sc.cur && ng >= sc.g[o]) return;
			sc.stamp[o] = sc.cur;
			sc.g[o] = ng;
			sc.parent[o] = k;
			sc.heap.push_back({ ng + h(o), ng, o });
			std::push_heap(sc.heap.begin(), sc.heap.end(), cmp);
		});
	}
	if (last < 0) return false;
	for (int k = last; k >= 0; k = sc.parent[k]) {
		out.push_back(k);
		if (k == from) break;
	}
	std::reverse(out.begin(), out.end());
	return true;
}

// The search to one state, `goal`.
template <typename H, typename Expand>
bool astar(AStarScratch &sc, size_t n, int from, int goal, int limit, H h, Expand expand, std::vector<int> &out) {
	return astar_to(sc, n, from, [goal](int k) { return k == goal; }, limit, h, expand, out);
}

} // namespace traffic
} // namespace city
