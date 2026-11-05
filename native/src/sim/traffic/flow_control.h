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
void apply_flow_control(CityState &s); // under the map's exclusive lock: the choices reach the network

} // namespace traffic
} // namespace city
