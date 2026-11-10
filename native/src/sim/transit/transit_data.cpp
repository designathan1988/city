// The plan's edits and its rail network (transit_data.h): nodes joined by a grid of the joining reach, Dijkstra over a heap.
#include "sim/transit/transit_data.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>

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

void transit_normalize(TransitData &t) {
	const auto known = [](TransitMode m) { return m == TransitMode::BUS || m == TransitMode::TRAIN || m == TransitMode::METRO; };
	t.stops.erase(std::remove_if(t.stops.begin(), t.stops.end(), [&](const TransitStop &s) { return !known(s.mode); }), t.stops.end());
	t.tracks.erase(std::remove_if(t.tracks.begin(), t.tracks.end(), [&](const RailTrack &k) { return !known(k.mode) || k.mode == TransitMode::BUS || int(k.points.size()) < MIN_LINE_STOPS; }), t.tracks.end());
	for (TransitLine &l : t.lines)
		l.stops.erase(std::remove_if(l.stops.begin(), l.stops.end(), [&](int id) {
			const TransitStop *s = t.stop(id);
			return !s || s->mode != l.mode;
		}), l.stops.end());
	t.lines.erase(std::remove_if(t.lines.begin(), t.lines.end(), [](const TransitLine &l) { return int(l.stops.size()) < MIN_LINE_STOPS; }), t.lines.end());
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

// Dijkstra from the two ends of the stretch `from` lies on, over until both ends of the stretch `to` lies on are taken off the queue:
// a point taken off it has its distance and the point before it for good (no later one is nearer, the stretches have no negative
// length), and so has every point back along its way. The queue pops as std::priority_queue over std::greater does.
std::vector<V2> rail_path(const RailGraph &g, const V2 &from, const V2 &to, RailSearch &search) {
	const OnStretch s = on_graph(g, from), e = on_graph(g, to);
	if (s.edge < 0 || e.edge < 0) return {};
	if (s.edge == e.edge) return { s.at, e.at };
	const size_t n = g.points.size();
	if (search.stamp.size() < n) {
		search.dist.resize(n);
		search.prev.resize(n);
		search.stamp.resize(n, 0);
	}
	if (++search.cur == 0) {
		std::fill(search.stamp.begin(), search.stamp.end(), 0);
		search.cur = 1;
	}
	const auto dist = [&](int k) { return search.stamp[size_t(k)] == search.cur ? search.dist[size_t(k)] : NO_DISTANCE; };
	const auto prev = [&](int k) { return search.stamp[size_t(k)] == search.cur ? search.prev[size_t(k)] : -1; };
	const auto set = [&](int k, float d, int p) {
		search.stamp[size_t(k)] = search.cur;
		search.dist[size_t(k)] = d;
		search.prev[size_t(k)] = p;
	};
	using Item = std::pair<float, int>;
	std::vector<Item> &open = search.open;
	open.clear();
	const auto push = [&](float d, int k) {
		open.push_back({ d, k });
		std::push_heap(open.begin(), open.end(), std::greater<Item>());
	};
	const RailGraph::Edge &se = g.edges[size_t(s.edge)], &te = g.edges[size_t(e.edge)];
	set(se.a, se.length * s.share, -1);
	set(se.b, se.length * (1 - s.share), -1);
	push(dist(se.a), se.a);
	push(dist(se.b), se.b);
	bool took_a = false, took_b = false;
	while (!open.empty()) {
		std::pop_heap(open.begin(), open.end(), std::greater<Item>());
		const auto [d, k] = open.back();
		open.pop_back();
		if (d > dist(k)) continue;
		took_a = took_a || k == te.a;
		took_b = took_b || k == te.b;
		if (took_a && took_b) break;
		for (int ei : g.at[size_t(k)]) {
			const RailGraph::Edge &edge = g.edges[size_t(ei)];
			const int o = edge.a == k ? edge.b : edge.a;
			if (d + edge.length < dist(o)) {
				set(o, d + edge.length, k);
				push(dist(o), o);
			}
		}
	}
	const float via_a = dist(te.a) == NO_DISTANCE ? NO_DISTANCE : dist(te.a) + te.length * e.share;
	const float via_b = dist(te.b) == NO_DISTANCE ? NO_DISTANCE : dist(te.b) + te.length * (1 - e.share);
	if (via_a == NO_DISTANCE && via_b == NO_DISTANCE) return {};
	std::vector<V2> chain;
	for (int k = via_a <= via_b ? te.a : te.b; k >= 0; k = prev(k)) chain.push_back(g.points[size_t(k)]);
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
