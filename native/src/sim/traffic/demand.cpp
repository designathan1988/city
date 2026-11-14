// The demand: what the streets carry follows lane metres x density x the day's profile (demand_for; the far traffic's flows read
// it by street, flow_rates.cpp); trips start and end on directed segments weighted by the zones beside them (home to work in the
// morning, work to home in the evening), with an exponential preference for short trips. A vehicle of the window is made here
// (make_vehicle) when the flows owe it one, with a trip of its own: out of the window by one of its ways out, or to a street
// inside.
#include "sim/traffic/kerb_stops.h"

#include "sim/traffic/lane_closures.h"

#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr int PICK_TRIES = 24, DEST_TRIES = 16, OUT_TRIES = 4, FLEET_MAX = 65536;
constexpr float SIDE_REACH_M = 120.f; // a segment counts fully once it is this long
constexpr float VF_BASE = 0.92f, VF_FASTEST = 1.08f; // a driver's share of its class's speed, slowest and fastest
constexpr float PLACE_LO = 0.1f, PLACE_HI = 0.9f, GOAL_LO = 0.2f, GOAL_HI = 0.8f;
constexpr float START_SPEED_SHARE = 0.6f;

const float *weights_now(const TrafficParams &p, float hour, bool origin) {
	if (hour >= p.morning_from_h && hour < p.morning_to_h) return origin ? p.zone_home : p.zone_work;
	if (hour >= p.evening_from_h && hour < p.evening_to_h) return origin ? p.zone_work : p.zone_home;
	return p.zone_mid;
}

float seg_weight(const CityState &s, int seg, const float *w) {
	const RoadSegment &g = s.roads.segs[seg];
	float best = std::max(w[s.catalog.zone(g.zl).base % BASE_ZONES], w[s.catalog.zone(g.zr).base % BASE_ZONES]);
	return best * std::min(1.f, float(g.len) / SIDE_REACH_M);
}

// A directed segment of the window (where trips start and end: the traffic round the view is the window's), drawn by the zones
// beside it at this hour.
int pick_ds(CityState &s, const float *w) {
	TrafficState &t = s.traffic;
	const int n = int(t.window_ds.size());
	int last = -1;
	for (int k = 0; k < PICK_TRIES && n > 0; k++) {
		const int ds = t.window_ds[size_t(t.rng.next() % uint32_t(n))];
		if (ds >= int(t.g.ds.size()) || !t.g.ds[size_t(ds)].ok) continue;
		bool leaves = false;
		each_conn_from(t.g, ds, [&](int) { leaves = true; });
		if (!leaves) continue;
		last = ds;
		if (t.rng.f() < seg_weight(s, ds >> 1, w)) return ds;
	}
	return last;
}

V2 mid_of(const CityState &s, int ds) {
	Pose p;
	lane_pose(s, ds, 0, s.traffic.g.ds[ds].len * 0.5f, p);
	return V2(p.x, p.z);
}

VehicleType draw_type(CityState &s) {
	const TrafficParams &p = s.catalog.traffic;
	if (s.traffic.rng.f() < p.bike_share) return VehicleType::BICYCLE;
	float r = s.traffic.rng.f();
	if (r >= p.car_share) return r < p.car_share + p.bus_share ? VehicleType::BUS : VehicleType::TRUCK;
	float x = s.traffic.rng.f(), acc = 0;
	for (int k = 0; k < CAR_STYLES; k++) {
		acc += p.style_share[k];
		if (x < acc) return VehicleType(k);
	}
	return VehicleType::SEDAN;
}

// True when a vehicle of length `len` fits on the lane at `at` without touching anyone.
bool lane_free(const CityState &s, int key, float at, float len) {
	const TrafficState &t = s.traffic;
	for (int id : t.lane_vehicles[key]) {
		const Vehicle &o = t.vehicles[id];
		const VehicleClass &oc = s.catalog.traffic.classes[int(o.type)];
		if (at > o.s - oc.length - oc.min_gap && at - len - oc.min_gap < o.s) return false;
	}
	return true;
}

