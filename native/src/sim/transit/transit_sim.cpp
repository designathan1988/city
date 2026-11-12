// Buses and the plan's rebuild (transit_sim.h). Trains and level crossings: rail_run.cpp.
#include "sim/transit/transit_sim.h"
#include "core/generation.h"

#include "sim/traffic/kerb_stops.h"

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

bool bus_lane_room(const CityState &s, int key, float start, float stop, bool reservations) {
	const auto &p = s.catalog.traffic;
	const float length = p.classes[int(VehicleType::BUS)].length;
	auto overlaps = [&](float at, int type) { return at > start - length * 2 && at - p.classes[type].length < stop + length; };
	for (int id : s.traffic.lane_vehicles[size_t(key)]) { const Vehicle &o = s.traffic.vehicles[size_t(id)]; if (overlaps(o.s, int(o.type))) return false; }
	return !reservations || !s.traffic.birth_reservations.any(key, s.roads.segment_generation((key / MAX_LANES) / 2), [&](const BirthReservations::Claim &claim) { return overlaps(claim.at, claim.type); });
}
bool birth_lane_valid(const CityState &s, const Bus &b, const Vehicle &v) {
	if (v.ds < 0 || size_t(v.ds) >= s.traffic.g.ds.size()) return false;
	const DirSeg &d = s.traffic.g.ds[size_t(v.ds)];
	if (!d.ok || v.lane < 0 || v.lane >= d.lanes) return false;
	const auto origin = s.transit_run.stop_lanes.find(b.birth_stop);
	return origin != s.transit_run.stop_lanes.end() && origin->second.ds == v.ds && origin->second.lane == v.lane && origin->second.at == b.birth_stop_at;
}
bool birth_room(const CityState &s, const Bus &b, const Vehicle &v) {
	return bus_lane_room(s, v.ds * MAX_LANES + v.lane, v.s, b.birth_stop_at, false);
}
void materialize_birth(CityState &s, Vehicle &v, int id) {
	v.bus_birth_pending = false;
	traffic::micro_add(s.traffic, id); traffic::lane_add(s, v.ds * MAX_LANES + v.lane, id);
	traffic::refresh_vehicle_pose(s, id);
}
void queue_bus_attempt(CityState &s, const TransitLine &l, Bus &b, Vehicle &v);

