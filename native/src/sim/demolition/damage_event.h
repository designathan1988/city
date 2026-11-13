// What the render is told, once, of each blow, shot and fall: appended by sim/commands while it holds the map
// exclusively, drained by the render under the ChangeLog's mutex (change_log.h). Events are a picture, not state:
// anything that must persist is in DamageState.
#pragma once

#include <cmath>
#include <cstdint>

namespace city {

// Fields by kind (x, z ground; y height; dx, dz direction; a, b, c, d scalars; u, v bits; id the subject):
//   BLAST          x y z, a = collapse range m, b = charge kg, c = soil (0 earth, 1 hard, 2 building), d = light range m, dx = the force
//   SHOT           x y z where the round ended, dx dz = its unit direction (xz), a = dy, u = what it met (0 nothing, 1 walker, 2 vehicle)
//   WOUND          x y z, dx dz = way of the bullet, u = 1 when a limb came off
//   BUILDING_DOWN  id = building, x z centre, a = force; the ruin is in DamageState, u = its index in DamageState::ruins as it fell
//   BUILDING_HIT   id = building, x y z the point struck, a = overpressure kPa
//   BUILDING_SCAR  id = building: part of it broke off and it stands (DamageState::scars)
//   VEHICLE_BLOWN  id, x y z, a = heading, u = type | paint << 8, b = burn seconds
//   VEHICLE_HIT    x y z, dx dz, u = 1 glass
//   VEHICLE_DROP   id, x y z, a = heading, u = type | paint << 8 (a two-wheeler's rider shot off, the machine falls over)
//   CASUALTY       id (pedestrian id, or 8'000'000 + vehicle id), x y z, a = heading, b c = blast point, d = power 0..1,
//                  u = look hash, v = flags (casualty_flags below)
//   GEYSER         x y z, a = seconds       LEAK / SPARK: x y z, a = seconds
//   FURNITURE      x y z, u = GoneKind, dx dz = the way thrown, a = 1 when it burst into splinters
//   IGNITE         id = building, x z
//   DRIP           x y z: somebody hurt walked there, bleeding
//   BODY_DEAD      id = the walker who died, u = the id their body lies on by now (DEAD_BODY_ID_BASE + n: TrafficState::dead)
//   BODY_GONE      id = a dead body's: the simulation keeps it no more (past blast.shot dead_kept), nor does the render
struct DamageEvent {
	enum Kind : uint8_t { BLAST, SHOT, WOUND, BUILDING_DOWN, BUILDING_HIT, BUILDING_SCAR, VEHICLE_BLOWN, VEHICLE_HIT, VEHICLE_DROP, CASUALTY, GEYSER,
		LEAK, SPARK, FURNITURE, IGNITE, DRIP, BODY_DEAD, BODY_GONE };
	Kind kind = BLAST;
	int id = -1;
	float x = 0, y = 0, z = 0, dx = 0, dz = 0, a = 0, b = 0, c = 0, d = 0;
	uint32_t u = 0, v = 0;
};

// CASUALTY flags (v): fate in bits 0-1, then which limbs went and which came off at this blow.
enum CasualtyKind : uint32_t { CAS_DEAD = 0, CAS_TORN = 1, CAS_KNOCKED = 2 };
enum CasLimb : uint32_t { CAS_LIMB_HEAD = 1u << 0, CAS_LIMB_ARM_L = 1u << 1, CAS_LIMB_ARM_R = 1u << 2, CAS_LIMB_LEG_L = 1u << 3, CAS_LIMB_LEG_R = 1u << 4 };
constexpr uint32_t VEHICLE_PAINT_SHIFT = 8; // VEHICLE_BLOWN / VEHICLE_DROP: u = type | paint << this
constexpr uint32_t CAS_KIND_MASK = 3, CAS_LOST_SHIFT = 2, CAS_SEVERED_SHIFT = 7, CAS_LIMB_MASK = 31;
constexpr uint32_t CAS_CHARRED = 1u << 12, CAS_CRAWL = 1u << 13, CAS_OPENED = 1u << 14, CAS_BLED = 1u << 15, CAS_STRUCK_SHIFT = 16; // struck part 0 none, 1 head ... 6 legR
constexpr int VEHICLE_BODY_ID_BASE = 8'000'000; // CASUALTY ids of a vehicle's occupants (the 4.0's convention)
// CASUALTY ids of the dead bodies lying in the simulation (TrafficState::dead), over every walker's and occupant's, going round
constexpr int DEAD_BODY_ID_BASE = 9'000'000, DEAD_BODY_IDS = 1'000'000;
// Whether a CASUALTY's id names a body the render keeps by it: a walker's (alive) or a dead body's; an occupant's is nobody's twice.
constexpr bool body_kept_by_id(int id) { return (id >= 0 && id < VEHICLE_BODY_ID_BASE) || id >= DEAD_BODY_ID_BASE; }

// The way a CASUALTY goes down, flat (x, z): away from the push's point (b, c), or the way it faced (a) when that point lies under
// FALL_PUSH_LEAST_M off. The render tips the body that way (effect_bodies.cpp) and the simulation lays the walker that way
// (traffic::lay_down): one rule, so the body drawn and the body the crowd goes round and the rounds meet lie alike.
constexpr float FALL_PUSH_LEAST_M = 0.05f;
inline void fall_way(const DamageEvent &e, float &fx, float &fz) {
	fx = e.x - e.b;
	fz = e.z - e.c;
	const float l = std::sqrt(fx * fx + fz * fz);
	if (l > FALL_PUSH_LEAST_M) {
		fx /= l;
		fz /= l;
		return;
	}
	fx = std::cos(e.a);
	fz = std::sin(e.a);
}

} // namespace city
