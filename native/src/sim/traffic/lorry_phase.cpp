// A lorry's phase (parking_state.h LorryPhase), its one setter and the moves a delivery makes: called to a gate it comes along the lanes,
// stops before the gate and waits there off the lanes until its way in is clear, backs onto the dock, unloads, drives out and joins the
// lane past the gate, where it is a truck of the traffic again. A move a delivery never makes is a mistake of its caller: the tests' DLL
// stops on it (core/checks.h), the game keeps the lorry where it was.
#include "core/checks.h"
#include "sim/traffic/lane_closures.h"

namespace city {
namespace traffic {

namespace {

constexpr int PHASES = int(LorryPhase::JOINING) + 1;

constexpr uint8_t phase_bit(LorryPhase p) { return uint8_t(1u << unsigned(p)); }

// LORRY_PHASES[from]: the phases a lorry in `from` may enter (each may stay where it is). A lorry given up on the way, or gone with its
// vehicle, leaves the list of lorries in whatever phase it was in: no phase follows it.
constexpr uint8_t LORRY_PHASES[PHASES] = {
		// COMING: stopped before its gate it leaves the lanes and waits there
		uint8_t(phase_bit(LorryPhase::COMING) | phase_bit(LorryPhase::WAITING)),
		// WAITING: its way in clear of standing bodies and the street it sweeps closed, it starts backing in
		uint8_t(phase_bit(LorryPhase::WAITING) | phase_bit(LorryPhase::ARRIVING)),
		// ARRIVING: at the dock
		uint8_t(phase_bit(LorryPhase::ARRIVING) | phase_bit(LorryPhase::DOCKED)),
		// DOCKED: unloaded and its way out clear, it starts out
		uint8_t(phase_bit(LorryPhase::DOCKED) | phase_bit(LorryPhase::LEAVING)),
		// LEAVING: at the street, with the stretches it sweeps there closed, it swings onto the lane
		uint8_t(phase_bit(LorryPhase::LEAVING) | phase_bit(LorryPhase::JOINING)),
		// JOINING: on the lane past the gate it is a truck again, no lorry
		uint8_t(phase_bit(LorryPhase::JOINING)),
};

} // namespace

bool enter_lorry_phase(Lorry &l, LorryPhase next) {
	if (!(LORRY_PHASES[int(l.phase)] & phase_bit(next))) {
		if (CHECKS) city_stop("a lorry's move its delivery never makes (lorry_phase.cpp LORRY_PHASES)");
		return false;
	}
	l.phase = next;
	return true;
}

} // namespace traffic
} // namespace city
