// The traffic.lane_graph invariant: every live connection sound (its lanes there, its conflicts and fork held by both sides with
// their places) and every node's list reaching exactly its own live connections. Whole: every connection and every list, and no
// live connection left on no list. Scoped: the connections on the listed nodes' lists (the nodes a drain just rebuilt). The rules
// themselves are the lane graph's own (sim/traffic/lane_graph_check.h), asked per record with nothing allocated.
#include "sim/checks/records.h"

#include "sim/state/city_state.h"
#include "sim/traffic/lane_graph_check.h"

namespace city::checks {

namespace {
constexpr const char *FEATURE = "traffic";
constexpr const char *CONNECTION = "lane_graph"; // a record: a connection's index in the pool
constexpr const char *NODE_LIST = "lane_graph_lists"; // a record: a node's id (NO_RECORD: a live connection on no list)
constexpr int NO_RECORD = -1;

// Node `n`'s list, and with `connections` the connections on it (a list with a fault is not walked again: it may not end).
// `listed` counts what the lists held.
void check_node(const LaneGraph &g, int n, size_t most, size_t &listed, bool connections, Broken &broken) {
	if (n < 0 || size_t(n) >= g.node_conn.size()) return;
	if (traffic::node_list_fault(g, n, most, listed)) {
		broken.add(FEATURE, NODE_LIST, n);
		return;
	}
	if (!connections) return;
	for (int c = g.node_conn[size_t(n)]; c >= 0; c = g.conns[size_t(c)].next_at_node)
		if (traffic::connection_fault(g, c)) broken.add(FEATURE, CONNECTION, c);
}
} // namespace

void check_lane_graph(const CityState &s, const CheckScope &scope, Broken &broken) {
	const LaneGraph &g = s.traffic.g;
	if (!scope.whole) {
		if (!scope.nodes) return;
		for (int n : *scope.nodes) {
			size_t listed = 0;
			check_node(g, n, g.conns.size(), listed, true, broken);
		}
		return;
	}
	size_t alive = 0, listed = 0;
	for (size_t ci = 0; ci < g.conns.size(); ci++) {
		if (!g.conns[ci].alive) continue;
		alive++;
		if (traffic::connection_fault(g, int(ci))) broken.add(FEATURE, CONNECTION, int(ci));
	}
	for (size_t n = 0; n < g.node_conn.size(); n++) check_node(g, int(n), alive, listed, false, broken);
	if (listed != alive) broken.add(FEATURE, NODE_LIST, NO_RECORD);
}

} // namespace city::checks
