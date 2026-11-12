// The plan's edits and its rail network (transit_data.h): nodes joined by a grid of the joining reach, Dijkstra over a heap.
#include "sim/transit/transit_data.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>

namespace city {

// The outer mutation publishes each changed record to the indices once, including cascaded stop/line removal.
class TransitMutation {
public:
	TransitMutation(TransitData &t, TransitChange *requested) : t_(t), local_(t.next_id), outer_(t.mutation_depth_++ == 0) {
		if (outer_) t.mutation_ = requested ? requested : &local_;
		delta = t.mutation_;
		stops_ = delta->stops.edits.size(); tracks_ = delta->tracks.edits.size(); lines_ = delta->lines.edits.size();
	}
	~TransitMutation() {
		if (--t_.mutation_depth_ == 0) {
			t_.changed(*delta, stops_, tracks_, lines_);
			t_.mutation_ = nullptr;
		}
	}
	TransitChange *delta;
private:
	TransitData &t_;
	TransitChange local_;
	bool outer_;
	size_t stops_, tracks_, lines_;
};

namespace {
constexpr int MIN_LINE_STOPS = 2;

constexpr int CELL_ROW_BITS = 32;

int64_t cell_key(int i, int j) { return (int64_t(i) << CELL_ROW_BITS) ^ uint32_t(j); }

} // namespace

V2 rail_segment_point(const V2 &p, const V2 &a, const V2 &b, float &share) {
	const V2 d = b - a;
	const float l2 = d.dot(d);
	share = l2 > 0 ? std::clamp((p - a).dot(d) / l2, 0.f, 1.f) : 0.f;
	return a + d * share;
}

int transit_add_stop(TransitData &t, TransitStop stop, TransitChange *change) {
	TransitMutation mutation(t, change); change = mutation.delta;
	stop.id = t.next_id++;
	t.stops_.push_back(stop);
	if (change) change->stops.add(t.stops_.size() - 1, t.stops_.back());
	return stop.id;
}

void transit_remove_stop(TransitData &t, int id, TransitChange *change) {
	TransitMutation mutation(t, change); change = mutation.delta;
	for (size_t i = 0; i < t.lines_.size();) {
		TransitLine &line = t.lines_[i];
		const bool contains = std::find(line.stops.begin(), line.stops.end(), id) != line.stops.end();
		if (!contains && int(line.stops.size()) >= MIN_LINE_STOPS) { i++; continue; }
		TransitLine before;
		if (change) before = line;
		line.stops.erase(std::remove(line.stops.begin(), line.stops.end(), id), line.stops.end());
		if (int(line.stops.size()) < MIN_LINE_STOPS) {
			if (change) change->lines.remove(i, std::move(before));
			t.lines_.erase(t.lines_.begin() + std::ptrdiff_t(i));
		} else {
			if (change) change->lines.replace(i, std::move(before), line);
			i++;
		}
	}
	for (size_t i = 0; i < t.stops_.size();) {
		if (t.stops_[i].id != id) { i++; continue; }
		if (change) change->stops.remove(i, t.stops_[i]);
		t.stops_.erase(t.stops_.begin() + std::ptrdiff_t(i));
	}
}

void transit_set_terminal(TransitData &t, int id, bool terminal, TransitChange *change) {
	TransitMutation mutation(t, change); change = mutation.delta;
	for (size_t i = 0; i < t.stops_.size(); i++) {
		TransitStop &stop = t.stops_[i];
		if (stop.id != id || stop.terminal == terminal) continue;
		const TransitStop before = stop;
		stop.terminal = terminal;
		if (change) change->stops.replace(i, before, stop);
	}
}

int transit_add_track(TransitData &t, TransitMode mode, const std::vector<V2> &points, TransitChange *change) {
	TransitMutation mutation(t, change); change = mutation.delta;
	const int id = t.next_id++;
	t.tracks_.push_back({ id, mode, points });
	if (change) change->tracks.add(t.tracks_.size() - 1, t.tracks_.back());
	return id;
}

void transit_remove_track(TransitData &t, int id, TransitChange *change) {
	TransitMutation mutation(t, change); change = mutation.delta;
	std::vector<int> on;
	for (const TransitStop &s : t.stops_)
		if (s.track == id) on.push_back(s.id);
	for (int s : on) transit_remove_stop(t, s, change);
	for (size_t i = 0; i < t.tracks_.size();) {
		if (t.tracks_[i].id != id) { i++; continue; }
		if (change) change->tracks.remove(i, t.tracks_[i]);
		t.tracks_.erase(t.tracks_.begin() + std::ptrdiff_t(i));
	}
}

int transit_add_line(TransitData &t, TransitMode mode, const std::vector<int> &stops, const std::vector<uint32_t> &colours, int vehicles, const std::string &name, TransitChange *change) {
	TransitMutation mutation(t, change); change = mutation.delta;
	const int id = t.next_id++;
	const uint32_t colour = colours.empty() ? 0 : colours[t.lines_.size() % colours.size()];
	t.lines_.push_back({ id, mode, stops, colour, vehicles, name });
	if (change) change->lines.add(t.lines_.size() - 1, t.lines_.back());
	return id;
}

void transit_remove_line(TransitData &t, int id, TransitChange *change) {
	TransitMutation mutation(t, change); change = mutation.delta;
	for (size_t i = 0; i < t.lines_.size();) {
		if (t.lines_[i].id != id) { i++; continue; }
		if (change) change->lines.remove(i, t.lines_[i]);
		t.lines_.erase(t.lines_.begin() + std::ptrdiff_t(i));
	}
}

void transit_set_vehicles(TransitData &t, int id, int vehicles, int most, TransitChange *change) {
	TransitMutation mutation(t, change); change = mutation.delta;
	const int count = std::clamp(vehicles, 1, most);
	for (size_t i = 0; i < t.lines_.size(); i++) {
		TransitLine &line = t.lines_[i];
		if (line.id != id || line.vehicles == count) continue;
		TransitLine before;
		if (change) before = line;
		line.vehicles = count;
		if (change) change->lines.replace(i, std::move(before), line);
	}
}

void transit_normalize(TransitData &t) {
	const auto known = [](TransitMode m) { return m == TransitMode::BUS || m == TransitMode::TRAIN || m == TransitMode::METRO; };
	t.stops_.erase(std::remove_if(t.stops_.begin(), t.stops_.end(), [&](const TransitStop &s) { return !known(s.mode); }), t.stops_.end());
	t.tracks_.erase(std::remove_if(t.tracks_.begin(), t.tracks_.end(), [&](const RailTrack &k) { return !known(k.mode) || k.mode == TransitMode::BUS || int(k.points.size()) < MIN_LINE_STOPS; }), t.tracks_.end());
	t.reindex_ids(); // stop() below reads the normalized stop table
	for (TransitLine &l : t.lines_)
		l.stops.erase(std::remove_if(l.stops.begin(), l.stops.end(), [&](int id) {
			const TransitStop *s = t.stop(id);
			return !s || s->mode != l.mode;
		}), l.stops.end());
	t.lines_.erase(std::remove_if(t.lines_.begin(), t.lines_.end(), [](const TransitLine &l) { return int(l.stops.size()) < MIN_LINE_STOPS; }), t.lines_.end());
	t.rebuild_index();
}

RailGraph rail_graph(const TransitData &t, TransitMode mode, float join, size_t retained_bytes) {
	RailGraph g(retained_bytes);
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
	for (const RailTrack &track : t.tracks()) {
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

TrackHit nearest_track(const TransitData &t, const V2 &p, float reach, TransitMode mode) {
	return t.track_near(p, reach, mode);
}

const TransitStop *stop_near(const TransitData &t, const V2 &p, float reach, TransitMode mode, bool any) {
	return t.near_stop(p, reach, mode, any);
}

} // namespace city
