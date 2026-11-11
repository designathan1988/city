// The cars that use the lots' car gates: a passing car is called to a free bay, stops on its lane before the gate, leaves
// the road, drives in, stands its stay, and when the fleet runs short drives out and joins the traffic again. Who is sent in and
// out is lot_demand.cpp. Off the lanes a car is a Vehicle
// with `off` set: on no lane list, out of the fleet count, its pose the manoeuvre's. A lot's drive is one car's at a time
// (lot_busy): a car is sent out, or turns in, only while no other car of its lot drives off the lanes.
#include "sim/traffic/traffic_system.h"

#include "sim/traffic/lane_closures.h"

#include "core/laps.h"

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

// Takes the car out of the lots' list and lets its bay go, and the lane it kept closed.
void untrack(CityState &s, LotCar &car) {
	TrafficState &t = s.traffic;
	ParkingState &p = t.park;
	if (car.closing) open_lanes(t, car.id);
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
	const uint64_t every = uint64_t(p.parker_look_ticks); // it looks every so often, all together: the walkers are filed once for them
	if (s.traffic.tick % every != 0) return false;
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
	car.waited += dt * float(every);
	return false;
}

void enter_phase(ParkingState &p, LotCar &car, CarPhase phase) {
	if (car.phase == CarPhase::COMING) p.coming--;
	if (car.phase == CarPhase::PARKING || car.phase == CarPhase::LEAVING) p.moving--;
	if (phase == CarPhase::PARKING || phase == CarPhase::LEAVING) p.moving++;
	car.phase = phase;
	car.t = car.waited = 0;
	car.reserved = false;
}

// A car's body along its way from `from` to `to`, a pose every man_disc_step_m and both ends.
std::vector<OffPose> way_between(const TrafficParams &p, const Manoeuvre &path, float from, float to) {
	std::vector<OffPose> out;
	for (float along = from; along < to; along += p.man_disc_step_m) out.push_back(path.pose_at(along));
	out.push_back(path.pose_at(to));
	return out;
}

// Stopped before its gate: off the road, along the way in. The lanes its body stands across on its way, up to where it stands on
// none, are closed to the traffic behind it until it gets there (lane_closures.h).
void leave_road(CityState &s, LotCar &car) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = t.vehicles[size_t(car.id)];
	const Bay &bay = t.park.bays[size_t(car.bay)];
	const VehicleClass &c = p.classes[int(v.type)];
	Pose at;
	lane_pose(s, v.ds, v.lane, v.s - c.length * 0.5f, at);
	car.path = bay_arrival(V2(at.x, at.z), std::atan2(at.fz, at.fx), bay.at, bay.open, bay.via, p.man_lead_m, p.man_across_m, p.man_least_m, p.man_tangent);
	float along = 0;
	while (along < car.path.length() && on_lanes(s, c.length * 0.5f, c.half_width, car.path.pose_at(along))) along = std::min(car.path.length(), along + p.man_disc_step_m);
	car.lane_clear_at = along;
	sweep_body(s, c.length * 0.5f, c.half_width, way_between(p, car.path, 0, along), p.parker_closure_margin_m, car.id, t.closures);
	car.closing = true;
	lane_del(s, v.ds * MAX_LANES + v.lane, car.id);
	micro_remove(t, car.id);
	v.ds = -1;
	v.off = true;
	v.v = 0;
	t.vehicle_count--;
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
	const Bay &bay = t.park.bays[size_t(car.bay)];
	const LaneSpot &lane = bay.lane;
	if (!v.micro || car.t > p.parker_call_timeout_s || v.stuck > p.parker_give_up_s) {
		give_up(s, car);
		return;
	}
	if (v.ds != lane.ds || v.conn >= 0 || v.lane != lane.lane || v.v > p.parker_stop_ms || v.s < v.stop_at - p.parker_stop_slack_m) return;
	// another car of its lot on the drive: it goes on rather than stand on the lane, over the stretch that one may need to join it
	if (lot_busy(t.park, bay.source)) {
		give_up(s, car);
		return;
	}
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
	if (car.closing && car.path.s >= car.lane_clear_at) { // its body is off the lanes: the traffic behind goes on
		open_lanes(s.traffic, car.id);
		car.closing = false;
	}
	if (!car.path.done()) return;
	ParkingState &pk = s.traffic.park;
	pk.parked_in++;
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
	v.s =std::min(t.g.ds[size_t(lane.ds)].len, lane.at + p.classes[int(v.type)].length * 0.5f);
	v.v = p.parker_join_ms;
	micro_add(t, id);
	lane_add(s, lane.ds * MAX_LANES + lane.lane, id);
	if (!new_trip(s, id, lane.ds)) despawn_vehicle(s, id, true);
}

