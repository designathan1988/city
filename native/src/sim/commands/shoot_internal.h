// One round of the pistol (shoot_commands.cpp) and what it does to a walker (shoot_people.cpp). Internal to sim/commands.
#pragma once

#include "sim/commands/strike_internal.h"

namespace city {

enum class BodyPart : uint8_t { HEAD = 0, TORSO, ARM_L, ARM_R, LEG_L, LEG_R };

// The round in flight: where it left, its unit direction, where it stopped and what it found there.
struct Round {
	CityState &s;
	const BlastParams &p;
	float ox, oy, oz, dx, dy, dz; // origin and unit direction (3D)
	float x = 0, y = 0, z = 0; // where it ended
	uint32_t stream; // the draws of this round (the same round, the same wound)
	ShotResult &out;

	float roll(uint32_t what, uint32_t who) const { return hash_float(hash_combine(hash_combine(stream, what), who)); }
	void emit(const DamageEvent &e) const { emit_damage_event(s, e); }
};

// The wound a round at (hx, hy, hz) leaves in walker `id`, on `part` (`lying`: it was shot where it lies).
void shoot_walker(Round &r, int id, BodyPart part, float hx, float hy, float hz, bool lying);
// The round meets dead body `k` of TrafficState::dead where it lies (the reference's shootBody 'hit': it stops there): the wound's spray, a
// limb struck comes off as on anybody lying, and the body jolts (a CASUALTY of its own id, the render's body takes it).
void shoot_dead(Round &r, int k, BodyPart part, float hx, float hy, float hz);
// A rider shot off a two-wheeler, or the bodywork of a car hit at relative height `up` (0..1).
void shoot_vehicle(Round &r, int id, float up, float hx, float hy, float hz);

} // namespace city
