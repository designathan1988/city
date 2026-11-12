// Public transport as the player lays it out: bus stops and terminals on the streets, rail tracks (a train's at grade, the metro's
// under the ground) with their stations, and the lines through them. Commands journal ordered deltas of changed records;
// the stops and stretches are filed by cell for the tools' picks. Pure C++, written only by sim/commands.
#pragma once

#include "core/math.h"
#include "core/table_delta.h"
#include "core/generation.h"
#include "core/bounds_index.h"

#include <cstdint>
#include <array>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace city {

enum class TransitMode : uint8_t { BUS = 0, TRAIN = 1, METRO = 2 };

struct TransitStop {
	int id = 0;
	TransitMode mode = TransitMode::BUS;
	V2 at; // a bus stop on the footway's edge, a station's platform on its track
	int seg = -1; // a bus stop: the road it is on
	int track = -1; // a station: the track it is on
	bool terminal = false; // a bus terminal: lines turn here and their buses wait their time
	bool has_entrance = false; // a metro station: its way down, on the footway nearest it
	V2 entrance;
};

struct RailTrack {
	int id = 0;
	TransitMode mode = TransitMode::TRAIN;
	std::vector<V2> points;
};

struct TransitLine {
	int id = 0;
	TransitMode mode = TransitMode::BUS;
	std::vector<int> stops; // in order: out along them and back
	uint32_t colour = 0; // RGB
	int vehicles = 0;
	std::string name;
};

struct TransitChange;
struct TrackHit;
class TransitMutation;
class TransitData {
public:
	const std::vector<TransitStop> &stops() const { return stops_; }
	const std::vector<RailTrack> &tracks() const { return tracks_; }
	const std::vector<TransitLine> &lines() const { return lines_; }
	int next_id = 1;
	uint64_t tracks_revision = next_generation(); // rail topology only, independent of bus/stop/line changes
	uint64_t stops_revision = next_generation(), lines_revision = next_generation();
	uint64_t bus_revision = next_generation(), rail_revision = next_generation(), bus_stops_revision = next_generation(); // runtime dependencies, not UI/name/entrance revisions
	bool empty() const { return stops_.empty() && tracks_.empty() && lines_.empty(); }
	const TransitStop *stop(int id) const;
	const TransitLine *line(int id) const;
	std::optional<uint32_t> first_line_colour(int stop) const {
		const auto it = stop_colours_.find(stop); return it == stop_colours_.end() ? std::optional<uint32_t>() : it->second;
	}
	const RailTrack *track(int id) const;
	uint64_t track_graph_version(TransitMode mode) const { return rail_tracks_versions_[mode == TransitMode::METRO ? 1 : 0]; }
	uint64_t track_version(int id) const { const auto it = track_versions_.find(id); return it == track_versions_.end() ? 0 : it->second; }
	uint64_t bus_route_version(int id) const { const auto it = bus_route_versions_.find(id); return it == bus_route_versions_.end() ? 0 : it->second; }
	uint64_t rail_line_version(int id) const { const auto it = rail_line_versions_.find(id); return it == rail_line_versions_.end() ? 0 : it->second; }
	uint64_t stop_version(int id) const { const auto it = stop_versions_.find(id); return it == stop_versions_.end() ? 0 : it->second; }
	bool restore(std::vector<TransitStop> stops, std::vector<RailTrack> tracks, std::vector<TransitLine> lines, int next);
	void apply(const TransitChange &change, bool forward);
	TrackHit track_near(const V2 &p, float reach, TransitMode mode) const;
	const TransitStop *near_stop(const V2 &p, float reach, TransitMode mode, bool any) const;
private:
	friend class TransitMutation;
	friend int transit_add_stop(TransitData &, TransitStop, TransitChange *);
	friend void transit_remove_stop(TransitData &, int, TransitChange *);
	friend void transit_set_terminal(TransitData &, int, bool, TransitChange *);
	friend int transit_add_track(TransitData &, TransitMode, const std::vector<V2> &, TransitChange *);
	friend void transit_remove_track(TransitData &, int, TransitChange *);
	friend int transit_add_line(TransitData &, TransitMode, const std::vector<int> &, const std::vector<uint32_t> &, int, const std::string &, TransitChange *);
	friend void transit_remove_line(TransitData &, int, TransitChange *);
	friend void transit_set_vehicles(TransitData &, int, int, int, TransitChange *);
	friend void transit_normalize(TransitData &);
	std::vector<TransitStop> stops_;
	std::vector<RailTrack> tracks_;
	std::vector<TransitLine> lines_;
	std::unordered_map<int, size_t> stop_pos_, track_pos_, line_pos_;
	std::unordered_map<int, uint64_t> stop_versions_, track_versions_;
	std::unordered_map<int, uint64_t> bus_route_versions_, rail_line_versions_;
	std::array<uint64_t, 2> rail_tracks_versions_{next_generation(), next_generation()};
	std::unordered_map<int, std::vector<int>> bus_stop_lines_, rail_stop_lines_;
	void rebuild_line_dependencies();
	std::unordered_map<int, uint32_t> stop_colours_;
	void rebuild_colours();
	BoundsIndex stop_index_, track_index_;
	std::unordered_map<int, Bounds2> stop_boxes_;
	struct Part { int track = 0; size_t point = 0; Bounds2 box; };
	std::vector<Part> parts_;
	FreeSlots free_parts_;
	std::unordered_map<int, std::vector<int>> track_parts_;
	int mutation_depth_ = 0;
	TransitChange *mutation_ = nullptr;
	void reindex_ids();
	void rebuild_index();
	void retire_stop(int id), retire_track(int id);
	void index_stop(const TransitStop &s), index_track(const RailTrack &t);
	void changed(const TransitChange &change, size_t stops_from, size_t tracks_from, size_t lines_from);
};

