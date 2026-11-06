// Buses and the plan's rebuild (transit_sim.h). Trains and level crossings: rail_run.cpp.
#include "sim/transit/transit_sim.h"

#include "sim/state/city_state.h"
#include "sim/transit/rail_run.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace transit {

namespace {
constexpr int STOP_SIDE_RIGHT = 0;
constexpr float NO_GOAL_M = 1e9f; // a bus never arrives: its stops hold it
constexpr float MIN_LIMIT_MS = 2.f;
constexpr int CROSSING_LOOK_LANES = 3; // the lanes ahead on a route a driver sees a closed crossing on

// The kerb lane beside a stop: the direction of its road with the stop's footway on its right.
bool lane_beside(const CityState &s, const TransitStop &q, StopLane &out) {
	if (q.seg < 0 || size_t(q.seg) >= s.roads.segs.size() || !s.roads.segs[size_t(q.seg)].alive) return false;
	const RoadSegment &g = s.roads.segs[size_t(q.seg)];
	double t, dist, cx, cz, dx, dz;
	closest_t(g.q, q.at.x, q.at.y, t, dist);
	g.q.at(t, cx, cz);
	g.q.deriv(t, dx, dz);
	const int side = dz * (q.at.x - cx) - dx * (q.at.y - cz) > 0 ? 1 : STOP_SIDE_RIGHT;
	const int ds = q.seg * 2 + side;
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	if (!d.ok) return false;
	out = { ds, 0, std::clamp((float(t) * float(g.len) - d.at0) * d.sign, 0.f, d.len) };
	return true;
}

float travel_time(const TrafficParams &p, float distance, float limit) {
	return distance * p.bus_detour / std::max(MIN_LIMIT_MS, limit * p.bus_pace);
}

void turn_on(Bus &b, const TransitLine &l) {
	if (b.next + b.dir < 0 || b.next + b.dir >= int(l.stops.size())) b.dir = -b.dir;
	b.next += b.dir;
}

bool is_end(const CityState &s, const TransitLine &l, const Bus &b) {
	const TransitStop *q = s.transit.stop(l.stops[size_t(b.next)]);
	return (q && q->terminal) || b.next == 0 || b.next + 1 == int(l.stops.size());
}

// A bus sent on along the streets to its next stop that it can reach; stops with no lane or no way are passed over.
void send_bus(CityState &s, const TransitLine &l, Bus &b, Vehicle &v) {
	TransitRun &run = s.transit_run;
	TrafficState &t = s.traffic;
	for (size_t tries = 0; tries < 2 * l.stops.size(); tries++) {
		const auto it = run.stop_lanes.find(l.stops[size_t(b.next)]);
		if (it != run.stop_lanes.end()) {
			std::vector<int> route;
			if (it->second.ds == v.ds) route = { v.ds };
			if (!route.empty() || (traffic::find_route(s, v.ds, it->second.ds, route) && route.size() >= 2)) {
				route_set(t.route_pool, v.route, t.route_waste, route.data(), int(route.size()));
				v.ri = 0;
				v.goal_s = NO_GOAL_M;
				const TrafficParams &p = s.catalog.traffic;
				const DirSeg &d = t.g.ds[size_t(it->second.ds)];
				v.bus_stop_at = std::min(it->second.at + p.classes[int(VehicleType::BUS)].length * 0.5f, d.len - d.stop_back - p.stop_margin_m); // short of the line
				return;
			}
		}
		turn_on(b, l);
	}
	v.bus_stop_at = -1;
}

int spawn_bus(CityState &s, const TransitLine &l, Bus &b) {
	const auto it = s.transit_run.stop_lanes.find(l.stops[size_t(b.next)]);
	if (it == s.transit_run.stop_lanes.end()) return -1;
	const TrafficParams &p = s.catalog.traffic;
	const StopLane &lane = it->second;
	const float len = p.classes[int(VehicleType::BUS)].length, start = std::max(len, lane.at - p.bus_approach_m);
	TrafficState &t = s.traffic;
	const int key = lane.ds * MAX_LANES + lane.lane;
	for (int id : t.lane_vehicles[size_t(key)]) {
		const Vehicle &o = t.vehicles[size_t(id)];
		if (o.s > start - len * 2 && o.s - p.classes[int(o.type)].length < lane.at + len) return -1; // not into another vehicle
	}
	const std::vector<uint32_t> &colours = p.line_colours;
	const int colour = int(std::find(colours.begin(), colours.end(), l.colour) - colours.begin());
	const int id = traffic::new_vehicle(s, VehicleType::BUS, uint8_t(LINE_PAINT + (colour < int(colours.size()) ? colour : 0)));
	Vehicle &v = t.vehicles[size_t(id)];
	v.ds = lane.ds;
	v.lane = lane.lane;
	v.s = start;
	v.bus = int(&b - s.transit_run.buses.data());
	v.micro = false; // micro_add registers it
	traffic::micro_add(t, id);
	traffic::lane_add(s, key, id);
	send_bus(s, l, b, v);
	return id;
}

// Away from the view: its dwell at the stop, on to the next, the time to drive there.
void pass_stop_away(CityState &s, const TransitLine &l, Bus &b, const StopLane &here) {
	const TrafficParams &p = s.catalog.traffic;
	const bool end = is_end(s, l, b);
	const DirSeg &d = s.traffic.g.ds[size_t(here.ds)];
	turn_on(b, l);
	const auto it = s.transit_run.stop_lanes.find(l.stops[size_t(b.next)]);
	const float away = it == s.transit_run.stop_lanes.end() ? d.len : (V2(d.ex, d.ez) - V2(s.traffic.g.ds[size_t(it->second.ds)].ex, s.traffic.g.ds[size_t(it->second.ds)].ez)).len();
	b.wait = (end ? p.bus_terminal_s : p.bus_dwell_s) + travel_time(p, away, d.speed);
}

void step_bus(CityState &s, Bus &b, float dt) {
	const TransitLine *l = s.transit.line(b.line);
	if (!l || l->stops.empty()) return;
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	if (b.vehicle < 0) {
		b.wait -= dt;
		if (b.wait > 0) return;
		const auto it = s.transit_run.stop_lanes.find(l->stops[size_t(b.next)]);
		if (it == s.transit_run.stop_lanes.end()) {
			turn_on(b, *l);
			return;
		}
		if (!traffic::in_window(s, it->second.ds)) {
			pass_stop_away(s, *l, b, it->second);
			return;
		}
		b.vehicle = spawn_bus(s, *l, b);
		b.wait = p.bus_spacing_s;
		return;
	}
	Vehicle &v = t.vehicles[size_t(b.vehicle)];
	if (!v.micro && b.dwell == 0) { // driven out of the view: on by the timetable
		traffic::despawn_vehicle(s, b.vehicle, true);
		return;
	}
	const auto it = s.transit_run.stop_lanes.find(l->stops[size_t(b.next)]);
	if (it == s.transit_run.stop_lanes.end()) {
		turn_on(b, *l);
		send_bus(s, *l, b, v);
		return;
	}
	const bool here = v.conn < 0 && v.ds == it->second.ds && v.v < p.bus_stopped_ms && v.s >= v.bus_stop_at - p.bus_at_stop_m;
	if (!here) return;
	b.dwell += dt;
	if (b.dwell < (is_end(s, *l, b) ? p.bus_terminal_s : p.bus_dwell_s)) return;
	b.dwell = 0;
	s.transit_run.stops_called++;
	turn_on(b, *l);
	send_bus(s, *l, b, v);
}

// A line's buses: those it had stay where they are, more wait at its terminal, up to its number.
void place_buses(CityState &s, const TransitLine &l, std::vector<Bus> &was, std::vector<Bus> &out) {
	const TrafficParams &p = s.catalog.traffic;
	int served = 0, start = 0;
	for (size_t i = 0; i < l.stops.size(); i++) {
		served += s.transit_run.stop_lanes.count(l.stops[i]) ? 1 : 0;
		const TransitStop *q = s.transit.stop(l.stops[i]);
		if (q && q->terminal && start == 0) start = int(i);
	}
	if (served < 2) return;
	int k = 0;
	for (Bus &b : was) {
		if (b.line != l.id || k >= l.vehicles) continue;
		if (b.next >= int(l.stops.size())) {
			b.next = start;
			b.dir = 1;
		}
		out.push_back(b);
		b.line = -1; // kept
		k++;
	}
	for (; k < l.vehicles; k++) {
		Bus b;
		b.line = l.id;
		b.next = start;
		b.wait = float(k) * p.bus_spacing_s;
		out.push_back(b);
	}
}

// An edit of the plan or of the roads: every stop's lane found again, the lines' buses going on from where they are.
void rebuild(CityState &s) {
	TransitRun &run = s.transit_run;
	std::vector<Bus> was = std::move(run.buses);
	run.buses.clear();
	run.stop_lanes.clear();
	for (const TransitStop &q : s.transit.stops) {
		StopLane lane;
		if (q.mode == TransitMode::BUS && lane_beside(s, q, lane)) run.stop_lanes[q.id] = lane;
	}
	for (const TransitLine &l : s.transit.lines)
		if (l.mode == TransitMode::BUS) place_buses(s, l, was, run.buses);
	for (const Bus &b : was)
		if (b.line >= 0 && b.vehicle >= 0) { // its line went, or has fewer
			s.traffic.vehicles[size_t(b.vehicle)].bus = -1;
			traffic::despawn_vehicle(s, b.vehicle, true);
		}
	for (size_t i = 0; i < run.buses.size(); i++) {
		Bus &b = run.buses[i];
		if (b.vehicle < 0) continue;
		Vehicle &v = s.traffic.vehicles[size_t(b.vehicle)];
		v.bus = int(i);
		if (const TransitLine *l = s.transit.line(b.line)) send_bus(s, *l, b, v);
	}
	place_rails(s);
	run.plan_seen = s.transit_revision;
	run.roads_seen = uint64_t(s.roads.version);
	run.built = true;
}
} // namespace