class BusRouteRecipient final : public traffic::RouteRecipient {
public:
	BusRouteRecipient(const CityState &s, int id, const TransitLine &l, const Bus &b, const Vehicle &v, const StopLane *goal) :
		id_(id), from_(v.ds), conn_(v.conn), ri_(v.ri), line_(l.id), next_(b.next), dir_(b.dir),
		generation_(v.generation), request_(v.bus_route_request), route_(v.route.version), line_version_(s.transit.bus_route_version(l.id)),
		missing_(goal == nullptr), goal_ds_(goal ? goal->ds : -1), goal_at_(goal ? goal->at : 0) {}
	traffic::RecipientState state(const CityState &s) const override {
		const Vehicle *v = vehicle(s); if (!v) return traffic::RecipientState::OBSOLETE;
		const TransitRun &run = s.transit_run;
		if (!run.built || run.roads_seen != uint64_t(s.roads.version) || run.bus_seen != s.transit.bus_revision || run.bus_stops_seen != s.transit.bus_stops_revision)
			return traffic::RecipientState::WAIT_SOURCE;
		const Bus &b = run.buses[size_t(v->bus)];
		if (s.transit.bus_route_version(line_) != line_version_ || b.next != next_ || b.dir != dir_ || v->ds != from_ || v->conn != conn_ || v->ri != ri_ || v->route.version != route_)
			return traffic::RecipientState::OBSOLETE;
		if (v->bus_birth_pending && !birth_lane_valid(s, b, *v)) return traffic::RecipientState::OBSOLETE;
		return traffic::RecipientState::READY;
	}
	bool complete(CityState &s, traffic::SearchStatus status, RouteDraft &prepared) override {
		Vehicle *pv = vehicle(s); if (!pv) return true;
		Vehicle &v = *pv; Bus &b = s.transit_run.buses[size_t(v.bus)];
		const TransitLine *l = s.transit.line(b.line); if (!l || l->stops.empty()) { v.bus_route_request = 0; return true; }
		const bool current = state(s) == traffic::RecipientState::READY;
		if (status == traffic::SearchStatus::SOURCE_RETIRED || status == traffic::SearchStatus::TARGET_RETIRED || (status == traffic::SearchStatus::INVALIDATED && (!missing_ || !current))) {
			v.bus_route_request = 0;
			if (v.bus_birth_pending && (status == traffic::SearchStatus::SOURCE_RETIRED || !birth_lane_valid(s, b, v))) { traffic::despawn_vehicle(s, id_, false); return true; }
			if (v.ds < 0 || size_t(v.ds) >= s.traffic.g.ds.size() || !s.traffic.g.ds[size_t(v.ds)].ok) { v.bus_stop_at = -1; return true; }
			queue_bus_attempt(s, *l, b, v); return true; // retain goal/try count; waiting/invalidation is not a failed route
		}
		const bool found = status == traffic::SearchStatus::FOUND && prepared.size() >= (goal_ds_ == from_ ? 1 : 2);
		const bool last = !found && b.route_tries + 1 >= 2 * l->stops.size();
		if (v.bus_birth_pending && (found || last) && !birth_room(s, b, v)) return false; // no mutation while deferring placement
		v.bus_route_request = 0;
		if (found) {
			s.traffic.route_pool.adopt(v.route, prepared); v.ri = 0; v.goal_s = NO_GOAL_M;
			const auto &p = s.catalog.traffic; const DirSeg &d = s.traffic.g.ds[size_t(goal_ds_)];
			v.bus_stop_at = std::min(goal_at_ + p.classes[int(VehicleType::BUS)].length * 0.5f, d.len - d.stop_back - p.stop_margin_m);
			b.route_tries = 0;
		} else {
			++b.route_tries; turn_on(b, *l); // the last failed attempt also advances, as the original loop did
			if (!last) { queue_bus_attempt(s, *l, b, v); return true; }
			v.bus_stop_at = -1; b.route_tries = 0;
		}
		if (v.bus_birth_pending) materialize_birth(s, v, id_);
		return true;
	}
private:
	const Vehicle *vehicle(const CityState &s) const {
		if (id_ < 0 || size_t(id_) >= s.traffic.vehicles.size()) return nullptr;
		const Vehicle &v = s.traffic.vehicles[size_t(id_)];
		if (!v.alive || v.generation != generation_ || v.bus_route_request != request_ || v.bus < 0 || size_t(v.bus) >= s.transit_run.buses.size()) return nullptr;
		const Bus &b = s.transit_run.buses[size_t(v.bus)];
		return b.vehicle == id_ && b.line == line_ ? &v : nullptr;
	}
	Vehicle *vehicle(CityState &s) const { return vehicle(static_cast<const CityState &>(s)) ? &s.traffic.vehicles[size_t(id_)] : nullptr; }
	int id_, from_, conn_, ri_, line_, next_, dir_;
	uint64_t generation_, request_, route_, line_version_;
	bool missing_; int goal_ds_; float goal_at_;
};
void queue_bus_attempt(CityState &s, const TransitLine &l, Bus &b, Vehicle &v) {
	const int id = int(&v - s.traffic.vehicles.data());
	const auto stop = s.transit_run.stop_lanes.find(l.stops[size_t(b.next)]);
	const StopLane *goal = stop == s.transit_run.stop_lanes.end() ? nullptr : &stop->second;
	v.bus_route_request = next_generation();
	s.traffic.route_admission.submit(s, {traffic::RoutePurpose::BUS, id}, {v.ds, goal ? goal->ds : -1}, std::make_unique<BusRouteRecipient>(s, id, l, b, v, goal));
}
// One captured attempt at a time; missing/unreachable stops retain the original 2*stops order across admission slices.
void send_bus(CityState &s, const TransitLine &l, Bus &b, Vehicle &v) { b.route_tries = 0; queue_bus_attempt(s, l, b, v); }

int spawn_bus(CityState &s, const TransitLine &l, Bus &b) {
	const auto it = s.transit_run.stop_lanes.find(l.stops[size_t(b.next)]);
	if (it == s.transit_run.stop_lanes.end()) return -1;
	const TrafficParams &p = s.catalog.traffic;
	const StopLane &lane = it->second;
	const float len = p.classes[int(VehicleType::BUS)].length, start = std::max(len, lane.at - p.bus_approach_m);
	TrafficState &t = s.traffic;
	const int key = lane.ds * MAX_LANES + lane.lane;
	if (!bus_lane_room(s, key, start, lane.at, true)) return -1; // earlier pending births arbitrate like earlier proposals
	const std::vector<uint32_t> &colours = p.line_colours;
	const int colour = int(std::find(colours.begin(), colours.end(), l.colour) - colours.begin());
	const int id = traffic::new_vehicle(s, VehicleType::BUS, uint8_t(LINE_PAINT + (colour < int(colours.size()) ? colour : 0)));
	Vehicle &v = t.vehicles[size_t(id)];
	v.ds = lane.ds;
	v.lane = lane.lane;
	v.s = start;
	v.bus = int(&b - s.transit_run.buses.data());
	v.micro = false;
	v.bus_birth_pending = true; // identity/RNG reserved, no partially routed actor enters physics or publication
	b.birth_stop = l.stops[size_t(b.next)]; b.birth_stop_at = lane.at;
	t.vehicle_index.erase(id);
	t.birth_reservations.put(key, s.roads.segment_generation(lane.ds / 2), id, v.s, int(v.type));
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
	if (v.bus_birth_pending) return;
	if (!v.micro && b.dwell == 0) { // driven out of the view: on by the timetable
		traffic::despawn_vehicle(s, b.vehicle, true);
		return;
	}
	if (v.bus_route_request) return; // old usable route stays physical; Pending never advances the destination again
	const auto it = s.transit_run.stop_lanes.find(l->stops[size_t(b.next)]);
	if (it == s.transit_run.stop_lanes.end()) {
		turn_on(b, *l);
		send_bus(s, *l, b, v);
		return;
	}
	const bool here = v.conn < 0 && v.ds == it->second.ds && v.v < p.bus_stopped_ms && v.s >= v.bus_stop_at - p.bus_at_stop_m;
	if (!here) return;
	b.dwell += dt;
	const bool served = traffic::bus_call(s, b.vehicle, b.call, dt);
	if (!served || b.dwell < (is_end(s, *l, b) ? p.bus_terminal_s : p.bus_dwell_s)) return;
	b.dwell = 0;
	traffic::bus_call_over(s, b.call);
	s.transit_run.stops_called++;
	turn_on(b, *l);
	send_bus(s, *l, b, v);
}

