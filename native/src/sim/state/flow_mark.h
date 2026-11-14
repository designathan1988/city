// What the far traffic tells the render of one directed segment (ChangeLog::flow_marks): the numbers its far cars are drawn from,
// sent only when what is drawn of it changed, and its line when that changed (the street as three points of its one curve).
// Pure C++: written by sim/traffic/flow_step.cpp (tell), read by render/agents/far_traffic.cpp.
#pragma once

#include <cstdint>

namespace city {

constexpr uint8_t FLOW_LIVE = 1; // the directed segment carries traffic
constexpr uint8_t FLOW_INSIDE = 2; // the window holds it: its cars stand in for the detailed vehicles, where those are not drawn
constexpr uint8_t FLOW_LINE = 4; // this mark carries the segment's line

struct FlowMark {
	int ds = -1;
	uint8_t flags = 0;
	float count = 0; // the cars drawn of it: whole ones (what it carries, its fraction a car or none by the street's own share)
	float queued = 0; // of them, standing at its end
	float speed = 0; // m/s, of those that move
	// With FLOW_LINE: the middle of its lanes at its start, its curve's control point and its end; its length between them, its
	// lanes and the metres from one lane to the next.
	float ax = 0, ay = 0, az = 0, cx = 0, cy = 0, cz = 0, bx = 0, by = 0, bz = 0;
	float len = 0, lane_step = 0;
	uint8_t lanes = 0;
};

} // namespace city
