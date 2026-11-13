// What the ragdoll's dynamics (ragdoll.cpp) and its contacts (ragdoll_contacts.cpp) both name: its particles, the parts they carry, how
// thick a limb stands and how a body drags on what it lies on. One owner for both files.
#pragma once

#include "render/agents/ragdoll.h"

namespace city::ragdoll {

// The particles: the joints in people.bin's ragdoll order, then the top of the head and the belly (Ragdoll PARTICLES).
enum Joint { PEL, CHE, NEC, HEA, LS, LE, LW, RS, RE, RW, LH, LK, LA, LT, RH, RK, RA, RT, TOP, BEL };
enum Ref { REF_SIDE, REF_LARM, REF_RARM, REF_LLEG, REF_RLEG };
struct PartFrame {
	int origin, to, ref;
};
inline constexpr PartFrame PART_FRAMES[RAGDOLL_PARTS] = { // torso, neck, head; arms; legs
	{ PEL, CHE, REF_SIDE }, { NEC, HEA, REF_SIDE }, { HEA, TOP, REF_SIDE }, { LS, LE, REF_LARM }, { LE, LW, REF_LARM }, { RS, RE, REF_RARM }, { RE, RW, REF_RARM },
	{ LH, LK, REF_LLEG }, { LK, LA, REF_LLEG }, { LA, LT, REF_LLEG }, { RH, RK, REF_RLEG }, { RK, RA, REF_RLEG }, { RA, RT, REF_RLEG },
};
inline constexpr float LIMB_HALF_M = 0.06f; // a limb's half thickness: what stands off the ground
inline constexpr float SLIDE_KEEP = 0.25f; // of the speed along the ground a step on it: a body drags
inline constexpr float NOTHING_M = 1e-5f;

} // namespace city::ragdoll
