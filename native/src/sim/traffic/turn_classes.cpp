// How a movement through a junction turns, and the lanes that may take it (lane_graph.cpp link_node asks both): straight on, a
// right turn, a left turn, a turn back. A movement is straight along the road the junction is on (that road keeps its lanes across
// however it bends), at a joint of two roads, or within traffic.json straight_dot of straight on; it is a turn back down its own
// segment or onto another that points nearly back (past turn_back_dot). The reference game's limits, 30 and 150 degrees.
#include "sim/traffic/lane_graph_internal.h"

namespace city {
namespace traffic {

Turn turn_class(const V2 &h, const V2 &o, bool same, bool carried, int degree, const TrafficParams &tp) {
	if (same) return Turn::UTURN;
	if (degree == 2 || carried || h.dot(o) > tp.straight_dot) return Turn::STRAIGHT;
	if (h.dot(o) < tp.turn_back_dot) return Turn::UTURN;
	return o.dot(V2(h.y, -h.x)) > 0 ? Turn::LEFT : Turn::RIGHT;
}

bool lane_allows(int lane, int n, Turn t) {
	if (t == Turn::UTURN) return lane == n - 1;
	if (n == 1 || t == Turn::STRAIGHT) return true;
	return t == Turn::RIGHT ? lane == 0 : lane == n - 1;
}

} // namespace traffic
} // namespace city
