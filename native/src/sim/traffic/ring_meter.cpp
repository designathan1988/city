// The ring meter (ring_meter.h, Roadcraft lab src/sim/intersections/cycles.ts CycleIndex and cycleFull): for each directed segment a
// Dijkstra over segment lengths from its exits back to itself, bounded by ring_max_storage_m and never through a U-turn, gives its
// shortest loop; loops found from several segments are kept once. A vehicle entering a loop it is not already on is let in only while
// the loop's vehicles, and itself, fill no more than ring_meter_share of its storage. The index is made whole when the lane graph has
// changed, before the step's vehicles ask (refresh_rings).
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {

// The shortest loop through `start` within `most` metres, into `out` (its segments, unsorted); false when there is none.
bool shortest_loop(const LaneGraph &g, int start, float most, RingIndex &r, std::vector<int> &out) {
	if (++r.cur == 0) {
		std::fill(r.stamp.begin(), r.stamp.end(), 0u);
		r.cur = 1;
	}
	auto later = [](const std::pair<float, int> &a, const std::pair<float, int> &b) { return a > b; };
	r.open.clear();
	auto reach = [&](int ds, float cost, int from) {
		if (cost > most || (r.stamp[size_t(ds)] == r.cur && cost >= r.best[size_t(ds)])) return;
		r.stamp[size_t(ds)] = r.cur;
		r.best[size_t(ds)] = cost;
		r.parent[size_t(ds)] = from;
		r.open.emplace_back(cost, ds);
		std::push_heap(r.open.begin(), r.open.end(), later);
	};
	const float first = g.ds[size_t(start)].len;
	int closing = -1;
	float closing_cost = most;
	auto exits = [&](int ds, float cost) {
		each_conn_from(g, ds, [&](int ci) {
			const Conn &c = g.conns[size_t(ci)];
			if (c.turn == Turn::UTURN || !g.ds[size_t(c.to_ds)].ok) return;
			if (c.to_ds == start) {
				if (ds != start && cost < closing_cost) { closing_cost = cost; closing = ds; }
				return;
			}
			reach(c.to_ds, cost + g.ds[size_t(c.to_ds)].len, ds);
		});
	};
	exits(start, first);
	while (!r.open.empty()) {
		std::pop_heap(r.open.begin(), r.open.end(), later);
		const auto [cost, ds] = r.open.back();
		r.open.pop_back();
		if (cost > r.best[size_t(ds)] || cost >= closing_cost) continue;
		exits(ds, cost);
	}
	if (closing < 0) return false;
	out.clear();
	out.push_back(start);
	for (int at = closing; at != start && at >= 0; at = r.parent[size_t(at)]) out.push_back(at);
	return true;
}

void build(CityState &s, RingIndex &r) {
	const LaneGraph &g = s.traffic.g;
	const float most = s.catalog.traffic.ring_max_storage_m;
	const size_t n = g.ds.size();
	r.loops.clear();
	r.of_ds.assign(n, {});
	r.best.assign(n, 0.f);
	r.parent.assign(n, -1);
	r.stamp.assign(n, 0u);
	r.cur = 0;
	std::vector<int> loop;
	for (size_t v = 0; v < n; v++) {
		if (!g.ds[v].ok || !shortest_loop(g, int(v), most, r, loop)) continue;
		std::sort(loop.begin(), loop.end());
		bool known = false;
		for (int id : r.of_ds[v]) known = known || r.loops[size_t(id)].ds == loop;
		if (known) continue;
		RingLoop made;
		made.ds = loop;
		for (int ds : loop) made.storage += g.ds[size_t(ds)].len * float(std::max(1, g.ds[size_t(ds)].lanes));
		const int id = int(r.loops.size());
		r.loops.push_back(std::move(made));
		for (int ds : loop) r.of_ds[size_t(ds)].push_back(id);
	}
	r.built = s.lane_graph_serial + 1;
}

bool on_loop(const RingLoop &l, int ds) { return std::binary_search(l.ds.begin(), l.ds.end(), ds); }

// The road the vehicles on loop `l` take up: their bodies and gaps on its lanes and on the connectors between its segments.
float loop_used(const CityState &s, const RingLoop &l) {
	const TrafficState &t = s.traffic;
	const TrafficParams &tp = s.catalog.traffic;
	auto taken = [&](int id) {
		const VehicleClass &c = tp.classes[int(t.vehicles[size_t(id)].type)];
		return c.length + c.min_gap;
	};
	float used = 0;
	for (int ds : l.ds) {
		for (int lane = 0; lane < t.g.ds[size_t(ds)].lanes; lane++)
			for (int id : t.lane_vehicles[size_t(ds) * MAX_LANES + size_t(lane)]) used += taken(id);
		each_conn_from(t.g, ds, [&](int ci) {
			if (!on_loop(l, t.g.conns[size_t(ci)].to_ds)) return;
			for (int id : t.conn_vehicles[size_t(ci)]) used += taken(id);
		});
	}
	return used;
}

} // namespace

void refresh_rings(CityState &s) {
	RingIndex &r = s.traffic.rings;
	if (r.built != s.lane_graph_serial + 1 || r.of_ds.size() != s.traffic.g.ds.size()) build(s, r);
}

bool ring_full(const CityState &s, const Conn &c, float need) {
	const RingIndex &r = s.traffic.rings;
	if (size_t(c.to_ds) >= r.of_ds.size()) return false;
	for (int id : r.of_ds[size_t(c.to_ds)]) {
		const RingLoop &l = r.loops[size_t(id)];
		if (on_loop(l, c.from_ds)) continue; // already on it: going round never locks it
		if (need + loop_used(s, l) > l.storage * s.catalog.traffic.ring_meter_share) return true;
	}
	return false;
}

} // namespace traffic
} // namespace city
