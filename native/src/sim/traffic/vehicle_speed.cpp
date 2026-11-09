// How fast a vehicle goes: the driver behind the wheel (their temper spreads the class's pedals, headway and gap, and their wish
// speed wanders slowly), its share of the limit (a bicycle at its own pace), and the bends of the connector ahead taken within the
// lateral acceleration the driver accepts, braked for in advance (the lab's sim/vehicles/driver.ts and curvature.ts).
#include "sim/traffic/traffic_system.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

void draw_driver(const TrafficParams &p, Vehicle &v, Rng &rng) {
	// two uniforms summed: a triangular spread, most drivers ordinary and the extremes rare
	v.aggression = clampf((rng.f() + rng.f() - 1) * p.driver_aggression_stretch, -1.f, 1.f);
	v.drift_amp = p.drift_amp_min + rng.f() * p.drift_amp_span; // nobody holds an exact speed, whatever their temper
	v.drift_rate = p.drift_rate_min + rng.f() * p.drift_rate_span;
}

VehicleClass driver_of(const CityState &s, const Vehicle &v) {
	const TrafficParams &p = s.catalog.traffic;
	VehicleClass c = p.classes[int(v.type)];
	const float g = v.aggression;
	c.a *= 1 + g * p.driver_accel_spread;
	c.b *= 1 + g * p.driver_brake_spread;
	c.b_emergency *= 1 + g * p.driver_emergency_spread;
	// the pushy keep a shorter headway and a smaller gap
	c.headway *= 1 - g * p.driver_headway_spread;
	c.min_gap *= 1 - g * p.driver_gap_spread;
	return c;
}

float top_speed(const CityState &s, const Vehicle &v, float limit) {
	const float own = limit * v.vf;
	return v.type == VehicleType::BICYCLE ? std::min(own, s.catalog.traffic.bike_speed_kmh / KMH_PER_MS * v.vf) : own;
}

float conn_speed(const CityState &s, const Conn &c) {
	return c.turn == Turn::STRAIGHT ? s.traffic.g.ds[c.to_ds].speed : s.catalog.traffic.turn_speed_kmh / KMH_PER_MS;
}

float conn_top(const CityState &s, const Vehicle &v, const Conn &c) { return top_speed(s, v, s.traffic.g.ds[c.to_ds].speed); }

namespace {

// Below these a chord, a braking rate and an easing ramp are none (they divide), and a curvature is a straight road (the lab's
// curvature.ts STRAIGHT).
constexpr float LEAST_CHORD_M = 1e-3f, LEAST_BRAKE = 1e-3f, LEAST_RAMP_M = 1e-6f, STRAIGHT = 1e-6f;

// The bend at `arc` along `c`: the turn of its heading over the chord `half` either side, per metre (a curvature, 1/m).
float bend_at(const Conn &c, float arc, float half) {
	const float at = clampf(arc, half, c.len - half);
	Pose a, b;
	conn_pose(c, at - half, a);
	conn_pose(c, at + half, b);
	return std::fabs(std::atan2(a.fx * b.fz - a.fz * b.fx, a.fx * b.fx + a.fz * b.fz)) / (2 * half);
}

// The share of the comfortable lateral acceleration this driver takes: their temper, and less in a heavy vehicle.
float lateral_share(const TrafficParams &p, const Vehicle &v) {
	const bool heavy = v.type == VehicleType::BUS || v.type == VehicleType::TRUCK;
	return (1 + p.lateral_aggression_spread * v.aggression) * (heavy ? p.lateral_heavy_share : 1.f);
}

// The fastest a bend of curvature `k` is taken: the lateral acceleration accepted falls with speed (a0 - fall v), so k v^2 equals
// it; never under the floor's speed.
float bend_speed(const TrafficParams &p, float share, float k) {
	const float a0 = share * p.lateral_accel_max, fall = share * p.lateral_accel_fall;
	const float v = (-fall + std::sqrt(fall * fall + 4 * k * a0)) / (2 * k);
	return std::max(v, std::sqrt(share * p.lateral_accel_min / k));
}

} // namespace

float curve_cap(const CityState &s, const Vehicle &v, const Conn &c, float front) {
	const TrafficParams &p = s.catalog.traffic;
	const float half = std::min(p.curve_chord_m, c.len * 0.5f);
	if (half <= LEAST_CHORD_M) return NO_LEADER;
	const VehicleClass d = driver_of(s, v);
	const float brake = std::max(d.b, LEAST_BRAKE), share = lateral_share(p, v);
	const float horizon = v.v * v.v / (2 * brake) + v.v * p.curve_look_s + p.curve_look_m;
	// the brake comes off as a ramp at the driver's jerk, not at once: braking down to a bend, then the ramp easing off
	const float ease_t = brake / p.jerk_up, eased = brake * brake / (2 * p.jerk_up);
	const float middle = front - p.classes[int(v.type)].length * 0.5f; // the body is on a bend until its middle has passed it
	float cap = NO_LEADER;
	for (float arc = std::floor(std::max(0.f, middle) / p.curve_sample_m) * p.curve_sample_m; arc <= c.len; arc += p.curve_sample_m) {
		const float ahead = std::max(0.f, arc - front);
		if (ahead > horizon) break;
		const float k = bend_at(c, arc, half);
		if (k < STRAIGHT) continue;
		const float safe = bend_speed(p, share, k), top = safe + eased;
		const float ramp = std::max(LEAST_RAMP_M, top * ease_t - brake * brake * brake / (3 * p.jerk_up * p.jerk_up));
		cap = std::min(cap, ahead <= ramp ? safe + eased * (ahead / ramp) : std::sqrt(top * top + 2 * brake * (ahead - ramp)));
	}
	return cap;
}

float turn_limit(const CityState &s, const Vehicle &v, const Conn &c, float dist) { return curve_cap(s, v, c, -dist); }

float desired_speed(const CityState &s, const Vehicle &v, int ds) {
	const DirSeg &d = s.traffic.g.ds[ds];
	return top_speed(s, v, d.speed) * (1 + v.drift_amp * std::sin(v.drift + float(s.traffic.time) * v.drift_rate));
}

namespace {
// the IDM's free-road exponent (Treiber, Hennecke and Helbing 2000: 4, so the code squares twice); the desired speed is divided by,
// so it is never taken under this
constexpr float IDM_DELTA = 4.f, MIN_V0 = 0.5f;
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
	float z = s_star / std::max(gap, IDM_LEAST_GAP_M);
	if (v <= v0) return z >= 1 ? c.a * (1 - z * z) : (free > 0 ? free * (1 - std::pow(z, 2 * c.a / free)) : free);
	return z >= 1 ? free + c.a * (1 - z * z) : free;
}

} // namespace traffic
} // namespace city
