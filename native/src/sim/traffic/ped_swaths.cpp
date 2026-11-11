// The swaths of the vehicles off the lanes as the walkers meet them (swaths.h files them): solid. The crowd step shows a walker
// the discs near it as bodies it never walks into nor squeezes through; one that stands on a disc walks on out of it and steps
// away from it (ped_stripes.cpp); somebody waiting at a kerb walks off it too, and nobody is born onto it. So a car that waits
// for the walkers to leave its way finds it clear and starts on it.
#include "sim/traffic/ped_stripes.h"

#include "sim/traffic/swaths.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

int swath_bodies(const TrafficState &t, const V2 &at, const V2 &fwd, const V2 &left, const WayFrame &f, float look, float body, Body *out, int n) {
	const ParkingState &pk = t.park;
	for (const Swath &w : pk.swaths) {
		if (!near_swath(w, at.x, at.y, look + body)) continue;
		for (int k = w.first; k < w.first + w.count && n < MAX_BODIES; k++) {
			const SwathDisc &d = pk.swath_discs[size_t(k)];
			const V2 rel(d.x - at.x, d.z - at.y);
			const float along = rel.dot(fwd), lat = f.u + rel.dot(left), reach = d.r + body;
			if (along < -reach || along - reach > look || lat + reach < f.lo || lat - reach > f.hi) continue;
			Body &b = out[n++];
			b = Body();
			b.along_lo = b.along_hi = along;
			b.lat_lo = b.lat_hi = lat;
			b.reach = reach;
			b.furniture = b.swath = true;
			b.under = std::hypot(along, lat - f.u) < reach; // it stands on this one: it walks on out of it and steps away
		}
	}
	return n;
}

bool on_a_swath(const TrafficState &t, float x, float z, float body) {
	const ParkingState &pk = t.park;
	for (const Swath &w : pk.swaths) {
		if (!near_swath(w, x, z, body)) continue;
		for (int k = w.first; k < w.first + w.count; k++) {
			const SwathDisc &d = pk.swath_discs[size_t(k)];
			if (std::hypot(x - d.x, z - d.z) < d.r + body) return true;
		}
	}
	return false;
}

void step_off_swath(const CityState &s, Pedestrian &w, float dt) {
	const ParkingState &pk = s.traffic.park;
	const TrafficParams &p = s.catalog.traffic;
	const float step = w.speed * dt; // it walks off it
	for (const Swath &sw : pk.swaths) {
		if (!near_swath(sw, w.cur.x, w.cur.z, p.ped_body_m)) continue;
		for (int k = sw.first; k < sw.first + sw.count; k++) {
			const SwathDisc &d = pk.swath_discs[size_t(k)];
			const V2 off(w.cur.x - d.x, w.cur.z - d.z);
			const float far = off.len();
			if (far >= d.r + p.ped_body_m) continue;
			const V2 away = far > step ? off * (1.f / far) : V2(-w.way_z, w.way_x); // on its middle: to its own side
			w.cur.x += away.x * step;
			w.cur.z += away.y * step;
			w.walked += step;
			return;
		}
	}
}

} // namespace traffic
} // namespace city
