// The cars that use the lots' car gates: a passing car is called to a free bay, stops on its lane before the gate, leaves
// the road, drives in, stands its stay, and when the fleet runs short drives out and joins the traffic again. Who is sent in and
// out is lot_demand.cpp. Off the lanes a car is a Vehicle
// with `off` set: on no lane list, out of the fleet count, its pose the manoeuvre's. Several cars of a lot drive at once, each by its
// swath (swaths.h): one starts on its way when no standing body lies on it, and gives way to the swaths of those that started before.
#include "sim/traffic/traffic_system.h"

#include "sim/traffic/lane_closures.h"
#include "sim/traffic/swaths.h"

#include "core/laps.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

ManoeuvreSpeeds speeds(const TrafficParams &p) { return { p.man_forward_ms, p.man_backward_ms, p.man_ramp_m, p.man_creep_ms }; }

// Driving into its bay or out of it: what ParkingState::moving counts.
bool driving(CarPhase phase) { return phase == CarPhase::PARKING || phase == CarPhase::LEAVING; }

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
	if (driving(car.phase)) p.moving--;
	Bay &bay = p.bays[size_t(car.bay)];
	bay.holder = -1;
	refile_bay(s, car.bay, false);
	const int index = t.vehicles[size_t(car.id)].car;
	t.vehicles[size_t(car.id)].car = -1;
	remove_car(p, t, index);
}

// Whether the car starts on the rest of its way now. Nobody on foot is to stand on it (its body as discs every metre; after
// parker_ask_way_s it starts anyway, the walkers stepping out of its swath), and no other vehicle off the lanes that has not started
// stands on it (swaths.h); from then on it waits for no walker and gives way only to the swaths of those that started before it.
bool starts_on_way(CityState &s, LotCar &car, float dt) {
	if (car.start_order) return true;
	const TrafficParams &p = s.catalog.traffic;
	const uint64_t every = uint64_t(p.parker_look_ticks); // it looks every so often, all together: the walkers are filed once for them
	if (s.traffic.tick % every != 0) return false;
	// the rest of its way, laid when its manoeuvre was made; coming out, as far as its wait inside the gate (the street is street_open's)
	const int size = int(car.way.size()), from = way_from(s, car.path.s, size), end = car.held_in() ? way_to(s, car.wait(), size) : size;
	bool clear = true;
	for (int k = from; k < end && clear; k++) {
		const SwathDisc &d = car.way[size_t(k)];
		clear = !walker_within(s, d.x, d.z, d.r + p.man_disc_pad_m);
	}
	if (!clear && car.waited < p.parker_ask_way_s) {
		car.waited += dt * float(every);
		return false;
	}
	if (way_has_standing(s, car.id, car.way, from, end)) return false; // a car or a lorry stands on it that has not started on its own way
	car.start_order = next_start(s);
	return true;
}

std::vector<OffPose> way_between(const TrafficParams &p, const Manoeuvre &path, float from, float to);

// At its wait inside the gate, whether a car coming out goes out now: the footway its way crosses clear of walkers (or parker_ask_way_s
// waited, `asked`: once it commits they step out of its swath) and every stretch of lane the rest of its way sweeps clear (lane_clear: nobody
// on it, nobody too near to stop short of it); then those stretches are closed to the traffic behind and the rest of its way is its swath.
bool street_open(CityState &s, LotCar &car, const VehicleClass &c, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	const uint64_t every = uint64_t(p.parker_look_ticks);
	if (s.traffic.tick % every != 0) return false;
	const int size = int(car.way.size()), from = way_from(s, car.path.s, size), kerb = way_to(s, car.lane_clear_at, size);
	bool clear = true;
	for (int k = from; k < kerb && clear; k++) {
		const SwathDisc &d = car.way[size_t(k)];
		clear = !walker_within(s, d.x, d.z, d.r + p.man_disc_pad_m);
	}
	if (!clear && car.asked < p.parker_ask_way_s) {
		car.asked += dt * float(every);
		return false;
	}
	std::vector<LaneClosure> stretches;
	sweep_body(s, c.length * 0.5f, c.half_width, way_between(p, car.path, car.lane_clear_at, car.path.length()), p.parker_closure_margin_m, car.id, stretches);
	for (const LaneClosure &k : stretches)
		if (!lane_clear(s, k)) return false;
	s.traffic.closures.insert(s.traffic.closures.end(), stretches.begin(), stretches.end());
	car.closing = true;
	car.committed = true;
	return true;
}

// A car's body along its way from `from` to `to`, a pose every man_disc_step_m and both ends.
std::vector<OffPose> way_between(const TrafficParams &p, const Manoeuvre &path, float from, float to) {
	std::vector<OffPose> out;
	for (float along = from; along < to; along += p.man_disc_step_m) out.push_back(path.pose_at(along));
	out.push_back(path.pose_at(to));
	return out;
}

// What a car stopped before its gate does about its way in (leave_road).
enum class Turn : uint8_t { IN, WAIT, TAKEN };

