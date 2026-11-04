// The pieces a blast throws, as rigid boxes and cylinders: gravity, then each corner under the ground struck with an
// impulse (a body bounces, slides, tumbles and settles; at rest it sleeps and is never written again). Two MultiMeshes
// (boxes, cylinders) each tinted by the instance; a burning piece (a wreck) sends flame and smoke puffs while it burns.
#pragma once

#include "core/random.h"
#include "render/effects/effect_params.h"
#include "render/effects/mm_layer.h"
#include "render/effects/puffs.h"

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/quaternion.hpp>

#include <deque>

namespace city {

struct PieceSpec {
	godot::Vector3 at, size, velocity, spin;
	godot::Quaternion turn;
	godot::Color tint; // sRGB
	bool cylinder = false; // size = (radius, length, radius)
	float burn_s = 0; // seconds it burns (a wreck), 0 none
	bool still = false; // laid where it is, never moved
	bool random_spin = true;
};

class Debris {
public:
	void init(const godot::RID &scenario, const godot::RID &material, const EffectParams &params);
	void reset();
	void release();
	void add(const PieceSpec &spec);
	// One frame: `ground_of(x, z)` the ground's height, puffs for what burns. Writes only when something moved or came.
	template <typename F>
	void update(double now, float dt, F &&ground_of, Puffs &puffs) {
		clock_ += dt;
		while (clock_ >= STEP_S) {
			clock_ -= STEP_S;
			for (Piece &p : pieces_)
				if (!p.asleep) step(p, ground_of);
		}
		for (Piece &p : pieces_) burn(p, now, dt, puffs);
		flush();
	}
	bool moving() const;
	int count() const { return int(pieces_.size()); }
	static constexpr float STEP_S = 1.f / 60.f;
	static constexpr int CORNERS = 8; // of a piece's box

private:
	struct Piece {
		godot::Vector3 p, v, w, size;
		godot::Quaternion q;
		godot::Color tint;
		godot::Vector3 corners[CORNERS];
		float inv_inertia = 0, rest = 0, burn_until = 0, carry = 0;
		bool cylinder = false, asleep = false, still = false, charred = false;
	};
	template <typename F>
	void step(Piece &b, F &&ground_of);
	void burn(Piece &p, double now, float dt, Puffs &puffs);
	void flush();
	MmLayer boxes_, cylinders_;
	std::deque<Piece> pieces_;
	EffectParams params_;
	Rng rng_{ 0xd3b415ULL };
	float clock_ = 0;
	bool dirty_ = false;
	int drawn_ = -1;
};

} // namespace city

#include "render/effects/debris_step.inl"
