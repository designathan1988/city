// The public transport's state, a member of CityState (Roadcraft 4.0 world/transit.ts data and sim/transit/transit.ts
// TransitSim): the player's plan and its index, written only by sim/commands (set_plan) and the save; and what runs
// on it, written only by transit::step: buses (service vehicles of the traffic), train and metro lines as loops with
// their trains, level crossings, and each bus stop's lane point.
#pragma once

#include "sim/state/pose_buffer.h"
#include "sim/transit/transit_plan.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

// Where a stop's bus stands (parking.ts BayLane): the kerb lane beside it and how far along it.
struct StopLane {
	int ds = -1, lane = 0;
	float at = 0, x = 0, z = 0;
};

struct Bus {
	int line = 0; // line id
	int vehicle = -1; // traffic vehicle id while it is on the road
	uint32_t tag = 0; // its service tag there (Vehicle::service)
	int next = 0, dir = 1; // the index of the stop it goes to, the way along the line (+1 out, -1 back)
	float dwell = 0, wait = 0; // seconds at its stop; seconds before it is put on the road
};

struct Train {
	int key = 0;
	float s = 0, v = 0; // along the line's loop, metres and m/s
	int next = 0; // index of the station it goes to on the loop
	float dwell = 0;
	std::vector<PoseTrail> cars; // each car's poses at the last publications (render interpolation)
};

// Where a train track crosses a lane of a road at grade: the lane, how far along it, how far along the line's loop.
struct LevelCrossing {
	int ds = -1, lane = 0;
	float at = 0, loop_s = 0;
};

struct Station {
	int stop = 0;
	float s = 0;
	bool turn = false; // the loop turns back here (the first station and the last one out)
};

// A train or metro line as run: the loop out along its stations and back, the trains on it and its crossings.
struct RailLine {
	int line = 0, colour = 0, cars = 0;
	TransitMode mode = TransitMode::TRAIN;
	std::vector<V2> points;
	std::vector<float> at; // distance along the loop of each point
	float length = 0;
	std::vector<Station> stations;
	std::vector<Train> trains;
	std::vector<LevelCrossing> crossings;
	float minx = 0, minz = 0, maxx = 0, maxz = 0;
};

struct TransitRun {
	uint64_t plan_seen = UINT64_MAX; // the plan revision the lines were made from
	uint64_t lanes_seen = UINT64_MAX; // the traffic's lane revision read last
	std::unordered_map<int, StopLane> stop_lanes; // bus stop id -> its lane point
	std::unordered_map<int, std::vector<int>> bus_lines_at; // bus stop id -> the bus lines calling there
	std::unordered_map<int, int> line_at; // line id -> index in plan.lines
	std::vector<Bus> buses;
	std::vector<RailLine> rails;
	uint32_t next_tag = 1;
	// Reused every step (never allocated in the tick): the lane barriers and the walkers' keep-out discs handed to
	// the traffic, and the stops a lane change reached.
	std::vector<int> touched_stops;
};

struct TransitState {
	TransitPlan plan;
	PlanIndex index;
	uint64_t revision = 0; // bumps on every change of the plan (commands, undo, load)
	TransitRun run;
};

namespace transit {
// The plan replaced (commands, undo/redo, load): its index made again and its revision bumped.
inline void set_plan(TransitState &t, TransitPlan p) {
	t.plan = std::move(p);
	set_index(t.index, t.plan);
	t.revision++;
}
} // namespace transit

} // namespace city
