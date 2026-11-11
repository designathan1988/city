// A node's arms as the road surfaces read them (road_junction.cpp draws the node, road_corner.cpp its corners): each segment end as
// sim/roads/node_outline.h gives it, at the road's height; and the corner between two neighbouring ends as that owner lays it.
#pragma once

#include "render/roads/road_mesh.h"
#include "sim/roads/node_outline.h"

#include <vector>

namespace city {

struct ArmEnd {
	godot::Vector3 p, d; // the carriageway's mouth on the centre line (at the road's height), outward along the arm, unit
	Arm arm;
	double at = 0; // the mouth's setback from the node
	const Profile *prof = nullptr;
	bool at_a = true; // the segment's end a is at this node
	int sid = -1;
	NodeEnd end; // the end in plan these are read from (node_outline.h)
};

// The node's segment ends, anticlockwise (arm_angle).
std::vector<ArmEnd> arm_ends(const RoadEmit &e, int node);
// Whether ends of a node are drawn as a junction's plate with corners: two or more, one with a setback.
bool plated(const std::vector<ArmEnd> &ends);
// The street side (kerb_drop.h STREET_RIGHT, STREET_LEFT) of an end's segment that is the arm's left, looking out from the node.
int left_side(const ArmEnd &en);
// The paving of the arm's left footway: what a footway's faces next to it wear.
godot::Color footway_colour(const StreetLook &L, const ArmEnd &en);
// The corner between end a and the next anticlockwise b of their node, in the node's frame (node_outline.h node_corner).
CornerOutline corner_of(const RoadEmit &e, const ArmEnd &a, const ArmEnd &b);
// One corner's kerb and footway; `la`, `lb` the surfaces of a's and b's segments (road_corner.cpp).
void draw_corner(const RoadEmit &e, const godot::Vector3 &node, const ArmEnd &a, const ArmEnd &b, const CornerOutline &o, const SurfaceLayout &la,
		const SurfaceLayout &lb);

} // namespace city
