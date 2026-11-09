// Node shape shared by drawing, the ground, lots and traffic (the lab's world/junction/): the arm of each segment end, each arm's
// setback (where the segment's mouth is) and a road's two mouths reconciled against its length, kerb-return fillets, the
// transition of one road carrying on at another width or bend, the cap over a node whose arms all leave on one side, the turning
// bulb at a dead end, and the outline a node's paving covers.
#pragma once

#include "core/math.h"
#include "sim/roads/geometry.h"
#include "sim/roads/profiles.h"

#include <vector>

namespace city {

class RoadNetwork;

// A node whose legs meet too sharply (Roadcraft Network.impossible, legAngles.ts): its node and the smallest gap, radians.
struct Tuning;

struct SharpNode {
	int node = -1;
	float gap = 0;
};
std::vector<SharpNode> nodes_too_sharp(const RoadNetwork &net, float min_gap);
// The smallest angle between two neighbouring legs of node n, radians (a full turn for a node of fewer than two).
float node_gap(const RoadNetwork &net, int n);

constexpr double KERB_RADIUS_MAX = 7.2; // largest kerb return radius (the lab's core/fillet.ts CURB_R_MAX)

Arm arm_of(const Quad &q, const Profile &p, int end);
// Setback of a dead end with a turning bulb: where its kerb returns leave the road's edges (bulb.ts buildBulb, the mouth `d`);
// 0 for no bulb (a road too wide or too short for one).
double bulb_retreat(const Tuning &tn, const Arm &arm, double len);
// Each arm's setback (input order) as its junction wants it, before its road's length is reconciled (clamp_segment_trims);
// `order` returns the arms sorted anticlockwise. `lens`: each arm's road's length (a bulb needs room past its mouth); `seam`: two
// arms of one road that only changes structure there (build.ts structureSeam).
void junction_retreats(const Tuning &tn, const std::vector<Arm> &arms, const std::vector<double> &lens, std::vector<double> &out,
		std::vector<int> &order, bool bulb = false, bool seam = false);

// A road's two mouths against its length (trim.ts clampSegmentTrims): it keeps a drivable reserve between them and scales both
// down to what is left; a road shorter than the reserve splits what little it has.
struct EndTrims {
	double a = 0, b = 0;
};
EndTrims clamp_segment_trims(const Tuning &tn, double want_a, double want_b, double len);

enum class CornerMode : uint8_t { NONE, FILLET, BEVEL, TAPER };
// The corner between arm a and the next arm anticlockwise b, where a's left kerb meets b's right kerb.
struct Corner {
	CornerMode mode = CornerMode::NONE;
	double trim_a = 0, trim_b = 0; // how far along each arm the corner reaches
	double r = 0, cx = 0, cz = 0; // kerb return (fillet only), centre relative to the node
};
Corner corner_fillet(const Tuning &tn, const Arm &a, const Arm &b);

// The angle from arm a anticlockwise round to arm b, in (0, 2 pi].
double arm_gap(const Arm &a, const Arm &b);
// Whether the corner from a to b is the open back of a node whose arms all leave on one side (polygon.ts nodeCap): it is closed
// by the cap, a circle round the node of its widest arm, not by a kerb return.
bool capped_corner(const Arm &a, const Arm &b);
// Whether a node of two arms carries one road on (a taper, a bend under the transition's limit) rather than turning a corner.
bool transition_pair(const Tuning &tn, const Arm &a, const Arm &b);

// The centreline of a transition (transition.ts TransitionAxis): a cubic from the mouth of arm a to the mouth of arm b, its ends
// along each arm, the markings' axis as much as the kerbs'.
struct TransitionAxis {
	XZ p0, p1, p2, p3;
	double length = 0;
	XZ point(double u) const;
	XZ normal(double u) const; // unit left of the way from a's mouth to b's
	int samples(const Tuning &tn) const; // the number of pieces an outline or a stroke is sampled in
};
// The axis from a's mouth (centre `ma`, the arm's outward direction `da`) to b's.
TransitionAxis transition_axis(const XZ &ma, const XZ &da, const XZ &mb, const XZ &db);
// The ease of the cross-section from the first arm's to the second's (transition.ts taperEase).
double taper_ease(double u);

// A turning bulb's line at one reach out of the carriageway (bulb.ts outline): in its arm's frame (x along the arm, from the node;
// z to its left), from the left mouth corner along its edge, round its kerb return, the long way round the circle behind the node,
// back by the right kerb return to the right mouth corner. `half_left`, `half_right`: the edges at this reach; `grow`: how far
// this reach stands out of the carriageway. Each return in `return_steps` pieces, the circle in `round_steps`. False when the
// circle would not clear the edges.
struct BulbLine {
	std::vector<XZ> pts;
	double mouth = 0;
};
bool bulb_line(const Tuning &tn, double half_left, double half_right, double grow, int round_steps, int return_steps, BulbLine &out);
// The farthest a bulb's line reaches from its node (bulb.ts bulbClearing: the circle and both returns), and the radius of its
// circle out to the casing of the road of arm `arm`.
double bulb_reach(const Tuning &tn);
double bulb_round(const Tuning &tn, const Arm &arm);

// The point where a corner's two outer footway edges meet, when the footway's back is squared (derive.ts: the block's corner, both
// footway edges running straight on until they meet): a corner of square_min_deg to 180 - square_min_deg degrees whose point stands
// within square_reach_m of both mouths. `pa`: a's outer left corner at its mouth, `da` the way out along a there; `pb`, `db` b's
// outer right corner and way. False where the footway's back follows the kerb return round instead.
bool corner_tip(const Tuning &tn, const XZ &pa, const XZ &da, const XZ &pb, const XZ &db, XZ &tip);

// The ring a node's paving covers out to its casing (x, z pairs), its arms' mouths joined by their corners: a corner's point where
// its two arms' outer edges meet, a transition's two outer kerb lines, a node cap's circle; a bulb's outer line. Empty for a node
// that carries its road straight on, or a plain dead end. What the ground is pulled under (sim/commands/road_grade_sync.cpp).
std::vector<double> node_outline(const RoadNetwork &net, int n);

} // namespace city
