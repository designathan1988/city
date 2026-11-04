// The plan's edits and queries (Roadcraft 4.0 src/world/transit.ts): the edits keep ids, colours and counts as the
// 4.0 does; the queries read the cells of PlanIndex round the point asked about, in the order a walk of every stop or
// track meets them (the 4.0's search of every one, without walking them all).
#include "sim/transit/transit_plan.h"

#include "sim/catalog/transit_params.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace transit {

namespace {
constexpr float INDEX_CELL_M = 32.f; // world/transit.ts TRACK_CELL, STOP_CELL
constexpr int NAME_TRAIN = 'T', NAME_METRO = 'M'; // a rail line's name starts with its mode; a bus line's is its number

int clamp_vehicles(const TransitParams &tp, int n) {
	return clampi(n, int(tp.vehicles_min), int(tp.vehicles_max));
}

// The stretches filed in the cells within `reach` of (x, z), each once, in walk order (`out` holds list indices).
void stretches_near(const PlanIndex &ix, float x, float z, float reach, std::vector<int> &out) {
	out.clear();
	ix.stretches.query(x - reach, z - reach, x + reach, z + reach, [&](int i) { out.push_back(i); });
	std::sort(out.begin(), out.end());
	out.erase(std::unique(out.begin(), out.end()), out.end());
}

bool rail_mode(TransitMode m) { return m == TransitMode::TRAIN || m == TransitMode::METRO; }
} // namespace

void set_index(PlanIndex &ix, const TransitPlan &p) {
	ix.stop_at.clear();
	ix.stops.clear();
	ix.stretches.clear();
	ix.list.clear();
	ix.stops.cell = ix.stretches.cell = INDEX_CELL_M;
	for (size_t i = 0; i < p.stops.size(); i++) {
		const TransitStop &s = p.stops[i];
		ix.stop_at[s.id] = int(i);
		ix.stops.insert_aabb(int(i), s.x, s.z, s.x, s.z);
	}
	for (const RailTrack &t : p.tracks) {
		for (size_t i = 1; i < t.points.size(); i++) {
			const V2 a = t.points[i - 1], b = t.points[i];
			const int k = int(ix.list.size());
			ix.list.push_back({ t.id, k, t.mode, a, b });
			ix.stretches.insert_aabb(k, std::min(a.x, b.x), std::min(a.y, b.y), std::max(a.x, b.x), std::max(a.y, b.y));
		}
	}
}

const TransitStop *stop_by_id(const TransitPlan &p, const PlanIndex &ix, int id) {
	auto it = ix.stop_at.find(id);
	return it == ix.stop_at.end() ? nullptr : &p.stops[size_t(it->second)];
}

const TransitStop *stop_near(const TransitPlan &p, const PlanIndex &ix, float x, float z, float reach, TransitMode mode) {
	std::vector<int> found;
	ix.stops.query(x - reach, z - reach, x + reach, z + reach, [&](int i) { found.push_back(i); });
	std::sort(found.begin(), found.end());
	const TransitStop *best = nullptr;
	float best_d = reach;
	for (int i : found) {
		const TransitStop &s = p.stops[size_t(i)];
		if (mode != TransitMode::COUNT && s.mode != mode) continue;
		const float d = std::hypot(s.x - x, s.z - z);
		if (d < best_d) { best_d = d; best = &s; }
	}
	return best;
}

TrackHit nearest_track(const PlanIndex &ix, float x, float z, float reach, TransitMode mode) {
	std::vector<int> near;
	stretches_near(ix, x, z, reach, near);
	TrackHit best;
	for (int k : near) {
		const TrackStretch &s = ix.list[size_t(k)];
		if (mode != TransitMode::COUNT && s.mode != mode) continue;
		float u = 0;
		const float d = dist_point_segment(V2(x, z), s.a, s.b, &u);
		if (d < reach && (best.track < 0 || d < best.d)) {
			const V2 q = lerp_v2(s.a, s.b, u);
			best = { s.track, q.x, q.y, d };
		}
	}
	return best;
}

bool track_point_near(const PlanIndex &ix, float x, float z, float reach, TransitMode mode, V2 &out) {
	std::vector<int> near;
	stretches_near(ix, x, z, reach, near);
	for (int k : near) {
		const TrackStretch &s = ix.list[size_t(k)];
		if (s.mode != mode) continue;
		for (const V2 &q : { s.a, s.b }) {
			if (std::hypot(q.x - x, q.y - z) < reach) {
				out = q;
				return true;
			}
		}
	}
	return false;
}

int add_stop(TransitPlan &p, TransitStop stop) {
	stop.id = p.next_id++;
	p.stops.push_back(stop);
	return stop.id;
}