// The route of a new trip from `from`, into `route`, and where on its last street it ends (`goal`: its share of that street's
// length): out of the window by one of its ways out, with the chance a trip is as long as the way there (exp(-d / trip_decay_m):
// the through traffic; `leaving`: whatever the way's length), else to a street inside by the zones at this hour. False: no
// destination, or no way to it.
bool draw_trip(CityState &s, VehicleType type, int from, bool leaving, std::vector<int> &route, float &goal) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const V2 a = mid_of(s, from);
	for (int k = 0; k < OUT_TRIES && !t.window_out_ds.empty(); k++) {
		const int out = t.window_out_ds[size_t(t.rng.next() % uint32_t(t.window_out_ds.size()))];
		if (out == from || size_t(out) >= t.g.ds.size() || !t.g.ds[size_t(out)].ok) continue;
		if (!leaving && t.rng.f() >= std::exp(-(mid_of(s, out) - a).len() / p.trip_decay_m)) continue;
		if (!find_route(s, from, out, route, type) || route.size() < 2) continue;
		goal = 1; // it drives the way out to its end: the far traffic takes it on that street
		return true;
	}
	const float *w = type == VehicleType::TRUCK ? p.zone_truck : weights_now(p, float(s.clock.hour()), false);
	for (int k = 0; k < DEST_TRIES; k++) {
		const int c = pick_ds(s, w);
		if (c < 0 || c == from) continue;
		const float d = (mid_of(s, c) - a).len();
		if (d <= p.trip_min_m || d >= p.trip_max_m || t.rng.f() >= std::exp(-d / p.trip_decay_m)) continue;
		if (!find_route(s, from, c, route, type) || route.size() < 2) return false;
		goal = t.rng.range(GOAL_LO, GOAL_HI);
		return true;
	}
	return false;
}

} // namespace

float side_weight(const CityState &s, int seg, bool origin) {
	return seg_weight(s, seg, weights_now(s.catalog.traffic, float(s.clock.hour()), origin));
}

int make_vehicle(CityState &s, int ds, bool along) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	if (t.vehicle_count >= FLEET_MAX || size_t(ds) >= t.g.ds.size() || !t.g.ds[size_t(ds)].ok) return -1;
	const VehicleType type = draw_type(s);
	const bool bike = type == VehicleType::BICYCLE;
	const DirSeg &d = t.g.ds[size_t(ds)];
	if (bike && !d.bikes) return -1;
	// Room first: a vehicle that cannot fit must not pay for a route search.
	const VehicleClass &vc = p.classes[int(type)];
	const int lane = bike ? 0 : int(t.rng.next() % uint32_t(d.lanes)); // a bicycle keeps to the edge of the carriageway
	const float at = along ? t.rng.range(PLACE_LO, PLACE_HI) * d.len : vc.length + p.mouth_clear_m;
	const int key = ds * MAX_LANES + lane;
	const float stop = d.len - d.stop_back - p.stop_margin_m; // it starts short of its stop line, able to stop there
	if (at < vc.length + p.mouth_clear_m || at > stop || !lane_free(s, key, at, vc.length)) return -1; // the lane's mouth kept clear
	if (stretch_held(t, key, at - vc.length - vc.min_gap, at + vc.min_gap)) return -1; // a body off the lanes holds it
	if (float(lane_load(t, key) + 1) > d.len / p.slot_m) return -1; // the lane is full, the buses' far queue included
	std::vector<int> route;
	float goal = 1;
	if (!draw_trip(s, type, ds, false, route, goal)) return -1;
	const int id = new_vehicle(s, type, uint8_t(t.rng.next() % PAINTS));
	Vehicle &v = t.vehicles[size_t(id)];
	assign_errand(s, v, id);
	v.ds = ds;
	v.lane = lane;
	v.s = at;
	v.v = std::min(d.speed * v.vf * START_SPEED_SHARE, std::sqrt(2 * vc.b * (stop - at)));
	route_set(t.route_pool, v.route, t.route_waste, route.data(), int(route.size()));
	v.goal_s = goal * t.g.ds[size_t(route.back())].len;
	micro_add(t, id); // registers it in micro_list
	lane_add(s, key, id);
	return id;
}