// Stopped before its gate: off the road, along the way in. The lanes its body stands across on its way, up to where it stands on
// none, are closed to the traffic behind it until it gets there (lane_closures.h), once nobody is on those stretches or too near to
// stop short of them, as for every body off the lanes (lane_clear): until then it waits at its gate (WAIT), its call's timeout
// bounding the wait. Not when its way in meets the swath of a vehicle leaving the lot or a standing body (swaths.h): it would stand
// on its lane over the stretch that one needs (TAKEN).
Turn leave_road(CityState &s, LotCar &car) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = t.vehicles[size_t(car.id)];
	const Bay &bay = t.park.bays[size_t(car.bay)];
	const VehicleClass &c = p.classes[int(v.type)];
	Pose at;
	lane_pose(s, v.ds, v.lane, v.s - c.length * 0.5f, at);
	Manoeuvre way = bay_arrival(V2(at.x, at.z), std::atan2(at.fz, at.fx), bay.at, bay.open, bay.via, p.man_lead_m, p.man_across_m, p.man_least_m, p.man_tangent);
	std::vector<SwathDisc> laid;
	lay_way(s, c, way, laid);
	if (way_meets_outward(s, car.id, laid, 0)) return Turn::TAKEN;
	float along = 0;
	while (along < way.length() && on_lanes(s, c.length * 0.5f, c.half_width, way.pose_at(along))) along = std::min(way.length(), along + p.man_disc_step_m);
	std::vector<LaneClosure> stretches;
	sweep_body(s, c.length * 0.5f, c.half_width, way_between(p, way, 0, along), p.parker_closure_margin_m, car.id, stretches);
	for (const LaneClosure &k : stretches)
		if (!lane_clear(s, k)) return Turn::WAIT; // someone stands where it turns, or comes on too near to stop short of it
	car.path = std::move(way);
	car.way = std::move(laid);
	car.lane_clear_at = along;
	t.closures.insert(t.closures.end(), stretches.begin(), stretches.end());
	car.closing = true;
	lane_del(s, v.ds * MAX_LANES + v.lane, car.id);
	micro_remove(t, car.id);
	v.ds = -1;
	v.off = true;
	v.v = 0;
	t.vehicle_count--;
	lot_car_moves(t.park, car, CarPhase::PARKING);
	set_off_pose(s, v, car.path.pose());
	return Turn::IN;
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
	const Turn turn = leave_road(s, car);
	if (turn == Turn::WAIT) return; // it stands at its gate's mark until where it turns is clear
	v.stop_at = -1;
	if (turn == Turn::TAKEN) give_up(s, car); // its way in is taken by one coming out: it goes on elsewhere
}

void parking(CityState &s, LotCar &car, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = s.traffic.vehicles[size_t(car.id)];
	if (!starts_on_way(s, car, dt)) { v.v = 0; return; }
	const float top = car.path.speed(speeds(p)), step = room_on_way(s, car.id, car.start_order, car.path, p.classes[int(v.type)], top * dt);
	car.path.advance(step);
	v.v = car.path.done() || step <= 0 ? 0 : top;
	set_off_pose(s, v, car.path.pose());
	if (car.closing && car.path.s >= car.lane_clear_at) { // its body is off the lanes: the traffic behind goes on
		open_lanes(s.traffic, car.id);
		car.closing = false;
	}
	if (!car.path.done()) return;
	ParkingState &pk = s.traffic.park;
	pk.parked_in++;
	lot_car_moves(pk, car, CarPhase::PARKED);
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

// It drives out to its wait inside the gate (LotCar::wait_at; at the kerb, where its body stands on no lane, when its bay has no gate run)
// and waits there until the footway and the street let it out (street_open); then it crosses without stopping, until it is on its lane's
// list. The footway before its gate is the walkers' while it waits.
void leaving(CityState &s, LotCar &car, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = s.traffic.vehicles[size_t(car.id)];
	const VehicleClass &c = p.classes[int(v.type)];
	if (!car.closing && car.lane_clear_at <= 0) {
		float along = car.path.length();
		while (along > 0 && on_lanes(s, c.length * 0.5f, c.half_width, car.path.pose_at(along))) along = std::max(0.f, along - p.man_disc_step_m);
		car.lane_clear_at = along;
	}
	if (!starts_on_way(s, car, dt)) { v.v = 0; return; }
	if (!car.committed && car.path.s >= car.wait() && !street_open(s, car, c, dt)) { v.v = 0; return; }
	const float top = car.path.speed(speeds(p)), was = car.path.s;
	car.path.advance(room_on_way(s, car.id, car.start_order, car.path, c, top * dt));
	if (!car.committed) car.path.s = std::min(car.path.s, car.wait()); // it stops at its wait until the footway and the street let it out
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

bool lot_car_moves(ParkingState &p, LotCar &car, CarPhase next) {
	const CarPhase was = car.phase;
	if (!enter_car_phase(car, next)) return false;
	if (next == was) return true;
	p.coming += int(next == CarPhase::COMING) - int(was == CarPhase::COMING);
	p.moving += int(driving(next)) - int(driving(was));
	car.t = car.waited = 0;
	car.start_order = 0;
	return true;
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
	file_swaths(s); // where every vehicle off the lanes stands and drives before any of them moves this tick
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
	lot_depart(s, target - double(flow_owed(t.flows))); // the vehicles the flows still owe the window are its fleet's already
	t.step_ms[STAGE_LOT_OUT] += lap.lap();
	// trips ending in a lot at the rate a trip ends; as many on their way as that brings in while a called car drives there
	const float most = std::max(p.parker_coming_min, std::ceil(float(target) * p.parker_coming_s / p.parker_trip_s));
	pk.owed = std::min(most, pk.owed + float(target) / p.parker_trip_s * p.parker_look_s);
	while (pk.owed >= 1 && float(pk.coming) < most && lot_call(s)) pk.owed -= 1;
	t.step_ms[STAGE_LOT_IN] += lap.lap();
}

} // namespace traffic
} // namespace city
