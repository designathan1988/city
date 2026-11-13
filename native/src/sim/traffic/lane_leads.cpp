// What a vehicle on a lane drives against (motion_law.h): on its lane, whoever is ahead of its body there, and beyond its stop
// line, what the head of the lane sees on its way through the junction.
#include "sim/traffic/kerb_stops.h"

#include "sim/traffic/driver.h"
#include "sim/traffic/lane_closures.h"
#include "sim/traffic/motion_law.h"

namespace city {
namespace traffic {

void lead_beyond(const CityState &s, int ci, float dist, Lead &l) {
	const TrafficState &t = s.traffic;
	const Conn &c = t.g.conns[ci];
	const float shut = closure_ahead(t, c.to_ds * MAX_LANES + c.to_lane, 0.f);
	if (shut >= 0) l.take(dist + c.len + shut, 0.f);
	const auto &on = t.conn_vehicles[ci];
	if (!on.empty()) {
		const Vehicle &b = t.vehicles[on.back()];
		l.take(dist + b.cs - body_length(s, b), b.v, b.acc);
		return;
	}
	const auto &lane = t.lane_vehicles[size_t(c.to_ds) * MAX_LANES + c.to_lane];
	if (!lane.empty()) {
		const Vehicle &b = t.vehicles[lane.front()];
		l.take(dist + c.len + b.s - body_length(s, b), b.v, b.acc);
	}
}

bool lane_leads(const CityState &s, int ds, int lane, const std::vector<int> &list, int k, Lead &l) {
	const TrafficState &t = s.traffic;
	const int id = list[size_t(k)];
	const Vehicle &v = t.vehicles[size_t(id)];
	int ahead = k + 1; // a bicycle in its band is beside it, not ahead
	while (ahead < int(list.size()) && out_of_the_way(s, v, t.vehicles[size_t(list[size_t(ahead)])], ds)) ahead++;
	const bool head = ahead == int(list.size());
	if (!head) {
		const Vehicle &o = t.vehicles[size_t(list[size_t(ahead)])];
		l.take(o.s - body_length(s, o) - v.s, o.v, o.acc);
	}
	if (!t.closures.empty()) {
		const float shut = closure_ahead(t, ds * MAX_LANES + v.lane, v.s);
		if (shut >= 0) l.take(shut, 0.f);
	}
	for (int o : t.sliders) {
		const Vehicle &w = t.vehicles[size_t(o)];
		if (o != id && w.slid_from == ds * MAX_LANES + lane && w.s > v.s) l.take(w.s - body_length(s, w) - v.s, w.v, w.acc);
	}
	// its own body still stands over the lane it changed out of: it drives against whoever is ahead of it there too
	if (v.slid_from >= 0 && v.slid_from / MAX_LANES == ds && v.slid_from != ds * MAX_LANES + lane) {
		for (int o : t.lane_vehicles[size_t(v.slid_from)]) {
			const Vehicle &u = t.vehicles[size_t(o)];
			if (u.s > v.s) l.take(u.s - body_length(s, u) - v.s, u.v, u.acc);
		}
	}
	if (v.kerb >= 0) l.take(kerb_stop_gap(t, v) + driver_of(s.catalog.traffic, v).min_gap, 0.f); // the IDM stops its standing gap short
	return head;
}

} // namespace traffic
} // namespace city
