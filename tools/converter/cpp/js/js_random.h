// Math.random's source in translated code: a generator the caller supplies, never one the runtime makes for itself, so a
// translated simulation is a function of its inputs and its seed (ECMA-262 21.3.2.27 leaves the source to the host).
// The translator gives every function that draws (and every function that calls one) a `js::Random &rng` parameter.
#pragma once

namespace js {

/** What the caller passes as `rng`: uniform numbers in [0, 1), from a generator it seeds and owns. */
struct Random {
	virtual ~Random() = default;
	virtual double next_unit() = 0;
};

} // namespace js
