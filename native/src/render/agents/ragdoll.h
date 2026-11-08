// A body that has lost its footing: twenty particles on the skeleton's joints held by distance constraints
// (Position Based Dynamics, Mueller et al. 2007; Jakobsen's Verlet particles, GDC 2001), with limits at the knees and elbows, and thirteen rigid parts
// read from them that carry the bones. Pure: positions in, frames out.
#pragma once

#include "core/random.h"

#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <cstdint>
#include <functional>
#include <vector>

namespace city {

constexpr int RAGDOLL_JOINTS = 18; // the particles on a bone, in cook_people.py RAGDOLL_JOINTS order
constexpr int RAGDOLL_PARTS = 13; // in cook_people.py RAGDOLL_PARTS order
constexpr uint32_t RAGDOLL_ALL_PARTS = (1u << RAGDOLL_PARTS) - 1;
// What can come off a body, in the order of the casualty's limb bits (sim/demolition/damage_event.h CasLimb).
enum RagdollLimb : int { LIMB_HEAD = 0, LIMB_ARM_L, LIMB_ARM_R, LIMB_LEG_L, LIMB_LEG_R, LIMBS };
// The end of a bone at a cut: the joint it shows at and the particle it points from, on the body or on the piece.
struct RagdollStump {
	int from = 0, at = 0;
};

// A wall of a building a body can strike: a box of the lot's frame, up to `top`.
struct BodyWall {
	godot::Vector2 at, along, deep; // the lot's corner and its axes
	float x0 = 0, y0 = 0, x1 = 0, y1 = 0, top = 0;
};

class Ragdoll {
public:
	using Ground = std::function<float(float, float)>;
	// `joints`: where they are; thrown at `velocity`, each a little differently by `scatter` m/s, and tipped: `tip` is what the head moves at over the feet,
	// so a body topples rather than dropping on its own feet.
	void begin(const godot::Vector3 joints[RAGDOLL_JOINTS], const godot::Vector3 &velocity, const godot::Vector3 &tip, float scatter, Rng &rng);
	void step(float dt, const Ground &ground);
	void set_walls(std::vector<BodyWall> walls) { walls_ = std::move(walls); }
	bool asleep() const { return asleep_; }
	// Each part's frame now: its origin on its joint, its axes along it. The frames only: what a bone does with them is the caller's.
	void frames(godot::Transform3D out[RAGDOLL_PARTS]);
	godot::Vector3 pelvis() const;
	godot::Vector3 belly() const;
	godot::Vector3 point(int particle) const { return now_[particle]; }
	// A piece cut off the body: only the particles of `parts` stay held together, each other stick let go; the rest
	// of the particles fly loose and unseen (the piece's body draws only `parts`). Its hinges go, and its frames keep the roll they had.
	void cut_to(uint32_t parts);
	static RagdollStump stump(RagdollLimb limb, bool piece);
	static uint32_t parts_of(RagdollLimb limb); // the parts a limb takes
	static constexpr int PELVIS = 0, BELLY = RAGDOLL_JOINTS + 1; // the particles guts hang from (ragdoll.cpp Joint)

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
	uint32_t kept_ = ~0u; // the particles a cut keeps together (bits); all of them on a whole body
};

} // namespace city
