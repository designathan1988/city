// Demand and spawning (Roadcraft traffic.ts:954-1047): the fleet target follows lane-metres x density x the day
// profile; trips start and end on directed segments weighted by the zones painted beside them (home -> work in the
// morning, work -> home in the evening), 200-7000 m apart with an exponential preference for short trips.
#include "sim/traffic/kerb_stops.h"

#include "core/units.h"

#include <cmath>

namespace city {
namespace traffic {

namespace {


const float *weights_now(const TrafficParams &p, float hour, bool origin) {
	if (hour >= p.morning_from_h && hour < p.morning_to_h) return origin ? p.zone_home : p.zone_work;
	if (hour >= p.evening_from_h && hour < p.evening_to_h) return origin ? p.zone_work : p.zone_home;
	return p.zone_mid;
}

float seg_weight(const CityState &s, int seg, const float *w) {
	const TrafficParams &p = s.catalog.traffic;
	const RoadSegment &g = s.roads.segs[seg];
	float best = std::max(w[s.catalog.zone(g.zl).base % BASE_ZONES], w[s.catalog.zone(g.zr).base % BASE_ZONES]);
	return best * std::min(1.f, float(g.len) / p.demand_side_reach_m);
}

int pick_ds(CityState &s, const float *w) {
	const TrafficParams &p = s.catalog.traffic;
	TrafficState &t = s.traffic;
	const int n = int(t.g.ds.size());
	int last = -1;
	for (int k = 0; k < p.demand_pick_tries && n > 0; k++) {
		int ds = int(t.rng.next() % uint32_t(n));
		if (!t.g.ds[ds].ok) ds ^= 1;
		if (!t.g.ds[ds].ok) continue;
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

// One trip attempt; returns false on failure (no destination, no route or no room).
bool spawn_one(CityState &s) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	VehicleType type = draw_type(s);
	bool truck = type == VehicleType::TRUCK;
	const bool bike = type == VehicleType::BICYCLE;
	float hour = float(s.clock.hour());
	int from = pick_ds(s, truck ? p.zone_truck : weights_now(p, hour, true));
	if (from < 0 || (bike && !t.g.ds[size_t(from)].bikes)) return false;
	V2 a = mid_of(s, from);
	int to = -1;
	for (int k = 0; k < p.demand_dest_tries && to < 0; k++) {
		int c = pick_ds(s, truck ? p.zone_truck : weights_now(p, hour, false));
		if (c < 0) continue;
		float d = (mid_of(s, c) - a).len();
		if (d > p.trip_min_m && d < p.trip_max_m && t.rng.f() < std::exp(-d / p.trip_decay_m)) to = c;
	}
	if (to < 0 || to == from) return false;
	// Room first: a spawn that cannot fit must not pay for a route search.
	const VehicleClass &vc = p.classes[int(type)];
	const DirSeg &d = t.g.ds[from];
	int lane = bike ? 0 : int(t.rng.next() % uint32_t(d.lanes)); // a bicycle keeps to the edge of the carriageway
	float at = t.rng.range(p.demand_place_lo, p.demand_place_hi) * d.len;
	const int key = from * MAX_LANES + lane;
	const float stop = d.len - d.stop_back - p.stop_margin_m; // it starts short of its stop line, able to stop there
	if (at < vc.length + p.mouth_clear_m || at > stop || !lane_free(s, key, at, vc.length)) return false; // keep the lane mouth clear
	if (float(lane_load(t, key) + 1) > d.len / p.slot_m) return false; // the lane is full, far vehicles included
	std::vector<int> route;
	if (!find_route(s, from, to, route, -1, -1, bike) || route.size() < 2) return false;
	const int id = new_vehicle(s, type, uint8_t(t.rng.next() % PAINTS));
	Vehicle &v = t.vehicles[size_t(id)];
	assign_errand(s, v, id);
	v.ds = from;
	v.lane = lane;
	v.s = at;
	v.v = std::min(d.speed * v.vf * p.demand_start_speed_share, std::sqrt(2 * vc.b * (stop - at)));
	route_set(t.route_pool, v.route, t.route_waste, route.data(), int(route.size()));
	v.goal_s = t.rng.range(p.demand_goal_lo, p.demand_goal_hi) * t.g.ds[to].len;
	// micro only close to the camera; farther out the trip starts in the far model (and is drawn if its lane is in
	// the window, which cannot happen for a segment beyond micro_spawn_m < meso_window_m, but the rule stays explicit)
	const V2 mid = mid_of(s, from);
	const float dx = mid.x - s.camera_x.load(), dz = mid.y - s.camera_z.load();
	if (dx * dx + dz * dz < p.micro_spawn_m * p.micro_spawn_m) {
		v.micro = false; // micro_add registers it in micro_list
		micro_add(t, id);
		lane_add(s, key, id);
		return true;
	}
	meso_enter(s, id, key, true);
	if (in_window(s, from)) materialize(s, key);
	return true;
}

} // namespace

int new_vehicle(CityState &s, VehicleType type, uint8_t paint) {
	const TrafficParams &p = s.catalog.traffic;
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
	v.vf = s.catalog.traffic.classes[int(type)].speed_share * (p.demand_vf_base + p.demand_vf_span * t.rng.f());
	v.drift = t.rng.range(0.f, TAU_F);
	// the driver's temper from a stream of its own (the vehicle and the tick): drawing it leaves the shared stream, which places
	// trips and walkers, as it was
	Rng own(uint64_t(hash_combine(uint32_t(id), uint32_t(t.tick))) | (TRAFFIC_SEED << DRIVER_SEED_SHIFT));
	draw_driver(p, v, own);
	t.vehicle_count++;
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
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	const float *w = v.type == VehicleType::TRUCK ? p.zone_truck : weights_now(p, float(s.clock.hour()), false);
	const V2 a = mid_of(s, from_ds);
	for (int k = 0; k < p.demand_dest_tries; k++) {
		const int c = pick_ds(s, w);
		if (c < 0 || c == from_ds) continue;
		const float d = (mid_of(s, c) - a).len();
		if (d <= p.trip_min_m || d >= p.trip_max_m || t.rng.f() >= std::exp(-d / p.trip_decay_m)) continue;
		std::vector<int> route;
		if (!find_route(s, from_ds, c, route, -1, -1, v.type == VehicleType::BICYCLE) || route.size() < 2) return false;
		route_set(t.route_pool, v.route, t.route_waste, route.data(), int(route.size()));
		v.ri = 0;
		v.goal_s = t.rng.range(p.demand_goal_lo, p.demand_goal_hi) * t.g.ds[size_t(c)].len;
		return true;
	}
	return false;
}

double fleet_target(const CityState &s) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	return std::min(double(p.demand_fleet_max), t.g.lane_metres * p.density_per_lane_m * day_factor(p, float(s.clock.hour())) * p.traffic_share * s.traffic_levels.traffic * s.traffic_levels.population * s.traffic_levels.demand * t.gov);
}

float day_factor(const TrafficParams &p, float h) {
	auto g = [h](float c, float w) { float d = h - c; return std::exp(-d * d / (2 * w * w)); };
	float f = p.day_base + p.day_morning_amp * g(p.day_morning_h, p.day_morning_w) + p.day_evening_amp * g(p.day_evening_h, p.day_evening_w) +
			p.day_midday_amp * g(p.day_midday_h, p.day_midday_w);
	if (h < p.day_night_to_h || h > p.day_night_from_h) f -= p.day_night_cut;
	return std::max(0.f, f);
}

void spawn_vehicles(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const double target = fleet_target(s);
	int attempts = 0;
	// a car driving in or out of a bay is a car driving: the demand counts it (it just is not on a lane)
	const double driving = double(t.vehicle_count) + double(t.park.moving);
	if (driving < p.fill_share * target) {
		attempts = int(p.spawn_fill_attempts);
	} else {
		t.spawn_credit += float(std::max(0.0, target - driving)) * dt / p.fleet_tau_s;
		attempts = std::min(int(t.spawn_credit), int(p.spawn_attempts_per_step));
		t.spawn_credit -= float(attempts);
	}
	int failures = 0;
	// a new trip's search can wait: past the tick's searches the rest of the attempts are not made (route_admission.cpp)
	for (int k = 0; k < attempts && failures < p.demand_max_failures && may_request_search(s); k++)
		if (!spawn_one(s)) failures++;
}

} // namespace traffic
} // namespace city
