// The kerb errands' state in TrafficState (traffic_state.h): what a vehicle's trip does at the kerb, the phases of a stop and the
// stop itself (sim/traffic/kerb_stops.cpp), and what the render does to the passengers of a vehicle that stopped. Pure C++; written only
// by sim/traffic, read by render/agents.
#pragma once

#include "core/math.h"

#include <cstdint>

namespace city {

// An errand at the kerb (kerb_stops.cpp): somebody set down, somebody picked up, a truck's mate away with a delivery.
enum class Errand : uint8_t { NONE = 0, DROP, PICK, DELIVER };
enum class KerbPhase : uint8_t { APPROACH, HALT, FETCH, OPEN, TRANSFER, AWAY, CLOSE };
// Vehicle::aboard, what the render does to its passengers: as drawn, at least one to set down, one less, one more.
constexpr uint8_t ABOARD_AS_DRAWN = 0, ABOARD_TO_SET_DOWN = 1, ABOARD_SET_DOWN = 2, ABOARD_PICKED_UP = 3;
struct KerbStop {
	int vehicle = -1, ds = -1, lane = 0, ped = -1;
	Errand errand = Errand::NONE;
	KerbPhase phase = KerbPhase::APPROACH;
	bool back = false; // the mate is on the way back
	bool came = false; // the one to pick up has walked its sidewalk to the door's arc: the stop walks it in from `from` by `foot` (kerb_way_in)
	float at = 0, t = 0, elapsed = 0, fetch = 0, walked = 0, transfer = 0; // where its front stops; seconds in the phase and in all; the moves
	V2 from; // where the person to pick up stood when it came; going out, where the walk to the foot starts (the door, or where it stood when the foot moved)
	V2 foot; // where the person sets foot on the footway, clear of the street's pieces: set down, from the door where the vehicle halted; picked up, on its way in
	uint32_t foot_serial = 0; // TrafficState::furniture_serial when the foot was found: when it moves the foot is found again
};

} // namespace city
