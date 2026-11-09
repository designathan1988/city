// A lane change is a path, not a slide (the lab's sim/params.ts laneChangeLength/Offset and laneChange.ts positioningSpeedCap): the
// sideways offset still to cover follows a quintic in the road driven, eased at both ends, over a length set by the heading a driver
// takes at its speed; and a driver who must still cross lanes before its turn eases off to the speed at which the changes fit.
#include "sim/traffic/traffic_system.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

// The quintic 10u^3 - 15u^4 + 6u^5 (zero slope and curvature at both ends): its steepest slope is 15/8 of its mean.
constexpr float QUINTIC_PEAK = 1.875f, Q10 = 10.f, Q15 = 15.f, Q6 = 6.f;
constexpr float LEAST_LENGTH_M = 1e-3f; // a path is never shorter (it divides)
constexpr int POSITION_BISECTIONS = 12; // the positioning speed to a 4000th of the limit

// The share of the offset covered after `u` of the path.
float covered(float u) {
	u = clampf(u, 0.f, 1.f);
	return u * u * u * (Q10 - Q15 * u + Q6 * u * u);
}

// The road a change covering `across` metres sideways is driven over at `speed`: the heading eases from lc_heading_slow to
// lc_heading_fast as the speed rises to lc_fast_kmh; never under lc_min_time_s of travel nor lc_min_bodies body lengths.
float path_length(const TrafficParams &p, float across, float speed, float body) {
	const float t = clampf(speed / (p.lc_fast_kmh / KMH_PER_MS), 0.f, 1.f);
	const float heading = p.lc_heading_slow + (p.lc_heading_fast - p.lc_heading_slow) * t;
	return std::max({ QUINTIC_PEAK * std::fabs(across) / std::tan(heading), std::max(0.f, speed) * p.lc_min_time_s, body * p.lc_min_bodies, LEAST_LENGTH_M });
}

// The road `steps` changes of `across` metres each need at `speed`: each signalled for, eased across and finished clear of the line.
float room_for(const TrafficParams &p, float across, int steps, float speed, float body) {
	const float one = std::max({ p.lc_room_min_m, speed * p.lc_room_s, path_length(p, across, speed, body) + p.lc_finish_m });
	return (one + speed * p.lc_signal_crawl_s) * float(steps);
}

} // namespace

void start_lane_change(const CityState &s, Vehicle &v, float offset) {
	const TrafficParams &p = s.catalog.traffic;
	v.lc_start = v.lc_off = offset;
	v.lc_driven = 0;
	v.lc_len = path_length(p, offset, v.v, p.classes[int(v.type)].length);
}

void drive_lane_change(Vehicle &v, float metres) {
	if (v.lc_off == 0) return;
	v.lc_driven += metres;
	v.lc_off = v.lc_driven >= v.lc_len ? 0.f : v.lc_start * (1 - covered(v.lc_driven / v.lc_len));
}

float lane_change_offset(const Vehicle &v, float behind) {
	if (v.lc_off == 0) return 0.f;
	const float driven = v.lc_driven - behind;
	return driven >= v.lc_len ? 0.f : v.lc_start * (1 - covered(driven / v.lc_len));
}

float positioning_cap(const CityState &s, const Vehicle &v, int target_lane, float to_line) {
	const TrafficParams &p = s.catalog.traffic;
	const DirSeg &d = s.traffic.g.ds[size_t(v.ds)];
	const int steps = std::abs(target_lane - v.lane);
	if (steps == 0) return NO_LEADER;
	const int next = v.lane + (target_lane > v.lane ? 1 : -1);
	const float across = std::fabs(d.u[v.lane] - d.u[next]), body = p.classes[int(v.type)].length;
	const float left = to_line - v.v; // a second of travel in hand: it eases off before the room runs out
	const float top = std::max(v.v, d.speed);
	if (room_for(p, across, steps, top, body) <= left) return NO_LEADER;
	float lo = 0, hi = top; // the room falls with the speed: the fastest that fits
	for (int i = 0; i < POSITION_BISECTIONS; i++) {
		const float mid = (lo + hi) * 0.5f;
		(room_for(p, across, steps, mid, body) <= left ? lo : hi) = mid;
	}
	return std::max(lo, p.lc_crawl_ms); // never under a crawl: a change that needs less is not made
}

} // namespace traffic
} // namespace city
