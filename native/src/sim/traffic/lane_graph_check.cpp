// What the lane graph must always be: a connection joins live lanes at a live node, every list reaches exactly the live
// connections, a conflict or a branch of a lane's fork is held by both sides (a fork's branches out of one lane, conflicts out of
// two) and its place comes with it. Asked by record (connection_fault, node_list_fault: lane_graph_check.h) by the record checks
// (sim/checks/lane_graph.cpp, the traffic.lane_graph invariant), and whole after every network change in the tests' DLL
// (lane_graph_fault): the first thing broken is kept in TrafficState::graph_fault, so the cause shows on the tick the edit made
// it, not when a vehicle runs into it minutes later.
#include "sim/traffic/lane_graph_check.h"

#include "sim/state/city_state.h"
#include "sim/traffic/lane_graph_internal.h"

#include <algorithm>
#include <string>

namespace city {
namespace traffic {

namespace {
bool live_lane(const LaneGraph &g, int ds, int lane) {
	return ds >= 0 && size_t(ds) < g.ds.size() && g.ds[size_t(ds)].ok && lane >= 0 && lane < g.ds[size_t(ds)].lanes;
}

// The connectors `c` meets by one list (its conflicts or the other branches of its lane's fork): each alive, holding `c` back on the
// same list, out of the same lane exactly when the list is the fork's, and each with its clear places.
const char *meets_fault(const LaneGraph &g, const Conn &c, int ci, std::vector<int> Conn::*list, size_t clears, bool fork) {
	const std::vector<int> &meets = c.*list;
	if (meets.size() != clears) return "a conflict without its clear places";
	for (int o : meets) {
		if (o < 0 || size_t(o) >= g.conns.size() || !g.conns[size_t(o)].alive) return "conflicts with a dead connection";
		const Conn &other = g.conns[size_t(o)];
		const std::vector<int> &back = other.*list;
		if (std::find(back.begin(), back.end(), ci) == back.end()) return "a conflict held by one side only";
		if ((other.from_ds == c.from_ds && other.from_lane == c.from_lane) != fork)
			return fork ? "a branch of its fork out of another lane" : "a conflict out of its own lane";
	}
	return nullptr;
}
} // namespace

const char *connection_fault(const LaneGraph &g, int ci) {
	const Conn &c = g.conns[size_t(ci)];
	if (!live_lane(g, c.from_ds, c.from_lane)) return "comes from a lane that is not there";
	if (!live_lane(g, c.to_ds, c.to_lane)) return "goes to a lane that is not there";
	const char *conflicts = meets_fault(g, c, ci, &Conn::conflicts, c.conflict_clear.size(), false);
	return conflicts ? conflicts : meets_fault(g, c, ci, &Conn::forks, c.fork_clear.size(), true);
}

const char *node_list_fault(const LaneGraph &g, int n, size_t most, size_t &listed) {
	if (n < 0 || size_t(n) >= g.node_conn.size()) return nullptr; // a node the graph has not grown to has no list
	for (int c = g.node_conn[size_t(n)]; c >= 0; c = g.conns[size_t(c)].next_at_node) {
		if (size_t(c) >= g.conns.size() || !g.conns[size_t(c)].alive) return "lists a dead connection";
		if (g.conns[size_t(c)].node != n) return "lists another node's connection";
		if (++listed > most) return "its list does not end";
	}
	return nullptr;
}

std::string lane_graph_fault(const CityState &s) {
	const LaneGraph &g = s.traffic.g;
	size_t alive = 0, listed = 0;
	for (size_t ci = 0; ci < g.conns.size(); ci++) {
		if (!g.conns[ci].alive) continue;
		alive++;
		if (const char *fault = connection_fault(g, int(ci)))
			return "conn " + std::to_string(ci) + " at node " + std::to_string(g.conns[ci].node) + ": " + fault;
	}
	for (size_t n = 0; n < g.node_conn.size(); n++)
		if (const char *fault = node_list_fault(g, int(n), alive, listed)) return "node " + std::to_string(n) + ": " + fault;
	return listed == alive ? std::string() : "a live connection on no node's list";
}

} // namespace traffic
} // namespace city