int new_vehicle(CityState &s, VehicleType type, uint8_t paint) {
	TrafficState &t = s.traffic;
	int id;
	if (!t.free_vehicles.empty()) {
		id = t.free_vehicles.back();
		t.free_vehicles.pop_back();
	} else {
		t.vehicles.emplace_back();
		id = int(t.vehicles.size()) - 1;
	}
	Vehicle &v = t.vehicles[size_t(id)];
	v = Vehicle();
	v.alive = true;
	v.micro = false; // micro_add registers it in micro_list
	v.type = type;
	v.paint = paint;
	v.vf = s.catalog.traffic.classes[int(type)].speed_share * (VF_BASE + (VF_FASTEST - VF_BASE) * t.rng.f());
	v.drift = t.rng.range(0.f, TAU_F);
	v.temper = t.rng.f() + t.rng.f() - 1; // two draws: most drivers ordinary, the timid and the pushy rare
	t.vehicle_count++;
	return id;
}

int vehicle_on_lane(CityState &s, VehicleType type, int ds, int lane, float at, float speed, const std::vector<int> &route) {
	TrafficState &t = s.traffic;
	const VehicleClass &vc = s.catalog.traffic.classes[int(type)];
	const int key = ds * MAX_LANES + lane;
	if (route.size() < 2 || route[0] != ds || at < vc.length || !lane_free(s, key, at, vc.length)) return -1;
	if (stretch_held(t, key, at - vc.length - vc.min_gap, at + vc.min_gap)) return -1;
	const int id = new_vehicle(s, type, 0);
	Vehicle &v = t.vehicles[size_t(id)];
	v.ds = ds;
	v.lane = lane;
	v.s = at;
	v.v = speed;
	route_set(t.route_pool, v.route, t.route_waste, route.data(), int(route.size()));
	v.goal_s = t.g.ds[size_t(route.back())].len * GOAL_HI;
	micro_add(t, id);
	lane_add(s, key, id);
	return id;
}

void free_vehicle(TrafficState &t, int id) {
	Vehicle &v = t.vehicles[size_t(id)];
	t.route_waste += size_t(v.route.cap);
	v = Vehicle();
	t.free_vehicles.push_back(id);
}

bool new_trip(CityState &s, int id, int from_ds) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	// a car seen at its trip's end drives on out of the window, by one of its ways out; with none (the window holds the whole
	// town) to a street inside
	std::vector<int> route;
	float goal = 1;
	if (!draw_trip(s, v.type, from_ds, true, route, goal)) return false;
	route_set(t.route_pool, v.route, t.route_waste, route.data(), int(route.size()));
	v.ri = 0;
	v.goal_s = goal * t.g.ds[size_t(route.back())].len;
	return true;
}

double demand_for(const CityState &s, double lane_metres) {
	const TrafficParams &p = s.catalog.traffic;
	return lane_metres * p.density_per_lane_m * day_factor(p, float(s.clock.hour())) * p.traffic_share * s.traffic_levels.traffic * s.traffic_levels.population * s.traffic_levels.demand * s.traffic.gov;
}

double fleet_target(const CityState &s) {
	return std::min(double(FLEET_MAX), demand_for(s, s.traffic.window_lane_m));
}

float day_factor(const TrafficParams &p, float h) {
	auto g = [h](float c, float w) { float d = h - c; return std::exp(-d * d / (2 * w * w)); };
	float f = p.day_base + p.day_morning_amp * g(p.day_morning_h, p.day_morning_w) + p.day_evening_amp * g(p.day_evening_h, p.day_evening_w) +
			p.day_midday_amp * g(p.day_midday_h, p.day_midday_w);
	if (h < p.day_night_to_h || h > p.day_night_from_h) f -= p.day_night_cut;
	return std::max(0.f, f);
}

} // namespace traffic
} // namespace city
