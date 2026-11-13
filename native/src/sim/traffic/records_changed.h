// The traffic's side of a road, node or lot record the commands change: what it holds keyed by that record, read back for the
// command that carries it (the connections built through a node, as hand links name them) or let go when the record changes (a
// street's kerb, a lot's bays, yard and gates). Pure C++; defined in lane_graph.cpp and parking_sources.cpp.
#pragma once

#include "sim/roads/road_network.h"
#include "sim/state/change_log.h"

#include <vector>

namespace city {

struct CityState;

namespace traffic {

// The connections the graph holds through `node`, as hand links name them ({from, from_lane, to, to_lane}), into `out` (cleared).
// The graph may be behind the commands not drained yet: a road named may be gone. lane_graph.cpp.
void links_built(const CityState &s, int node, std::vector<LaneLink> &out);
// parking_sources.cpp: an edit touched the street: its kerb is laid out again.
void forget_parking_seg(CityState &s, int seg);
// A building came, went or changed where `box` stood (ChangeLog::lot_fronts): its bays, its yard and the lorries bound to its docks
// go, and the kerbs its gates kept bays off are read again there, never where the record now at `id` stands.
void forget_parking_lot(CityState &s, int id, const LotBox::Box &box);

} // namespace traffic
} // namespace city
