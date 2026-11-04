// Rigid bodies joined by limited ball joints, solved by extended position based dynamics (XPBD): Macklin, Mueller and Chentanez,
// "XPBD: Position-Based Simulation of Compliant Constrained Dynamics", Motion in Games 2016 (the constraint with compliance
// alpha / h^2 and its accumulated multiplier); Mueller, Macklin, Chentanez, Jeschke and Kim, "Detailed Rigid Body Simulation with
// Extended Position Based Dynamics", Computer Graphics Forum 39(8), 2020 (a frame is split in small steps with one constraint
// iteration each; bodies are corrected in position and in rotation; joints, limits, contact and friction as constraints; the
// velocities follow from the positions). Pure single-precision C++, no Godot: the same inputs give the same motion on every
// machine. The owner of the bodies (a person falling, render/effects/ragdoll_world.h) fills `bodies`, `joints` and `points`.
#pragma once

#include "core/math3.h"

#include <vector>

namespace city::phys {

struct Body {
	V3 x, x_prev, v, w; // centre of mass, its place at the start of the step, linear and angular velocity
	Q4 q, q_prev; // rotation of the bind (model) frame into the world
	float inv_mass = 0;
	M3 inertia, inv_inertia; // body frame (the model frame at bind)
	M3 inv_world; // inverse inertia in the world, refreshed with q
	bool alive = true; // a lost limb is not there: not stepped, never joined
};

// A ball joint with limits: `r_parent` and `r_child` are the pivot seen from each body's centre in its own frame. The limits are
// those of the child's anatomical pose (sim/physics/person_rig.h): ex, ey, ez its axes in the parent's frame, `offset` the turn
// from the anatomical zero to the bind pose. Ranges in radians: flex about ex, lateral about ez, twist about ey.
struct Joint {
	int parent = -1, child = -1;
	V3 r_parent, r_child, ex, ey = V3(0, 1, 0), ez;
	Q4 offset;
	float flex_lo = 0, flex_hi = 0, lat_lo = 0, lat_hi = 0, twist_lo = 0, twist_hi = 0;
	bool attached = true; // false once the limb is torn off
};

struct ContactPoint {
	int body = 0;
	V3 r; // a skin point, from the body's centre, body frame
};

class Ground {
public:
	virtual ~Ground() = default;
	virtual float height(float x, float z) const = 0;
};

struct SolverParams {
	int substeps = 1, joint_iterations = 1;
	float gravity = 0, friction = 0, slope_span_m = 0, rest_speed = 0; // gravity: magnitude, m/s^2
};

// What one step cost, counted (the budget the owner reports).
struct StepCost {
	int bodies = 0, joints = 0, points_tested = 0, contacts = 0, substeps = 0;
};

class Articulation {
public:
	std::vector<Body> bodies;
	std::vector<Joint> joints;
	std::vector<ContactPoint> points;
	SolverParams params;
	float quiet_s = 0; // how long every body has been slower than params.rest_speed

	// Gives each body its world inverse inertia (call after setting q).
	void refresh();
	// Advances `dt` seconds, adding to `cost`.
	void step(float dt, const Ground &ground, StepCost &cost);
	// The fastest speed of any live body's centre or of a point of its skin at one body radius (m/s).
	float fastest() const;
	// How far a joint's two pivots are apart now (metres), and how far its limits are overshot (radians; `component` 0 flex, 1 lateral, 2 twist).
	float gap_of(size_t joint) const;
	float overshoot_of(size_t joint, int &component) const;
	// Largest distance between the two pivots of a joint (metres): how far the bodies have been pulled apart.
	float worst_gap() const;
	// Largest angle (radians) a limit of an attached joint is overshot by.
	float worst_overshoot() const;

private:
	struct Contact {
		int body = 0;
		V3 r, n;
		float depth = 0;
	};
	std::vector<Contact> contacts_;
	std::vector<V3> before_; // where each skin point was when the frame began
	std::vector<V3> limit_axis_; // per joint: the axis its limit was last corrected about, in this step (zero: not at a limit)
	V3 point_at(const ContactPoint &p) const;
	void advance_body(Body &b, float h) const;
	void solve_joint(const Joint &j);
	void solve_limits(size_t joint);
	void solve_contacts(const Ground &ground, bool record); // record: keep the contacts for the friction of the velocities
	void velocities(float h);
	void friction_velocity(float h);
	// The swing and twist of the child's anatomical pose: s_u, s_t the swing vector components, tau the twist (radians).
	void measure(const Joint &j, Q4 &anat, float &su, float &st, float &tau) const;
	// Rotates the child by `delta` (a rotation in the parent's frame) and the parent the other way, shared by inverse inertia.
	V3 rotate_pair(const Joint &j, const Q4 &delta); // returns the world axis it turned about (zero when nothing)
	void stop_at_limits();
	void push(Body &b, const V3 &p, const V3 &r) const; // a positional impulse p at r (from the centre, world)
	float inverse_mass_at(const Body &b, const V3 &r, const V3 &n) const;
};

} // namespace city::phys
