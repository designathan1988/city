// The plan's edits and its rail network (transit_data.h): nodes joined by a grid of the joining reach, Dijkstra over a heap.
#include "sim/transit/transit_data.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>

namespace city {

namespace {
constexpr float NO_DISTANCE = std::numeric_limits<float>::max();
constexpr int MIN_LINE_STOPS = 2;

constexpr int CELL_ROW_BITS = 32;

int64_t cell_key(int i, int j) { return (int64_t(i) << CELL_ROW_BITS) ^ uint32_t(j); }

struct OnStretch {
	int edge = -1;
	float share = 0;
	V2 at;
};

V2 on_segment(const V2 &p, const V2 &a, const V2 &b, float &share) {
	const V2 d = b - a;
	const float l2 = d.dot(d);
	share = l2 > 0 ? std::clamp((p - a).dot(d) / l2, 0.f, 1.f) : 0.f;
	return a + d * share;
}

OnStretch on_graph(const RailGraph &g, const V2 &p) {
	OnStretch best;
	float dist = NO_DISTANCE;
	for (size_t i = 0; i < g.edges.size(); i++) {
		float share = 0;
		const V2 q = on_segment(p, g.points[size_t(g.edges[i].a)], g.points[size_t(g.edges[i].b)], share);
		const float d = (p - q).len();
		if (d < dist) {
			dist = d;
			best = { int(i), share, q };
		}
	}
	return best;
}
} // namespace

const TransitStop *TransitData::stop(int id) const {
	for (const TransitStop &s : stops)
		if (s.id == id) return &s;
	return nullptr;
}

const TransitLine *TransitData::line(int id) const {
	for (const TransitLine &l : lines)
		if (l.id == id) return &l;
	return nullptr;
}

int transit_add_stop(TransitData &t, TransitStop stop) {
	stop.id = t.next_id++;
	t.stops.push_back(stop);
	return stop.id;
}

void transit_remove_stop(TransitData &t, int id) {
	for (TransitLine &l : t.lines) l.stops.erase(std::remove(l.stops.begin(), l.stops.end(), id), l.stops.end());
	t.lines.erase(std::remove_if(t.lines.begin(), t.lines.end(), [](const TransitLine &l) { return int(l.stops.size()) < MIN_LINE_STOPS; }), t.lines.end());
	t.stops.erase(std::remove_if(t.stops.begin(), t.stops.end(), [&](const TransitStop &s) { return s.id == id; }), t.stops.end());
}

void transit_set_terminal(TransitData &t, int id, bool terminal) {
	for (TransitStop &s : t.stops)
		if (s.id == id) s.terminal = terminal;
}

int transit_add_track(TransitData &t, TransitMode mode, const std::vector<V2> &points) {
	const int id = t.next_id++;
	t.tracks.push_back({ id, mode, points });
	return id;
}

void transit_remove_track(TransitData &t, int id) {
	std::vector<int> on;
	for (const TransitStop &s : t.stops)
		if (s.track == id) on.push_back(s.id);
	for (int s : on) transit_remove_stop(t, s);
	t.tracks.erase(std::remove_if(t.tracks.begin(), t.tracks.end(), [&](const RailTrack &k) { return k.id == id; }), t.tracks.end());
}

int transit_add_line(TransitData &t, TransitMode mode, const std::vector<int> &stops, const std::vector<uint32_t> &colours, int vehicles, const std::string &name) {
	const int id = t.next_id++;
	const uint32_t colour = colours.empty() ? 0 : colours[t.lines.size() % colours.size()];
	t.lines.push_back({ id, mode, stops, colour, vehicles, name });
	return id;
}

void transit_remove_line(TransitData &t, int id) {
	t.lines.erase(std::remove_if(t.lines.begin(), t.lines.end(), [&](const TransitLine &l) { return l.id == id; }), t.lines.end());
}

void transit_set_vehicles(TransitData &t, int id, int vehicles, int most) {
	for (TransitLine &l : t.lines)
		if (l.id == id) l.vehicles = std::clamp(vehicles, 1, most);
}

RailGraph rail_graph(const TransitData &t, TransitMode mode, float join) {
	RailGraph g;
	std::unordered_map<int64_t, std::vector<int>> cells;
	auto node = [&](const V2 &p) {
		const int ci = int(std::floor(p.x / join)), cj = int(std::floor(p.y / join));
		int found = -1;
		for (int i = ci - 1; i <= ci + 1; i++)
			for (int j = cj - 1; j <= cj + 1; j++) {
				const auto it = cells.find(cell_key(i, j));
				if (it == cells.end()) continue;
				for (int k : it->second)
					if ((found < 0 || k < found) && (g.points[size_t(k)] - p).len() < join) found = k;
			}
		if (found >= 0) return found;
		g.points.push_back(p);
		g.at.emplace_back();
		const int k = int(g.points.size()) - 1;
		cells[cell_key(ci, cj)].push_back(k);
		return k;
	};
	for (const RailTrack &track : t.tracks) {
		if (track.mode != mode) continue;
		int prev = node(track.points[0]);
		for (size_t i = 1; i < track.points.size(); i++) {
			const int cur = node(track.points[i]);
			if (cur == prev) continue;
			const int e = int(g.edges.size());
			g.edges.push_back({ prev, cur, (g.points[size_t(prev)] - g.points[size_t(cur)]).len() });
			g.at[size_t(prev)].push_back(e);
			g.at[size_t(cur)].push_back(e);
			prev = cur;
		}
	}
	return g;
}

std::vector<V2> rail_path(const RailGraph &g, const V2 &from, const V2 &to) {
	const OnStretch s = on_graph(g, from), e = on_graph(g, to);
	if (s.edge < 0 || e.edge < 0) return {};
	if (s.edge == e.edge) return { s.at, e.at };
	const size_t n = g.points.size();
	std::vector<float> dist(n, NO_DISTANCE);
	std::vector<int> prev(n, -1);
	using Item = std::pair<float, int>;
	std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open;
	const RailGraph::Edge &se = g.edges[size_t(s.edge)], &te = g.edges[size_t(e.edge)];
	dist[size_t(se.a)] = se.length * s.share;
	dist[size_t(se.b)] = se.length * (1 - s.share);
	open.push({ dist[size_t(se.a)], se.a });
	open.push({ dist[size_t(se.b)], se.b });
	while (!open.empty()) {
		const auto [d, k] = open.top();
		open.pop();
		if (d > dist[size_t(k)]) continue;
		for (int ei : g.at[size_t(k)]) {
			const RailGraph::Edge &edge = g.edges[size_t(ei)];
			const int o = edge.a == k ? edge.b : edge.a;
			if (d + edge.length < dist[size_t(o)]) {
				dist[size_t(o)] = d + edge.length;
				prev[size_t(o)] = k;
				open.push({ dist[size_t(o)], o });
			}
		}
	}
	const float via_a = dist[size_t(te.a)] == NO_DISTANCE ? NO_DISTANCE : dist[size_t(te.a)] + te.length * e.share;
	const float via_b = dist[size_t(te.b)] == NO_DISTANCE ? NO_DISTANCE : dist[size_t(te.b)] + te.length * (1 - e.share);
	if (via_a == NO_DISTANCE && via_b == NO_DISTANCE) return {};
	std::vector<V2> chain;
	for (int k = via_a <= via_b ? te.a : te.b; k >= 0; k = prev[size_t(k)]) chain.push_back(g.points[size_t(k)]);
	std::reverse(chain.begin(), chain.end());
	chain.insert(chain.begin(), s.at);
	chain.push_back(e.at);
	return chain;
}

TrackHit nearest_track(const TransitData &t, const V2 &p, float reach, TransitMode mode) {
	TrackHit best;
	best.d = reach;
	for (const RailTrack &track : t.tracks) {
		if (mode != TransitMode::BUS && track.mode != mode) continue;
		for (size_t i = 1; i < track.points.size(); i++) {
			float share = 0;
			const V2 q = on_segment(p, track.points[i - 1], track.points[i], share);
			const float d = (p - q).len();
			if (d < best.d) best = { track.id, q, d };
		}
	}
	return best;
}

const TransitStop *stop_near(const TransitData &t, const V2 &p, float reach, TransitMode mode, bool any) {
	const TransitStop *best = nullptr;
	float dist = reach;
	for (const TransitStop &s : t.stops) {
		if (!any && s.mode != mode) continue;
		const float d = (s.at - p).len();
		if (d < dist) {
			dist = d;
			best = &s;
		}
	}
	return best;
}

} // namespace city
