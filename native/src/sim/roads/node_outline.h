// A node's paving in plan, the one owner every reader asks: the road surfaces that draw it (render/roads road_corner.cpp,
// road_junction.cpp), the paving's outline the lots are cut by (paving_outline.h) and the probes. Its segment ends anticlockwise,
// each with its carriageway's mouth; the corner between two neighbouring ends, as junction_corner.h lays it, at the stations the
// catalogue gives (tuning road.corner_arc_step_rad, road.corner_bevel_steps); a turning bulb's ring, open where its road comes in
// (road.bulb_steps).
#pragma once

#include "sim/roads/junction_corner.h"

#include <vector>

namespace city {

class RoadNetwork;

// One segment end at a node.
struct NodeEnd {
	int seg = -1;
	bool at_a = true; // the segment's end a is at this node
	double at = 0; // the mouth's setback from the node, along the segment
	double x = 0, z = 0; // the carriageway's mouth on the centre line, in the world's plan
	V2 dir; // outward along the segment at its mouth, unit
	Arm arm;
	const Profile *prof = nullptr;
};

// The node's segment ends, anticlockwise (arm_angle).
std::vector<NodeEnd> node_ends(const RoadNetwork &net, int node);
// Whether the ends are laid as a junction's plate with corners: two or more, one with a setback.
bool node_plated(const std::vector<NodeEnd> &ends);
// The corner between end a and the next anticlockwise b of the node at (nx, nz), in the node's frame (metres from it).
CornerOutline node_corner(const RoadNetwork &net, double nx, double nz, const NodeEnd &a, const NodeEnd &b);
// A turning bulb's circle of radius r round its node, open where end `en` comes in: each station as (along the arm, across it to
// the arm's left), metres from the node, from the arm's left round to its right.
std::vector<V2> bulb_ring(const RoadNetwork &net, const NodeEnd &en, double r);
// The setback a bulb's ring opens at: the end's own, or a hair when it has none.
double bulb_setback(const NodeEnd &en);

} // namespace city
