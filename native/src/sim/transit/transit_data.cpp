// The plan's edits, its cells and its rail network (transit_data.h): nodes joined by a grid of the joining reach, Dijkstra over a heap;
// the picks over the cells round the point only.
#include "sim/transit/transit_data.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>

namespace city {

namespace {
constexpr float NO_DISTANCE = std::numeric_limits<float>::max();
constexpr int MIN_LINE_STOPS = 2;

constexpr int CELL_ROW_BITS = 32; // a cell's key: its row over the column's 32 bits

int64_t cell_key(int i, int j) { return (int64_t(i) << CELL_ROW_BITS) ^ uint32_t(j); }

constexpr float TRANSIT_CELL_M = 25.f; // the side of a cell the stops and stretches are filed in (the lab's world/transit.ts STOP_CELL, TRACK_CELL)

int cell_of(float v) { return int(std::floor(v / TRANSIT_CELL_M)); }

// Every cell within `reach` of `p`, each once.
template <typename Visit>
void cells_round(const V2 &p, float reach, Visit visit) {
	for (int i = cell_of(p.x - reach); i <= cell_of(p.x + reach); i++)
		for (int j = cell_of(p.y - reach); j <= cell_of(p.y + reach); j++) visit(cell_key(i, j));
}

// Nearer, or as near and earlier in the lists' order: what a walk of the lists in order keeps with a strict comparison.
bool before(float d, int64_t order, float best_d, int64_t best_order, bool found) { return d < best_d || (found && d == best_d && order < best_order); }
int64_t stretch_order(int track, int point) { return (int64_t(track) << CELL_ROW_BITS) | uint32_t(point); }

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
	const auto it = cells.stop_at.find(id);
	if (it != cells.stop_at.end() && size_t(it->second) < stops.size() && stops[size_t(it->second)].id == id) return &stops[size_t(it->second)];
	for (const TransitStop &s : stops) // a copy being edited, its cells not made again yet
		if (s.id == id) return &s;
	return nullptr;
}

void transit_index(TransitData &t) {
	TransitCells &c = t.cells;
	c = TransitCells();
	for (size_t i = 0; i < t.stops.size(); i++) {
		const TransitStop &q = t.stops[i];
		c.stop_at.emplace(q.id, int(i));
		c.stops[cell_key(cell_of(q.at.x), cell_of(q.at.y))].push_back(int(i));
	}
	for (size_t k = 0; k < t.tracks.size(); k++) {
		const std::vector<V2> &pts = t.tracks[k].points;
		for (size_t i = 1; i < pts.size(); i++) {
			const V2 &a = pts[i - 1], &b = pts[i];
			for (int x = cell_of(std::min(a.x, b.x)); x <= cell_of(std::max(a.x, b.x)); x++)
				for (int y = cell_of(std::min(a.y, b.y)); y <= cell_of(std::max(a.y, b.y)); y++) c.stretches[cell_key(x, y)].push_back({ int(k), int(i) });
		}
	}
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
	transit_index(t); // the lines' stops below are looked up by it
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
	int64_t best_order = 0;
	cells_round(p, reach, [&](int64_t key) {
		const auto it = t.cells.stretches.find(key);
		if (it == t.cells.stretches.end()) return;
		for (const auto &[k, i] : it->second) {
			const RailTrack &track = t.tracks[size_t(k)];
			if (mode != TransitMode::BUS && track.mode != mode) continue;
			float share = 0;
			const V2 q = on_segment(p, track.points[size_t(i) - 1], track.points[size_t(i)], share);
			const float d = (p - q).len();
			const int64_t order = stretch_order(k, i);
			if (!before(d, order, best.d, best_order, best.track >= 0)) continue;
			best = { track.id, q, d };
			best_order = order;
		}
	});
	return best;
}

bool track_point_near(const TransitData &t, const V2 &p, float reach, TransitMode mode, V2 &out) {
	int64_t best = -1;
	cells_round(p, reach, [&](int64_t key) {
		const auto it = t.cells.stretches.find(key);
		if (it == t.cells.stretches.end()) return;
		for (const auto &[k, i] : it->second) {
			const RailTrack &track = t.tracks[size_t(k)];
			if (track.mode != mode) continue;
			for (int at : { i - 1, i }) {
				const int64_t order = stretch_order(k, at);
				if ((track.points[size_t(at)] - p).len() >= reach || (best >= 0 && order >= best)) continue;
				best = order;
				out = track.points[size_t(at)];
			}
		}
	});
	return best >= 0;
}

const TransitStop *stop_near(const TransitData &t, const V2 &p, float reach, TransitMode mode, bool any) {
	const TransitStop *best = nullptr;
	float dist = reach;
	int best_at = 0;
	cells_round(p, reach, [&](int64_t key) {
		const auto it = t.cells.stops.find(key);
		if (it == t.cells.stops.end()) return;
		for (int i : it->second) {
			const TransitStop &s = t.stops[size_t(i)];
			if (!any && s.mode != mode) continue;
			const float d = (s.at - p).len();
			if (!before(d, i, dist, best_at, best != nullptr)) continue;
			dist = d;
			best = &s;
			best_at = i;
		}
	});
	return best;
}

} // namespace city
