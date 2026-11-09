// What a vehicle signals and how its wheels sit (the lab's sim/vehicleSignals.ts indicatorSide and pathCurvature), read off the
// simulation at each publication: the side of a lane change still being driven across, of the lane it signals for before moving,
// of the lane its next turn needs, of the turn ahead within the distance a driver signals it from, and of the turn on the connector
// it is driving until most of it is behind; and the bend of the path under its front, which its front wheels steer by.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

// The second derivative of the lane change's quintic 10u^3 - 15u^4 + 6u^5 (lane_change_path.cpp): 60u - 180u^2 + 120u^3.
constexpr float QUINTIC_D2_A = 60.f, QUINTIC_D2_B = 180.f, QUINTIC_D2_C = 120.f;

AgentSignal turn_side(Turn turn) {
	if (turn == Turn::LEFT || turn == Turn::UTURN) return AgentSignal::LEFT;
	return turn == Turn::RIGHT ? AgentSignal::RIGHT : AgentSignal::NONE;
}

// The side lane `to` lies on from lane `from` of the same directed segment: the lanes count from the rightmost (lane_graph_types.h).
AgentSignal lane_side(int from, int to) {
	if (to == from) return AgentSignal::NONE;
	return to > from ? AgentSignal::LEFT : AgentSignal::RIGHT;
}

} // namespace

AgentSignal indicator_side(const CityState &s, const Vehicle &v) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	if (v.off) return AgentSignal::NONE;
	if (v.conn >= 0) {
		const Conn &c = t.g.conns[size_t(v.conn)];
		return v.cs < p.turn_signal_until * c.len ? turn_side(c.turn) : AgentSignal::NONE;
	}
	if (v.ds < 0 || v.ds >= int(t.g.ds.size())) return AgentSignal::NONE;
	const DirSeg &d = t.g.ds[size_t(v.ds)];
	// still driving across from the lane it left: lc_off is that lane's offset from this one, to the left when d.sign * lc_off > 0
	// (traffic_system.cpp ends), so it moves to the right
	const float left_of_lane = d.sign * v.lc_off;
	if (std::fabs(left_of_lane) > p.lane_change_done_m) return left_of_lane > 0 ? AgentSignal::RIGHT : AgentSignal::LEFT;
	if (v.lane_intent >= 0) return lane_side(v.lane, v.lane_intent); // signalling before it moves across (lane_change.cpp)
	if (v.ri + 1 >= v.route.len) return v.stop_at >= 0 ? lane_side(v.lane, v.stop_lane) : AgentSignal::NONE;
	const int ci = pick_connector(s, v);
	if (ci < 0) return AgentSignal::NONE;
	const Conn &c = t.g.conns[size_t(ci)];
	// the lane its turn leaves from is another: it signals towards it until it gets there
	if (c.from_lane != v.lane) return lane_side(v.lane, c.from_lane);
	if (d.len - v.s > std::max(p.turn_signal_min_m, v.v * p.turn_signal_s)) return AgentSignal::NONE;
	return turn_side(c.turn);
}

namespace {
// The turn from heading `a` to heading `b`, radians, positive to the left (a heading's left is (fz, -fx): turning to it, a x b < 0).
float turn(const Pose &a, const Pose &b) { return -std::atan2(a.fx * b.fz - a.fz * b.fx, a.fx * b.fx + a.fz * b.fz); }
} // namespace

float path_curvature(const CityState &s, const Vehicle &v) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Pose a, b;
	if (v.conn >= 0) {
		const Conn &c = t.g.conns[size_t(v.conn)];
		const float half = std::min(p.steer_chord_m, c.len * 0.5f);
		if (half <= 0) return 0.f;
		const float at = clampf(v.cs, half, c.len - half);
		conn_pose(c, at - half, a);
		conn_pose(c, at + half, b);
		return turn(a, b) / (2 * half);
	}
	if (v.ds < 0 || v.ds >= int(t.g.ds.size())) return 0.f;
	const DirSeg &d = t.g.ds[size_t(v.ds)];
	const float half = std::min(p.steer_chord_m, d.len * 0.5f);
	float k = 0;
	if (half > 0) {
		const float at = clampf(v.s, half, d.len - half);
		lane_pose(s, v.ds, v.lane, at - half, a);
		lane_pose(s, v.ds, v.lane, at + half, b);
		k = turn(a, b) / (2 * half);
	}
	// the lane change's own bend: the offset to its left (d.sign * lc_off) changes over the road as the quintic does, d2/ds2
	if (v.lc_off != 0 && v.lc_len > 0) {
		const float u = clampf(v.lc_driven / v.lc_len, 0.f, 1.f);
		k += d.sign * -v.lc_start * (QUINTIC_D2_A * u - QUINTIC_D2_B * u * u + QUINTIC_D2_C * u * u * u) / (v.lc_len * v.lc_len);
	}
	return k;
}

} // namespace traffic
} // namespace city
