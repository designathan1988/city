// Public transport running (the plan is sim/transit/transit_data.h): each bus line's buses as vehicles of the traffic, stop to stop and
// back, on the road near the camera and on their timetable away from it; each train and metro line a loop out along its stations and
// back, its trains on it with a gap between them; level crossings closing lanes as a train comes. Stepped with the traffic.
#pragma once

#include "sim/state/pose_buffer.h"
#include "sim/transit/transit_data.h"

#include <unordered_map>
#include <vector>

namespace city {

struct CityState;
struct Vehicle;

struct Bus {
	int line = 0, vehicle = -1;
	int next = 0, dir = 1; // the index of the stop it is going to, and the way along the line
	float dwell = 0, wait = 0; // seconds at the stop; seconds before it is put on the road
};

struct Train {
	float s = 0, v = 0, dwell = 0; // along the loop
	int next = 0; // the station it is going to
	std::vector<PoseTrail> cars;
};

struct LoopStation {
	int stop = 0;
	float s = 0;
	bool turn = false; // the loop turns back here
};

// Where a train track crosses a lane at grade: how far along the lane and along the loop.
struct LevelCrossing {
	int key = 0; // ds * MAX_LANES + lane
	float at = 0, loop_s = 0;
};

struct RailLine {
	int line = 0;
	bool metro = false;
	uint32_t colour = 0;
	std::vector<V2> points;
	std::vector<float> at; // the distance along the loop of each point
	float length = 0;
	std::vector<LoopStation> stations;
	std::vector<Train> trains;
	std::vector<LevelCrossing> crossings;
};

// Where a bus stands at a stop: the kerb lane beside it, with its front at `at`.
struct StopLane {
	int ds = -1, lane = 0;
	float at = 0;
};

struct TransitRun {
	uint64_t plan_seen = 0, roads_seen = 0;
	bool built = false;
	std::vector<Bus> buses;
	std::vector<RailLine> rails;
	std::unordered_map<int, StopLane> stop_lanes; // per bus stop
	std::unordered_map<int, std::vector<std::pair<int, int>>> crossings_by_lane; // key: (rail, crossing)
	int stops_called = 0, trains_at_stations = 0; // counted for the probe
};

namespace transit {

void step(CityState &s, float dt);
void reset(CityState &s); // a new map or a load: nothing running
// What the render draws: each car of the trains near the camera (`look` carries the colour, `id` underground or not).
void publish(CityState &s, PoseFrame &f, int64_t stamp, int64_t before, float cx, float cz, float radius);
// Metres to the nearest closed level crossing ahead on a lane, minus where a vehicle waits for it; negative when none is.
float crossing_gap(const CityState &s, int key, float at);
// Metres to what holds a vehicle on lane `ds`: a closed crossing, or a bus's stop on the last lane of its leg; negative when nothing does.
float hold_ahead(const CityState &s, const Vehicle &v, int ds, bool through);
void bus_gone(CityState &s, int vehicle); // the traffic took it off the road

} // namespace transit
} // namespace city
