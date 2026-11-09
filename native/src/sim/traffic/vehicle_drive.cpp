// How hard a driver presses the pedals this step: each obstacle driven against by the adaptive cruise control model (the improved
// IDM blended with the constant-acceleration heuristic, Treiber and Kesting, Traffic Flow Dynamics, 2013, 11.3), the pedals moved at
// the driver's jerk, and never faster than the vehicle can still stop short of any of them (the lab's sim/drive/operational.ts and
// physicalMotion.ts).
#include "sim/traffic/vehicle_drive.h"

#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float CAH_LEAST_DEN = 1e-6f; // m²/s²: the heuristic's denominator is never taken under this (it divides)

// The constant-acceleration heuristic against `o` (Kesting, Treiber and Helbing 2010, eq. 6): the braking needed if what is ahead keeps
// its acceleration, read as a driver can, never harder than an emergency stop.
float cah_accel(const VehicleClass &d, float v, const Lead::Seen &o) {
	const float al = clampf(o.acc, -d.b_emergency, d.a), vl = o.v, gap = std::max(o.gap, IDM_LEAST_GAP_M);
	if (vl * (v - vl) <= -2 * gap * al) return v * v * al / std::max(vl * vl - 2 * gap * al, CAH_LEAST_DEN);
	const float closing = std::max(0.f, v - vl);
	return al - closing * closing / (2 * gap);
}

// The adaptive cruise control acceleration against `o`: the improved IDM where it asks no harder braking than the heuristic, else
// the blend that keeps calm in a gap that is closing but not dangerous (Treiber and Kesting, 11.3.7, coolness acc_coolness).
float acc_accel(const VehicleClass &d, float v, float v0, const Lead::Seen &o, const TrafficParams &p) {
	const float iidm = idm_accel(d, v, v0, o.gap, o.v), cah = cah_accel(d, v, o);
	if (iidm >= cah) return iidm;
	return (1 - p.acc_coolness) * iidm + p.acc_coolness * (cah + d.b * std::tanh((iidm - cah) / d.b));
}

// The highest speed from which `d` still stops `pad` short of `o` braking at its emergency rate (the safe speed of Krauss 1998).
float safe_speed(const VehicleClass &d, const Lead::Seen &o, float pad, float dt) {
	const float be = d.b_emergency, room = std::max(0.f, o.gap - pad + (o.v > 0 ? o.v * o.v / (2 * be) : 0.f));
	return std::max(0.f, -be * dt + std::sqrt(be * be * dt * dt + 2 * be * room));
}

} // namespace

// The most every obstacle allows, the pedals moved from where the driver's foot was at its jerk (easing on at jerk_up, ordinary
// braking at jerk_down; braking past comfortable, and lifting off at a crawl, are not held back), a speed over the wanted one shed at
// the comfortable rate, and never over the speed it can still stop short of each obstacle from.
float drive_accel(const CityState &s, const Vehicle &v, float v0, const Lead &l, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	const VehicleClass d = driver_of(s, v);
	float target = idm_accel(d, v.v, v0, NO_LEADER, 0.f);
	for (int i = 0; i < l.count; i++) target = std::min(target, acc_accel(d, v.v, v0, l.seen[i], p));
	target = clampf(target, -d.b_emergency, d.a);
	const float foot = clampf(v.acc, -d.b_emergency, d.a);
	const float a = target < -d.b || (v.v < p.crawl_ms && target < foot) ? std::min(target, foot + p.jerk_up * dt)
			: clampf(target, foot - p.jerk_down * dt, foot + p.jerk_up * dt);
	float next = clampf(v.v + a * dt, 0.f, std::max(0.f, std::max(v0, v.v - d.b * dt)));
	for (int i = 0; i < l.count; i++) next = std::min(next, safe_speed(d, l.seen[i], p.vehicle_safe_pad_m, dt));
	return (next - v.v) / dt;
}

} // namespace traffic
} // namespace city
