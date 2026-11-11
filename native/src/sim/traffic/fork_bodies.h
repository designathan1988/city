// Who still stands where the ways of a lane's fork part (Conn::forks): a body that took another branch out of the same lane, on that
// branch's connector or come off it onto its lane with its body still reaching back over it (vehicle_pose.cpp), whose middle has not
// passed that branch's clear arc for the two bodies' classes (Conn::fork_clear, junction_sweeps.cpp). The one owner of that test: the
// junction's admission refuses a commitment while such a body stands there (box_clear, jam_release), and the motion law follows it
// (fork_leads).
#pragma once

#include "sim/traffic/motion_law.h"

namespace city {
namespace traffic {

// Calls `take(u, front)` for each body on another branch of connector `c`'s fork not yet clear of a body of class `mine` on `c`;
// `front` is how far the body's front is along its own way, which starts where `c`'s starts.
template <typename F>
void fork_bodies(const CityState &s, const Conn &c, int mine, F &&take) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	for (size_t k = 0; k < c.forks.size(); k++) {
		const int branch = c.forks[k];
		const Conn &o = t.g.conns[size_t(branch)];
		const auto standing = [&](const Vehicle &u, float front) {
			if (front - body_length(s, u) * 0.5f < c.fork_clear[k][size_t(body_class(p, u.type) * BODY_CLASSES + mine)]) take(u, front);
		};
		for (int id : t.conn_vehicles[size_t(branch)]) standing(t.vehicles[size_t(id)], t.vehicles[size_t(id)].cs);
		for (int id : t.lane_vehicles[size_t(o.to_ds) * MAX_LANES + o.to_lane])
			if (t.vehicles[size_t(id)].came == branch) standing(t.vehicles[size_t(id)], o.len + t.vehicles[size_t(id)].s);
	}
}

} // namespace traffic
} // namespace city
