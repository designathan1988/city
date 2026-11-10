// The cars that use the lots' car gates: a passing car is called to a free bay, stops on its lane before the gate, leaves
// the road, drives in, stands its stay, and when the fleet runs short drives out and joins the traffic again. Who is sent in and
// out is lot_demand.cpp. Off the lanes a car is a Vehicle
// with `off` set: on no lane list, out of the fleet count, its pose the manoeuvre's.
#include "sim/traffic/sight.h"
#include "sim/traffic/traffic_system.h"

#include "sim/traffic/lane_closures.h"
#include "sim/traffic/lot_wait.h"
#include "sim/traffic/vehicle_overlaps.h"

#include "core/laps.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr int DISCS = 3;

// The lot's way and the stretch of kerb between the bay's lane and its gate, taken for car `id` (ParkingState::take_access): the
// stretch reaches a body's length either side of where it turns in and joins.
bool take_way(ParkingState &p, const Bay &bay, int id, float len) {
	return p.take_access(bay.source, id, bay.lane.ds * MAX_LANES + bay.lane.lane, std::min(bay.lane.at, bay.lane.entry) - len, std::max(bay.lane.at, bay.lane.entry) + len);
}

// Whether the car may take this tick's step of `len` metres: where its body would stand then is clear of every other body.
bool step_free(const CityState &s, const LotCar &car, float len) {
	const OffPose q = car.path.pose_at(std::min(car.path.length(), car.path.s + len));
	return body_free(s, car.id, q.x, q.z, q.angle, BODY_TOUCH_M);
}

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
	open_lanes(t, car.id); // whatever of the street its body closed
	p.let_access(bay.source, car.id);
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

// The stretches of lane the rest of its way sweeps taken, when they are clear: whether it holds them (LotCar::closed). Taken once.
bool take_way_lanes(CityState &s, LotCar &car) {
	if (car.closed) return true;
	std::vector<LaneClosure> sweep;
	sweep_path(s, car.path, car.path.s, car.path.length(), s.traffic.vehicles[size_t(car.id)].type, car.id, sweep);
	if (!take_lanes(s, sweep)) return false;
	car.closed = true;
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
	const bool closed = car.closed; // its way's stretches, taken in coming(): the phase keeps them
	enter_phase(t.park, car, CarPhase::PARKING);
	car.closed = closed;
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
	if (!take_way(t.park, t.park.bays[size_t(car.bay)], car.id, p.classes[int(v.type)].length)) return; // another drives the lot's way or its kerb: it waits at its stop
	const Bay &bay = t.park.bays[size_t(car.bay)];
	Pose at;
	lane_pose(s, v.ds, v.lane, v.s - p.classes[int(v.type)].length * 0.5f, at);
	const Manoeuvre in = bay_arrival(V2(at.x, at.z), std::atan2(at.fz, at.fx), bay.at, bay.open, bay.via, p.man_lead_m, p.man_across_m, p.man_least_m, p.man_tangent);
	const OffPose first = in.pose();
	if (!body_free(s, car.id, first.x, first.z, first.angle, BODY_TOUCH_M)) return; // where its way in starts, another stands: it waits on its lane
	// what its way in sweeps of the street, clear and taken, before it leaves its lane (as a lorry does): else it waits there
	car.path = in;
	if (!take_way_lanes(s, car)) return;
	v.stop_at = -1;
	leave_road(s, car);
}

void parking(CityState &s, LotCar &car, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = s.traffic.vehicles[size_t(car.id)];
	const float top = car.path.speed(speeds(p));
	if (!may_go(s, car, dt) || !step_free(s, car, top * dt)) { v.v = 0; return; }
	car.path.advance(top * dt);
	v.v = car.path.done() ? 0 : top;
	set_off_pose(s, v, car.path.pose());
	if (!car.path.done()) return;
	ParkingState &pk = s.traffic.park;
	pk.let_access(pk.bays[size_t(car.bay)].source, car.id); // in its bay: the way and the street are free for the next
	open_lanes(s.traffic, car.id);
	car.closed = false;
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
	v.s = std::min(t.g.ds[size_t(lane.ds)].len, lane.at + p.classes[int(v.type)].length * 0.5f);
	v.v = p.parker_join_ms;
	micro_add(t, id);
	lane_add(s, lane.ds * MAX_LANES + lane.lane, id);
	if (!new_trip(s, id, lane.ds)) trip_ends_here(s, id); // nowhere to go from its gate (sight.h)
}

void leaving(CityState &s, LotCar &car, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = s.traffic.vehicles[size_t(car.id)];
	const Bay &bay = s.traffic.park.bays[size_t(car.bay)];
	if (!take_way(s.traffic.park, bay, car.id, p.classes[int(v.type)].length) || !may_go(s, car, dt)) { v.v = 0; return; }
	const float top = car.path.speed(speeds(p));
	const OffPose next = car.path.pose_at(std::min(car.path.length(), car.path.s + top * dt));
	// onto the street only with what the rest of its way sweeps clear and taken (no car on it nor too near to stop short of it)
	if (!step_free(s, car, top * dt) || (lanes_under_body(s, next, v.type) && !take_way_lanes(s, car))) { v.v = 0; return; }
	car.path.advance(top * dt);
	v.v = top;
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


int lot_car_wait(const CityState &s, int id) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const Vehicle &v = t.vehicles[size_t(id)];
	if (v.car < 0) return LOT_WAIT_NONE;
	const LotCar &car = t.park.cars[size_t(v.car)];
	const Bay &bay = t.park.bays[size_t(car.bay)];
	const auto held = t.park.access.find(bay.source);
	if (held != t.park.access.end() && held->second != id) return LOT_WAIT_ACCESS;
	for (const ParkingState::Crossing &c : t.park.crossings)
		if (c.holder != id && c.key == bay.lane.ds * MAX_LANES + bay.lane.lane && c.from < bay.lane.at + p.classes[int(v.type)].length && c.to > bay.lane.at - p.classes[int(v.type)].length) return LOT_WAIT_ACCESS;
	const float top = car.path.speed(speeds(p));
	const OffPose next = car.path.pose_at(std::min(car.path.length(), car.path.s + top * (1.f / s.catalog.tune.sim_tick_hz)));
	if (!body_free(s, id, next.x, next.z, next.angle, BODY_TOUCH_M)) return LOT_WAIT_BODY;
	if (car.closed || (car.phase == CarPhase::LEAVING && !lanes_under_body(s, next, v.type))) return LOT_WAIT_NONE;
	std::vector<LaneClosure> sweep;
	sweep_path(s, car.path, car.path.s, car.path.length(), v.type, id, sweep);
	for (const LaneClosure &c : sweep)
		if (!lane_clear(s, c)) return LOT_WAIT_LANES;
	return LOT_WAIT_NONE;
}

int lot_car_blocker(const CityState &s, int id) {
	const TrafficState &t = s.traffic;
	const Vehicle &v = t.vehicles[size_t(id)];
	if (v.car < 0) return -1;
	const LotCar &car = t.park.cars[size_t(v.car)];
	const float top = car.path.speed(speeds(s.catalog.traffic));
	const OffPose next = car.path.pose_at(std::min(car.path.length(), car.path.s + top * (1.f / s.catalog.tune.sim_tick_hz)));
	return spot_blocker(s, id, int(v.type), next.x, next.z, next.angle, BODY_TOUCH_M);
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
