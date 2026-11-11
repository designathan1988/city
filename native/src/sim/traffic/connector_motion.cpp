// The vehicles crossing a node on a connector: each drives against the one ahead (on the connector or at the tail of the lane it
// enters), the walkers on the exit crosswalk and the lights down its route, and at the connector's end it is handed to that lane
// (its body reaching back over the connector until it has driven its length, vehicle_pose.cpp). box_hold says what holds one.
#include "sim/traffic/motion_law.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {
constexpr float MOVING_OFF_SHARE = 0.5f; // box_hold: obstacles that leave it this share of its free-road pickup do not hold it

// What the k-th vehicle on connector `ci` drives against, at its top speed `top` there: the one ahead (on the connector
// or the exit lane's tail) and walkers on the exit crosswalk, a stop at the connector's end (the entry one is behind it).
void connector_leads(const CityState &s, int ci, size_t k, float top, Lead &car, Lead &walkers) {
	const TrafficState &t = s.traffic;
	const auto &list = t.conn_vehicles[ci];
	const Conn &c = t.g.conns[ci];
	const Vehicle &v = t.vehicles[list[k]];
	if (k > 0) {
		const Vehicle &o = t.vehicles[list[k - 1]];
		car.take(o.cs - body_length(s, o) - v.cs, o.v);
	} else {
		const auto &lane = t.lane_vehicles[size_t(c.to_ds) * MAX_LANES + c.to_lane];
		if (!lane.empty()) {
			const Vehicle &o = t.vehicles[lane.front()];
			car.take(c.len - v.cs + o.s - body_length(s, o), o.v);
		}
	}
	if (v.cs < c.len && peds_in_way(s, ci, FRONT_PAST, front_time(s, v, c.len - v.cs, top))) walkers.take(c.len - v.cs, 0.f);
}
} // namespace

void step_connector(CityState &s, int ci, float dt) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	auto &list = t.conn_vehicles[ci];
	const Conn &c = t.g.conns[ci];
	const float vt = conn_speed(s, c);
	for (size_t k = 0; k < list.size(); k++) {
		Vehicle &v = t.vehicles[list[k]];
		if (v.moved_tick == t.tick) continue; // came over its lane's end this tick, already moved by the lane's step
		Lead l, walkers;
		const float top = top_speed(s, v, vt);
		connector_leads(s, ci, k, top, l, walkers);
		l.take(walkers.gap, walkers.v);
		const LightStop lights = lights_ahead(s, v, v.ri + 1, c.to_lane, c.len - v.cs);
		v.amber_hold = lights.amber_hold;
		l.take(lights.gap, 0.f);
		float acc = finish_accel(s, v, idm_accel(p.classes[int(v.type)], v.v, top, l.gap, l.v), l, dt);
		integrate(s, v, acc, dt, v.cs);
	}
	while (!list.empty() && t.vehicles[size_t(list.front())].cs >= c.len) {
		const int id = list.front();
		conn_pop_front(s, ci);
		Vehicle &v = t.vehicles[size_t(id)];
		v.conn = -1;
		v.ri++;
		count_entry(t, v.ds = c.to_ds);
		v.lane = c.to_lane;
		v.s = v.cs - c.len;
		lane_add(s, v.ds * MAX_LANES + v.lane, id);
		v.came = ci; // its body reaches back onto the connector until it has driven its length (vehicle_pose.cpp)
	}
}

BoxHold box_hold(const CityState &s, int id) {
	const TrafficState &t = s.traffic;
	const Vehicle &v = t.vehicles[size_t(id)];
	const auto &on = t.conn_vehicles[size_t(v.conn)];
	const size_t k = size_t(std::find(on.begin(), on.end(), id) - on.begin());
	if (k >= on.size()) return BoxHold::STARTING;
	const VehicleClass &vc = s.catalog.traffic.classes[int(v.type)];
	const Conn &c = t.g.conns[size_t(v.conn)];
	const float top = top_speed(s, v, conn_speed(s, c));
	Lead car, walkers;
	connector_leads(s, v.conn, k, top, car, walkers);
	const float light = lights_ahead(s, v, v.ri + 1, c.to_lane, c.len - v.cs).gap;
	// what takes most of its free-road acceleration holds it (creeping up to a standing leader is being held by it)
	const float by[int(BoxHold::STARTING)] = { idm_accel(vc, v.v, top, car.gap, car.v), idm_accel(vc, v.v, top, walkers.gap, 0.f),
		idm_accel(vc, v.v, top, light, 0.f) };
	const int most = int(std::min_element(by, by + int(BoxHold::STARTING)) - by);
	return by[most] > MOVING_OFF_SHARE * idm_accel(vc, v.v, top, NO_LEADER, 0.f) ? BoxHold::STARTING : BoxHold(most);
}

} // namespace traffic
} // namespace city
