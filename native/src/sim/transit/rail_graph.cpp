// Rail graph and paths (Roadcraft 4.0 world/transit.ts:164-349): the points are joined through a spatial hash (never
// every point for each point), the search keeps a binary heap (never a scan of every point for the nearest).
#include "sim/transit/rail_graph.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <queue>

namespace city {
namespace transit {

namespace {
struct OnEdge {
	int e = -1;
	float u = 0;
	V2 q;
};

// The stretch nearest `p` and where on it (railPath onEdge).
OnEdge on_edge(const RailGraph &g, V2 p) {
	OnEdge best;
	float best_d = BIG;
	for (size_t i = 0; i < g.edges.size(); i++) {
		const RailEdge &e = g.edges[i];
		float u = 0;
		const float d = dist_point_segment(p, g.points[size_t(e.a)], g.points[size_t(e.b)], &u);
		if (best.e < 0 || d < best_d) {
			best = { int(i), u, lerp_v2(g.points[size_t(e.a)], g.points[size_t(e.b)], u) };
			best_d = d;
		}
	}
	return best;
}
} // namespace

void rail_graph(const TransitPlan &p, TransitMode mode, float join_m, RailGraph &out) {
	out.mode = mode;
	out.points.clear();
	out.edges.clear();
	out.at.clear();
	SpatialHash cells;
	cells.cell = join_m;
	// The first point (lowest index) within reach, as the search of every point found.
	auto node = [&](V2 q) {
		int found = -1;
		cells.query(q.x - join_m, q.y - join_m, q.x + join_m, q.y + join_m, [&](int k) {
			if ((found < 0 || k < found) && (out.points[size_t(k)] - q).len() < join_m) found = k;
		});
		if (found >= 0) return found;
		out.points.push_back(q);
		out.at.emplace_back();
		const int k = int(out.points.size()) - 1;
		cells.insert_aabb(k, q.x, q.y, q.x, q.y);
		return k;
	};
	for (const RailTrack &t : p.tracks) {
		if (t.mode != mode || t.points.empty()) continue;
		int prev = node(t.points[0]);
		for (size_t i = 1; i < t.points.size(); i++) {
			const int cur = node(t.points[i]);
			if (cur != prev) {
				const int e = int(out.edges.size());
				out.edges.push_back({ prev, cur, (out.points[size_t(prev)] - out.points[size_t(cur)]).len() });
				out.at[size_t(prev)].push_back(e);
				out.at[size_t(cur)].push_back(e);
			}
			prev = cur;
		}
	}
}

bool rail_path(const RailGraph &g, V2 from, V2 to, std::vector<V2> &out) {
	out.clear();
	const OnEdge s = on_edge(g, from), t = on_edge(g, to);
	if (s.e < 0 || t.e < 0) return false;
	if (s.e == t.e) {
		out.push_back(s.q);
		out.push_back(t.q);
		return true;
	}
	const size_t n = g.points.size();
	std::vector<double> dist(n, double(BIG));
	std::vector<int> prev(n, -1);
	std::vector<uint8_t> done(n, 0);
	const RailEdge &se = g.edges[size_t(s.e)];
	dist[size_t(se.a)] = double(se.length) * s.u;
	dist[size_t(se.b)] = double(se.length) * (1 - s.u);
	using Item = std::pair<double, int>;
	std::priority_queue<Item, std::vector<Item>, std::greater<Item>> queue;
	queue.push({ dist[size_t(se.a)], se.a });
	queue.push({ dist[size_t(se.b)], se.b });
	while (!queue.empty()) {
		const auto [key, k] = queue.top();
		queue.pop();
		if (done[size_t(k)] || key > dist[size_t(k)]) continue;
		done[size_t(k)] = 1;
		for (int ei : g.at[size_t(k)]) {
			const RailEdge &e = g.edges[size_t(ei)];
			const int o = e.a == k ? e.b : e.a;
			if (dist[size_t(k)] + e.length < dist[size_t(o)]) {
				dist[size_t(o)] = dist[size_t(k)] + e.length;
				prev[size_t(o)] = k;
				queue.push({ dist[size_t(o)], o });
			}
		}
	}
	const RailEdge &te = g.edges[size_t(t.e)];
	const double via_a = dist[size_t(te.a)] + double(te.length) * t.u, via_b = dist[size_t(te.b)] + double(te.length) * (1 - t.u);
	if (std::min(via_a, via_b) >= double(BIG)) return false;
	out.push_back(s.q);
	const size_t first = out.size();
	for (int k = via_a <= via_b ? te.a : te.b; k >= 0; k = prev[size_t(k)]) out.push_back(g.points[size_t(k)]);
	std::reverse(out.begin() + long(first), out.end());
	out.push_back(t.q);
	return true;
}

} // namespace transit
} // namespace city
