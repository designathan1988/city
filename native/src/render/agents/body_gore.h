// What hangs off a body opened or cut: the guts out of the belly, a chain of links stepped by Verlet, its
// first link held to the wound while it holds (some tear loose after a while) and the rest lying where they fall; and a bone's
// end out of a stump, drawn from the joint along the limb it ends, with the cuff of flesh round it. Each belongs to a body (its
// slot in render/agents/body_renderer.h, which steps them and asks it for the body's particles), and goes with it. Three layers
// of the debris material (render/shaders/debris.gdshader), written only while something moves. The look is aftermath.json gore.
#pragma once

#include "core/random.h"
#include "render/agents/ragdoll.h"
#include "render/effects/effect_params.h"
#include "render/effects/mm_layer.h"

#include <functional>
#include <vector>

namespace city {

class BodyGore {
public:
	// The body's particle `particle` now, false when the body in `slot` is gone.
	using Anchor = std::function<bool(int slot, int particle, godot::Vector3 &out)>;
	void init(const godot::RID &scenario, const godot::RID &material, const GoreLook &look, float gravity); // gravity m/s2, the debris'
	bool ready() const { return links_.mm.is_valid(); }
	void reset();
	void release();
	// Guts out of the body in `slot` at its particle `particle` (the belly, the pelvis), now at `at`, thrown by `kick`.
	void spill(int slot, int particle, const godot::Vector3 &at, const godot::Vector3 &kick);
	// A bone's end out of the body in `slot`: from the joint `stump.at`, along `stump.from` to it and on, `length` long.
	void bone(int slot, RagdollStump stump, float length);
	void forget(int slot); // the body went: what hung off it goes too
	// One frame: `bodies_moved` when a body's bones moved this frame (its bones out of a stump follow it).
	void update(float dt, const Anchor &anchor, const std::function<float(float, float)> &ground, bool bodies_moved);
	int chains() const { return int(chains_.size()); }
	int bones() const { return int(bones_.size()); }

private:
	struct Chain {
		std::vector<godot::Vector3> p, o;
		int slot = -1, particle = 0;
		bool held = true;
		float hold_s = 0, age = 0, link = 0, radius = 0, still = 0;
		godot::Color tint;
	};
	struct Bone {
		int slot = -1;
		RagdollStump stump;
		float length = 0;
	};
	void step(Chain &c, float dt, const Anchor &anchor, const std::function<float(float, float)> &ground);
	void flush(const Anchor &anchor);
	GoreLook look_;
	float gravity_ = 0;
	std::vector<Chain> chains_;
	std::vector<Bone> bones_;
	MmLayer links_, sticks_, cuffs_;
	static constexpr uint64_t GORE_STREAM = 0x676f7265ULL;
	Rng rng_{ GORE_STREAM };
	bool dirty_ = false;
};

} // namespace city
