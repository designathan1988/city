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
	// After begin: only these parts (a bit each, RAGDOLL_PARTS order) are a body of their own, a limb torn off: the sticks among their
	// particles hold, every other particle stays on the piece's root (the joint it came off at).
	void keep_parts(uint32_t parts);
	bool asleep() const { return asleep_; }
	// How far its deepest joint or bone point lies under the ground (its middle under the surface), 0 when none does: a probe.
	float deepest(const Ground &ground) const;
	// Each part's frame now: its origin on its joint, its axes along it. The frames only: what a bone does with them is the caller's.
	void frames(godot::Transform3D out[RAGDOLL_PARTS]);
	godot::Vector3 pelvis() const;
	// Its joints out of another body's (two limbs' half thicknesses apart), the other held where it is: two bodies falling together lie
	// one on the other.
	void push_out_of(const Ragdoll &other);
	// How far its joints lie within another body's, nearer than they touch; 0 when none does: a probe.
	float overlap(const Ragdoll &other) const;

private:
	static constexpr int PARTICLES = RAGDOLL_JOINTS + 2; // and the top of the head and the belly, which brace what the joints leave free
	struct Stick {
		int a = 0, b = 0;
		float rest = 0, most = 0; // never nearer than rest nor farther than most: a bone (rest = most), or a joint's range
	};
	void link(int a, int b, float share = 1.f, bool least_only = false);
	// A joint's range of bend at `mid` as its two ends' distance: the angle between mid -> a and mid -> b kept within less_rad less and
	// more_rad more than it stands now (no more than straight).
	void link_angle(int a, int mid, int b, float less_rad, float more_rad);
	void bones_on_ground(const Ground &ground);
	void bones_off_walls(); // no bone across a wall: its nearer end brought to the side the other stands on
	bool uncross(int a, int b, const BodyWall &w, bool across_deep); // one bone against one of a wall's two thin ways
	void relax();
	void hinge(int top, int middle, int end, float way);
	void off_walls(int particle);
	std::vector<BodyWall> walls_;
	godot::Vector3 now_[PARTICLES], was_[PARTICLES];
	godot::Vector3 last_side_[RAGDOLL_PARTS];
	static constexpr int MOST_STICKS = 48;
	Stick sticks_[MOST_STICKS];
	int sticks_used_ = 0;
	uint32_t kept_ = ~0u; // the particles that move of themselves (a bit each); all for a whole body
	int root_ = 0; // where the others stay, for a piece
	float still_ = 0;
	bool asleep_ = false;
};

} // namespace city
