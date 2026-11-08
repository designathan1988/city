// What the render is told, once, of each blow, shot and fall: appended by sim/commands while it holds the map
// exclusively, drained by the render under the ChangeLog's mutex (change_log.h). Events are a picture, not state:
// anything that must persist is in DamageState.
#pragma once

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
//                  u = look hash, v = flags (casualty_flags below, the walker's age among them)
//   GEYSER         x y z, a = seconds
//   LEAK / SPARK   x y z on a wall a blow struck, a = seconds: a pipe spurting, the wiring shorting
//   FURNITURE      x y z, u = GoneKind, dx dz = the way thrown, a = FURNITURE_WHOLE, _SPLINTERS or _SNAPPED, v = 1 a pole with a lamp
//   WIRE           a wire torn loose, hanging from the pole left standing: x y z its foot and ground, dx dz its seat line (the arm,
//                  toward the street, unit), b c the fallen pole's foot, d the ground there, a = the angle (rad, of x z) out from the
//                  burst at the fallen end, u = 1 when the standing pole is rural
//   IGNITE         id = building, x z
//   DRIP           x y z: somebody hurt walked there, bleeding
struct DamageEvent {
	enum Kind : uint8_t { BLAST, SHOT, WOUND, BUILDING_DOWN, BUILDING_HIT, BUILDING_SCAR, VEHICLE_BLOWN, VEHICLE_HIT, VEHICLE_DROP, CASUALTY, GEYSER,
		LEAK, SPARK, FURNITURE, IGNITE, DRIP, WIRE };
	Kind kind = BLAST;
	int id = -1;
	float x = 0, y = 0, z = 0, dx = 0, dz = 0, a = 0, b = 0, c = 0, d = 0;
	uint32_t u = 0, v = 0;
};

// FURNITURE a: how a pole went.
constexpr float FURNITURE_WHOLE = 0.f, FURNITURE_SPLINTERS = 1.f, FURNITURE_SNAPPED = 2.f;

// CASUALTY flags (v): fate in bits 0-1, then which limbs went and which came off at this blow.
enum CasualtyKind : uint32_t { CAS_DEAD = 0, CAS_TORN = 1, CAS_KNOCKED = 2 };
enum CasLimb : uint32_t { CAS_LIMB_HEAD = 1u << 0, CAS_LIMB_ARM_L = 1u << 1, CAS_LIMB_ARM_R = 1u << 2, CAS_LIMB_LEG_L = 1u << 3, CAS_LIMB_LEG_R = 1u << 4 };
constexpr uint32_t VEHICLE_PAINT_SHIFT = 8; // VEHICLE_BLOWN / VEHICLE_DROP: u = type | paint << this
constexpr uint32_t CAS_KIND_MASK = 3, CAS_LOST_SHIFT = 2, CAS_SEVERED_SHIFT = 7, CAS_LIMB_MASK = 31;
constexpr uint32_t CAS_CHARRED = 1u << 12, CAS_CRAWL = 1u << 13, CAS_OPENED = 1u << 14, CAS_BLED = 1u << 15, CAS_STRUCK_SHIFT = 16; // struck part 0 none, 1 head ... 6 legR
constexpr int VEHICLE_BODY_ID_BASE = 8'000'000; // CASUALTY ids of a vehicle's occupants (the 4.0's convention)
// CASUALTY: the walker's age (PedAge) + 1, 0 for an occupant, who has none. It travels in the event: the walker may be gone by
// the time the render reads it, and the render does not read traffic (pose_buffer.h is its only window).
constexpr uint32_t CAS_AGE_SHIFT = 19, CAS_AGE_MASK = 3;
inline uint32_t cas_age(int age) { return uint32_t(age + 1) << CAS_AGE_SHIFT; }
inline int cas_age_of(uint32_t v) { return int((v >> CAS_AGE_SHIFT) & CAS_AGE_MASK) - 1; }

} // namespace city
