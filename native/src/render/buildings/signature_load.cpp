// A signature design's load (signature_bodies.h SignatureBodies::State), its one setter and the moves a load makes: asked of a job once, or
// made at once on the main thread when a body drawn apart asks its mesh; made, its mesh kept, or its file unreadable, said once: either is
// final, never asked or made again. A move a load never makes is a mistake of its caller: the tests' DLL stops on it (core/checks.h), the
// game keeps the kind where it was.
#include "render/buildings/signature_bodies.h"

#include "core/checks.h"

namespace city {

namespace {

using State = SignatureBodies::State;
constexpr int STATES = int(State::FAILED) + 1;

constexpr uint8_t state_bit(State s) { return uint8_t(1u << unsigned(s)); }

// DESIGN_MOVES[from]: the states a kind's design in `from` may enter (each may stay where it is).
constexpr uint8_t DESIGN_MOVES[STATES] = {
		// NONE: asked of a job, or made (or failed) on the main thread at once
		uint8_t(state_bit(State::NONE) | state_bit(State::ASKED) | state_bit(State::READY) | state_bit(State::FAILED)),
		// ASKED: the job's result, or the main thread's own making meanwhile, made it or failed
		uint8_t(state_bit(State::ASKED) | state_bit(State::READY) | state_bit(State::FAILED)),
		// READY: its mesh kept while its chunks' instances come and go
		uint8_t(state_bit(State::READY)),
		// FAILED: said once, never read again
		uint8_t(state_bit(State::FAILED)),
};

} // namespace

bool SignatureBodies::enter(int kind, State next) {
	const State now = state_[size_t(kind)];
	if (!(DESIGN_MOVES[int(now)] & state_bit(next))) {
		if (CHECKS) city_stop("a signature design's move its load never makes (signature_load.cpp DESIGN_MOVES)");
		return false;
	}
	state_[size_t(kind)] = next;
	return true;
}

} // namespace city
