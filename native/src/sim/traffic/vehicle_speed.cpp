// How fast a vehicle goes: its share of the limit (a bicycle at its own pace) with the slow drift of its speed; the speed its
// driver takes a bend at, on a connector and where a street curves, and the braking down to it; and the following law: the
// improved IDM, and against a leader its blend with the constant-acceleration heuristic (Kesting, Treiber, Helbing 2010,
// "Enhanced Intelligent Driver Model to access the impact of driving strategies on traffic capacity").
#include "sim/traffic/driver.h"
#include "sim/traffic/motion_law.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr float DRIFT_RATE = 0.1f;
constexpr float LEAST_RADIUS_SHARE = 0.25f; // a lane on the inside of a bend keeps at least this share of the centre line's radius
} // namespace

float top_speed(const CityState &s, const Vehicle &v, float limit) {
	const float own = limit * v.vf;
	return v.type == VehicleType::BICYCLE ? std::min(own, s.catalog.traffic.bike_speed_kmh / KMH_PER_MS * v.vf) : own;
}

float bend_pace(const CityState &s, const Vehicle &v, float k) {
	const TrafficParams &p = s.catalog.traffic;
	k = std::fabs(k);
	if (k < p.bend_least) return NO_LEADER;
	const float share = (1 + p.bend_temper * v.temper) * (body_class(p, v.type) == BODY_CLASSES - 1 ? p.bend_heavy_share : 1.f);
	const float most = share * p.bend_lat_max, fall = share * p.bend_lat_fall;
	// v * v * k = most - fall * v: where the bend's lateral acceleration meets what the driver accepts at that speed
	const float met = (-fall + std::sqrt(fall * fall + 4 * k * most)) / (2 * k);
	return std::max(met, std::sqrt(share * p.bend_lat_min / k));
}

float conn_top(const CityState &s, const Vehicle &v, const Conn &c) {
	return std::min(top_speed(s, v, s.traffic.g.ds[size_t(c.to_ds)].speed), bend_pace(s, v, c.bend));
}

float turn_limit(const CityState &s, const Vehicle &v, const Conn &c, float dist) {
	const TrafficParams &p = s.catalog.traffic;
	const float vb = bend_pace(s, v, c.bend);
	if (vb >= NO_LEADER) return NO_LEADER;
	return std::sqrt(vb * vb + 2 * driver_of(p, v).b * std::max(0.f, dist - p.stop_margin_m));
}

float bend_ahead(const CityState &s, const Vehicle &v, int ds, int lane, float from) {
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	if (d.bend == 0 || from >= d.bend_to) return NO_LEADER;
	// the lane's own radius: the centre line's, less its offset toward the bend's centre (u is left of a -> b, the bend left of travel)
	const float toward = (d.bend > 0 ? 1.f : -1.f) * d.u[std::max(0, std::min(d.lanes - 1, lane))] * d.sign;
	const float k = std::fabs(d.bend) / std::max(LEAST_RADIUS_SHARE, 1 - std::fabs(d.bend) * toward);
	const float vb = bend_pace(s, v, k);
	if (vb >= NO_LEADER) return NO_LEADER;
	return std::sqrt(vb * vb + 2 * driver_of(s.catalog.traffic, v).b * std::max(0.f, d.bend_from - from));
}

float held_to(float acc, float v, float cap, float b, float dt) {
	return cap >= NO_LEADER ? acc : std::min(acc, std::max(-b, (cap - v) / dt));
}

float desired_speed(const CityState &s, const Vehicle &v, int ds) {
	const DirSeg &d = s.traffic.g.ds[ds];
	float drift = s.catalog.traffic.speed_drift * std::sin(v.drift + float(s.traffic.time) * DRIFT_RATE);
	return top_speed(s, v, d.speed) * (1 + drift);
}

namespace {
constexpr float IDM_DELTA = 4.f, MIN_V0 = 0.5f, MIN_GAP = 0.02f, STANDING = 1e-3f, LEAST = 1e-6f;
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

float follow_accel(const VehicleClass &c, float coolness, float v, float v0, float gap, float vl, float al) {
	const float own = idm_accel(c, v, v0, gap, vl);
	if (gap >= NO_LEADER) return own;
	// what it must do if the one ahead keeps its acceleration, as a driver reads it (never harder than an emergency stop): a
	// leader that will have stopped before it is reached, else one closed on while both move; something standing is the second
	al = clampf(al, -c.b_emergency, c.a);
	const float room = std::max(gap, MIN_GAP);
	float kept;
	if (vl > STANDING && vl * (v - vl) <= -2 * room * al) {
		kept = v * v * al / std::max(vl * vl - 2 * room * al, LEAST);
	} else {
		const float closing = std::max(0.f, v - vl);
		kept = al - closing * closing / (2 * room);
	}
	if (own >= kept) return own;
	return (1 - coolness) * own + coolness * (kept + c.b * std::tanh((own - kept) / c.b));
}

} // namespace traffic
} // namespace city
