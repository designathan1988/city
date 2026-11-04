// The lights for vehicles: one rule for admission, commitments and braking (red holds; amber holds whoever can still
// stop at the line as the intergreen is timed, a decision held until green), and the stop each vehicle sees along its
// route, past road joints and junctions, as a driver sees the signals down the street.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

bool stop_is_comfortable(const CityState &s, const Vehicle &v, float stop_m) {
	return stop_m >= 0 && v.v * v.v <= 2 * s.catalog.traffic.classes[int(v.type)].b * stop_m;
}

// At the amber, stops whoever can stop at the line the way the intergreen is timed (CONTRAN MBST vol. V 5.7.1): reacting
// in signal_reaction_s, then braking at signal_decel; the others reach the line before the amber ends.
float amber_reach(const CityState &s, const Vehicle &v) {
	const TrafficParams &p = s.catalog.traffic;
	return v.v * p.signal_reaction_s + v.v * v.v / (2 * p.signal_decel);
}

bool stops_at_amber(const CityState &s, const Vehicle &v, float stop_m) { return stop_m >= 0 && stop_m >= amber_reach(s, v); }

bool light_stops(const CityState &s, const Vehicle &v, int ds, float stop_m) {
	switch (light_for(s, ds)) {
		case Light::RED: return true;
		case Light::AMBER: return v.amber_hold == ds || stops_at_amber(s, v, stop_m);
		default: return false;
	}
}

LightStop lights_ahead(const CityState &s, const Vehicle &v, int ri, int lane, float start) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const VehicleClass &vc = p.classes[int(v.type)];
	// a standing obstacle starts to matter at the IDM's desired gap to it; never nearer than a head looks at its junction,
	// nor than an amber's stopping distance (each light is seen when it turns amber, as the intergreen supposes)
	const float horizon = std::max({ p.look_ahead_m, vc.min_gap + v.v * vc.headway + v.v * v.v / (2 * std::sqrt(vc.a * vc.b)),
			amber_reach(s, v) + p.stop_margin_m });
	LightStop out;
	float back = NO_LEADER; // the previous lane's stop while it can still stop there, and whether that is its own lane
	bool back_own = false;
	for (int k = ri, ln = lane; k + 1 < v.route.len; k++) {
		const int ds = route_at(t.route_pool, v.route, k);
		const DirSeg &d = t.g.ds[size_t(ds)];
		const bool own = k == ri && start <= 0;
		const float room = d.len - d.stop_back - p.stop_margin_m, stop = start + room;
		if (stop > horizon) break;
		// its own line once over it or committed there is the gate's business
		if (t.g.junctions[size_t(d.to)].ctl == Control::SIGNAL && !(own && (stop + p.stop_margin_m < 0 || v.pass >= 0))) {
			const Light l = light_for(s, ds);
			if (l != Light::GREEN && v.amber_hold == ds) out.amber_hold = ds;
			if (light_stops(s, v, ds, stop)) {
				if (l == Light::AMBER) out.amber_hold = ds;
				// a line too close past a junction for the body: the stop moves back to that junction's own line
				const bool behind = room < vc.length && back < NO_LEADER;
				out.gap = behind ? back : std::max(0.f, stop);
				out.here = own || (behind && back_own);
				break;
			}
		}
		const int ci = connector_to(s, ds, ln, route_at(t.route_pool, v.route, k + 1));
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
