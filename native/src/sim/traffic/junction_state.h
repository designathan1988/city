// What the junctions keep between ticks beside the lane graph's own plan (TrafficState::junction): whose turn a merge is, who
// waits at each kerb of a crossing and for how long, a ring's load this tick. Each is a cell written where the fact happens (an
// entry admitted, a walker held at a kerb) and read by the gate and the signal: nothing here is found by walking the map.
#pragma once

#include <cstdint>
#include <vector>

namespace city {

constexpr double KERB_NEVER = -1.0e30; // no walker has spoken at that kerb yet

// Somebody waits at the kerb of a crossing: a walker held there asks again several times a second (pedestrians.cpp) and says how
// long it has waited; the cell is fresh while its last word is recent (ped_crossing.cpp kerb_waited).
struct KerbAsking {
	double seen = KERB_NEVER; // the time of the last word
	float longest = 0; // the longest wait told since the cell was last stale, seconds
};

// A ring's bodies against its room, read once a tick (junction_admission.cpp ring_full).
struct RingLoad {
	uint32_t tick = UINT32_MAX;
	float bodies = 0, room = 0; // metres of bodies with their gaps on its lanes, and its lanes' metres
};

struct JunctionState {
	std::vector<int> merge_last; // per lane key: the lane of origin (a lane key) whose head entered it last; -1 none yet
	std::vector<KerbAsking> kerb; // per node * NODE_ARMS_MAX + the arm's slot
	std::vector<RingLoad> ring; // per segment: the load of the ring it is part of, as walked from it this tick
};

} // namespace city
