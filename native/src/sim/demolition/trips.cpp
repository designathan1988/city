// The walkers a fright tripped go down when their trip comes (ped_fright.cpp sets trip_at and trip_s): forwards whatever their pace
// (Smeesters, Hayes, McMahon 2001, J. Biomech. 34:309-317), each as a body of its own carried on by its run (a CASUALTY knocked
// down: effect_bodies.cpp throws it from a point behind it at ped_trip_throw of the effects' push), and up again after trip_s. Run by
// the damage clock under the exclusive map lock, as the bleeding (wounds.cpp).
#include "sim/demolition/damage_event.h"
#include "sim/demolition/damage_state.h"
#include "sim/state/city_state.h"
#include "sim/traffic/traffic_system.h"

#include <cmath>

namespace city {

namespace {
constexpr float TRIP_BEHIND_M = 1.f; // the push a trip's fall is drawn with comes from this far behind the walker
} // namespace

void trip_step(CityState &s) {
	TrafficState &t = s.traffic;
	const float now = float(t.time);
	for (int id = 0; id < int(t.peds.size()); id++) {
		Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive || w.trip_at <= 0 || now < w.trip_at) continue;
		w.trip_at = 0;
		if (traffic::lying(t, w)) continue; // already on the ground (a blow, a round): the trip is over before it came
		DamageEvent e = walker_casualty(w, id, CAS_KNOCKED);
		e.b = w.cur.x - w.cur.fx * TRIP_BEHIND_M; // pushed from behind: forwards
		e.c = w.cur.z - w.cur.fz * TRIP_BEHIND_M;
		e.d = s.catalog.traffic.ped_trip_throw;
		e.dx = w.trip_s;
		float fx = 0, fz = 0;
		fall_way(e, fx, fz); // forwards, as the render pitches the body
		traffic::lay_down(t, w, w.trip_s, fx, fz);
		s.log.damage_events.push_back(e);
	}
}

} // namespace city
