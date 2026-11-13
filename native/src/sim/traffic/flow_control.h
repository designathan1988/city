// The control a junction left on automatic gets from the flows measured at it: the game's side of sim/roads/flow_stats and
// sim/roads/control_advisor, both translated from the Roadcraft 4.0 (its SimWorld.stepControlAdvisor). The count and the choice are
// the traffic's, under its mutex; what the network shows comes from RoadNode::auto_signal, written under the map's exclusive lock,
// because the mesh jobs read control_of and never the traffic.
#pragma once

#include "core/js_runtime.h"
#include "sim/roads/control_advisor.h"
#include "sim/roads/flow_stats.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace city {

struct CityState;

struct FlowControl {
	std::shared_ptr<FlowStats> flow = std::make_shared<FlowStats>();
	std::shared_ptr<ControlAdvisor> advisor = std::make_shared<ControlAdvisor>();
	js::Set<int64_t> advised; // the junctions whose control the flows can still change
	std::vector<int> changed; // chosen anew, waiting for the map's exclusive lock
};

namespace traffic {

void count_entry(CityState &s, int connector); // a vehicle enters a junction's movement: the flow from its leg
void step_flow_control(CityState &s, float dt); // under the map's shared lock and the traffic mutex
// Under the map's exclusive lock: the choices reach the network; a junction whose control changed has its arms' kept furniture
// placed again (its signal posts) and their props drawn again.
void apply_flow_control(CityState &s);
// What the flows and the advisor keyed by what an edit took away, forgotten by the traffic's drain (lane_graph_edit.cpp), since an id
// freed is the next record's: a removed road's leg at both its nodes, every key of a removed node (`gone_nodes`, and any node of
// `nodes` found dead), and at every node of `nodes` the legs that are no longer its arms. A crossing or a control set by the player
// (the node only on `nodes`) keeps the choice. Each list sorted and unique.
void forget_flows(CityState &s, const std::vector<int> &removed_segs, const std::vector<int> &nodes, const std::vector<int> &gone_nodes);

} // namespace traffic
} // namespace city
