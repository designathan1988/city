// The lane graph's vocabulary: the directed segments, the lane connectors through junctions with their conflicts, the junction
// control and the graph that holds them. Pure C++ (no Godot types); written only by sim/traffic, read by render/agents.
#pragma once

#include "core/generation.h"

#include <cstdint>
#include <vector>

namespace city {

constexpr int MAX_LANES = 4; // lanes per direction (the widest one-way profile has 4)
constexpr int CONN_STEPS = 8; // arc-length table of a connector curve

enum class Turn : uint8_t { STRAIGHT = 0, RIGHT = 1, LEFT = 2, UTURN = 3 };
enum class Control : uint8_t { NONE = 0, PRIORITY = 1, SIGNAL = 2 };
enum class Light : uint8_t { GREEN = 0, AMBER = 1, RED = 2 };
// Why a vehicle is not moving (traffic_probe, tests).
enum class Hold : uint8_t { QUEUED = 0, WRONG_LANE, RED, EXIT_ROOM, CONFLICT_BUSY, YIELD, ON_CONNECTOR, FREE, CROSSING, COUNT };

// One direction of a segment: ds = seg * 2 + dir; dir 0 runs a -> b on the profile's fwd lanes, dir 1 b -> a on back.
// s = 0 at the entry mouth, s = len at the exit mouth (stop line).
struct DirSeg {
	bool ok = false;
	int seg = -1, from = -1, to = -1, lanes = 0;
	float u[MAX_LANES] = {}; // profile offsets (a -> b frame) of lane 0 (rightmost) ... lanes-1
	float len = 0, speed = 0; // metres, m/s
	float at0 = 0, sign = 1; // arc length along the segment at s = 0 and its direction
	float ex = 0, ez = 0; // endpoint (s = len) in world XZ, computed once at graph build (A* heuristic)
	int stage = -1; // signal stage serving this approach at node `to`
	int rank = 2; // right of way at node `to`: 3 main, 2 free, 1 yield
	bool must_stop = false; // a Stop at node `to`: nobody goes over the line still rolling
	float stop_back = 0; // the stop line is this far before s = len (behind the crosswalk at node `to`)
	float cycle_shift = 0; // a bike strip beside lane 0: how far across (u, a -> b frame), 0 for none
	float edge_shift = 0; // no strip: how far across a bicycle rides within lane 0, on the kerb edge of the carriageway
	bool bikes = true; // bicycles may ride it (the street has a footway)
	float detector = 0, extension = 0; // a signalled approach: its detection section before the line, green per call
};

// A lane connector through a node: quadratic Bézier from the end of an in-lane to the start of an out-lane.
// Connections live in one pool with a free list; each node and each lane of origin keeps a linked list of them
// (Roadcraft lanes.ts nodeConn/laneConn), so a node rebuild touches only its own connections.
struct Conn {
	int node = -1, from_ds = -1, from_lane = 0, to_ds = -1, to_lane = 0;
	Turn turn = Turn::STRAIGHT;
	float x0 = 0, z0 = 0, cx = 0, cz = 0, x1 = 0, z1 = 0, y0 = 0, y1 = 0;
	float table[CONN_STEPS + 1] = {};
	float len = 0;
	int next_at_node = -1, next_from = -1; // next connection of the same node / of the same lane of origin
	bool alive = false;
	std::vector<int> conflicts;
	std::vector<float> conflict_at; // per conflict: metres along THAT connector after which its vehicles are clear
};

struct Junction {
	Control ctl = Control::NONE;
	// per stage, seconds: minimum and maximum green, then the intergreen (amber, all-red); its approaches
	std::vector<float> green, green_max, amber, all_red;
	std::vector<float> least; // the minimum greens, under any fixed plan
	std::vector<std::vector<int>> approaches;
	float cycle = 0; // with the minimum greens
	int run = 0; // the stage running, since `run_start` (its green); its green once ended (-1 while the green lasts)
	double run_start = 0;
	float run_green = -1;
	bool fixed = false; // fixed time: the stage follows the clock (`offset`), not the detectors
	float offset = 0;
	double settle = 0; // a new plan takes over only then: the stage running ends its intergreen first
	int degree = 0; // arms with lanes
};

struct LaneGraph {
	uint64_t routing_revision = next_generation(); // structural edges/endpoints and route-relevant junction control
	int version = -1; // RoadNetwork::version it was built from
	std::vector<DirSeg> ds;
	std::vector<Conn> conns; // pool: dead entries are on free_conns
	std::vector<int> free_conns;
	std::vector<int> node_conn; // per node: first connection of its list (-1)
	std::vector<int> lane_conn; // per ds * MAX_LANES + lane: first connection leaving it (-1)
	std::vector<std::vector<int>> route_conns; // first equivalent connector in original lane/list order, prepared with its node
	std::vector<uint8_t> route_ready; // absent/unprepared low-level graphs keep the linked-list path
	std::vector<Junction> junctions; // per node
	double lane_metres = 0;
};

// Walks the connections leaving `ds` (all its lanes), skipping dead pool entries.
template <typename F>
void each_conn_from(const LaneGraph &g, int ds, F &&fn) {
	for (int lane = 0; lane < MAX_LANES; lane++) {
		for (int c = g.lane_conn[size_t(ds) * MAX_LANES + lane]; c >= 0; c = g.conns[c].next_from)
			if (g.conns[c].alive) fn(c);
	}
}

// One bounded physical adjacency read for a route continuation. Dead links consume a step of their own.
struct RouteConnCursor { size_t at = 0; int lane = 0, link = -2; };
enum class ConnRead : uint8_t { ENTRY, SKIP, END };
inline ConnRead next_physical_connection(const LaneGraph &g, int ds, RouteConnCursor &cursor, int &ci) {
	if (ds < 0 || size_t(ds) >= g.ds.size() || (size_t(ds) + 1) * MAX_LANES > g.lane_conn.size()) return ConnRead::END;
	// At most MAX_LANES heads; preserves the original physical lane/list order.
	while (cursor.lane < MAX_LANES) {
		if (cursor.link == -2) cursor.link = g.lane_conn[size_t(ds) * MAX_LANES + cursor.lane];
		if (cursor.link >= 0) break;
		++cursor.lane; cursor.link = -2;
	}
	if (cursor.lane >= MAX_LANES) return ConnRead::END;
	ci = cursor.link; cursor.link = g.conns[size_t(ci)].next_from;
	return g.conns[size_t(ci)].alive ? ConnRead::ENTRY : ConnRead::SKIP;
}
inline ConnRead next_route_connection(const LaneGraph &g, int ds, RouteConnCursor &cursor, int &ci) {
	if (ds >= 0 && size_t(ds) < g.route_conns.size() && size_t(ds) < g.route_ready.size() && g.route_ready[size_t(ds)]) {
		const auto &edges = g.route_conns[size_t(ds)];
		if (cursor.at >= edges.size()) return ConnRead::END;
		ci = edges[cursor.at++]; return ConnRead::ENTRY;
	}
	return next_physical_connection(g, ds, cursor, ci);
}

} // namespace city
