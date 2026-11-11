// What lies under a point of the map, read from the profile of the segments near it (Roadcraft poleLines.ts onFootway,
// landscape.ts footwayAt and medianAt): which sidewalk, at what arc and offset, and whether a crossing lands there.
#pragma once

#include "sim/roads/junction.h"
#include "sim/roads/road_network.h"

#include <vector>

namespace city {

// A place on a segment: arc v from node a and u across the profile (+u on the left of a -> b).
struct FootwayHit {
	bool ok = false;
	int seg = -1;
	int side = -1; // the sidewalk: 0 the right one (edges.front()), 1 the left one; -1 a median
	double v = 0;
	float u = 0;
	float away = 0; // how far the point was off the band
	float kx = 0, kz = 0; // unit direction toward the carriageway
};

// The world place of (v, u) on a segment, and its height at the road's level.
void footway_point(const RoadNetwork &net, int seg, double v, float u, double &x, double &z, double &y);
// The segment's frame at arc v: its tangent (a to b) and its left.
void segment_frame(const RoadNetwork &net, int seg, double v, double &fx, double &fz, double &lx, double &lz);
// The arc and offset of a point on a segment, and how far it lies past the segment's ends along the tangent (0 beside it): a
// point ahead of an end has that end as its closest place, and only `past` tells it from one beside the road.
void segment_place(const RoadNetwork &net, int seg, double x, double z, double &v, float &u, float &past);
// The sidewalk under (x, z), or within `reach` of its band, between the segment's mouths.
FootwayHit footway_at(const RoadNetwork &net, double x, double z, double reach);
// The farthest a point may stand from a segment's centre line and still find that segment's sidewalk within `reach`
// (footway_at): the widest profile's half and `reach` across it, `reach` along it. What asks which segments a point's footway
// may be on pads its search by this, and a reader of a box of the roads (a job's cut) holds this much round the points it asks.
double footway_reach(const RoadNetwork &net, double reach);
// The median under (x, z) within `reach` of its centre line (side -1): a plant may stand there.
FootwayHit median_at(const RoadNetwork &net, double x, double z, double reach);
bool on_road(const RoadNetwork &net, double x, double z); // on any segment's surface, sidewalks included
// On a carriageway or its kerb (a kerb barrier may stand on the sidewalk behind them).
bool on_carriageway(const RoadNetwork &net, double x, double z, float keep);
// On a footway: a sidewalk between a segment's mouths (footway_at), or a junction's corner block (JunctionKerbs).
bool on_footway(const RoadNetwork &net, double x, double z);
// A junction's corners as its footways lie there (junction.h corner_fillet): its arms in turn round it, their mouths and each
// corner's kerb return. A point stands on a corner's footway beyond both of its kerbs, short of both mouths, and off the asphalt
// the kerb return (a fillet) or the cut (a bevel) gives the junction there.
struct JunctionKerbs {
	double x = 0, z = 0;
	std::vector<Arm> arms; // anticlockwise (arm_angle)
	std::vector<double> mouths; // each arm's setback
	std::vector<Corner> corners; // corners[k]: between arms[k] and the next anticlockwise
	JunctionKerbs(const RoadNetwork &net, int node);
	bool on_footway(double px, double pz) const;
	// The kerb return drawn round the corner a walker goes round from one point to the other (one beyond the kerb of the one arm
	// on that corner's side, the other beyond the other arm's): its centre in the world; false when no corner has the two or that
	// corner's kerb is no return's arc (junction.h fillet_drawn).
	bool kerb_return(double px, double pz, double qx, double qz, double &cx, double &cz) const;
};
// Whether something of `radius` on `hit`'s sidewalk stands in a crosswalk's landing (a mouth of a three-arm node or more).
bool on_landing(const RoadNetwork &net, const FootwayHit &hit, float radius, float clear);

} // namespace city
