// data/render/ragdoll.json read once, strictly, into what the falling bodies need: the rig definition of sim/physics/person_rig.h
// (bodies by name, joints with their limits in radians, the right side mirrored from the left), the solver settings and which body
// each part a blow can take starts at. A missing key or an unknown name is reported.
#pragma once

#include "sim/physics/person_rig.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <string>
#include <vector>

namespace city {

struct RagdollParams {
	static constexpr int LIMBS = 5; // head, arm_l, arm_r, leg_l, leg_r (CasLimb)
	phys::RigDef rig;
	phys::SolverParams solver; // gravity is the effects' own, set by the owner
	float rest_after_s = 0, span_m = 0;
	int max_people = 0;
	std::vector<std::string> names; // body names, in rig order
	int limb_root[LIMBS] = {}; // head, arm_l, arm_r, leg_l, leg_r: the body each part starts at

	bool load(const godot::Dictionary &t, godot::PackedStringArray &errors);
};

} // namespace city
