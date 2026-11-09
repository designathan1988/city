// The vehicles' tail smoke (the lab's render/exhaust.ts emit): each running engine drawn near the camera puffs at its pipe, a little
// standing, more pulling away, and a car off the road raises dust; the puffs are the effects' own (puffs.h). AgentRenderer lists the
// engines it drew this frame (ExhaustSource); EffectWorld emits them on its clock. The rates and places are data/render/agents.json
// exhaust's.
#pragma once

#include "core/random.h"
#include "render/data_reader.h"
#include "render/effects/puffs.h"

#include <cstdint>
#include <vector>

namespace city {

// An engine running this frame: its vehicle (who keeps its share of a puff owed), its body's middle on the ground and heading, its
// length, its speed and whether it is off the road (a lot, a drive: its wheels raise dust).
struct ExhaustSource {
	uint32_t id = 0;
	float x = 0, y = 0, z = 0, fx = 0, fz = 1;
	float length = 0, speed = 0;
	bool off_road = false;
};

class Exhaust {
public:
	bool load(const DataValue &d); // agents.json exhaust
	void reset() { owed_.clear(); }
	// The puffs `sources` give over `dt` seconds.
	void emit(const std::vector<ExhaustSource> &sources, float dt, Puffs &puffs, Rng &rng);

private:
	float idle_per_s_ = 0, moving_per_s_ = 0, per_speed_ = 0, moving_extra_max_ = 0, moving_from_ms_ = 0;
	float spread_m_ = 0, puff_m_ = 0, life_s_ = 0, pipe_up_m_ = 0;
	float dust_per_s_ = 0, dust_m_ = 0, dust_life_s_ = 0, dust_up_m_ = 0;
	std::vector<float> owed_; // by vehicle id: the share of a puff its engine owes
};

} // namespace city
