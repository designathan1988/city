// Lorries to the docks of the works near the camera. A truck on the lanes is called to a stop before a gate; there it leaves the
// lanes and, the street it sweeps clear and closed, backs onto the dock along the way a worker planned (yard.h), unloads, drives out
// and is a truck of the traffic again past the gate.
#include "sim/traffic/traffic_system.h"

#include "sim/traffic/lane_closures.h"

#include "core/laps.h"
#include "sim/traffic/yard.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr float LOOK_S = 2.f; // a look for a dock with no lorry

// The world's body of a lorry whose rear axle stands at `p`.
OffPose body_at(const TrafficParams &tp, const AxlePose &p) {
	return { float(p.x + std::cos(p.angle) * tp.lorry_rear_m), float(p.z + std::sin(p.angle) * tp.lorry_rear_m), float(p.angle) };
}

ManoeuvreSpeeds speeds(const TrafficParams &p) {
	return { p.lorry_forward_ms, p.lorry_backward_ms, p.lorry_ramp_m, p.lorry_creep_ms };
}

Manoeuvre shunt(const TrafficParams &p, const std::vector<AxlePose> &way) {
	std::vector<OffStep> steps;
	steps.reserve(way.size());
	for (const AxlePose &q : way) {
		const OffPose b = body_at(p, q);
		steps.push_back({ b.x, b.z, b.angle, q.reverse });
	}
	return Manoeuvre(steps);
}

bool close_if_clear(CityState &s, Lorry &l, const std::vector<LaneClosure> &stretches) {
	if (l.closed) return true;
	for (const LaneClosure &c : stretches)
		if (!lane_clear(s, c)) return false;
	s.traffic.closures.insert(s.traffic.closures.end(), stretches.begin(), stretches.end());
	l.closed = !stretches.empty();
	return true;
}

// Nobody stands where it goes over the next few metres; held up long enough it asks its way through.
bool may_go(CityState &s, Lorry &l, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	const float span = p.classes[int(VehicleType::TRUCK)].length * 0.5f, r = p.classes[int(VehicleType::TRUCK)].half_width + p.lorry_walker_clear_m;
	for (float at = l.path.s; at <= std::min(l.path.length(), l.path.s + p.lorry_look_m); at += 1.f) {
		const OffPose q = l.path.pose_at(at);
		for (float k : { -span, 0.f, span })
			if (walker_within(s, q.x + std::cos(q.angle) * k, q.z + std::sin(q.angle) * k, r)) {
				l.waited += dt;
				return l.waited >= p.lorry_ask_way_s;
			}
	}
	l.waited = 0;
	return true;
}

void forget(CityState &s, int index) {
	ParkingState &pk = s.traffic.park;
	open_lanes(s.traffic, pk.lorries[size_t(index)].id);
	s.traffic.vehicles[size_t(pk.lorries[size_t(index)].id)].lorry = -1;
	if (index + 1 != int(pk.lorries.size())) {
		pk.lorries[size_t(index)] = std::move(pk.lorries.back());
		s.traffic.vehicles[size_t(pk.lorries[size_t(index)].id)].lorry = index;
	}
	pk.lorries.pop_back();
}

// On the lane past the gate: a truck of the traffic again.
void join(CityState &s, int index) {
	TrafficState &t = s.traffic;
	Lorry &l = t.park.lorries[size_t(index)];
	const int id = l.id, ds = l.gate.ds, lane = l.gate.lane;
	const float at = l.gate.join_at + s.catalog.traffic.classes[int(VehicleType::TRUCK)].length * 0.5f;
	t.park.lorries_left++;
	forget(s, index);
	Vehicle &v = t.vehicles[size_t(id)];
	v.off = false;
	t.vehicle_count++;
	if (!t.g.ds[size_t(ds)].ok || lane >= t.g.ds[size_t(ds)].lanes) { despawn_vehicle(s, id, false); return; }
	v.ds = ds;
	v.lane = lane;
	v.s = std::min(t.g.ds[size_t(ds)].len, at);
	v.v = s.catalog.traffic.lorry_join_ms;
	micro_add(t, id);
	lane_add(s, ds * MAX_LANES + lane, id);
	if (!new_trip(s, id, ds)) despawn_vehicle(s, id, true);
}

void give_up(CityState &s, int index) {
	Lorry &l = s.traffic.park.lorries[size_t(index)];
	const int id = l.id;
	Vehicle &v = s.traffic.vehicles[size_t(id)];
	v.stop_at = -1;
	forget(s, index);
	if (v.pass >= 0) release(s, id, v.pass);
	if (v.ds < 0 || !new_trip(s, id, v.ds)) v.goal_s = 0;
}

