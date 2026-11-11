// Where a street parks: the one owner of its kerb bays and of the runs they are painted in, read by the traffic (the cars
// that stand in them, sim/traffic/parking_sources.cpp) and by the render (road_markings.cpp). The bays lie along the
// profile's parking bands, kept off the junctions (CTB art. 181 I), the crosswalks and the lots' car gates; the rest of
// that kerb carries the yellow no-parking line (CONTRAN MBST vol. IV 7.1, LPP). A place is in the segment's frame: arc v
// from node a and u across the profile (+u on the left of a -> b).
#pragma once

#include "sim/roads/road_network.h"

#include <utility>
#include <vector>

namespace city {

// A stretch of a kerb: side 0 the right one, 1 the left one; the band runs from u_kerb to u_inner across.
struct KerbStretch {
	int side = 0;
	double v0 = 0, v1 = 0;
	float u_kerb = 0, u_inner = 0;
};

struct KerbLayout {
	std::vector<KerbStretch> bays; // one stall each
	std::vector<KerbStretch> runs; // the bays of a run taken together (their lines close them)
	std::vector<KerbStretch> bans; // where that kerb does not park
};

using ArcSpan = std::pair<double, double>;

// The sides of a street in its frame: +u is on the left of a -> b. The one owner of which side a band, a lot's gate or a kerb is on.
constexpr int STREET_RIGHT = 0, STREET_LEFT = 1;
inline int street_side(double u) { return u > 0 ? STREET_LEFT : STREET_RIGHT; }

// Whether the profile of `sid` has a parking band at all.
bool parks_along(const RoadNetwork &net, int sid);
// `gates`: what keeps bays off each side (sim/buildings/lot_access.h street_gates).
void kerb_layout(const RoadNetwork &net, int sid, const std::vector<ArcSpan> gates[2], KerbLayout &out);
// Whether a car may stand at the kerb at arc `v` of `sid`: past both ends' corners and crosswalks (CTB art. 181 I), the stretch
// the kerb's bays are laid in; a lot's gate opens onto the street only there (sim/traffic/parking_lanes.cpp gate_lane).
bool kerb_standing(const RoadNetwork &net, int sid, double v);
// Whether a lot's person's gate may open onto the footway at arc `v` of `sid`: off each end's crosswalk landing and the signal post that
// stands just past it, with a walker's body round the post (from the mouth to the crossing's far edge, the post's footprint and
// traffic.json ped_body_m). A junction's plate and a car's corner clearance are no bar to a person's gate: a lot across a T's stem
// opens onto the footway between its two crosswalks.
bool kerb_walkable(const RoadNetwork &net, int sid, double v);
// The arc of curve point `t` on segment `sid` (the inverse of t_at_length).
double arc_of_t(const RoadSegment &g, double t);

} // namespace city
