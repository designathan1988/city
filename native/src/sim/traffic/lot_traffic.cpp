// The cars that use the lots' car gates: a passing car is called to a free bay, stops on its lane before the gate, leaves
// the road, drives in, stands its stay, and when the fleet runs short drives out and joins the traffic again. Who is sent in and
// out is lot_demand.cpp. Off the lanes a car is a Vehicle
// with `off` set: on no lane list, out of the fleet count, its pose the manoeuvre's.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr int DISCS = 3;

ManoeuvreSpeeds speeds(const TrafficParams &p) { return { p.man_forward_ms, p.man_backward_ms, p.man_ramp_m, p.man_creep_ms }; }

void remove_car(ParkingState &p, TrafficState &t, int index) {
	const int last = int(p.cars.size()) - 1;
	if (index != last) {
		p.cars[size_t(index)] = std::move(p.cars[size_t(last)]);
		t.vehicles[size_t(p.cars[size_t(index)].id)].car = index;
	}
	p.cars.pop_back();
}

// Takes the car out of the lots' list and lets its bay go.
void untrack(CityState &s, LotCar &car) {
	TrafficState &t = s.traffic;
	ParkingState &p = t.park;
	if (car.phase == CarPhase::COMING) p.coming--;
	if (car.phase == CarPhase::PARKING || car.phase == CarPhase::LEAVING) p.moving--;
	Bay &bay = p.bays[size_t(car.bay)];
	bay.holder = -1;
	refile_bay(s, car.bay, false);
	const int index = t.vehicles[size_t(car.id)].car;
	t.vehicles[size_t(car.id)].car = -1;
	remove_car(p, t, index);
}

// Whether nobody stands where the car will go over the rest of its way (its body as discs every metre): after
// parker_ask_way_s it goes on, the walkers stepping out of its way, and the ground is its own from then on.
bool may_go(CityState &s, LotCar &car, float dt) {
	if (car.reserved) return true;
	const TrafficParams &p = s.catalog.traffic;
	const VehicleClass &c = p.classes[int(s.traffic.vehicles[size_t(car.id)].type)];
	const float reach = std::max(0.f, c.length * 0.5f - c.half_width), r = c.half_width + p.man_disc_pad_m;
	bool clear = true;
	for (float at = car.path.s; clear; at = std::min(car.path.length(), at + p.man_disc_step_m)) {
		const OffPose q = car.path.pose_at(at);
		const V2 f = v2_from_angle(q.angle) * reach;
		for (int k = 0; k < DISCS && clear; k++) clear = !walker_within(s, q.x + f.x * float(k - 1), q.z + f.y * float(k - 1), r);
		if (at >= car.path.length()) break;
	}
	if (clear || car.waited >= p.parker_ask_way_s) {
		car.reserved = true;
		return true;
	}
	car.waited += dt;
	return false;
}

// Room in the traffic for a car joining `lane` at its joining point.
bool gap_open(const CityState &s, const Vehicle &v, const LaneSpot &lane) {
	const TrafficParams &p = s.catalog.traffic;
	const float len = p.classes[int(v.type)].length, front = lane.at + len * 0.5f;
	for (int id : s.traffic.lane_vehicles[size_t(lane.ds) * MAX_LANES + lane.lane]) {
		const Vehicle &o = s.traffic.vehicles[size_t(id)];
		const float rear = o.s - p.classes[int(o.type)].length;
		if (o.s > front - len - p.parker_gap_behind_m && rear < front + p.parker_gap_ahead_m) return false;
	}
	return true;
}

void enter_phase(ParkingState &p, LotCar &car, CarPhase phase) {
	if (car.phase == CarPhase::COMING) p.coming--;
	if (car.phase == CarPhase::PARKING || car.phase == CarPhase::LEAVING) p.moving--;
	if (phase == CarPhase::PARKING || phase == CarPhase::LEAVING) p.moving++;
	car.phase = phase;
	car.t = car.waited = 0;
	car.reserved = false;
}

// Stopped before its gate: off the road, along the way in.
void leave_road(CityState &s, LotCar &car) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = t.vehicles[size_t(car.id)];
	const Bay &bay = t.park.bays[size_t(car.bay)];
	Pose at;
	lane_pose(s, v.ds, v.lane, v.s - p.classes[int(v.type)].length * 0.5f, at);
	lane_del(s, v.ds * MAX_LANES + v.lane, car.id);
	micro_remove(t, car.id);
	v.ds = -1;
	v.off = true;
	v.v = 0;
	t.vehicle_count--;
	car.path = bay_arrival(V2(at.x, at.z), std::atan2(at.fz, at.fx), bay.at, bay.open, bay.via, p.man_lead_m, p.man_across_m, p.man_least_m, p.man_tangent);
	enter_phase(t.park, car, CarPhase::PARKING);
	set_off_pose(s, v, car.path.pose());
}

// A car sent to a bay that will not reach it (kept out of its lane, called away by the far model): on to somewhere else.
void give_up(CityState &s, LotCar &car) {
	const int id = car.id;
	Vehicle &v = s.traffic.vehicles[size_t(id)];
	v.stop_at = -1;
	untrack(s, car);
	if (v.pass >= 0) release(s, id, v.pass);
	if (v.ds < 0 || !new_trip(s, id, v.ds)) v.goal_s = 0; // on a connector, or nowhere to go: its trip ends on the lane it reaches
}

