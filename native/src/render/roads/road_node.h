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
// A point of the end's mouth line: `u` metres across from its centre line to the arm's left (looking out from the node).
godot::Vector3 mouth_point(const ArmEnd &en, double u);
// Whether ends of a node are drawn as a junction's plate with corners: two or more, one with a setback.
bool plated(const std::vector<ArmEnd> &ends);
// The street side (kerb_drop.h STREET_RIGHT, STREET_LEFT) of an end's segment that is the arm's left, looking out from the node.
int left_side(const ArmEnd &en);
// The paving of the arm's left footway: what a footway's faces next to it wear.
godot::Color footway_colour(const StreetLook &L, const ArmEnd &en);
// The corner between end a and the next anticlockwise b of their node, in the node's frame (node_outline.h node_corner).
CornerOutline corner_of(const RoadEmit &e, const ArmEnd &a, const ArmEnd &b);
// A stretch of the rim of a node's paving, from p to q round the node, and how far the node and its two ends stand from the kerb.
struct PlateSlice {
	godot::Vector3 p, q;
	float kerb_node = 0, kerb_p = 0, kerb_q = 0;
};
// A stretch of the rim that is kerb: the kerb is its own line.
PlateSlice kerb_slice(const godot::Vector3 &node, const godot::Vector3 &p, const godot::Vector3 &q);
// A stretch of the rim across half an arm's mouth: the kerb is the line through `kerb_at` along the arm (`along`, unit, flat).
PlateSlice mouth_slice(const godot::Vector3 &node, const godot::Vector3 &p, const godot::Vector3 &q, const godot::Vector3 &kerb_at, const godot::Vector3 &along);
// A node's paving, from the node out to its rim (road_plate.cpp): each point in every arm's own asphalt by how near that arm's mouth
// is, exactly an arm's on its mouth, and knowing how far its kerb is (road_look.h UV2).
void draw_plate(const RoadEmit &e, const godot::Vector3 &node, const std::vector<ArmEnd> &ends, const std::vector<PlateSlice> &rim);
// One corner's kerb and footway; `la`, `lb` the surfaces of a's and b's segments (road_corner.cpp).
void draw_corner(const RoadEmit &e, const godot::Vector3 &node, const ArmEnd &a, const ArmEnd &b, const CornerOutline &o, const SurfaceLayout &la,
		const SurfaceLayout &lb);

} // namespace city
