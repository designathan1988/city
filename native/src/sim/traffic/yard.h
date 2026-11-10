// A works' loading yard as a lorry drives it: what is read from the world under the locks (YardInput), and the ways worked out from
// that copy alone, a little each tick (YardPlanner): for each dock, from a stop on the lane before a gate, in reverse onto the dock,
// and from the dock out to the lane past the gate. The work is counted, never timed: a map gives the same plan on the same tick.
#pragma once

#include "sim/traffic/way_search.h"

#include <array>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace city {

struct CityState;
class RoadNetwork;

namespace traffic {

// What of the roads a yard was made from (yard_input.cpp): the segments read and the boxes they were read in (x0, z0, x1, z1), as
// RoadNetwork::for_segments_near reads them. Only a change of one of those segments, or a segment come into one of the boxes, can
// change the yard (Roadcraft 4.0 lorryDocks.ts knownYard: a yard stands while what it was made from near it does).
struct YardReads {
	std::vector<int> segs; // sorted
	std::vector<std::array<double, 4>> boxes;
	void read(const RoadNetwork &net, double x0, double z0, double x1, double z1); // a box read: the box and the segments in it
	// Whether the network change that removed `removed` and added or changed `changed` reaches what was read.
	bool touched_by(const RoadNetwork &net, const std::vector<int> &removed, const std::vector<int> &changed) const;
};

struct YardGate {
	V2 inner, outer; // the gate's middle just inside, and its foot on the lane
	AxlePose stop, join; // the rear axle standing on the lane before the gate and past it
	int ds = -1, lane = 0;
	float stop_at = 0, join_at = 0; // the same along the lane, the body's middle
};

struct YardDock {
	int bay = -1; // ParkingState::bays
	V2 door, out; // the middle of its closed end and the unit direction out of it
};

struct YardInput {
	Steered body;
	double x0 = 0, z0 = 0, cell = 0;
	int nx = 0, nz = 0;
	float street_window = 0; // the street counts only this near a gate: the search does not wander the whole frontage
	float dock_gap = 0, pre_dock = 0, pre_step = 0, reach = 0, turn = 0, settle_step = 0, fit_margin = 0;
	std::vector<V2> lot;
	std::vector<std::vector<V2>> open, shut; // carriageways and gate mouths; other lots and this lot's solid things
	std::vector<YardGate> gates;
	std::vector<YardDock> docks;
	YardReads reads;
};

// A dock's ways, rear-axle poses; `into_street` / `away_street`: where along them the part on the street ends and begins.
// How far a dock's ways got, the farthest any gate did (the probes): no room to start the reverse, no room at the stop on the lane,
// no way either way, one way but the lane between stop and join does not hold the body, one way driven back for the other, both found.
enum YardReach { YARD_NO_PRE, YARD_NO_STOP, YARD_NO_WAY, YARD_NO_LANE, YARD_ONE_WAY, YARD_BOTH_WAYS, YARD_REACHES };

struct DockWays {
	int bay = -1, gate = -1; // -1: no way
	int reached = YARD_NO_PRE;
	std::vector<AxlePose> into, away;
	int into_street = 0, away_street = 0;
};

// The searches of a plan by kind: the way in and the way out at the coarse grain; then, for a way still missing (the way in when
// neither was found; the one missing when the other cannot be driven back along the lane), the graduated grain and the fine one.
enum YardSearch { YARD_SEARCH_IN, YARD_SEARCH_OUT, YARD_SEARCH_GRADUATED, YARD_SEARCH_FINE, YARD_SEARCHES };

// What a plan spent, probes never read by the simulation: the time filling its field (the lot, the streets, the shut rings and the
// clearance), at the docks (their straight reverse and the gates tried), and in its searches' stages; each kind of search, how many
// ran, how many found a way, and what they spent.
struct YardCost {
	double total_ms = 0, field_ms = 0, dock_ms = 0, ways_ms = 0;
	double slice_ms = 0; // the longest a call of advance took: what a tick spends at most
	int slice_stage = -1; // the stage that call began at (YardPlanner::Stage)
	int searches[YARD_SEARCHES] = {}, found[YARD_SEARCHES] = {};
	SearchCost search[YARD_SEARCHES];
};

struct YardPlan {
	std::vector<YardGate> gates;
	std::vector<DockWays> docks;
	YardCost cost;
	YardReads reads; // what of the roads it was made from
};

class YardPlanner {
public:
	explicit YardPlanner(YardInput in);
	~YardPlanner();
	bool advance(int work); // spends `work`; true once `plan` is whole
	YardPlan plan;

private:
	enum class Stage { LAY, OPEN, STREET, SHUT, CLEARANCE, DOCK, GATE, WAY_IN, WAY_OUT, WAY_GRADUATED, WAY_FINE, DONE };
	YardInput in_;
	Field field_, lot_;
	Stage stage_ = Stage::LAY;
	size_t ring_ = 0, dock_ = 0, gate_ = 0;
	int row_ = 0;
	std::unique_ptr<RingFill> fill_; // the ring being set
	std::unique_ptr<ClearanceFill> clearance_;
	std::unique_ptr<WaySearch> search_;
	std::unordered_map<int64_t, std::shared_ptr<Holonomic>> maps_; // bounded: one per dock and grain, gone with the planner; the distance maps on this field, by goal (holonomic_key)
	WaySpace space_; // the memory its searches use, one after another
	DockWays ways_;
	AxlePose docked_, pre_;
	std::vector<int> order_;
	std::vector<AxlePose> onto_, away_;
	bool found_in_ = false, found_out_ = false;
	bool seek_out_ = false; // the search running looks for the way out
	WayGrain in_grain_ = WayGrain::COARSE, out_grain_ = WayGrain::COARSE; // the finest grain each way was searched at, at this gate
	std::vector<AxlePose> lane_; // the leg along the lane from this gate's stop to its join
	int lane_fits_ = -1; // whether the body fits along it; -1: not worked out for this gate yet
	void lay_step(int &work);
	std::unique_ptr<RingFill> open_ring(size_t k);
	void open_step(int &work);
	void street_step(int &work);
	void shut_step(int &work);
	void next_dock(int &work);
	void next_gate(int &work);
	void search_step(int &work);
	void search_for(const AxlePose &from, const AxlePose &to, bool reversing, WayGrain grain, Stage stage);
	void close_dock();
	bool search_on(const YardGate &g);
	bool lane_holds(const YardGate &g);
	bool join_ways(const YardGate &g);
};

// The yard's input; false when it has none to plan (`out.reads` holds what of the roads was read all the same).
bool yard_input(const CityState &s, int building, YardInput &out);
// A network change dropped `removed` and added or changed `changed`: the yards (planned and being planned) it reaches are let go,
// to be planned again when a lorry is next called to them; the others stand.
void forget_yards_near(CityState &s, const std::vector<int> &removed, const std::vector<int> &changed);

} // namespace traffic
} // namespace city
