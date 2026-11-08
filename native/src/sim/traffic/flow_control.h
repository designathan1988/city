// The control a junction left on automatic gets from the flows measured at it: the vehicles entering are counted
// (flow_measure.h), the junctions counted at are looked at every flow.look_every_s, and what their flows warrant (flow_warrants.h)
// reaches the network. The count and the choice are the traffic's, under its mutex; what the network shows comes from
// RoadNode::auto_signal, written under the map's exclusive lock, because the mesh jobs read control_of and never the traffic.
#pragma once

#include "sim/traffic/flow_measure.h"
#include "sim/traffic/flow_warrants.h"

#include <vector>

namespace city {

struct CityState;

struct FlowControl {
	FlowMeasure flow;
	FlowWarrants warrants;
	std::vector<int> advised; // the junctions whose control the flows can still change, in the order they were first counted
	std::vector<int> changed; // chosen anew, waiting for the map's exclusive lock
};

namespace traffic {

void count_entry(CityState &s, int connector); // a vehicle enters a junction's movement: the flow from its leg
void step_flow_control(CityState &s, float dt); // under the map's shared lock and the traffic mutex
void apply_flow_control(CityState &s); // under the map's exclusive lock: the choices reach the network

} // namespace traffic
} // namespace city