void remove_stop(TransitPlan &p, int id) {
	for (TransitLine &l : p.lines) l.stops.erase(std::remove(l.stops.begin(), l.stops.end(), id), l.stops.end());
	p.lines.erase(std::remove_if(p.lines.begin(), p.lines.end(), [](const TransitLine &l) { return l.stops.size() < 2; }), p.lines.end());
	p.stops.erase(std::remove_if(p.stops.begin(), p.stops.end(), [id](const TransitStop &s) { return s.id == id; }), p.stops.end());
}

void set_terminal(TransitPlan &p, int id, bool terminal) {
	for (TransitStop &s : p.stops)
		if (s.id == id) s.terminal = terminal;
}

int add_track(TransitPlan &p, TransitMode mode, const std::vector<V2> &points) {
	RailTrack t;
	t.id = p.next_id++;
	t.mode = mode;
	t.points = points;
	p.tracks.push_back(std::move(t));
	return p.tracks.back().id;
}

void remove_track(TransitPlan &p, int id) {
	p.tracks.erase(std::remove_if(p.tracks.begin(), p.tracks.end(), [id](const RailTrack &t) { return t.id == id; }), p.tracks.end());
	std::vector<int> on;
	for (const TransitStop &s : p.stops)
		if (s.track == id) on.push_back(s.id);
	for (int s : on) remove_stop(p, s);
}

int add_line(TransitPlan &p, const TransitParams &tp, TransitMode mode, const std::vector<int> &stops) {
	TransitLine l;
	l.id = p.next_id++;
	l.mode = mode;
	l.stops = stops;
	l.colour = tp.line_colours.empty() ? 0 : int(p.lines.size() % tp.line_colours.size());
	l.vehicles = int(mode == TransitMode::BUS ? tp.bus_vehicles : tp.rail_vehicles);
	const int same = int(std::count_if(p.lines.begin(), p.lines.end(), [mode](const TransitLine &o) { return o.mode == mode; }));
	if (mode != TransitMode::BUS) l.name.push_back(char(mode == TransitMode::METRO ? NAME_METRO : NAME_TRAIN));
	l.name += std::to_string(same + 1);
	p.lines.push_back(std::move(l));
	return p.lines.back().id;
}

void remove_line(TransitPlan &p, int id) {
	p.lines.erase(std::remove_if(p.lines.begin(), p.lines.end(), [id](const TransitLine &l) { return l.id == id; }), p.lines.end());
}

void set_line_vehicles(TransitPlan &p, const TransitParams &tp, int id, int vehicles) {
	for (TransitLine &l : p.lines)
		if (l.id == id) l.vehicles = clamp_vehicles(tp, vehicles);
}

void normalize(TransitPlan &p, const TransitParams &tp) {
	auto finite = [](float v) { return std::isfinite(v); };
	p.stops.erase(std::remove_if(p.stops.begin(), p.stops.end(), [&](const TransitStop &s) {
		return s.mode >= TransitMode::COUNT || !finite(s.x) || !finite(s.z) || (s.has_entrance && (!finite(s.ex) || !finite(s.ez)));
	}), p.stops.end());
	for (RailTrack &t : p.tracks)
		t.points.erase(std::remove_if(t.points.begin(), t.points.end(), [&](const V2 &q) { return !finite(q.x) || !finite(q.y); }), t.points.end());
	p.tracks.erase(std::remove_if(p.tracks.begin(), p.tracks.end(), [](const RailTrack &t) {
		return !rail_mode(t.mode) || t.points.size() < 2;
	}), p.tracks.end());
	PlanIndex ix;
	set_index(ix, p);
	for (TransitLine &l : p.lines) {
		l.stops.erase(std::remove_if(l.stops.begin(), l.stops.end(), [&](int id) {
			const TransitStop *s = stop_by_id(p, ix, id);
			return !s || s->mode != l.mode;
		}), l.stops.end());
		l.vehicles = clamp_vehicles(tp, l.vehicles);
		if (l.colour < 0 || size_t(l.colour) >= tp.line_colours.size()) l.colour = 0;
	}
	p.lines.erase(std::remove_if(p.lines.begin(), p.lines.end(), [](const TransitLine &l) {
		return l.mode >= TransitMode::COUNT || l.stops.size() < 2;
	}), p.lines.end());
	int next = std::max(1, p.next_id);
	for (const TransitStop &s : p.stops) next = std::max(next, s.id + 1);
	for (const RailTrack &t : p.tracks) next = std::max(next, t.id + 1);
	for (const TransitLine &l : p.lines) next = std::max(next, l.id + 1);
	p.next_id = next;
}

} // namespace transit
} // namespace city
