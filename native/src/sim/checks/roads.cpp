// The roads.links_on_arms invariant: every hand link and ban a live node holds (RoadNode::links) names two of its own arms, both
// alive. A link left on a road gone would be inherited by the next road that takes the id (road_network.h ids are reused), so
// j_remove_segment drops it and a road laid again carries it (sim/commands/road_refs.h). Whole: every node. Scoped: the nodes
// listed (a command logs both nodes of every road it lays, removes or reprofiles, and every node whose links it writes). A record:
// the node's id. Cost O(links of the nodes looked at), each a lookup in the node's short list of arms; nothing allocated.
#include "sim/checks/records.h"

#include "sim/state/city_state.h"

#include <algorithm>

namespace city::checks {

namespace {
constexpr const char *FEATURE = "roads";
constexpr const char *LINKS_ON_ARMS = "links_on_arms";

bool arm(const RoadNetwork &net, const RoadNode &n, int seg) {
	return seg >= 0 && size_t(seg) < net.segs.size() && net.segs[size_t(seg)].alive && std::find(n.segs.begin(), n.segs.end(), seg) != n.segs.end();
}

void check_node(const RoadNetwork &net, int id, Broken &broken) {
	if (id < 0 || size_t(id) >= net.nodes.size()) return;
	const RoadNode &n = net.nodes[size_t(id)];
	if (!n.alive) return; // a node made again starts with no links (RoadNetwork::add_node)
	for (const LaneLink &l : n.links)
		if (!arm(net, n, l.from) || !arm(net, n, l.to)) {
			broken.add(FEATURE, LINKS_ON_ARMS, id);
			return;
		}
}
} // namespace

void check_roads(const CityState &s, const CheckScope &scope, Broken &broken) {
	const RoadNetwork &net = s.roads;
	if (scope.whole) {
		for (size_t id = 0; id < net.nodes.size(); id++) check_node(net, int(id), broken);
		return;
	}
	if (scope.nodes)
		for (int id : *scope.nodes) check_node(net, id, broken);
}

} // namespace city::checks
