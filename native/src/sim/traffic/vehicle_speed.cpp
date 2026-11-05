// How fast a vehicle goes: its share of the limit (a bicycle at its own pace), on a connector and braking to a turn, with the
// slow drift of its speed.
#include "sim/traffic/traffic_system.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr float DRIFT_RATE = 0.1f;
} // namespace

float top_speed(const CityState &s, const Vehicle &v, float limit) {
	const float own = limit * v.vf;
	return v.type == VehicleType::BICYCLE ? std::min(own, s.catalog.traffic.bike_speed_kmh / KMH_PER_MS * v.vf) : own;
}

float conn_speed(const CityState &s, const Conn &c) {
	return c.turn == Turn::STRAIGHT ? s.traffic.g.ds[c.to_ds].speed : s.catalog.traffic.turn_speed_kmh / KMH_PER_MS;
}

float turn_limit(const CityState &s, const Vehicle &v, const Conn &c, float dist) {
	if (c.turn == Turn::STRAIGHT) return NO_LEADER;
	const TrafficParams &p = s.catalog.traffic;
	const float vt = p.turn_speed_kmh / KMH_PER_MS, b = p.classes[int(v.type)].b;
	return std::sqrt(vt * vt + 2 * b * std::max(0.f, dist - p.stop_margin_m));
}

float desired_speed(const CityState &s, const Vehicle &v, int ds) {
	const DirSeg &d = s.traffic.g.ds[ds];
	float drift = s.catalog.traffic.speed_drift * std::sin(v.drift + float(s.traffic.time) * DRIFT_RATE);
	return top_speed(s, v, d.speed) * (1 + drift);
}

} // namespace traffic
} // namespace city
