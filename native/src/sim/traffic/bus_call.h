// What a bus at a stop has in hand: who is still to get off, who is stepping down from the middle door to the footway, who is
// walking up to the front door to get on (sim/traffic/bus_calls.cpp).
#pragma once

#include "core/math.h"

namespace city {
namespace traffic {

constexpr int BUS_CALL_WALKERS = 2; // the most stepping off, and the most walking up, at once

struct BusCall {
	// A walker the call moves between a door and the footway: `foot` its place on the footway, `walked` of the `takes` seconds.
	struct Mover {
		int ped = -1;
		V2 foot;
		float walked = 0, takes = 0;
	};
	bool begun = false;
	int leaving = 0;
	float t = 0, next_off = 0;
	Mover alighters[BUS_CALL_WALKERS], boarders[BUS_CALL_WALKERS];
};

} // namespace traffic
} // namespace city
