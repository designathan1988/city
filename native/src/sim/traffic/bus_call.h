// What a bus at a stop has in hand: who is still to get off and who is on the way from its door to the footway, who is walking
// up to get on (sim/traffic/bus_calls.cpp). Everybody it holds or called is let go or taken aboard before the call is over.
#pragma once

#include "core/math.h"

namespace city {
namespace traffic {

struct BusCall {
	// Somebody the call walks between a door and the footway: the walker, where from, seconds walked and the seconds it takes. One
	// getting on walks its sidewalk to the front door's arc first (kerb_call; `walked` counts from the call); once it `came`, its way in
	// runs from `from` across to `foot` and on to the door (kerb_way_in).
	struct Boarder {
		int ped = -1;
		V2 from, foot;
		float walked = 0, takes = 0;
		bool came = false;
	};
	bool begun = false;
	int leaving = 0;
	float t = 0, next_off = 0;
	Boarder boarders[2];
	Boarder leaver; // the one getting off now: from the middle door to `leaver_to` on the footway
	V2 leaver_to;
};

} // namespace traffic
} // namespace city
