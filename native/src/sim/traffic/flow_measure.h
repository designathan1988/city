// The flows measured at the junctions: vehicles an hour entering each junction from each of its legs, as an exponentially
// weighted mean of the counts of each second (time constant flow.trend_window / 3, data/catalog/tuning.json), aged lazily: a
// leg nobody entered keeps its last rate and the second it was taken, and decays when it is read. A junction out of the micro
// window is held (nothing is counted there): its rates stand still until it is counted again, so a signal there does not fall
// back for want of a count. Owned by the traffic (FlowControl), under its mutex.
#pragma once

#include "sim/catalog/catalog.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

struct FlowMeasure {
	struct Leg {
		int seg = -1;
		float entered = 0; // vehicles counted this second, folded into the rate at the next whole second
		bool counting = false; // counted since the last whole second
		bool known = false; // has a rate
		float rate = 0; // vehicles an hour, as it stood at the end of second `at`
		int32_t at = 0;
	};
	struct Junction {
		std::vector<Leg> legs;
		int32_t held_since = -1; // the second its rates were held at; -1 when they age
		bool counting = false;
	};

	float measured = 0; // seconds of traffic measured since the city was laid out
	void record(int node, int seg); // a vehicle entered `node` from `seg`
	void step(float dt, const Tuning &tn); // the counts of each whole second become rates
	float rate(int node, int seg, const Tuning &tn) const; // vehicles an hour entering `node` from `seg`, aged to now
	void hold(int node); // its rates stand still from now (out of the micro window)
	void resume(int node); // its rates go on from where they were held, the seconds out not aged
	std::vector<int> take_counted(); // the junctions counted at since the last call, in the order they were first counted

private:
	std::unordered_map<int, Junction> junctions_;
	std::vector<int> counted_; // junctions with counts this second
	std::vector<int> counted_since_; // junctions counted since the last take_counted
	float carry_ = 0;
	int32_t second_ = 0; // whole seconds aged so far: the clock the rates are kept against
};

} // namespace city
