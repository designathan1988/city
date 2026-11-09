// What a body opened by a bullet or a blast shows (the reference game's render/gore.ts): the guts out of the belly, chains of
// Verlet links held to the body's belly or pelvis while it holds (some tear loose after a while) and lying on the ground once
// still, and a bone's end out of a stump with the raw flesh round it, following the body while it lasts. Owned by EffectWorld;
// drawn as tinted cylinders with the debris material (debris.gdshader), sizes and tints in effects.json shapes.gore.
#pragma once

#include "core/random.h"
#include "render/effects/effect_params.h"
#include "render/effects/mm_layer.h"

#include <godot_cpp/variant/color.hpp>

#include <deque>
#include <functional>
#include <vector>

namespace city {

class Gore {
public:
	// Where a joint of a body is now: false once the body is gone (BodyRenderer::joint).
	using Anchor = std::function<bool(uint32_t body, int joint, godot::Vector3 &at)>;
	void init(const godot::RID &scenario, const godot::RID &material, const EffectParams &params);
	void reset();
	void release();
	// Guts out of body `body` at its joint `joint` (now at `at`), thrown by `kick` (m/s).
	void spill(uint32_t body, int joint, const godot::Vector3 &at, const godot::Vector3 &kick, Rng &rng);
	// A bone's end out of the stump at joint `at` of body `body`, along the bone from joint `from`, `len` metres out.
	void bone(uint32_t body, int from, int at, float len);
	// One frame: `ground_of(x, z)` the ground's height. Writes only what moved or changed.
	void update(float dt, const std::function<float(float, float)> &ground_of, const Anchor &anchor);
	bool moving() const;

private:
	struct Chain {
		std::vector<godot::Vector3> p, o;
		uint32_t body = 0;
		int joint = 0;
		bool held = true;
		float hold = 0, seg = 0, radius = 0, age = 0, still = 0;
		godot::Color tint;
	};
	struct Stub {
		uint32_t body = 0;
		int from = 0, at = 0;
		float len = 0;
	};
	void cylinder(MmLayer &layer, int slot, const godot::Vector3 &a, const godot::Vector3 &b, float r, const godot::Color &tint);
	MmLayer guts_, bones_, cuffs_;
	std::deque<Chain> chains_;
	std::deque<Stub> stubs_;
	EffectParams params_;
	bool dirty_ = false;
};

} // namespace city