void step(CityState &s, float dt) {
	TransitRun &run = s.transit_run;
	if (!run.built || run.plan_seen != s.transit_revision || run.roads_seen != uint64_t(s.roads.version)) rebuild(s);
	for (Bus &b : run.buses) step_bus(s, b, dt);
	for (RailLine &r : run.rails)
		for (Train &tr : r.trains) step_train(s, r, tr, dt);
	train_zones(s);
}

void reset(CityState &s) { s.transit_run = TransitRun(); }

float hold_ahead(const CityState &s, const Vehicle &v, int ds, bool through, bool head) {
	float hold = -1;
	if (!s.transit_run.crossings_by_lane.empty()) {
		const TrafficState &t = s.traffic;
		hold = crossing_gap(s, ds * MAX_LANES + v.lane, v.s);
		float offset = t.g.ds[size_t(ds)].len - v.s;
		for (int k = v.ri + 1; k < v.route.len && k <= v.ri + CROSSING_LOOK_LANES; k++) {
			const int next = route_at(t.route_pool, v.route, k);
			const float gap = crossing_gap(s, next * MAX_LANES, 0); // every lane of a road has its crossing
			if (gap >= 0) hold = hold < 0 ? offset + gap : std::min(hold, offset + gap);
			offset += t.g.ds[size_t(next)].len;
		}
		// a level crossing too near the stop line to wait between the two: it waits before the track until it may go on
		if (through && v.pass < 0 && (head || traffic::light_for(s, ds) != Light::GREEN)) {
			const float track = crossing_near_end(s, ds * MAX_LANES + v.lane, t.g.ds[size_t(ds)].len);
			if (track >= 0 && v.s < track) {
				const float wait = std::max(0.f, track - s.catalog.traffic.crossing_stop_m - v.s);
				hold = hold < 0 ? wait : std::min(hold, wait);
			}
		}
	}
	if (!through && v.bus_stop_at >= 0) { // the IDM stops s0 short of what it is given
		const float stop = std::max(0.f, v.bus_stop_at - v.s + s.catalog.traffic.classes[int(v.type)].min_gap);
		hold = hold < 0 ? stop : std::min(hold, stop);
	}
	return hold;
}