// A line's buses: those it had stay where they are, more wait at its terminal, up to its number.
void place_buses(CityState &s, const TransitLine &l, std::vector<Bus> &was, const std::vector<size_t> *indices, std::vector<Bus> &out) {
	const TrafficParams &p = s.catalog.traffic;
	int served = 0, start = 0;
	for (size_t i = 0; i < l.stops.size(); i++) {
		served += s.transit_run.stop_lanes.count(l.stops[i]) ? 1 : 0;
		const TransitStop *q = s.transit.stop(l.stops[i]);
		if (q && q->terminal && start == 0) start = int(i);
	}
	if (served < 2) return;
	int k = 0;
	if (indices) for (size_t at : *indices) {
		if (k >= l.vehicles) break;
		Bus &b = was[at];
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

// Only the bus domain: existing fleet order within each line is retained, without a whole-fleet scan per line.
void rebuild_buses(CityState &s, bool roads_changed) {
	TransitRun &run = s.transit_run;
	std::vector<Bus> was = std::move(run.buses);
	std::unordered_map<int, std::vector<size_t>> by_line;
	by_line.reserve(s.transit.lines().size());
	for (size_t i = 0; i < was.size(); ++i) by_line[was[i].line].push_back(i);
	run.buses.clear();
	for (const TransitLine &l : s.transit.lines()) {
		if (l.mode != TransitMode::BUS) continue;
		const auto found = by_line.find(l.id);
		place_buses(s, l, was, found == by_line.end() ? nullptr : &found->second, run.buses);
	}
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
		const auto seen = run.bus_routes_seen.find(b.line);
		if (roads_changed || seen == run.bus_routes_seen.end() || seen->second != s.transit.bus_route_version(b.line))
			if (const TransitLine *l = s.transit.line(b.line)) send_bus(s, *l, b, v);
	}
	run.bus_routes_seen.clear();
	for (const TransitLine &l : s.transit.lines()) if (l.mode == TransitMode::BUS) run.bus_routes_seen.emplace(l.id, s.transit.bus_route_version(l.id));
}
void refresh_stop_lanes(CityState &s) {
	TransitRun &run = s.transit_run;
	run.stop_lanes.clear();
	for (const TransitStop &q : s.transit.stops()) {
		StopLane lane;
		if (q.mode == TransitMode::BUS && lane_beside(s, q, lane)) run.stop_lanes[q.id] = lane;
	}
	run.bus_stops_seen = s.transit.bus_stops_revision;
}
} // namespace

void step(CityState &s, float dt) {
	TransitRun &run = s.transit_run;
	const bool roads = !run.built || run.roads_seen != uint64_t(s.roads.version);
	const bool buses = !run.built || run.bus_seen != s.transit.bus_revision;
	const bool rails = !run.built || run.rail_seen != s.transit.rail_revision;
	if (roads || run.bus_stops_seen != s.transit.bus_stops_revision) refresh_stop_lanes(s);
	if (roads || buses) rebuild_buses(s, roads);
	if (roads || rails) place_rails(s, rails);
	run.bus_seen = s.transit.bus_revision;
	run.rail_seen = s.transit.rail_revision;
	run.roads_seen = uint64_t(s.roads.version);
	run.built = true;
	for (Bus &b : run.buses) {
		step_bus(s, b, dt);
		if (b.vehicle >= 0) s.traffic.vehicles[size_t(b.vehicle)].bus_doors = bus_door_share(s, b);
	}
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
		for (int k = v.ri + 1; traffic::vehicle_route_goal_current(s, v) && k < v.route.len && k <= v.ri + CROSSING_LOOK_LANES; k++) {
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
	traffic::bus_call_over(s, b.call);
	b.vehicle = -1;
	b.wait = p.bus_spacing_s; b.route_tries = 0;
	v.bus = -1;
	if (v.bus_birth_pending) return; // a cancelled unpublished birth has not travelled away from a stop
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
	const float shut_in = b.call.begun && b.dwell >= total ? p.kerb_door_open_s : total - b.dwell; // somebody still coming holds the door
	return std::clamp(std::min(b.dwell, shut_in) / p.kerb_door_open_s, 0.f, 1.f);
}

} // namespace transit
} // namespace city
