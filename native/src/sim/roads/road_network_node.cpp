// What a node is for those who go through it: a crossing between junctions, a U-turn, a banned movement, crosswalks.
#include "sim/roads/road_network.h"

#include <algorithm>

namespace city {

bool RoadNetwork::mid_block(int node) const {
	const RoadNode &n = nodes[size_t(node)];
	return n.crossing != CROSSING_NONE && n.segs.size() == 2;
}

int RoadNetwork::crossing_arm(int node) const {
	const RoadNode &n = nodes[size_t(node)];
	return std::min(n.segs[0], n.segs[1]);
}

int RoadNetwork::uturn_arm(int node) const {
	const RoadNode &n = nodes[size_t(node)];
	if (n.uturn == UTURN_NONE || n.segs.size() != 2) return -1;
	return n.uturn == UTURN_FROM_LOW ? std::min(n.segs[0], n.segs[1]) : std::max(n.segs[0], n.segs[1]);
}

bool RoadNetwork::movement_banned(int node, int from, int to) const {
	for (const LaneLink &l : nodes[size_t(node)].links)
		if (l.from_lane == NO_LANE && l.from == from && l.to == to) return true;
	return false;
}

bool RoadNetwork::crosswalk_on(int node, int seg) const {
	return has_crosswalks(node) && (!mid_block(node) || seg == crossing_arm(node));
}

bool RoadNetwork::has_crosswalks(int node) const {
	const RoadNode &n = nodes[size_t(node)];
	if (int(n.segs.size()) < CROSSWALK_ARMS && !mid_block(node)) return false;
	for (int seg : n.segs) {
		const Profile &p = profile_of(seg);
		if (p.ring || p.rail) return false;
	}
	return true;
}

} // namespace city
