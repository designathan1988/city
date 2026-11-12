// The transport plan's record and spatial indices. Mutations, load and replay maintain them; tool reads never rebuild them.
// Track pieces, rather than whole-polyline boxes, are indexed; candidate order preserves original record/point ties.
#include "sim/transit/transit_data.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <tuple>
#include <set>
#include <unordered_set>

namespace city {
namespace {
template <typename T> void positions(const std::vector<T> &table, std::unordered_map<int, size_t> &out) {
	out.clear();
	for (size_t i = 0; i < table.size(); i++) out.emplace(table[i].id, i);
}
Bounds2 point_box(const V2 &p) { return {p.x, p.y, p.x, p.y}; }
Bounds2 piece_box(const V2 &a, const V2 &b) {
	return {std::min(a.x, b.x), std::min(a.y, b.y), std::max(a.x, b.x), std::max(a.y, b.y)};
}
Bounds2 query_box(const V2 &p, float reach) {
	const float inf = std::numeric_limits<float>::infinity();
	return {std::nextafter(p.x - reach, -inf), std::nextafter(p.y - reach, -inf),
		std::nextafter(p.x + reach, inf), std::nextafter(p.y + reach, inf)};
}
template<class T, class Uses, class Same> std::set<int> projection_ids(const TableDelta<T> &delta, size_t from, Uses uses, Same same) {
	std::set<int> out;
	for (size_t i = from; i < delta.edits.size(); ++i) {
		const auto &e = delta.edits[i];
		if (e.before && e.after && same(*e.before, *e.after)) continue;
		if (e.before && uses(*e.before)) out.insert(e.before->id);
		if (e.after && uses(*e.after)) out.insert(e.after->id);
	}
	return out;
}
void add_users(const std::unordered_map<int, std::vector<int>> &users, const std::set<int> &stops, std::set<int> &out) {
	for (int id : stops) { const auto found = users.find(id); if (found != users.end()) out.insert(found->second.begin(), found->second.end()); }
}
bool bus_stop_same(const TransitStop &a, const TransitStop &b) {
	return std::tie(a.id, a.mode, a.at.x, a.at.y, a.seg) == std::tie(b.id, b.mode, b.at.x, b.at.y, b.seg);
}
bool rail_stop_same(const TransitStop &a, const TransitStop &b) {
	// make_loop reads referenced stop ids/positions, even in a not-yet-normalized mixed-mode input.
	return std::tie(a.id, a.at.x, a.at.y) == std::tie(b.id, b.at.x, b.at.y);
}
bool route_line_same(const TransitLine &a, const TransitLine &b) {
	return std::tie(a.id, a.mode, a.stops) == std::tie(b.id, b.mode, b.stops);
}
bool bus_line_same(const TransitLine &a, const TransitLine &b) {
	return std::tie(a.id, a.mode, a.stops, a.vehicles) == std::tie(b.id, b.mode, b.stops, b.vehicles);
}
bool rail_line_same(const TransitLine &a, const TransitLine &b) {
	return std::tie(a.id, a.mode, a.stops, a.vehicles, a.colour) == std::tie(b.id, b.mode, b.stops, b.vehicles, b.colour);
}
template <typename T> std::vector<int> changed_ids(const TableDelta<T> &delta, size_t from) {
	std::vector<int> ids;
	for (size_t i = from; i < delta.edits.size(); i++) {
		if (delta.edits[i].before) ids.push_back(delta.edits[i].before->id);
		if (delta.edits[i].after) ids.push_back(delta.edits[i].after->id);
	}
	std::sort(ids.begin(), ids.end()); ids.erase(std::unique(ids.begin(), ids.end()), ids.end()); return ids;
}
} // namespace

const TransitStop *TransitData::stop(int id) const {
	const auto it = stop_pos_.find(id); return it == stop_pos_.end() ? nullptr : &stops_[it->second];
}
const TransitLine *TransitData::line(int id) const {
	const auto it = line_pos_.find(id); return it == line_pos_.end() ? nullptr : &lines_[it->second];
}
const RailTrack *TransitData::track(int id) const {
	const auto it = track_pos_.find(id); return it == track_pos_.end() ? nullptr : &tracks_[it->second];
}
void TransitData::reindex_ids() { positions(stops_, stop_pos_); positions(tracks_, track_pos_); positions(lines_, line_pos_); }
void TransitData::index_stop(const TransitStop &s) {
	const Bounds2 box = point_box(s.at); stop_boxes_[s.id] = box; stop_index_.insert(s.id, box);
}
void TransitData::retire_stop(int id) {
	const auto it = stop_boxes_.find(id);
	if (it == stop_boxes_.end()) return;
	stop_index_.erase(id, it->second); stop_boxes_.erase(it);
}
void TransitData::index_track(const RailTrack &t) {
	auto &handles = track_parts_[t.id];
	for (size_t k = 1; k < t.points.size(); k++) {
		int id;
		if (free_parts_.empty()) { id = int(parts_.size()); parts_.emplace_back(); }
		else { id = free_parts_.back(); free_parts_.pop_back(); }
		Part &part = parts_[size_t(id)]; part = {t.id, k, piece_box(t.points[k - 1], t.points[k])};
		handles.push_back(id); track_index_.insert(id, part.box);
	}
}
void TransitData::retire_track(int id) {
	const auto it = track_parts_.find(id);
	if (it == track_parts_.end()) return;
	for (int handle : it->second) { track_index_.erase(handle, parts_[size_t(handle)].box); free_parts_.push_back(handle); }
	track_parts_.erase(it);
}
void TransitData::rebuild_colours() {
	stop_colours_.clear();
	for (const TransitLine &line : lines_)
		for (int stop : line.stops) stop_colours_.try_emplace(stop, line.colour);
}
void TransitData::rebuild_line_dependencies() {
	bus_stop_lines_.clear(); rail_stop_lines_.clear();
	for (const TransitLine &l : lines_) {
		auto &users = l.mode == TransitMode::BUS ? bus_stop_lines_ : rail_stop_lines_;
		for (int stop : l.stops) users[stop].push_back(l.id);
	}
}
void TransitData::rebuild_index() {
	reindex_ids(); stop_index_.clear(); track_index_.clear(); stop_boxes_.clear(); track_parts_.clear();
	parts_.clear(); free_parts_.clear();
	stop_versions_.clear(); track_versions_.clear(); rebuild_colours();
	for (const auto &s : stops_) { index_stop(s); stop_versions_[s.id] = next_generation(); }
	for (const auto &t : tracks_) { index_track(t); track_versions_[t.id] = next_generation(); }
	stops_revision = next_generation(); tracks_revision = next_generation(); lines_revision = next_generation();
	bus_revision = next_generation(); rail_revision = next_generation(); bus_stops_revision = next_generation();
	rail_tracks_versions_ = {next_generation(), next_generation()};
	rebuild_line_dependencies(); bus_route_versions_.clear(); rail_line_versions_.clear();
	for (const TransitLine &l : lines_) {
		if (l.mode == TransitMode::BUS) bus_route_versions_[l.id] = next_generation();
		else rail_line_versions_[l.id] = next_generation();
	}
}
void TransitData::changed(const TransitChange &change, size_t stops_from, size_t tracks_from, size_t lines_from) {
	const auto bus_stop = [](const TransitStop &s) { return s.mode == TransitMode::BUS; };
	const auto any_stop = [](const TransitStop &) { return true; };
	const auto bus_line = [](const TransitLine &l) { return l.mode == TransitMode::BUS; };
	const auto rail_line = [](const TransitLine &l) { return l.mode != TransitMode::BUS; };
	const auto any_line = [](const TransitLine &) { return true; };
	const auto lane_stops = projection_ids(change.stops, stops_from, bus_stop, bus_stop_same);
	const auto rail_stops = projection_ids(change.stops, stops_from, any_stop, rail_stop_same);
	const auto bus_fleet = projection_ids(change.lines, lines_from, bus_line, bus_line_same);
	auto bus_routes = projection_ids(change.lines, lines_from, bus_line, route_line_same);
	auto rail_lines = projection_ids(change.lines, lines_from, rail_line, rail_line_same);
	auto rail_routes = projection_ids(change.lines, lines_from, rail_line, route_line_same);
	// Include both sides of a compound edit: old users may disappear and new users may be installed together.
	add_users(bus_stop_lines_, lane_stops, bus_routes); add_users(rail_stop_lines_, rail_stops, rail_lines);
	add_users(rail_stop_lines_, rail_stops, rail_routes);
	if (lines_from < change.lines.edits.size()) positions(lines_, line_pos_);
	if (!projection_ids(change.lines, lines_from, any_line, route_line_same).empty()) rebuild_line_dependencies();
	add_users(bus_stop_lines_, lane_stops, bus_routes); add_users(rail_stop_lines_, rail_stops, rail_lines);
	add_users(rail_stop_lines_, rail_stops, rail_routes);
	for (int id : bus_routes) {
		const auto *l = line(id);
		if (l && l->mode == TransitMode::BUS) bus_route_versions_[id] = next_generation(); else bus_route_versions_.erase(id);
	}
	for (int id : rail_routes) {
		const auto *l = line(id);
		if (l && l->mode != TransitMode::BUS) rail_line_versions_[id] = next_generation(); else rail_line_versions_.erase(id);
	}
	if (!bus_fleet.empty() || !bus_routes.empty()) bus_revision = next_generation();
	if (!lane_stops.empty()) bus_stops_revision = next_generation();
	if (tracks_from < change.tracks.edits.size() || !rail_lines.empty()) rail_revision = next_generation();
	if (stops_from < change.stops.edits.size()) {
		const auto ids = changed_ids(change.stops, stops_from);
		for (int id : ids) retire_stop(id);
		positions(stops_, stop_pos_);
		for (int id : ids) {
			if (const auto *s = stop(id)) { index_stop(*s); stop_versions_[id] = next_generation(); }
			else stop_versions_.erase(id);
		}
		stops_revision = next_generation();
	}
	if (tracks_from < change.tracks.edits.size()) {
		const auto ids = changed_ids(change.tracks, tracks_from);
		for (int id : ids) retire_track(id);
		positions(tracks_, track_pos_);
		for (int id : ids) {
			if (const auto *t = track(id)) { index_track(*t); track_versions_[id] = next_generation(); }
			else track_versions_.erase(id);
		}
		tracks_revision = next_generation();
		std::array<bool, 2> touched{};
		for (size_t i = tracks_from; i < change.tracks.edits.size(); ++i) {
			const auto &e = change.tracks.edits[i];
			for (const auto *side : {&e.before, &e.after}) if (*side) {
				if ((*side)->mode == TransitMode::TRAIN) touched[0] = true;
				if ((*side)->mode == TransitMode::METRO) touched[1] = true;
			}
		}
		for (size_t mode = 0; mode < touched.size(); ++mode) if (touched[mode]) rail_tracks_versions_[mode] = next_generation();
	}
	if (lines_from < change.lines.edits.size()) {
		bool colour_changed = false;
		for (size_t i = lines_from; i < change.lines.edits.size() && !colour_changed; i++) {
			const auto &edit = change.lines.edits[i];
			colour_changed = !edit.before || !edit.after || edit.before->id != edit.after->id
				|| edit.before->stops != edit.after->stops || edit.before->colour != edit.after->colour;
		}
		if (colour_changed) rebuild_colours();
		lines_revision = next_generation();
	}
}
void TransitData::apply(const TransitChange &change, bool forward) {
	change.stops.apply(stops_, forward); change.tracks.apply(tracks_, forward); change.lines.apply(lines_, forward);
	next_id = forward ? change.next_after : change.next_before;
	changed(change, 0, 0, 0);
}
bool TransitData::restore(std::vector<TransitStop> stops, std::vector<RailTrack> tracks, std::vector<TransitLine> lines, int next) {
	if (next <= 0) return false;
	std::unordered_set<int> ids;
	const auto valid_id = [&](int id) { return id > 0 && id < next && ids.insert(id).second; };
	const auto finite = [](const V2 &p) { return std::isfinite(p.x) && std::isfinite(p.y); };
	for (const auto &s : stops) if (!valid_id(s.id) || !finite(s.at) || !finite(s.entrance)) return false;
	for (const auto &t : tracks) {
		if (!valid_id(t.id)) return false;
		for (const auto &p : t.points) if (!finite(p)) return false;
	}
	for (const auto &l : lines) if (!valid_id(l.id)) return false;
	stops_ = std::move(stops); tracks_ = std::move(tracks); lines_ = std::move(lines); next_id = next;
	rebuild_index(); return true;
}

TrackHit TransitData::track_near(const V2 &p, float reach, TransitMode mode) const {
	TrackHit best; best.d = reach;
	if (reach <= 0) return best;
	std::vector<int> candidates;
	track_index_.query(query_box(p, reach), [&](int id) { candidates.push_back(id); });
	std::sort(candidates.begin(), candidates.end(), [&](int a, int b) {
		const auto &pa = parts_[size_t(a)], &pb = parts_[size_t(b)];
		const size_t ta = track_pos_.at(pa.track), tb = track_pos_.at(pb.track);
		return ta != tb ? ta < tb : pa.point < pb.point;
	});
	for (int id : candidates) {
		const Part &part = parts_[size_t(id)]; const RailTrack &t = *track(part.track);
		if (mode != TransitMode::BUS && t.mode != mode) continue;
		float share = 0;
		const V2 q = rail_segment_point(p, t.points[part.point - 1], t.points[part.point], share);
		const float d = (p - q).len();
		if (d < best.d) best = {t.id, q, d};
	}
	return best;
}
const TransitStop *TransitData::near_stop(const V2 &p, float reach, TransitMode mode, bool any) const {
	if (reach <= 0) return nullptr;
	std::vector<int> candidates;
	stop_index_.query(query_box(p, reach), [&](int id) { candidates.push_back(id); });
	std::sort(candidates.begin(), candidates.end(), [&](int a, int b) { return stop_pos_.at(a) < stop_pos_.at(b); });
	const TransitStop *best = nullptr; float dist = reach;
	for (int id : candidates) {
		const TransitStop &s = *stop(id);
		if (!any && s.mode != mode) continue;
		const float d = (s.at - p).len();
		if (d < dist) { dist = d; best = &s; }
	}
	return best;
}

} // namespace city
