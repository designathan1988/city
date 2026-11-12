// A kerb stop's phase (kerb_state.h KerbPhase), its one setter and the order an errand at the kerb keeps: the vehicle pulls up at its
// stop and halts; somebody it picks up is fetched from the footway first; the door opens; the person moves between the seat and the
// footway (a truck's mate away with the load meanwhile, and back); the door shuts. A door never opens before the vehicle has halted.
// A move the errand never makes is a mistake of its caller: the tests' DLL stops on it (core/checks.h), the game keeps the stop where
// it was.
#include "core/checks.h"
#include "sim/traffic/kerb_stops.h"

namespace city {
namespace traffic {

namespace {

constexpr int PHASES = int(KerbPhase::CLOSE) + 1;

constexpr uint8_t phase_bit(KerbPhase p) { return uint8_t(1u << unsigned(p)); }

// KERB_PHASES[from]: the phases a stop in `from` may enter (each may stay where it is). The stop ends from any of them (left its lane,
// given up, its walker lost, its door shut): no phase follows that.
constexpr uint8_t KERB_PHASES[PHASES] = {
		// APPROACH: stood still at its stop
		uint8_t(phase_bit(KerbPhase::APPROACH) | phase_bit(KerbPhase::HALT)),
		// HALT: the one it picks up called over (FETCH), or the door's swing clear (OPEN)
		uint8_t(phase_bit(KerbPhase::HALT) | phase_bit(KerbPhase::FETCH) | phase_bit(KerbPhase::OPEN)),
		// FETCH: walked in to the door
		uint8_t(phase_bit(KerbPhase::FETCH) | phase_bit(KerbPhase::OPEN)),
		// OPEN: the door open, the move between seat and footway begins
		uint8_t(phase_bit(KerbPhase::OPEN) | phase_bit(KerbPhase::TRANSFER)),
		// TRANSFER: set down, picked up or the mate back in (CLOSE); the mate out with the load (AWAY)
		uint8_t(phase_bit(KerbPhase::TRANSFER) | phase_bit(KerbPhase::AWAY) | phase_bit(KerbPhase::CLOSE)),
		// AWAY: the delivery done, the mate walks back to the door
		uint8_t(phase_bit(KerbPhase::AWAY) | phase_bit(KerbPhase::TRANSFER)),
		// CLOSE: the door shut, the stop ends
		uint8_t(phase_bit(KerbPhase::CLOSE)),
};

} // namespace

bool enter_kerb_phase(KerbStop &k, KerbPhase next) {
	if (!(KERB_PHASES[int(k.phase)] & phase_bit(next))) {
		if (CHECKS) city_stop("a kerb stop's move its errand never makes (kerb_phase.cpp KERB_PHASES)");
		return false;
	}
	k.phase = next;
	return true;
}

} // namespace traffic
} // namespace city
