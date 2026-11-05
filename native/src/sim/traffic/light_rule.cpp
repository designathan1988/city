// The lights for vehicles: one rule for admission, commitments and braking (red holds; amber holds whoever should stop,
// a decision held until green), and the stop each vehicle sees along its route, past road joints and junctions, as a
// driver sees the signals down the street.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

bool stop_is_comfortable(const CityState &s, const Vehicle &v, float stop_m) {
	return stop_m >= 0 && v.v * v.v <= 2 * s.catalog.traffic.classes[int(v.type)].b * stop_m;
}

// How far a vehicle needs to stop the way the intergreen is timed (CONTRAN MBST vol. V 5.7.1): reacting in
// signal_reaction_s, then braking at signal_decel.
float amber_reach(const TrafficParams &p, float v) { return v * p.signal_reaction_s + v * v / (2 * p.signal_decel); }

namespace {

// Seconds to a line `line_m` ahead: on at `v`, braking at `b` as late as it can to take the line at `top` (never above
// `v`), the way a driver slows for a turn; too close for that, it brakes all the way.
float time_to_line(float v, float top, float b, float line_m) {
	const float slowing_m = (v * v - top * top) / (2 * b);
	if (slowing_m > line_m) return (v - std::sqrt(v * v - 2 * b * line_m)) / b;
	return (line_m - slowing_m) / v + (v - top) / b;
}

} // namespace

// The driver stops at the amber unless the vehicle cannot be brought to rest safely (MBST vol. V table 3.1). Whoever
// can stop as the intergreen is timed stops. The others are what the amber is there for (5.7.1), but it only covers a
// vehicle reaching the line before it ends: timed for the regulated speed held from the amber's first instant, it says
// nothing of one that gets here late in it, or slower (moving off, slowing down, just out of the far model), so that
// one must stop too, as hard as it takes, while its emergency brake can still do it.
bool amber_stops(const TrafficParams &p, const AmberApproach &a) {
	if (a.stop_m < 0) return false;
	if (a.stop_m >= amber_reach(p, a.v)) return true;
	if (a.v * a.v > 2 * a.brake_hard * a.stop_m) return false;
	return time_to_line(a.v, std::min(a.v, a.top), a.brake, a.stop_m + p.stop_margin_m) > a.left;
}

bool stops_at_amber(const CityState &s, const Vehicle &v, int ds, float stop_m, int ci) {
	const VehicleClass &vc = s.catalog.traffic.classes[int(v.type)];
	AmberApproach a;
	a.v = v.v;
	a.stop_m = stop_m;
	a.top = ci >= 0 ? turn_limit(s, v, s.traffic.g.conns[size_t(ci)], s.traffic.g.ds[size_t(ds)].stop_back) : v.v;
	a.brake = vc.b;
	a.brake_hard = vc.b_emergency;
	a.left = amber_left(s, ds);
	return amber_stops(s.catalog.traffic, a);
}

bool light_stops(const CityState &s, const Vehicle &v, int ds, float stop_m, int ci) {
	switch (light_for(s, ds)) {
		case Light::RED: return true;
		case Light::AMBER: return v.amber_hold == ds || stops_at_amber(s, v, ds, stop_m, ci);
		default: return false;
	}
}

LightStop lights_ahead(const CityState &s, const Vehicle &v, int ri, int lane, float start) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const VehicleClass &vc = p.classes[int(v.type)];
	// a standing obstacle starts to matter at the IDM's desired gap to it; never nearer than a head looks at its junction,
	// nor than an amber's stopping distance (each light is seen while it can still stop for it)
	const float horizon = std::max({ p.look_ahead_m, vc.min_gap + v.v * vc.headway + v.v * v.v / (2 * std::sqrt(vc.a * vc.b)),
			amber_reach(p, v.v) + p.stop_margin_m });
	LightStop out;
	float back = NO_LEADER; // the previous lane's stop while it can still stop there, and whether that is its own lane
	bool back_own = false;
	for (int k = ri, ln = lane; k + 1 < v.route.len; k++) {
		const int ds = route_at(t.route_pool, v.route, k);
		const DirSeg &d = t.g.ds[size_t(ds)];
		const bool own = k == ri && start <= 0;
		const float room = d.len - d.stop_back - p.stop_margin_m, stop = start + room;
		if (stop > horizon) break;
		const int ci = connector_to(s, ds, ln, route_at(t.route_pool, v.route, k + 1));
		// its own line once over it or committed there is the gate's business
		if (t.g.junctions[size_t(d.to)].ctl == Control::SIGNAL && !(own && (stop + p.stop_margin_m < 0 || v.pass >= 0))) {
			const Light l = light_for(s, ds);
			if (l != Light::GREEN && v.amber_hold == ds) out.amber_hold = ds;
			if (light_stops(s, v, ds, stop, ci)) {
				if (l == Light::AMBER) out.amber_hold = ds;
				// a line too close past a junction for the body: the stop moves back to that junction's own line
				const bool behind = room < vc.length && back < NO_LEADER;
				out.gap = behind ? back : std::max(0.f, stop);
				out.here = own || (behind && back_own);
				break;
			}
		}
		if (ci < 0) break;
		const bool can_back = !own || (stop >= 0 && (v.pass < 0 || stop_is_comfortable(s, v, stop)));
		back = can_back ? stop : NO_LEADER;
		back_own = own;
		start += d.len + t.g.conns[size_t(ci)].len;
		ln = t.g.conns[size_t(ci)].to_lane;
	}
	return out;
}

} // namespace traffic
} // namespace city
