// The driver behind a vehicle's wheel, its one owner: the temper drawn once for it (-1 timid .. +1 pushy) shapes what its class
// gives it: how hard it pulls away and brakes, the headway and the standing gap it keeps, how long it waits before looking for
// another way, how it weighs the others in a lane change. Every place that asks a vehicle's own following law asks here; the
// spreads are traffic.json's (temper_*), the emergency brake is the vehicle's, never the driver's.
#pragma once

#include "sim/traffic/traffic_state.h"

#include <algorithm>

namespace city {
namespace traffic {

// The class row as this driver drives it.
inline VehicleClass driver_of(const TrafficParams &p, const Vehicle &v) {
	VehicleClass c = p.classes[int(v.type)];
	c.a *= 1 + v.temper * p.temper_accel;
	c.b *= 1 + v.temper * p.temper_brake;
	c.headway *= 1 - v.temper * p.temper_headway;
	c.min_gap *= 1 - v.temper * p.temper_gap;
	return c;
}

// How long this driver waits where an ordinary one waits `base` seconds.
inline float patience(const TrafficParams &p, const Vehicle &v, float base) {
	return base * (1 - v.temper * p.temper_patience);
}

// The weight this driver gives what a lane change costs or gains the others (MOBIL's politeness).
inline float politeness(const TrafficParams &p, const Vehicle &v) {
	return std::clamp(p.lc_politeness * (1 - v.temper * p.temper_polite), 0.f, 1.f);
}

// The gain a change by choice must be worth to this driver.
inline float change_threshold(const TrafficParams &p, const Vehicle &v) {
	return p.lc_threshold * (1 - v.temper * p.temper_threshold);
}

} // namespace traffic
} // namespace city
