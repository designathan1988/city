// What the lane graph must always be, checked where it changes (the debug DLL, after every network change): a connection joins
// live lanes at a live node, every list reaches exactly the live connections, a conflict or a diverging sibling is held by both
// sides (siblings out of one lane, conflicts out of two) and its place comes with it. The first thing broken is kept in TrafficState::graph_fault for the tests: the cause shows on the tick
// the edit made it, not when a vehicle runs into it minutes later.
#include "sim/state/city_state.h"
#include "sim/traffic/lane_graph_internal.h"

#include <algorithm>
#include <string>

namespace city {
namespace traffic {

namespace {
std::string fault_of(const Conn &c, int ci, const char *what) {
	return "conn " + std::to_string(ci) + " at node " + std::to_string(c.node) + ": " + what;
}

bool live_lane(const LaneGraph &g, int ds, int lane) {
	return ds >= 0 && size_t(ds) < g.ds.size() && g.ds[size_t(ds)].ok && lane >= 0 && lane < g.ds[size_t(ds)].lanes;
}

// The connectors `c` meets by one list (its conflicts or its diverging siblings): each alive, holding `c` back on the same list, out
// of the same lane exactly when they are siblings, and each with its clear places.
std::string meets_fault(const LaneGraph &g, const Conn &c, int ci, std::vector<int> Conn::*list, size_t clears, bool siblings) {
	const std::vector<int> &meets = c.*list;
	if (meets.size() != clears) return fault_of(c, ci, "a conflict without its clear places");
	for (int o : meets) {
		if (o < 0 || size_t(o) >= g.conns.size() || !g.conns[size_t(o)].alive) return fault_of(c, ci, "conflicts with a dead connection");
		const Conn &other = g.conns[size_t(o)];
		const std::vector<int> &back = other.*list;
		if (std::find(back.begin(), back.end(), ci) == back.end()) return fault_of(c, ci, "a conflict held by one side only");
		if ((other.from_ds == c.from_ds && other.from_lane == c.from_lane) != siblings)
			return fault_of(c, ci, siblings ? "a diverging sibling out of another lane" : "a conflict out of its own lane");
	}
	return {};
}

std::string conn_fault(const LaneGraph &g, int ci) {
	const Conn &c = g.conns[size_t(ci)];
	if (!live_lane(g, c.from_ds, c.from_lane)) return fault_of(c, ci, "comes from a lane that is not there");
	if (!live_lane(g, c.to_ds, c.to_lane)) return fault_of(c, ci, "goes to a lane that is not there");
	const std::string conflicts = meets_fault(g, c, ci, &Conn::conflicts, c.conflict_clear.size(), false);
	return conflicts.empty() ? meets_fault(g, c, ci, &Conn::diverges, c.diverge_clear.size(), true) : conflicts;
}
} // namespace

std::string lane_graph_fault(const CityState &s) {
	const LaneGraph &g = s.traffic.g;
	size_t alive = 0, listed = 0;
	for (size_t ci = 0; ci < g.conns.size(); ci++) {
		if (!g.conns[ci].alive) continue;
		alive++;
		const std::string fault = conn_fault(g, int(ci));
		if (!fault.empty()) return fault;
	}
	for (size_t n = 0; n < g.node_conn.size(); n++)
		for (int c = g.node_conn[n]; c >= 0; c = g.conns[size_t(c)].next_at_node) {
			if (!g.conns[size_t(c)].alive) return "node " + std::to_string(n) + " lists a dead connection";
			if (size_t(g.conns[size_t(c)].node) != n) return "node " + std::to_string(n) + " lists another node's connection";
			if (++listed > alive) return "node " + std::to_string(n) + ": its list does not end";
		}
	return listed == alive ? std::string() : "a live connection on no node's list";
}

} // namespace traffic
} // namespace city
