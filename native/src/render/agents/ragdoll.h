// A body that has lost its footing (the 4.0's ragdoll.ts, there on Jolt): twenty particles on the skeleton's joints held by distance constraints
// (Position Based Dynamics, Mueller et al. 2007; Jakobsen's Verlet particles, GDC 2001), with limits at the knees and elbows, and thirteen rigid parts
// read from them that carry the bones. Pure: positions in, frames out.
#pragma once

#include "core/random.h"

#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <functional>
#include <vector>

namespace city {

// The particles on a bone, in people.bin's order (its ragdoll block): pelvis, upper spine, neck, head, then the left arm (upper arm,
// forearm, hand), the right arm, the left leg (thigh, calf, foot, toe) and the right leg
constexpr int RAGDOLL_JOINTS = 18;
// The parts, in people.bin's order: torso, neck, head, the upper arm and forearm left then right, the thigh, calf and foot left then right
constexpr int RAGDOLL_PARTS = 13;
constexpr int RAGDOLL_HITS = 4; // the most hard landings a step tells


// A wall of a building a body can strike: a box of the lot's frame, up to `top`.
struct BodyWall {
	godot::Vector2 at, along, deep; // the lot's corner and its axes
	float x0 = 0, y0 = 0, x1 = 0, y1 = 0, top = 0;
};

class Ragdoll {
public:
	using Ground = std::function<float(float, float)>;
	// `joints`: where they are; thrown at `velocity`, each a little differently by `scatter` m/s, and tipped: `tip` is what the head moves at over the feet,
	// so a body topples rather than dropping on its own feet. `brace`: what the hands move at besides (the living put them out the way they fall), the
	// elbows at half of it, halfway along the arm.
	void begin(const godot::Vector3 joints[RAGDOLL_JOINTS], const godot::Vector3 &velocity, const godot::Vector3 &tip, float scatter, const godot::Vector3 &brace,
			Rng &rng);
	// One step; where particles struck the ground faster than `hard_ms` (a body landing hard) go into `hits`, how many returned.
	int step(float dt, const Ground &ground, float hard_ms, godot::Vector3 hits[RAGDOLL_HITS]);
	void set_walls(std::vector<BodyWall> walls) { walls_ = std::move(walls); }
	bool asleep() const { return asleep_; }
	// Each part's frame now: its origin on its joint, its axes along it. The frames only: what a bone does with them is the caller's.
	void frames(godot::Transform3D out[RAGDOLL_PARTS]);
	godot::Vector3 pelvis() const;

private:
	static constexpr int PARTICLES = RAGDOLL_JOINTS + 2; // and the top of the head and the belly, which brace what the joints leave free
	struct Stick {
		int a = 0, b = 0;
		float rest = 0;
		bool least_only = false; // a limit: never nearer than this, free beyond
	};
	void link(int a, int b, float share = 1.f, bool least_only = false);
	void relax();
	void hinge(int top, int middle, int end, float way);
	void off_walls(int particle);
	std::vector<BodyWall> walls_;
	godot::Vector3 now_[PARTICLES], was_[PARTICLES];
	godot::Vector3 last_side_[RAGDOLL_PARTS];
	static constexpr int MOST_STICKS = 48;
	Stick sticks_[MOST_STICKS];
	int sticks_used_ = 0;
	float still_ = 0;
	bool asleep_ = false;
};

} // namespace city