// Stopped before the gate, it leaves the lanes where it stands: the way in starts from there.
void coming(CityState &s, int index) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Lorry &l = t.park.lorries[size_t(index)];
	Vehicle &v = t.vehicles[size_t(l.id)];
	if (!v.alive || !v.micro || l.t > p.lorry_call_timeout_s) { give_up(s, index); return; }
	if (v.ds != l.gate.ds || v.conn >= 0 || v.lane != l.gate.lane || v.v > p.parker_stop_ms || v.s < v.stop_at - p.lorry_stop_slack_m) return;
	v.stop_at = -1;
	Pose at;
	lane_pose(s, v.ds, v.lane, v.s - p.classes[int(v.type)].length * 0.5f, at);
	const double angle = std::atan2(double(at.fz), double(at.fx));
	const AxlePose here{ at.x - at.fx * p.lorry_rear_m, at.z - at.fz * p.lorry_rear_m, angle, false };
	const AxlePose &start = l.into.front();
	const bool behind = (start.x - here.x) * at.fx + (start.z - here.z) * at.fz < 0;
	l.into.insert(l.into.begin(), { here.x, here.z, here.angle, behind });
	l.into_street++;
	lane_del(s, v.ds * MAX_LANES + v.lane, l.id);
	micro_remove(t, l.id);
	v.off = true;
	v.ds = -1;
	v.v = 0;
	t.vehicle_count--;
	set_off_pose(s, v, body_at(p, here));
	l.phase = LorryPhase::WAITING;
	l.t = 0;
}

void drive(CityState &s, int index, float dt) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Lorry &l = t.park.lorries[size_t(index)];
	Vehicle &v = t.vehicles[size_t(l.id)];
	const bool leaving = l.phase == LorryPhase::LEAVING;
	if (leaving && l.path.s >= l.street_at) {
		if (!close_if_clear(s, l, l.close_out)) { v.v = 0; return; }
		l.phase = LorryPhase::JOINING;
	}
	if (!may_go(s, l, dt)) { v.v = 0; return; }
	const float was = l.path.s, speed = l.path.speed(speeds(p));
	l.path.advance(speed * dt);
	if (leaving) l.path.s = std::min(l.path.s, l.street_at);
	v.v = l.path.s > was ? speed : 0;
	set_off_pose(s, v, l.path.pose());
	if (l.phase == LorryPhase::ARRIVING && l.closed && l.path.s >= l.street_at) {
		open_lanes(t, l.id);
		l.closed = false;
	}
	if (!l.path.done()) return;
	if (l.phase == LorryPhase::ARRIVING) {
		l.phase = LorryPhase::DOCKED;
		l.stay = p.lorry_unload_min_s + t.rng.f() * (p.lorry_unload_max_s - p.lorry_unload_min_s);
		t.park.lorries_docked++;
		v.v = 0;
		return;
	}
	join(s, index);
}

void step_one(CityState &s, int index, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	Lorry &l = s.traffic.park.lorries[size_t(index)];
	Vehicle &v = s.traffic.vehicles[size_t(l.id)];
	l.t += dt;
	switch (l.phase) {
		case LorryPhase::COMING: coming(s, index); return;
		case LorryPhase::WAITING:
			v.v = 0;
			if (!close_if_clear(s, l, l.close_in)) return;
			l.path = shunt(p, l.into);
			l.street_at = l.path.at_step(l.into_street);
			l.phase = LorryPhase::ARRIVING;
			return;
		case LorryPhase::DOCKED:
			v.v = 0;
			l.stay -= dt;
			if (l.stay > 0) return;
			l.path = shunt(p, l.away);
			l.street_at = l.path.at_step(l.away_street);
			l.phase = LorryPhase::LEAVING;
			return;
		default: drive(s, index, dt); return;
	}
}

// A yard's plan, with the docks done so far while it is being worked out; none while another is: the first asked for starts.
const YardPlan *yard_of(CityState &s, int building) {
	ParkingState &pk = s.traffic.park;
	const auto hit = pk.yards.find(building);
	if (hit != pk.yards.end()) return hit->second.get();
	if (pk.planner) return pk.planner_building == building ? &pk.planner->plan : nullptr;
	YardInput in;
	if (yard_input(s, building, in)) {
		pk.planner = std::make_shared<YardPlanner>(std::move(in));
		pk.planner_building = building;
		pk.plan_ms = 0;
	} else { // nothing to plan: kept as such until what it read of the roads changes
		auto none = std::make_shared<YardPlan>();
		none->reads = std::move(in.reads);
		pk.yards[building] = std::move(none);
	}
	return nullptr;
}

