// The vehicles' tail smoke: each engine owes puffs at its rate, a little standing and more pulling away, and pays them at the tail
// of its body, scattered about the pipe; one off the road raises a puff of dust now and then (the lab's render/exhaust.ts emit).
#include "render/effects/exhaust.h"

#include "core/shader_contracts/effects.h"

#include <algorithm>
#include <cmath>

namespace city {

bool Exhaust::load(const DataValue &d) {
	idle_per_s_ = d["idle_per_s"].f();
	moving_per_s_ = d["moving_per_s"].f();
	per_speed_ = d["per_speed"].f();
	moving_extra_max_ = d["moving_extra_max"].f();
	moving_from_ms_ = d["moving_from_ms"].f();
	spread_m_ = d["spread_m"].f();
	puff_m_ = d["puff_m"].f();
	life_s_ = d["life_s"].f();
	pipe_up_m_ = d["pipe_up_m"].f();
	dust_per_s_ = d["dust_per_s"].f();
	dust_m_ = d["dust_m"].f();
	dust_life_s_ = d["dust_life_s"].f();
	dust_up_m_ = d["dust_up_m"].f();
	return true;
}

void Exhaust::emit(const std::vector<ExhaustSource> &sources, float dt, Puffs &puffs, Rng &rng) {
	if (dt <= 0) return;
	for (const ExhaustSource &e : sources) {
		if (e.id >= owed_.size()) owed_.resize(size_t(e.id) + 1, 0.f);
		float &owed = owed_[e.id];
		const bool moving = std::fabs(e.speed) > moving_from_ms_;
		owed += dt * (moving ? moving_per_s_ + std::min(moving_extra_max_, std::fabs(e.speed) * per_speed_) : idle_per_s_);
		const float tx = e.x - e.fx * e.length * 0.5f, tz = e.z - e.fz * e.length * 0.5f; // the tail, under its pipe
		for (; owed >= 1; owed -= 1)
			puffs.puff(tx + (rng.f() - 0.5f) * spread_m_, e.y + pipe_up_m_, tz + (rng.f() - 0.5f) * spread_m_, PUFF_EXHAUST, puff_m_, life_s_);
		if (e.off_road && moving && rng.f() < dt * dust_per_s_) puffs.puff(tx, e.y + dust_up_m_, tz, PUFF_DUST, dust_m_, dust_life_s_);
	}
}

} // namespace city
