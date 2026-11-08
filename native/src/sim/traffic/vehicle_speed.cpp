// How fast a vehicle goes: its share of the limit (a bicycle at its own pace), on a connector and braking to a turn, with the
// slow drift of its speed.
#include "sim/traffic/traffic_system.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

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
	const TrafficParams &p = s.catalog.traffic;
	const DirSeg &d = s.traffic.g.ds[ds];
	float drift = p.speed_drift * std::sin(v.drift + float(s.traffic.time) * p.speed_drift_rate);
	return top_speed(s, v, d.speed) * (1 + drift);
}

namespace {
// the IDM's free-road exponent (Treiber, Hennecke and Helbing 2000: 4, so the code squares twice); the desired speed and the gap
// are divided by, so neither is taken under these
constexpr float IDM_DELTA = 4.f, MIN_V0 = 0.5f, MIN_GAP = 0.02f;
} // namespace

float idm_accel(const VehicleClass &c, float v, float v0, float gap, float vl) {
	v0 = std::max(v0, MIN_V0);
	float free;
	if (v <= v0) {
		const float r = v / v0, r2 = r * r;
		free = c.a * (1 - r2 * r2); // IDM_DELTA is 4: no pow in the per-vehicle path
	} else {
		free = -c.b * (1 - std::pow(v0 / v, c.a * IDM_DELTA / c.b));
	}
	if (gap >= NO_LEADER) return free;
	float s_star = c.min_gap + std::max(0.f, v * c.headway + v * (v - vl) / (2 * std::sqrt(c.a * c.b)));
	float z = s_star / std::max(gap, MIN_GAP);
	if (v <= v0) return z >= 1 ? c.a * (1 - z * z) : (free > 0 ? free * (1 - std::pow(z, 2 * c.a / free)) : free);
	return z >= 1 ? free + c.a * (1 - z * z) : free;
}

} // namespace traffic
} // namespace city
