// A cooked person (people.bin: skin vertices with four bone weights each) as an articulated body of rigid.h: the bodies and
// joints of data/render/ragdoll.json measured on the variation's own mesh (everything here is derived, nothing typed in):
// each body's centre and inertia are the weighted first and second moments of its skin vertices (the masses are the table's
// fractions), a joint's pivot is the mean of the vertices weighted by both bodies, the neutral pose and the axes of a joint come
// from the pivots, and the points that test the ground are skin vertices spread by farthest-point sampling. Pure, no Godot.
#pragma once

#include "sim/physics/rigid.h"

#include <string>
#include <vector>

namespace city::phys {

enum class Neutral { Bind, Hanging, Collinear };

struct BodyDef {
	std::vector<int> bones; // cooked bone indices that move with this body
	float mass = 0;
	int parent = -1;
};

// Ranges in radians, already in the child's own convention (the loader mirrors the right side).
struct JointDef {
	int child = 0, parent = 0;
	Neutral neutral = Neutral::Bind;
	bool ref_up = false; // the axis the swing is measured against: forward, or up (the ankle)
	int distal = -1; // the body whose own joint pivot ends this bone, -1: the skin vertex farthest from the pivot
	float flex_lo = 0, flex_hi = 0, lat_lo = 0, lat_hi = 0, twist_lo = 0, twist_hi = 0;
};

struct RigDef {
	std::vector<BodyDef> bodies;
	std::vector<JointDef> joints;
	int bone_count = 0, contact_points = 0;
	float min_gyration_m = 0;
};

// The skin of one level of detail: `bones` and `weights` four per vertex (weights summing to one).
struct SkinView {
	const float *positions = nullptr; // three per vertex, the bind pose (+Z forward, +Y up, soles at 0)
	const int *bones = nullptr;
	const float *weights = nullptr;
	int vertices = 0;
};

struct PersonRig {
	Articulation proto; // the bodies at rest in the bind pose (rotation identity, centres at the bind centres)
	std::vector<int> bone_body; // for each cooked bone its body, -1 for a bone the rig leaves out
	std::vector<V3> centre; // each body's centre at bind (the pivot of a bone's matrix)
	std::vector<std::vector<int>> descendants; // each body and all bodies below it (a lost limb takes its hand along)
	// False with `error` set when a body has no vertex or a joint no shared vertex.
	bool build(const SkinView &skin, const RigDef &def, std::string &error);
};

} // namespace city::phys