void coming(CityState &s, LotCar &car) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = t.vehicles[size_t(car.id)];
	const LaneSpot &lane = t.park.bays[size_t(car.bay)].lane;
	if (!v.micro || car.t > p.parker_call_timeout_s || v.stuck > p.parker_give_up_s) {
		give_up(s, car);
		return;
	}
	if (v.ds != lane.ds || v.conn >= 0 || v.lane != lane.lane || v.v > p.parker_stop_ms || v.s < v.stop_at - p.parker_stop_slack_m) return;
	v.stop_at = -1;
	leave_road(s, car);
}

void parking(CityState &s, LotCar &car, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = s.traffic.vehicles[size_t(car.id)];
	if (!may_go(s, car, dt)) { v.v = 0; return; }
	const float top = car.path.speed(speeds(p));
	car.path.advance(top * dt);
	v.v = car.path.done() ? 0 : top;
	set_off_pose(s, v, car.path.pose());
	if (!car.path.done()) return;
	ParkingState &pk = s.traffic.park;
	pk.parked_in++;
	car.until = s.traffic.time + double(s.traffic.rng.range(p.parker_stay_min_s, p.parker_stay_max_s));
	enter_phase(pk, car, CarPhase::PARKED);
}

// On the lane past its gate: the same car is a vehicle of the traffic again, bound somewhere else.
void join_lane(CityState &s, LotCar &car) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const int id = car.id;
	const LaneSpot lane = t.park.bays[size_t(car.bay)].lane;
	t.park.left_by++;
	untrack(s, car);
	Vehicle &v = t.vehicles[size_t(id)];
	v.off = false;
	t.vehicle_count++;
	if (!t.g.ds[size_t(lane.ds)].ok || lane.lane >= t.g.ds[size_t(lane.ds)].lanes) { despawn_vehicle(s, id, false); return; }
	v.ds = lane.ds;
	v.lane = lane.lane;
	v.s = std::min(t.g.ds[size_t(lane.ds)].len, lane.at + p.classes[int(v.type)].length * 0.5f);
	v.v = p.parker_join_ms;
	micro_add(t, id);
	lane_add(s, lane.ds * MAX_LANES + lane.lane, id);
	if (!new_trip(s, id, lane.ds)) despawn_vehicle(s, id, true);
}

void leaving(CityState &s, LotCar &car, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = s.traffic.vehicles[size_t(car.id)];
	const Bay &bay = s.traffic.park.bays[size_t(car.bay)];
	const bool at_kerb = car.path.length() - car.path.s <= p.parker_lane_edge_m;
	if (!may_go(s, car, dt) || (at_kerb && !gap_open(s, v, bay.lane))) { v.v = 0; return; }
	const float top = car.path.speed(speeds(p));
	car.path.advance(top * dt);
	v.v = top;
	set_off_pose(s, v, car.path.pose());
	if (car.path.done()) join_lane(s, car);
}

void parked(CityState &s, LotCar &car) {
	const TrafficParams &p = s.catalog.traffic;
	TrafficState &t = s.traffic;
	if (t.time >= car.until) car.ripe = !car.go;
	if (!car.go || t.time < car.until) return;
	const Bay &bay = t.park.bays[size_t(car.bay)];
	car.path = bay_departure(bay.at, bay.open, bay.via, bay.lane.p, bay.lane.t, p.man_lead_m, p.man_across_m, p.man_least_m, p.man_tangent);
	enter_phase(t.park, car, CarPhase::LEAVING);
}

} // namespace

void set_off_pose(CityState &s, Vehicle &v, const OffPose &q) {
	v.cur.x = q.x;
	v.cur.z = q.z;
	v.cur.y = s.terrain.height_at(q.x, q.z) + s.catalog.traffic.body_lift_m;
	v.cur.fx = std::cos(q.angle);
	v.cur.fz = std::sin(q.angle);
}


bool lot_car_at_rest(const CityState &s, int id) {
	const Vehicle &v = s.traffic.vehicles[size_t(id)];
	return v.car >= 0 && s.traffic.park.cars[size_t(v.car)].phase == CarPhase::PARKED && !s.traffic.park.cars[size_t(v.car)].go;
}

void lot_car_gone(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	if (v.car < 0) return;
	if (!v.off) {
		give_up(s, t.park.cars[size_t(v.car)]);
		return;
	}
	untrack(s, t.park.cars[size_t(v.car)]);
	free_vehicle(t, id);
}

void lot_car_forget(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	if (v.car >= 0) untrack(s, t.park.cars[size_t(v.car)]);
}

void step_lots(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	ParkingState &pk = t.park;
	const TrafficParams &p = s.catalog.traffic;
	for (int i = int(pk.cars.size()) - 1; i >= 0; i--) {
		LotCar &car = pk.cars[size_t(i)];
		car.t += dt;
		switch (car.phase) {
			case CarPhase::COMING: coming(s, car); break;
			case CarPhase::PARKING: parking(s, car, dt); break;
			case CarPhase::PARKED: parked(s, car); break;
			case CarPhase::LEAVING: leaving(s, car, dt); break;
		}
	}
	pk.clock += dt;
	if (pk.clock < p.parker_look_s) return;
	pk.clock = 0;
	const double target = fleet_target(s);
	lot_depart(s, target);
	// trips ending in a lot at the rate a trip ends; as many on their way as that brings in while a called car drives there
	const float most = std::max(p.parker_coming_min, std::ceil(float(target) * p.parker_coming_s / p.parker_trip_s));
	pk.owed = std::min(most, pk.owed + float(target) / p.parker_trip_s * p.parker_look_s);
	while (pk.owed >= 1 && float(pk.coming) < most && lot_call(s)) pk.owed -= 1;
}

} // namespace traffic
} // namespace city