// Only records changed by one command, in each table's mutation order; counters are restored with the same undo step.
struct TransitChange {
	int next_before = 1, next_after = 1;
	TableDelta<TransitStop> stops;
	TableDelta<RailTrack> tracks;
	TableDelta<TransitLine> lines;
	explicit TransitChange(int next = 1) : next_before(next), next_after(next) {}
	bool empty() const { return stops.empty() && tracks.empty() && lines.empty() && next_before == next_after; }
	void apply(TransitData &t, bool forward) const {
		t.apply(*this, forward);
	}
};

// The tracks of one mode as a graph: points within `join` of one another are one node, each run between two a stretch.
struct RailQueries;
struct RailGraph {
	RailGraph();
	explicit RailGraph(size_t retained_bytes);
	struct Edge {
		int a = 0, b = 0;
		float length = 0;
	};
	std::vector<V2> points;
	std::vector<Edge> edges;
	std::vector<std::vector<int>> at; // per point: its stretches
	std::shared_ptr<RailQueries> queries; // bounded answers and a nonblocking scratch lease for this immutable graph
};

struct TrackHit {
	int track = -1;
	V2 at;
	float d = 0;
};

// Edits mutate `t` and optionally record ordered deltas. A stop removed leaves every line, and a line under two stops goes;
// a track removed takes its stations. Commands validate first and publish the accumulated delta once.
int transit_add_stop(TransitData &t, TransitStop stop, TransitChange *change = nullptr);
void transit_remove_stop(TransitData &t, int id, TransitChange *change = nullptr);
void transit_set_terminal(TransitData &t, int id, bool terminal, TransitChange *change = nullptr);
int transit_add_track(TransitData &t, TransitMode mode, const std::vector<V2> &points, TransitChange *change = nullptr);
void transit_remove_track(TransitData &t, int id, TransitChange *change = nullptr);
int transit_add_line(TransitData &t, TransitMode mode, const std::vector<int> &stops, const std::vector<uint32_t> &colours, int vehicles, const std::string &name, TransitChange *change = nullptr);
void transit_remove_line(TransitData &t, int id, TransitChange *change = nullptr);
void transit_set_vehicles(TransitData &t, int id, int vehicles, int most, TransitChange *change = nullptr);

// A plan read from a file: anything malformed dropped; a line keeps only stops that exist and are of its mode.
void transit_normalize(TransitData &t);
RailGraph rail_graph(const TransitData &t, TransitMode mode, float join, size_t retained_bytes = 0);
V2 rail_segment_point(const V2 &p, const V2 &a, const V2 &b, float &share);
// The points to pass along the tracks from one point on them to another; empty when no way joins them.
std::vector<V2> rail_path(const RailGraph &g, const V2 &from, const V2 &to);
// The nearest point of a track within `reach`, of a mode unless BUS is given (a bus has no track); track -1 when none.
TrackHit nearest_track(const TransitData &t, const V2 &p, float reach, TransitMode mode);
// The nearest stop within `reach`; `any` leaves the mode out. Null when none.
const TransitStop *stop_near(const TransitData &t, const V2 &p, float reach, TransitMode mode, bool any);
// How long a run of track lies on a road without leaving it, sampled every `step` metres: a level crossing is a few metres.
template <typename OnRoad>
float longest_on_road(const std::vector<V2> &points, float step, OnRoad on_road) {
	float run = 0, worst = 0;
	for (size_t i = 1; i < points.size(); i++) {
		const V2 d = points[i] - points[i - 1];
		const float len = d.len();
		for (float at = 0; at < len; at += step) {
			run = on_road(points[i - 1] + d * (at / len)) ? run + step : 0;
			worst = run > worst ? run : worst;
		}
	}
	return worst;
}

} // namespace city
