// The reach of the segment graph (route_reach.h, Roadcraft lab src/sim/routing/laneReach.ts): Tarjan's strongly connected components
// (R. Tarjan, "Depth-first search and linear graph algorithms", SIAM J. Comput. 1, 1972), run without recursion over the connectors'
// segment-to-segment edges, once for cars and once for bicycles, when the lane graph has changed and a question comes. A goal in the
// origin's own component is reached; one in another is reached when the condensation leads there, walked once per component asked.
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {

constexpr int BITS = 64; // a word of RouteReach::ahead

// The segment graph of `mode` as compressed rows: a segment's edges lead to the segments its connectors enter (for bicycles, those
// a bicycle may ride).
void lay_rows(const LaneGraph &g, int mode, RouteReach &r) {
	const size_t n = g.ds.size();
	r.row.assign(n + 1, 0);
	r.col.clear();
	for (size_t v = 0; v < n; v++) {
		r.row[v] = int(r.col.size());
		if (!g.ds[v].ok) continue;
		each_conn_from(g, int(v), [&](int ci) {
			const int to = g.conns[size_t(ci)].to_ds;
			if (to >= 0 && g.ds[size_t(to)].ok && (mode == 0 || g.ds[size_t(to)].bikes)) r.col.push_back(to);
		});
	}
	r.row[n] = int(r.col.size());
}

// Tarjan's components of the rows laid, into r.comp[mode]; returns how many.
int components(const LaneGraph &g, int mode, RouteReach &r) {
	const size_t n = g.ds.size();
	std::vector<int> &comp = r.comp[mode];
	comp.assign(n, -1);
	r.index.assign(n, -1);
	r.low.assign(n, 0);
	r.at.assign(n, 0);
	r.on.assign(n, 0);
	r.stack.clear();
	int next_index = 0, count = 0;
	auto open = [&](int v) {
		r.index[size_t(v)] = r.low[size_t(v)] = next_index++;
		r.at[size_t(v)] = r.row[size_t(v)];
		r.stack.push_back(v);
		r.on[size_t(v)] = 1;
		r.call.push_back(v);
	};
	for (size_t root = 0; root < n; root++) {
		if (!g.ds[root].ok || r.index[root] >= 0) continue;
		r.call.clear();
		open(int(root));
		while (!r.call.empty()) {
			const int v = r.call.back();
			if (r.at[size_t(v)] < r.row[size_t(v) + 1]) {
				const int w = r.col[size_t(r.at[size_t(v)]++)];
				if (r.index[size_t(w)] < 0) open(w);
				else if (r.on[size_t(w)]) r.low[size_t(v)] = std::min(r.low[size_t(v)], r.index[size_t(w)]);
				continue;
			}
			r.call.pop_back();
			if (!r.call.empty()) r.low[size_t(r.call.back())] = std::min(r.low[size_t(r.call.back())], r.low[size_t(v)]);
			if (r.low[size_t(v)] != r.index[size_t(v)]) continue;
			for (int w = -1; w != v;) {
				w = r.stack.back();
				r.stack.pop_back();
				r.on[size_t(w)] = 0;
				comp[size_t(w)] = count;
			}
			count++;
		}
	}
	return count;
}

// The condensation of `mode`'s components: which lead into which.
void condense(int mode, int count, RouteReach &r) {
	std::vector<std::vector<int>> &next = r.next[mode];
	next.assign(size_t(count), {});
	const std::vector<int> &comp = r.comp[mode];
	for (size_t v = 0; v + 1 < r.row.size(); v++)
		for (int e = r.row[v]; e < r.row[v + 1]; e++) {
			const int a = comp[v], b = comp[size_t(r.col[size_t(e)])];
			if (a >= 0 && b >= 0 && a != b) next[size_t(a)].push_back(b);
		}
	for (std::vector<int> &out : next) {
		std::sort(out.begin(), out.end());
		out.erase(std::unique(out.begin(), out.end()), out.end());
	}
	r.ahead[mode].clear();
}

void build(CityState &s, RouteReach &r) {
	const LaneGraph &g = s.traffic.g;
	for (int mode = 0; mode < REACH_MODES; mode++) {
		lay_rows(g, mode, r);
		condense(mode, components(g, mode, r), r);
	}
	r.built = s.lane_graph_serial + 1;
}

// The components component `from` of `mode` reaches, itself included.
const std::vector<uint64_t> &ahead_of(RouteReach &r, int mode, int from) {
	auto found = r.ahead[mode].find(from);
	if (found != r.ahead[mode].end()) return found->second;
	const std::vector<std::vector<int>> &next = r.next[mode];
	std::vector<uint64_t> &seen = r.ahead[mode][from];
	seen.assign((next.size() + BITS - 1) / BITS, 0);
	r.call.clear();
	r.call.push_back(from);
	seen[size_t(from / BITS)] |= uint64_t(1) << (from % BITS);
	while (!r.call.empty()) {
		const int c = r.call.back();
		r.call.pop_back();
		for (int d : next[size_t(c)]) {
			uint64_t &word = seen[size_t(d / BITS)];
			const uint64_t bit = uint64_t(1) << (d % BITS);
			if (word & bit) continue;
			word |= bit;
			r.call.push_back(d);
		}
	}
	return seen;
}

} // namespace

bool route_reaches(CityState &s, int from_ds, int goal_ds, bool by_bike) {
	RouteReach &r = s.traffic.reach;
	if (r.built != s.lane_graph_serial + 1 || r.comp[0].size() != s.traffic.g.ds.size()) build(s, r);
	const int mode = by_bike ? 1 : 0;
	const std::vector<int> &comp = r.comp[mode];
	if (from_ds < 0 || goal_ds < 0 || size_t(from_ds) >= comp.size() || size_t(goal_ds) >= comp.size()) return false;
	const int a = comp[size_t(from_ds)], b = comp[size_t(goal_ds)];
	if (a < 0 || b < 0) return false;
	if (a == b) return true;
	return (ahead_of(r, mode, a)[size_t(b / BITS)] >> (b % BITS)) & 1u;
}

} // namespace traffic
} // namespace city
