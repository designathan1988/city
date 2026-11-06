// What a bus at a stop has in hand: who is still to get off, who is walking up to get on (sim/traffic/bus_calls.cpp).
#pragma once

#include "core/math.h"

namespace city {
namespace traffic {

struct BusCall {
	struct Boarder {
		int ped = -1;
		V2 from;
		float walked = 0, takes = 0;
	};
	bool begun = false;
	int leaving = 0;
	float t = 0, next_off = 0;
	Boarder boarders[2];
};

} // namespace traffic
} // namespace city
