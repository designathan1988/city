// The pieces a blast throws, as rigid boxes, cylinders and rounds: gravity, then each corner under the ground struck with an
// impulse (a body bounces, slides, tumbles and settles; at rest it sleeps and is never written again). Three MultiMeshes
// (boxes, cylinders, rounds: an organ, flattened) each tinted by the instance; a burning piece (a wreck) sends flame and smoke
// puffs while it burns.
#pragma once

#include "core/random.h"
#include "render/effects/effect_params.h"
#include "render/effects/mm_layer.h"
#include "render/effects/puffs.h"
#include "render/effects/rigid_step.h"

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/quaternion.hpp>

#include <deque>

namespace city {

constexpr uint64_t DEBRIS_SEED = 0xd3b415ULL; // the thrown pieces' own random stream: any fixed seed

struct PieceSpec {
	godot::Vector3 at, size, velocity, spin;
	godot::Quaternion turn;
	godot::Color tint; // sRGB
	bool cylinder = false; // size = (radius, length, radius)
	bool round = false; // a sphere stretched to the box `size` (an organ)
	bool dusty = false; // of a building: concrete dust over it
	int wreck = -1; // drawn as this vehicle's burnt body, whose box `size` is
	float burn_s = 0; // seconds it burns (a wreck), 0 none
	bool still = false; // laid where it is, never moved
	bool random_spin = true;
	float life_s = 0; // seconds until it is gone (a body that gets up, a tracer), 0 forever
};

class Debris {
public:
	void init(const godot::RID &scenario, const godot::RID &material, const EffectParams &params);
	void reset();
	void release();
	// The bodies a wreck may be: drawn with the pieces' material, a layer each.
	void set_wrecks(const godot::RID &scenario, const godot::RID &material, const std::vector<godot::Ref<godot::Mesh>> &bodies);
	void add(const PieceSpec &spec);
	// One frame: `ground_of(x, z)` the ground's height, puffs for what burns. Writes only when something moved or came.
	template <typename F>
	void update(double now, float dt, F &&ground_of, Puffs &puffs) {
		clock_ += dt;
		while (clock_ >= STEP_S) {
			clock_ -= STEP_S;
			for (Piece &p : pieces_)
				if (!p.asleep && rigid_step(p, ground_of, params_)) dirty_ = true;
		}
		for (Piece &p : pieces_) burn(p, now, dt, puffs);
		reap(dt);
		flush();
	}
	bool moving() const;
	int count() const { return int(pieces_.size()); }
	static constexpr float STEP_S = RIGID_STEP_S;

private:
	struct Piece : RigidPiece {
		godot::Vector3 size;
		godot::Color tint;
		float burn_until = 0, carry = 0;
		float life = 0;
		bool cylinder = false, round = false, still = false, charred = false, dusty = false;
		int wreck = -1;
	};
	void burn(Piece &p, double now, float dt, Puffs &puffs);
	void reap(float dt); // the pieces whose time is up leave
	void flush();
	MmLayer boxes_, cylinders_, rounds_;
	std::vector<MmLayer> wrecks_;
	std::deque<Piece> pieces_;
	EffectParams params_;
	Rng rng_{ DEBRIS_SEED };
	float clock_ = 0;
	bool dirty_ = false;
	int drawn_ = -1;
};

} // namespace city
