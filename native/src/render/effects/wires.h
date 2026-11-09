// The wires torn off the poles a blast felled (the reference game's render/blast.ts `wire` and `stepRope`): a rope of Verlet
// points pinned at the arm of the pole still standing, its loose end where the felled pole stood, thrown away from the blast; it
// swings down, lies on the ground, and its live end sparks there a while. Owned by EffectWorld; drawn as thin tinted boxes with the
// debris material (debris.gdshader), one a link; sizes in effects.json shapes.wires.
#pragma once

#include "core/random.h"
#include "render/effects/effect_params.h"
#include "render/effects/mm_layer.h"
#include "render/effects/puffs.h"

#include <deque>
#include <functional>
#include <vector>

namespace city {

constexpr uint64_t WIRES_SEED = 0x317e5ULL; // the ropes' own random stream: any fixed seed

class Wires {
public:
	void init(const godot::RID &scenario, const godot::RID &material, const EffectParams &params);
	void reset();
	void release();
	// A rope pinned at `pin`, loose at `loose`, thrown by `kick` (m/s).
	void add(const godot::Vector3 &pin, const godot::Vector3 &loose, const godot::Vector3 &kick);
	void update(float dt, const std::function<float(float, float)> &ground_of, Puffs &puffs);
	bool moving() const;
	int count() const { return int(ropes_.size()); }

private:
	struct Rope {
		std::vector<godot::Vector3> p, o;
		godot::Vector3 pin;
		float seg = 0, sparks = 0, age = 0, carry = 0;
	};
	void step(Rope &r, float dt, const std::function<float(float, float)> &ground_of);
	void flush();
	MmLayer links_;
	std::deque<Rope> ropes_;
	EffectParams params_;
	Rng rng_{ WIRES_SEED };
	float clock_ = 0;
	bool dirty_ = false;
};

} // namespace city
