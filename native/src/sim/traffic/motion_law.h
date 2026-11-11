// The motion law the lane's step (vehicle_motion.cpp) and the connector's step (connector_motion.cpp) share: the leader a vehicle
// drives against, a body's length, the acceleration after the emergency cap and the jerk limit, and the move by it.
#pragma once

#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

// The nearest leader of a vehicle: the gap to its rear and its speed (NO_LEADER: none).
struct Lead {
	float gap = NO_LEADER, v = 0;
	void take(float g, float lv) {
		if (g < gap) {
			gap = g;
			v = lv;
		}
	}
};

inline float body_length(const CityState &s, const Vehicle &v) { return s.catalog.traffic.classes[int(v.type)].length; }

// The acceleration after the emergency cap and the jerk limit, never more than stops it short of its leader (vehicle_motion.cpp).
float finish_accel(const CityState &s, const Vehicle &v, float acc, const Lead &l, float dt);
// Moves `v` by the commanded `acc` along `pos` (its lane's s or its connector's cs) and keeps the acceleration that speed change
// really was (vehicle_motion.cpp).
void integrate(const CityState &s, Vehicle &v, float acc, float dt, float &pos);
// Moves the vehicles on one connector and hands those over its end to the lane it enters (connector_motion.cpp).
void step_connector(CityState &s, int ci, float dt);

} // namespace traffic
} // namespace city
