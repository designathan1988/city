// Internal surface of the lane graph (sim/traffic only): the full build, the incremental rebuild of one segment or
// node, the connection pool and the state that must grow with it.
#pragma once

#include <string>

#include "sim/state/city_state.h"
#include "sim/traffic/traffic_system.h"

namespace city {

constexpr double FLAT_TANGENT = 1e-9; // below this a curve has no direction at that point
namespace traffic {

// lane_graph.cpp
void build_dir(CityState &s, int seg); // (re)builds the two directed segments of `seg`, keeping lane_metres right
std::string lane_graph_fault(const CityState &s); // lane_graph_check.cpp: the first thing the graph must not be, or nothing
void build_node(CityState &s, int n); // allocates the node's connections from the pool and sets their conflicts
void build_lane_graph(CityState &s); // full build: new map and load
void drop_lane_state(CityState &s, int ds); // vehicles and meso entries of one directed segment leave

// lane_graph_edit.cpp
int alloc_conn(CityState &s); // a free pool entry (or a new one), with its per-connection state cleared
void ensure_conn_state(TrafficState &t); // sizes conn_vehicles/conn_pass/... to LaneGraph::conns
void graph_grow(CityState &s, int segs, int nodes); // grows every per-segment/per-node vector, keeping the values
void clear_node(CityState &s, int n); // drops the node's connections (moving or despawning their vehicles)
void apply_network_changes(CityState &s); // drains ChangeLog net_* at the start of traffic::step

} // namespace traffic
} // namespace city
