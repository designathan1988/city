// What a vehicle signals with its indicators (the lab's sim/vehicleSignals.ts indicatorSide), read off the simulation at each
// publication: the side of a lane change still being driven across, of the lane its next turn needs, of the turn ahead within
// the distance a driver signals it from, and of the turn on the connector it is driving until most of it is behind.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

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
	if (v.ri + 1 >= v.route.len) return v.stop_at >= 0 ? lane_side(v.lane, v.stop_lane) : AgentSignal::NONE;
	const int ci = pick_connector(s, v);
	if (ci < 0) return AgentSignal::NONE;
	const Conn &c = t.g.conns[size_t(ci)];
	// the lane its turn leaves from is another: it signals towards it until it gets there
	if (c.from_lane != v.lane) return lane_side(v.lane, c.from_lane);
	if (d.len - v.s > std::max(p.turn_signal_min_m, v.v * p.turn_signal_s)) return AgentSignal::NONE;
	return turn_side(c.turn);
}

} // namespace traffic
} // namespace city
