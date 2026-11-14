// The height of every road: each segment a line of heights at stations STATION_M apart at most, laid on the line it is
// designed on (the ground under it, averaged along the road and held to the ground grade), level over its junction plates
// at the height its node settles on, on a deck with ramps or through a bore where it is built raised or sunken; the nodes
// settle in rounds until none rises. Only what changed since the last solve is solved again (the segments and nodes the
// network marks, the ground under the segments a stroke reached), and the same map gives the same heights.
// A segment keeps its heights alone. The ground read under it and its design line are the solve's own (GroundLines): made
// when a solve needs them, kept within grade.ground_cache_mb, made again from the ground when they were let go.
// The ground is handed in (sim/roads does not include sim/terrain): the natural ground, water standing as a floor.
#pragma once

#include "core/id_table.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace city {

class JobPool;
class RoadNetwork;
struct Tuning;

// How a segment is built: saved in its flags.
enum class Structure : uint8_t { GROUND = 0, ELEVATED = 1, BRIDGE = 2, TUNNEL = 3 };
constexpr bool is_raised(Structure s) { return s == Structure::ELEVATED || s == Structure::BRIDGE; }
constexpr bool is_sunken(Structure s) { return s == Structure::TUNNEL; }
constexpr size_t STRUCTURE_COUNT = 4;

// What a structure is, the one owner of its figures, read from the tuning (grade): clearance, the deck surface over the
// ground under it (negative: a tunnel's depth under it); deck, the structure under the asphalt; supports, whether it
// stands on bents and piers.
struct StructureSpec {
	float clearance = 0;
	float deck = 0;
	bool supports = false;
};
StructureSpec structure_spec(Structure s, const Tuning &tn);

// The ground a road is laid over at (x, z): the natural height, water standing as a floor.
using GroundAt = std::function<float(float x, float z)>;

// What the stations and the plates are measured by: metres.
constexpr float STATION_M = 1.6f; // the most a profile's stations stand apart
constexpr int STATIONS_MOST = 400; // a road of more is read coarser, never longer
constexpr float PLATE_MARGIN_M = 2.f; // a station and 0.4 m past the junction's casing
constexpr float CASING_BAND_M = 1.f; // the band past the footway the casing holds
constexpr float FOOTWAY_RISE_M = 0.15f; // the kerb a footway stands on

// A segment's solved profile: its heights at arc i * step from its a node, and the plates level at each end.
constexpr int CURVE_NUMBERS = 6; // what the curve of a segment is: its two ends and its control point
struct GradeProfile {
	bool valid = false;
	Structure structure = Structure::GROUND;
	bool manual = false; // a node of it has a height of its own (lift): an authored alignment
	bool walls = false; // its cutting is held by retaining walls
	int a = -1, b = -1;
	float len = 0, step = 0, half = 0, plate_a = 0, plate_b = 0;
	std::vector<float> h;
	float ylo = 0, yhi = 0; // the lowest and highest of h, as the solve left it
	double q[CURVE_NUMBERS] = {}; // the curve it was solved along
};

// The ground under a segment's stations, across its casing, and the line the segment is designed on. The solve's own: a
// reader of heights never takes them, and a solve that finds them gone makes them again from the same ground.
struct GroundLines {
	std::vector<float> top; // the highest across the casing, spread to its neighbours, over the surface clearance
	std::vector<float> mean; // the mean across the casing over the clearance: what is cut balances what is filled
	std::vector<float> line; // the design line: the mean along the road, within the ground grade
	double q[CURVE_NUMBERS] = {}; // the curve and the casing they were read along
	float half = -1;
	uint32_t used = 0; // the solve that last read them
	bool held = false; // they stand in the cache's list
	// The ground was read for this frame: its stations, its curve and its casing.
	bool read_for(const GradeProfile &p) const;
	size_t bytes() const { return (top.capacity() + mean.capacity() + line.capacity()) * sizeof(float); }
};

