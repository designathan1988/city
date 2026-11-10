// Public transport as the player lays it out: bus stops and terminals on the streets, rail tracks (a train's at grade, the metro's
// under the ground) with their stations, and the lines through them. Each edit makes a new TransitData, kept whole by the undo
// history (the plan is small); the stops and stretches are filed by cell for the tools' picks. Pure C++, written only by sim/commands.
#pragma once

#include "core/math.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
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

struct TransitData {
	std::vector<TransitStop> stops;
	std::vector<RailTrack> tracks;
	std::vector<TransitLine> lines;
	int next_id = 1;
	bool empty() const { return stops.empty() && tracks.empty() && lines.empty(); }
	const TransitStop *stop(int id) const;
	const TransitLine *line(int id) const;
};

// The tracks of one mode as a graph: points within `join` of one another are one node, each run between two a stretch.
struct RailGraph {
	struct Edge {
		int a = 0, b = 0;
		float length = 0;
	};
	std::vector<V2> points;
	std::vector<Edge> edges;
	std::vector<std::vector<int>> at; // per point: its stretches
};

// The memory of the rail searches, one after another: each point's distance, the point it is reached from, and the queue. A point's
// distance counts only when stamped with the search's own number, so no search clears what the last one wrote.
struct RailSearch {
	std::vector<float> dist;
	std::vector<int> prev;
	std::vector<uint32_t> stamp;
	uint32_t cur = 0;
	std::vector<std::pair<float, int>> open;
};

struct TrackHit {
	int track = -1;
	V2 at;
	float d = 0;
};

// Edits: each changes `t`, the plan the commands replace whole. A stop removed leaves every line, and a line under two stops goes;
// a track removed takes its stations.
int transit_add_stop(TransitData &t, TransitStop stop);
void transit_remove_stop(TransitData &t, int id);
void transit_set_terminal(TransitData &t, int id, bool terminal);
int transit_add_track(TransitData &t, TransitMode mode, const std::vector<V2> &points);
void transit_remove_track(TransitData &t, int id);
int transit_add_line(TransitData &t, TransitMode mode, const std::vector<int> &stops, const std::vector<uint32_t> &colours, int vehicles, const std::string &name);
void transit_remove_line(TransitData &t, int id);
void transit_set_vehicles(TransitData &t, int id, int vehicles, int most);

// A plan read from a file: anything malformed dropped; a line keeps only stops that exist and are of its mode.
void transit_normalize(TransitData &t);
RailGraph rail_graph(const TransitData &t, TransitMode mode, float join);
// The points to pass along the tracks from one point on them to another; empty when no way joins them.
std::vector<V2> rail_path(const RailGraph &g, const V2 &from, const V2 &to, RailSearch &search);
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
