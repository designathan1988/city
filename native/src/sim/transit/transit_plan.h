// Public transport as the player lays it out (Roadcraft 4.0 src/world/transit.ts): bus stops on the footway's edge,
// train and metro tracks, stations on them, and lines through stops of one mode (out along them and back). The edits
// are pure (each changes a copy the command then hands to j_set_transit); the queries read the index that
// set_plan builds once per version, never every stop or track for a pointer move.
#pragma once

#include "core/grid.h"
#include "core/math.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace city {

struct TransitParams;

enum class TransitMode : uint8_t { BUS = 0, TRAIN = 1, METRO = 2, COUNT };

struct TransitStop {
	int id = 0;
	TransitMode mode = TransitMode::BUS;
	float x = 0, z = 0; // a bus stop on the footway's edge, a station's platform on its track
	int segment = -1; // a bus stop: the road it is on
	int track = -1; // a station: the track it is on
	bool terminal = false; // a bus terminal: lines turn here and their buses wait their time
	bool has_entrance = false; // a metro station: its way down, on the footway nearest it
	float ex = 0, ez = 0;
};

struct RailTrack {
	int id = 0;
	TransitMode mode = TransitMode::TRAIN; // a train's track at grade, or the metro's under the ground
	std::vector<V2> points; // (x, z)
};

struct TransitLine {
	int id = 0;
	TransitMode mode = TransitMode::BUS;
	std::vector<int> stops; // its stops or stations, in order: out along them and back
	int colour = 0; // index into TransitParams::line_colours
	int vehicles = 0;
	std::string name;
};

struct TransitPlan {
	std::vector<TransitStop> stops;
	std::vector<RailTrack> tracks;
	std::vector<TransitLine> lines;
	int next_id = 1;
	bool empty() const { return stops.empty() && tracks.empty() && lines.empty(); }
};

// A stretch of a track as filed: its track, its place in a walk of every track, its mode and ends.
struct TrackStretch {
	int track = 0, order = 0;
	TransitMode mode = TransitMode::TRAIN;
	V2 a, b;
};

// The plan's index (world/transit.ts stopIndex, trackCells): stops by id and by cell, stretches by cell; made by
// set_plan for each version (the plan is replaced whole by each edit).
struct PlanIndex {
	std::unordered_map<int, int> stop_at; // stop id -> index in plan.stops
	SpatialHash stops, stretches; // stop indices; indices into `list`
	std::vector<TrackStretch> list;
};

struct TrackHit {
	int track = -1;
	float x = 0, z = 0, d = 0;
};

namespace transit {

void set_index(PlanIndex &ix, const TransitPlan &p); // the index of `p` (set_plan's)
const TransitStop *stop_by_id(const TransitPlan &p, const PlanIndex &ix, int id);
// The stop (or station) nearest (x, z) closer than `reach`, of `mode` unless it is COUNT; null when none.
const TransitStop *stop_near(const TransitPlan &p, const PlanIndex &ix, float x, float z, float reach, TransitMode mode);
// The point of a track nearest (x, z) within `reach`, of `mode` unless COUNT (track -1 when none).
TrackHit nearest_track(const PlanIndex &ix, float x, float z, float reach, TransitMode mode);
// A track point (an end or a corner) of `mode` within `reach`, in the order a walk of every track meets it.
bool track_point_near(const PlanIndex &ix, float x, float z, float reach, TransitMode mode, V2 &out);
// How far, at most, a run of track lies on a road without leaving it, sampled every `step` metres.
template <typename OnRoad>
float longest_on_road(const std::vector<V2> &points, float step, OnRoad &&on_road) {
	float run = 0, worst = 0;
	for (size_t i = 1; i < points.size(); i++) {
		const V2 a = points[i - 1], b = points[i];
		const float len = (b - a).len();
		for (float d = 0; d < len; d += step) {
			run = on_road(a + (b - a) * (d / len)) ? run + step : 0;
			worst = worst > run ? worst : run;
		}
	}
	return worst;
}

// Edits (world/transit.ts): each changes `p` in place; the add ones return the new id.
int add_stop(TransitPlan &p, TransitStop stop);
void remove_stop(TransitPlan &p, int id); // and from every line (a line left with fewer than two goes)
void set_terminal(TransitPlan &p, int id, bool terminal);
int add_track(TransitPlan &p, TransitMode mode, const std::vector<V2> &points);
void remove_track(TransitPlan &p, int id); // and the stations on it
int add_line(TransitPlan &p, const TransitParams &tp, TransitMode mode, const std::vector<int> &stops);
void remove_line(TransitPlan &p, int id);
void set_line_vehicles(TransitPlan &p, const TransitParams &tp, int id, int vehicles);
// Drops anything malformed of a plan read from a save; a line keeps only stops that exist and are of its mode.
void normalize(TransitPlan &p, const TransitParams &tp);

} // namespace transit
} // namespace city
