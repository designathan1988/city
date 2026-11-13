// The flow invariants, over what the traffic measures at the junctions and the control it chooses for them (sim/traffic/
// flow_control.h), checked once the traffic has drained the change log (its drain forgets what a removed road or node keyed:
// flow_control.cpp forget_flows):
// - flow.stats_on_live_legs: every node the flows are kept for (FlowStats::rates, counts, held) is alive, and every leg they are kept
//   for is one of its arms. A record: the node's id.
// - flow.advisor_on_live_nodes: every node with a choice or a change time (ControlAdvisor::choices, changed_at) or still advised
//   (FlowControl::advised) is alive. A record: the node's id.
// A node freed and made again before one drain passes both (only an identity on the ids would tell it). Whole or scoped alike: only
// the junctions counted in the micro window are keyed, so the pass is O(keys), each leg a lookup in its node's short list of arms;
// nothing allocated.
#include "sim/checks/records.h"

#include "sim/state/city_state.h"

#include <algorithm>

namespace city::checks {

namespace {
constexpr const char *FEATURE = "flow";
constexpr const char *STATS_ON_LIVE_LEGS = "stats_on_live_legs";
constexpr const char *ADVISOR_ON_LIVE_NODES = "advisor_on_live_nodes";

const RoadNode *alive_node(const RoadNetwork &net, int64_t n) {
	if (n < 0 || size_t(n) >= net.nodes.size()) return nullptr;
	return net.nodes[size_t(n)].alive ? &net.nodes[size_t(n)] : nullptr;
}

// Every node of a table keyed by node then leg alive, and every leg one of its arms.
template <typename Legs>
void legs_on_arms(const RoadNetwork &net, const js::Map<int64_t, Legs> &by_node, Broken &broken) {
	for (const auto &kept : by_node) {
		const RoadNode *node = alive_node(net, kept.first);
		bool sound = node != nullptr;
		if (sound)
			for (const auto &leg : kept.second)
				sound = sound && std::find(node->segs.begin(), node->segs.end(), int(leg.first)) != node->segs.end();
		if (!sound) broken.add(FEATURE, STATS_ON_LIVE_LEGS, int(kept.first));
	}
}

// Every key of a table keyed by node alive.
template <typename Table>
void nodes_live(const RoadNetwork &net, const Table &by_node, const char *check, Broken &broken) {
	for (const auto &kept : by_node)
		if (!alive_node(net, kept.first)) broken.add(FEATURE, check, int(kept.first));
}
} // namespace

void check_flow(const CityState &s, const CheckScope &, Broken &broken) {
	const RoadNetwork &net = s.roads;
	const FlowControl &c = s.traffic.flow_control;
	const FlowStats &f = *c.flow;
	legs_on_arms(net, f.rates, broken);
	legs_on_arms(net, f.counts, broken);
	nodes_live(net, f.held, STATS_ON_LIVE_LEGS, broken);
	const ControlAdvisor &a = *c.advisor;
	nodes_live(net, a.choices, ADVISOR_ON_LIVE_NODES, broken);
	nodes_live(net, a.changed_at, ADVISOR_ON_LIVE_NODES, broken);
	for (int64_t n : c.advised)
		if (!alive_node(net, n)) broken.add(FEATURE, ADVISOR_ON_LIVE_NODES, int(n));
}

} // namespace city::checks
