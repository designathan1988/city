// A walker's mode (ped_state.h PedMode), its one setter and the moves a walker's life makes, as the reference game's walkers live it:
// out of a door or a car onto the footway, along it, round a corner and over a crossing after waiting at its kerb for its leave, in at a
// door, into a car. A move the life never makes is a mistake of its caller: the tests' DLL stops on it (core/checks.h), the game keeps
// the walker where it was.
#include "core/checks.h"
#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

namespace {

constexpr int MODES = int(PedMode::LOT) + 1;

constexpr uint8_t mode_bit(PedMode m) { return uint8_t(1u << unsigned(m)); }

// PED_MODES[from]: the modes a walker in `from` may enter (each may stay where it is).
constexpr uint8_t PED_MODES[MODES] = {
		// SIDEWALK: round the node at its end (WAY), held at the kerb of the crossing that way starts with (WAIT), held by a stop that
		// called it to a car's door (KERB), in at its door (LOT); a walker is born on the sidewalk and put from there where it starts
		uint8_t(mode_bit(PedMode::SIDEWALK) | mode_bit(PedMode::WAY) | mode_bit(PedMode::WAIT) | mode_bit(PedMode::KERB) | mode_bit(PedMode::LOT)),
		// WAY: its next line, the kerb of a crossing it waits at, the next sidewalk
		uint8_t(mode_bit(PedMode::WAY) | mode_bit(PedMode::WAIT) | mode_bit(PedMode::SIDEWALK)),
		// WAIT: held still, or let over
		uint8_t(mode_bit(PedMode::WAIT) | mode_bit(PedMode::WAY)),
		// KERB: let go on foot where it stands
		uint8_t(mode_bit(PedMode::KERB) | mode_bit(PedMode::SIDEWALK)),
		// LOT: out onto the sidewalk at the foot of its way (in at the door it leaves the street)
		uint8_t(mode_bit(PedMode::LOT) | mode_bit(PedMode::SIDEWALK)),
};

} // namespace

bool enter_mode(Pedestrian &w, PedMode next) {
	if (!(PED_MODES[int(w.mode)] & mode_bit(next))) {
		if (CHECKS) city_stop("a walker's move its life never makes (ped_mode.cpp PED_MODES)");
		return false;
	}
	w.mode = next;
	return true;
}

} // namespace traffic
} // namespace city
