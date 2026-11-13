// What the lane graph must always be, by record (lane_graph_check.cpp): a connection's fault and a node list's fault as static
// text, nothing allocated, so the record checks (sim/checks/lane_graph.cpp) can ask them per connection and per node.
#pragma once

#include "sim/traffic/lane_graph_types.h"

#include <cstddef>

namespace city {
namespace traffic {

// The first rule connection `ci` (alive) breaks: its lanes live, its conflicts and its fork's branches alive, held by both sides,
// out of the right lane, each with its clear places. nullptr when it breaks none.
const char *connection_fault(const LaneGraph &g, int ci);
// The first rule node `n`'s list breaks: every entry a live connection of that node, the list ending within `most` entries.
// `listed` counts the entries walked (added to). nullptr when it breaks none.
const char *node_list_fault(const LaneGraph &g, int n, size_t most, size_t &listed);

} // namespace traffic
} // namespace city