// The plan on its way gets its share of this tick: the work is counted, so the same map is planned on the same tick; the clock only
// feeds the probes.
void plan_yard(CityState &s) {
	ParkingState &pk = s.traffic.park;
	if (!pk.planner) return;
	Laps took;
	const bool done = pk.planner->advance(int(s.catalog.traffic.lorry_plan_work));
	const double ms = took.lap();
	pk.plan_ms += ms;
	pk.plan_tick_ms = std::max(pk.plan_tick_ms, ms);
	if (!done) return;
	pk.plan_slowest_ms = std::max(pk.plan_slowest_ms, pk.plan_ms);
	pk.yards[pk.planner_building] = std::make_shared<YardPlan>(std::move(pk.planner->plan));
	pk.planner.reset();
}

// The nearest passing truck a gate can take, -1 for none.
int passing_truck(const CityState &s, const YardGate &g) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	int best = -1;
	float best_d = p.lorry_call_reach_m;
	for (int id : t.micro_list) {
		const Vehicle &v = t.vehicles[size_t(id)];
		if (v.type != VehicleType::TRUCK || v.car >= 0 || v.lorry >= 0 || v.ds < 0 || v.conn >= 0 || v.pass >= 0) continue;
		if (v.ds == g.ds && v.s > g.stop_at) continue;
		const float d = (V2(v.cur.x, v.cur.z) - g.outer).len();
		if (d < best_d) { best_d = d; best = id; }
	}
	return best;
}

void call(CityState &s) {
	TrafficState &t = s.traffic;
	ParkingState &pk = t.park;
	const TrafficParams &p = s.catalog.traffic;
	std::vector<int> docks;
	for (int b = 0; b < int(pk.bays.size()); b++) {
		const Bay &bay = pk.bays[size_t(b)];
		if (!bay.alive || !bay.dock || bay.holder >= 0) continue;
		bool busy = false;
		for (const Lorry &l : pk.lorries) busy = busy || l.building == bay.source;
		if (!busy) docks.push_back(b);
	}
	if (docks.empty()) { pk.lorry_calls[CALL_NO_DOCK]++; return; }
	const int b = docks[size_t(t.rng.next() % uint32_t(docks.size()))];
	Bay &bay = pk.bays[size_t(b)];
	const YardPlan *plan = yard_of(s, bay.source);
	if (!plan) { pk.lorry_calls[CALL_PLANNING]++; return; }
	const DockWays *ways = nullptr;
	for (const DockWays &w : plan->docks) ways = w.bay == b ? &w : ways;
	if (!ways && pk.planner && pk.planner_building == bay.source) { pk.lorry_calls[CALL_PLANNING]++; return; }
	if (!ways || ways->gate < 0) { pk.lorry_calls[CALL_NO_WAY]++; return; }
	const YardGate &gate = plan->gates[size_t(ways->gate)];
	const int id = passing_truck(s, gate);
	if (id < 0) { pk.lorry_calls[CALL_NO_TRUCK]++; return; }
	Vehicle &v = t.vehicles[size_t(id)];
	std::vector<int> route;
	if (!find_route(s, v.ds, gate.ds, route, v.type) || route.empty()) { pk.lorry_calls[CALL_NO_ROUTE]++; return; }
	route_set(t.route_pool, v.route, t.route_waste, route.data(), int(route.size()));
	v.ri = 0;
	v.stop_at = gate.stop_at + p.classes[int(v.type)].length * 0.5f;
	v.stop_lane = gate.lane;
	v.goal_s = BIG;
	v.lorry = int(pk.lorries.size());
	Lorry l;
	l.id = id;
	l.building = bay.source;
	l.bay = b;
	l.gate = gate;
	l.into = ways->into;
	l.away = ways->away;
	l.into_street = ways->into_street;
	l.away_street = ways->away_street;
	sweep_lanes(s, l.into, 0, ways->into_street, id, l.close_in);
	sweep_lanes(s, l.away, ways->away_street, int(l.away.size()) - 1, id, l.close_out);
	pk.lorries.push_back(std::move(l));
	pk.lorries_sent++;
	pk.lorry_calls[CALL_SENT]++;
}
} // namespace

void lorry_gone(CityState &s, int id) {
	if (s.traffic.vehicles[size_t(id)].lorry >= 0) forget(s, s.traffic.vehicles[size_t(id)].lorry);
}

void step_lorries(CityState &s, float dt) {
	ParkingState &pk = s.traffic.park;
	for (int i = int(pk.lorries.size()) - 1; i >= 0; i--) step_one(s, i, dt);
	plan_yard(s);
	pk.lorry_clock += dt;
	if (pk.lorry_clock < LOOK_S) return;
	pk.lorry_clock = 0;
	if (int(pk.lorries.size()) < int(s.catalog.traffic.lorry_most)) call(s);
}

} // namespace traffic
} // namespace city