void bus_gone(CityState &s, int vehicle) {
	Vehicle &v = s.traffic.vehicles[size_t(vehicle)];
	if (v.bus < 0) return;
	Bus &b = s.transit_run.buses[size_t(v.bus)];
	const TrafficParams &p = s.catalog.traffic;
	b.vehicle = -1;
	b.wait = p.bus_spacing_s;
	v.bus = -1;
	const TransitLine *l = s.transit.line(b.line);
	if (!l || b.next >= int(l->stops.size())) return;
	const auto it = s.transit_run.stop_lanes.find(l->stops[size_t(b.next)]);
	if (it == s.transit_run.stop_lanes.end()) return;
	const DirSeg &d = s.traffic.g.ds[size_t(it->second.ds)]; // on by the timetable: the time to drive to the stop it was going to
	b.wait = travel_time(p, (V2(v.cur.x, v.cur.z) - V2(d.ex, d.ez)).len(), d.speed);
}

float bus_door_share(const CityState &s, const Bus &b) {
	const TransitLine *l = s.transit.line(b.line);
	if (!l || l->stops.empty() || b.dwell <= 0) return 0;
	const TrafficParams &p = s.catalog.traffic;
	const float total = is_end(s, *l, b) ? p.bus_terminal_s : p.bus_dwell_s;
	return std::clamp(std::min(b.dwell, total - b.dwell) / p.kerb_door_open_s, 0.f, 1.f);
}

} // namespace transit
} // namespace city