// It drives out to where its body still stands on no lane and waits there until every stretch of lane the rest of its way
// sweeps is clear (nobody on it, nobody too near to stop short of it: lane_clear, the lorries' rule); then those stretches are
// closed to the traffic and it goes, until it is on its lane's list.
void leaving(CityState &s, LotCar &car, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = s.traffic.vehicles[size_t(car.id)];
	const VehicleClass &c = p.classes[int(v.type)];
	if (!car.closing && car.lane_clear_at <= 0) {
		float along = car.path.length();
		while (along > 0 && on_lanes(s, c.length * 0.5f, c.half_width, car.path.pose_at(along))) along = std::max(0.f, along - p.man_disc_step_m);
		car.lane_clear_at = along;
	}
	if (!may_go(s, car, dt)) { v.v = 0; return; }
	if (car.path.s >= car.lane_clear_at && !car.closing) {
		std::vector<LaneClosure> stretches;
		sweep_body(s, c.length * 0.5f, c.half_width, way_between(p, car.path, car.lane_clear_at, car.path.length()), p.parker_closure_margin_m, car.id, stretches);
		for (const LaneClosure &k : stretches)
			if (!lane_clear(s, k)) { v.v = 0; return; }
		s.traffic.closures.insert(s.traffic.closures.end(), stretches.begin(), stretches.end());
		car.closing = true;
	}
	const float top = car.path.speed(speeds(p)), was = car.path.s;
	car.path.advance(top * dt);
	if (!car.closing) car.path.s = std::min(car.path.s, car.lane_clear_at); // it stops short of the lane until the lane has room
	v.v = car.path.s > was ? top : 0;
	set_off_pose(s, v, car.path.pose());
	if (car.path.done()) join_lane(s, car);
}

} // namespace

void set_off_pose(CityState &s, Vehicle &v, const OffPose &q) {
	v.cur.x = q.x;
	v.cur.z = q.z;
	// the ground under it read every few ticks (it moves a metre or so between two looks; the render interpolates)
	if (v.cur.y == 0 || s.traffic.tick % uint64_t(s.catalog.traffic.parker_look_ticks) == 0) v.cur.y = s.terrain.height_at(q.x, q.z) + s.catalog.traffic.body_lift_m;
	v.cur.fx = std::cos(q.angle);
	v.cur.fz = std::sin(q.angle);
}


bool lot_busy(const ParkingState &pk, int source) {
	for (const LotCar &car : pk.cars)
		if ((car.phase == CarPhase::PARKING || car.phase == CarPhase::LEAVING) && pk.bays[size_t(car.bay)].source == source) return true;
	return false;
}

bool lot_car_at_rest(const CityState &s, int id) {
	const Vehicle &v = s.traffic.vehicles[size_t(id)];
	return v.car >= 0 && s.traffic.park.cars[size_t(v.car)].phase == CarPhase::PARKED;
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

void settle_parked(CityState &s) {
	TrafficState &t = s.traffic;
	ParkingState &pk = t.park;
	const TrafficParams &p = s.catalog.traffic;
	for (int i = int(pk.cars.size()) - 1; i >= 0; i--) {
		if (pk.cars[size_t(i)].phase != CarPhase::PARKED) continue;
		const int id = pk.cars[size_t(i)].id, b = pk.cars[size_t(i)].bay;
		const Vehicle &v = t.vehicles[size_t(id)];
		Bay &bay = pk.bays[size_t(b)];
		bay.row_type = uint8_t(v.type);
		bay.row_paint = v.paint;
		bay.row_id = pk.next_row++;
		bay.stay_until = t.time + double(t.rng.range(p.parker_stay_min_s, p.parker_stay_max_s));
		add_row(pk, b);
		untrack(s, pk.cars[size_t(i)]);
		free_vehicle(t, id);
	}
}

void step_lots(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	ParkingState &pk = t.park;
	const TrafficParams &p = s.catalog.traffic;
	step_lorries(s, dt);
	for (int i = int(pk.cars.size()) - 1; i >= 0; i--) {
		LotCar &car = pk.cars[size_t(i)];
		car.t += dt;
		switch (car.phase) {
			case CarPhase::COMING: coming(s, car); break;
			case CarPhase::PARKING: parking(s, car, dt); break;
			case CarPhase::PARKED: break; // its Vehicle stands until the look makes it a row
			case CarPhase::LEAVING: leaving(s, car, dt); break;
		}
	}
	pk.clock += dt;
	if (pk.clock < p.parker_look_s) return;
	pk.clock = 0;
	Laps lap;
	settle_parked(s);
	const double target = fleet_target(s);
	lot_depart(s, target);
	t.step_ms[STAGE_LOT_OUT] += lap.lap();
	// trips ending in a lot at the rate a trip ends; as many on their way as that brings in while a called car drives there
	const float most = std::max(p.parker_coming_min, std::ceil(float(target) * p.parker_coming_s / p.parker_trip_s));
	pk.owed = std::min(most, pk.owed + float(target) / p.parker_trip_s * p.parker_look_s);
	while (pk.owed >= 1 && float(pk.coming) < most && lot_call(s)) pk.owed -= 1;
	t.step_ms[STAGE_LOT_IN] += lap.lap();
}

} // namespace traffic
} // namespace city