// The ground lines by segment id, within a budget of memory. A copy of the network takes none of them (they are made
// again when asked): a trial copy and a reader's cut cost nothing of it.
struct GroundCache {
	std::vector<GroundLines> by_seg;
	std::vector<int> held; // the segments whose lines stand
	size_t bytes = 0;
	uint32_t solve = 0; // counts the solves: what a solve reads is marked with it
	GroundCache() = default;
	GroundCache(const GroundCache &) {}
	GroundCache &operator=(const GroundCache &) {
		by_seg.clear();
		held.clear();
		bytes = 0;
		return *this;
	}
	// The lines of segment s about to be written (counted out of `bytes` until `close` counts them in again, read now).
	GroundLines &open(int s);
	void close(int s);
	// The lines of segment s let go.
	void drop(int s);
	// The oldest read let go until what stands fits `budget` bytes.
	void fit(size_t budget);
};

// What a node settled on in the last solve: kept whole while none of its roads changed.
struct GradeNode {
	float height = 0; // where it stands
	float start = 0; // where it stood before the rounds (equal: no road carried it up)
	float grade = 0; // its grade height: the mean design line of its roads at grade at their plates, and its lift
	float cap = 0; // the most a ramp from a land end lets an aloft node rise
	bool aloft = false, capped = false, known = false;
};

// A road solved by itself (grade_draft.cpp): the road tool's draft before it is laid, a segment as it is laid. Nothing of
// the network is solved: the work is the road's own stations. `stand_a`, `stand_b`: the height each end stands at (the
// node it joins, or the ground there and its lift); `half`: its casing's half width (the profile's half and the band).
struct Quad;
struct RoadAlone {
	const Quad *q = nullptr;
	const std::vector<double> *table = nullptr; // its arc table (geometry.h arc_table)
	float half = 0;
	Structure structure = Structure::GROUND;
	bool authored = false; // an end of it has a height of its own: it takes the ramp's grade
	float stand_a = 0, stand_b = 0;
};
// Its stations `step` apart from its a end: the deck's height at each and the natural ground under its centre line.
struct AloneStations {
	float step = 0;
	std::vector<float> deck, ground;
};
AloneStations solve_alone(const RoadAlone &road, const GroundAt &ground_at, const Tuning &tn);

class RoadGrade {
public:
	IdTable<GradeProfile> prof; // by segment id
	IdTable<GradeNode> node; // by node id
	GroundCache ground; // the solve's own
	int unsettled = 0; // the nodes still rising when the last solve's rounds met grade.rounds_most (0: it settled)

	// What changed since the last solve (RoadNetwork calls these on every edit).
	void seg_changed(int s) { segs_.push_back(s); }
	void node_changed(int n) { nodes_.push_back(n); }
	void ground_changed(int s) { reground_.push_back(s); }
	void everything() { all_ = true; }
	bool pending() const { return all_ || !segs_.empty() || !nodes_.empty() || !reground_.empty(); }

	// Solves what changed. `moved`: the segments whose heights changed (their surfaces, lanes and ground follow);
	// `gone`: the segments solved before that are no longer alive.
	void solve(RoadNetwork &net, const GroundAt &ground_at, std::vector<int> &moved, std::vector<int> &gone, JobPool *jobs = nullptr);

	bool has(int s) const { return s >= 0 && s < int(prof.size()) && prof[size_t(s)].valid; }
	// The deck height of segment s at arc `at` from its a node (linear between stations, continuous).
	double at(int s, double arc) const;

	// A solve's working state by segment and by node, kept between solves at its resting values (every entry a solve
	// writes is put back after it), so a solve costs what it reaches, not the size of the network.
	struct Scratch {
		std::vector<char> touched, regrounded, seg_solved, lined, node_solved, aloft;
		std::vector<float> height, start, grade, cap;
		std::vector<int> heighted; // the nodes whose height this solve wrote
		void grow(size_t segs, size_t nodes);
	};

private:
	// The segments marked since the last solve, in id order; the marks are taken (the scratch says which were regrounded).
	std::vector<int> take_marks(const RoadNetwork &net);
	// Whether segment s, carried by a node, moved past a station's rounding from the heights it had (`then`).
	bool carried(int s, const std::vector<float> &then);
	std::vector<int> segs_, nodes_, reground_;
	bool all_ = false;
	Scratch scratch_;
	// By segment: its heights moved since they were last told (moved), by less than a station's rounding.
	std::vector<char> unshown_;
};

} // namespace city
