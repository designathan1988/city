// The driving law every vehicle moves by, on a lane or across a junction: the leader it keeps its distance to, its body's length, the
// acceleration it may take (emergency cap, jerk limit, never into its leader) and the step that moves it (vehicle_motion.cpp,
// connector_motion.cpp).
#pragma once

#include "sim/traffic/traffic_system.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

struct Lead {
	float gap = NO_LEADER, v = 0;
	void take(float g, float lv) {
		if (g < gap) { gap = g; v = lv; }
	}
};

inline float body(const CityState &s, const Vehicle &v) { return s.catalog.traffic.classes[int(v.type)].length; }

// Acceleration after the emergency cap and the jerk limit.
inline float finish_accel(const CityState &s, const Vehicle &v, float acc, const Lead &l, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	const VehicleClass &c = s.catalog.traffic.classes[int(v.type)];
	acc = clampf(acc, -c.b_emergency, c.a);
	// jerk limit from the last acceleration as felt, never harder than an emergency stop: a standing vehicle's is zero,
	// so it moves off as soon as its way clears (as Roadcraft 4.0 drivers do)
	const float foot = clampf(v.acc, -c.b_emergency, c.a);
	if (acc > foot) acc = std::min(acc, foot + s.catalog.traffic.jerk_up * dt);
	if (l.gap < NO_LEADER) {
		float be = c.b_emergency, room = std::max(0.f, l.gap - p.vehicle_safe_pad_m + (l.v > 0 ? l.v * l.v / (2 * be) : 0.f));
		float safe = -be * dt + std::sqrt(be * be * dt * dt + 2 * be * room);
		if (v.v + acc * dt > safe) acc = (safe - v.v) / dt;
	}
	return acc;
}

// Moves `v` by the commanded `acc`; keeps the acceleration that speed change really was (as Roadcraft 4.0 does): a
// vehicle held at a standstill is not braking, whatever its driver asks for.
inline void integrate(const CityState &s, Vehicle &v, float acc, float dt, float &pos) {
	const TrafficParams &p = s.catalog.traffic;
	float nv = std::max(0.f, v.v + acc * dt);
	pos += (v.v + nv) * 0.5f * dt;
	v.acc = (nv - v.v) / dt;
	v.v = nv;
	v.brake = acc < p.brake_light_accel || nv < p.brake_light_speed;
	v.stuck = nv < p.vehicle_stuck_speed ? v.stuck + dt : 0.f;
	v.wait = nv < p.vehicle_wait_speed ? v.wait + dt : 0.f;
	v.held = nv < p.vehicle_wait_speed ? v.held + dt : 0.f;
	v.lc_cool = std::max(0.f, v.lc_cool - dt);
}

} // namespace traffic
} // namespace city
