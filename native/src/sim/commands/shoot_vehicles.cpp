// What a round does to traffic (Roadcraft actionsWiring.ts shootVehicle): a rider hit on a two-wheeler is thrown down dead
// and the machine falls over; a car hit through its glass (the upper part of the box) loses its driver's control and rolls
// to a stop where it stands, hit anywhere on the bodywork it takes the round; enough rounds make it burst.
#include "sim/commands/shoot_internal.h"

#include "sim/traffic/traffic_system.h"

#include <cmath>

namespace city {

namespace {
constexpr float RIDER_BACK_M = 4.f; // the rider is thrown from this far behind the round
constexpr float RIDER_PUSH = 0.15f;
constexpr float RIDER_HIT_M = 1.2f; // the wound is drawn this high over a two-wheeler's ground
} // namespace

void shoot_vehicle(Round &r, int id, float up, float hx, float hy, float hz) {
	TrafficState &t = r.s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	r.out.hit = ShotResult::VEHICLE;
	r.out.id = id;
	const uint32_t type = uint32_t(v.type) | (uint32_t(v.paint) << VEHICLE_PAINT_SHIFT);
	if (v.type == VehicleType::MOTO) {
		DamageEvent wound;
		wound.kind = DamageEvent::WOUND;
		wound.x = hx; wound.y = v.cur.y + RIDER_HIT_M; wound.z = hz;
		wound.dx = r.dx; wound.dz = r.dz;
		r.emit(wound);
		DamageEvent body;
		body.kind = DamageEvent::CASUALTY;
		body.id = VEHICLE_BODY_ID_BASE + id;
		body.x = v.cur.x; body.y = v.cur.y; body.z = v.cur.z;
		body.a = std::atan2(v.cur.fz, v.cur.fx);
		body.b = hx - r.dx * RIDER_BACK_M;
		body.c = hz - r.dz * RIDER_BACK_M;
		body.d = RIDER_PUSH;
		body.v = CAS_DEAD;
		r.emit(body);
		DamageEvent drop;
		drop.kind = DamageEvent::VEHICLE_DROP;
		drop.id = id;
		drop.x = v.cur.x; drop.y = v.cur.y; drop.z = v.cur.z;
		drop.a = std::atan2(v.cur.fz, v.cur.fx);
		drop.u = type;
		r.emit(drop);
		r.out.killed = true;
		traffic::despawn_vehicle(r.s, id, false);
		return;
	}
	const bool glass = up > r.p.glass_above;
	DamageEvent hit;
	hit.kind = DamageEvent::VEHICLE_HIT;
	hit.id = id;
	hit.x = hx; hit.y = hy; hit.z = hz;
	hit.dx = r.dx; hit.dz = r.dz;
	hit.u = glass ? 1u : 0u;
	r.emit(hit);
	if (glass) {
		v.vf = 0.f; // no driver: its wish speed is nil, it brakes to a stop where it is
		DamageEvent wound = hit;
		wound.kind = DamageEvent::WOUND;
		wound.u = 0;
		r.emit(wound);
	}
	if (int(++v.shots) >= r.p.vehicle_shots_to_burst) {
		r.out.burst = true;
		r.out.burst_x = v.cur.x; r.out.burst_y = v.cur.y; r.out.burst_z = v.cur.z;
	}
}

} // namespace city
