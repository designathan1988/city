// Internal surface of the lane graph (sim/traffic only): the full build, the incremental rebuild of one segment or
// node, the connection pool and the state that must grow with it.
#pragma once

#include "sim/state/city_state.h"

namespace city {
namespace traffic {

inline int in_ds(const RoadNetwork &net, int seg, int node) { return seg * 2 + (net.segs[seg].b == node ? 0 : 1); }
inline int out_ds(const RoadNetwork &net, int seg, int node) { return seg * 2 + (net.segs[seg].a == node ? 0 : 1); }

// lane_graph.cpp
void build_dir(CityState &s, int seg); // (re)builds the two directed segments of `seg`, keeping lane_metres right
void build_node(CityState &s, int n); // allocates the node's connections from the pool and sets their conflicts
void build_lane_graph(CityState &s); // full build: new map and load

// lane_graph_edit.cpp
int alloc_conn(CityState &s); // a free pool entry (or a new one), with its per-connection state cleared
void ensure_conn_state(TrafficState &t); // sizes conn_vehicles/conn_pass/... to LaneGraph::conns
void graph_grow(CityState &s, int segs, int nodes); // grows every per-segment/per-node vector, keeping the values
void clear_node(CityState &s, int n); // drops the node's connections (moving or despawning their vehicles)
void apply_network_changes(CityState &s); // drains ChangeLog net_* at the start of traffic::step

} // namespace traffic
} // namespace city
