// A lane change as a path (lane_shift.h), as the Roadcraft lab's params.ts and integrate.ts make it.
#include "sim/traffic/lane_shift.h"

#include "sim/state/city_state.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
// The quintic's own numbers (params.ts laneChangeOffset, laneChangeSlope: 10u^3 - 15u^4 + 6u^5 and its derivative 30u^2(1-u)^2).
constexpr float Q3 = 10.f, Q4 = 15.f, Q5 = 6.f, SLOPE = 30.f;
constexpr float LEAST_LENGTH = 1e-6f; // params.ts: a change is never of no length
constexpr float HALF = 0.5f;
constexpr int SPEED_HALVINGS = 12; // positioningSpeedCap: the fastest fitting speed bisected this many times

// The road `steps` changes of `across` metres each need at `speed` (laneChange.ts roomAt): each lane signalled for at a crawl.
float room_at(const TrafficParams &p, float across, int steps, float speed, float length) {
	const float change = shift_length(p, across, speed, length);
	return (std::max({ p.lc_least_room_m, speed * p.lc_change_time_s, change + p.lc_finish_margin_m }) + speed * p.lc_signal_crawl_s) * float(steps);
}

// The lanes between and the distance across to the next one, toward `to`.
int steps_to(const CityState &s, const Vehicle &v, int to, float &across) {
	const DirSeg &d = s.traffic.g.ds[size_t(v.ds)];
	const int next = v.lane + (to > v.lane ? 1 : -1);
	across = std::fabs(d.u[v.lane] - d.u[next]);
	return std::abs(to - v.lane);
}
} // namespace

bool position_fits(const CityState &s, const Vehicle &v, int to) {
	float across = 0;
	const int steps = steps_to(s, v, to, across);
	const TrafficParams &p = s.catalog.traffic;
	return s.traffic.g.ds[size_t(v.ds)].len - v.s >= room_at(p, across, steps, std::min(v.v, p.lc_crawl_ms), p.classes[int(v.type)].length);
}

float position_speed(const CityState &s, const Vehicle &v, int to) {
	float across = 0;
	const int steps = steps_to(s, v, to, across);
	const TrafficParams &p = s.catalog.traffic;
	const float length = p.classes[int(v.type)].length;
	const DirSeg &d = s.traffic.g.ds[size_t(v.ds)];
	const float left = d.len - v.s - v.v; // a second of travel in hand: the driver eases off before the room runs out
	float hi = std::max(v.v, d.speed);
	if (room_at(p, across, steps, hi, length) <= left) return hi;
	float lo = 0;
	for (int i = 0; i < SPEED_HALVINGS; i++) {
		const float mid = (lo + hi) * HALF;
		if (room_at(p, across, steps, mid, length) <= left) lo = mid;
		else hi = mid;
	}
	return std::max(lo, p.lc_crawl_ms);
}

bool way_on(CityState &s, int id, float line, float &v0, float &hold) {
	TrafficState &t = s.traffic;
	const Vehicle &v = t.vehicles[size_t(id)];
	if (pick_connector(s, v) >= 0) return true;
	const int to = lane_toward(s, v.ds, route_at(t.route_pool, v.route, v.ri + 1), v.lane);
	if (to < 0 || !position_fits(s, v, to)) return route_from_lane(s, id);
	v0 = std::min(v0, position_speed(s, v, to));
	hold = std::max(0.f, line - s.catalog.traffic.stop_margin_m);
	return true;
}

float shift_length(const TrafficParams &p, float across, float speed, float length) {
	const float t = std::clamp(speed / p.lc_fast_ms, 0.f, 1.f);
	const float heading = p.lc_heading_slow + (p.lc_heading_fast - p.lc_heading_slow) * t;
	return std::max({ p.lc_profile_peak * std::fabs(across) / std::tan(heading), std::max(0.f, speed) * p.lc_min_time_s, length * p.lc_min_bodies, LEAST_LENGTH });
}

float shift_offset(float start, float travelled, float length) {
	const float u = std::clamp(travelled / length, 0.f, 1.f);
	const float done = u * u * u * (Q3 - Q4 * u + Q5 * u * u);
	return u >= 1.f ? 0.f : start * (1.f - done);
}

float shift_slope(float start, float travelled, float length) {
	const float u = std::clamp(travelled / length, 0.f, 1.f);
	if (u >= 1.f) return 0.f;
	return -start * SLOPE * u * u * (1.f - u) * (1.f - u) / length;
}

float side_reach(const TrafficParams &p, const Vehicle &v) {
	const VehicleClass &c = p.classes[int(v.type)];
	const float angle = std::atan(std::fabs(shift_slope(v.lat_start, v.lat_done, v.lat_len)));
	return c.half_width * std::cos(angle) + c.length * HALF * std::sin(angle);
}

void shift_end(TrafficState &t, int id) {
	Vehicle &v = t.vehicles[size_t(id)];
	if (v.shadow_key >= 0 && size_t(v.shadow_key) < t.lane_shadows.size()) {
		std::vector<int> &list = t.lane_shadows[size_t(v.shadow_key)];
		list.erase(std::remove(list.begin(), list.end(), id), list.end());
	}
	v.shadow_key = -1;
	v.lat = v.lat_start = v.lat_done = v.lat_len = 0;
}

void shift_start(CityState &s, int id, int from_key, float across) {
	TrafficState &t = s.traffic;
	shift_end(t, id); // a change begins only once the last is over (lane_change.cpp): nothing of it is left to carry
	Vehicle &v = t.vehicles[size_t(id)];
	v.lat = v.lat_start = across;
	v.lat_done = 0;
	v.lat_len = shift_length(s.catalog.traffic, across, v.v, s.catalog.traffic.classes[int(v.type)].length);
	if (from_key < 0 || size_t(from_key) >= t.lane_shadows.size()) return;
	v.shadow_key = from_key;
	v.shadow_clear = std::fabs(across) * HALF;
	t.lane_shadows[size_t(from_key)].push_back(id);
}

void shift_step(CityState &s, int id, float ds) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	if (v.lat == 0) return;
	v.lat_done += ds;
	v.lat = shift_offset(v.lat_start, v.lat_done, v.lat_len);
	if (v.shadow_key >= 0 && (v.lat == 0 || std::fabs(v.lat) + side_reach(s.catalog.traffic, v) <= v.shadow_clear)) {
		std::vector<int> &list = t.lane_shadows[size_t(v.shadow_key)];
		list.erase(std::remove(list.begin(), list.end(), id), list.end());
		v.shadow_key = -1;
	}
	if (v.lat == 0) v.lat_start = v.lat_done = v.lat_len = 0;
}

} // namespace traffic
} // namespace city
