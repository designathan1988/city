// What a bus at a stop has in hand: who is still to get off, who is stepping down to the footway, who is walking up to get on
// (sim/traffic/bus_calls.cpp).
#pragma once

#include "core/math.h"

namespace city {
namespace traffic {

constexpr int BUS_CALL_WALKERS = 2; // at most two off and two on at a stop (the lab's kerbStops.ts planBusStop)

struct BusCall {
	// Somebody on foot between the bus and the footway: from `from` to `to` in `takes` seconds; a boarder then steps in through the
	// doorway to `in` in the step's seconds, up from the height it walked at (`y`) to the step's over the road (`step_y`).
	struct Walker {
		int ped = -1;
		V2 from, to, in;
		float walked = 0, takes = 0, y = 0, step_y = 0;
	};
	bool begun = false;
	int leaving = 0;
	float t = 0, next_off = 0;
	Walker boarders[BUS_CALL_WALKERS], leavers[BUS_CALL_WALKERS];
};

} // namespace traffic
} // namespace city
