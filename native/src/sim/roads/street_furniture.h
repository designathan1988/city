// Where the street furniture stands (Roadcraft jobs/props.ts placeSegment / placeJunction, world/signalPosts.ts): the
// one owner of every lamp, signal post, hydrant, bin, bench and street tree, read by the walkers (they keep off the
// footprints, sim/traffic/ped_furniture.cpp) and by the render (meshes, wires, night light pools and street trees).
// Rules and sizes are data/catalog/street_furniture.json; a place is in its segment's frame: arc v from node a and u
// across the profile (+u on the left of a -> b).
#pragma once

#include "sim/roads/road_network.h"

#include <cstdint>
#include <vector>

namespace city {

struct FurnitureSpot {
	FurnitureKind kind = FurnitureKind::LAMP;
	int seg = -1;
	int node = -1; // a signal post: the junction whose arriving traffic it shows
	int side = -1; // the sidewalk it stands on: 0 the right one (edges.front()), 1 the left one, -1 none
	int row = 0; // a lamp: 0 the main row (its poles are wired), 1 the second row across a median
	double v = 0;
	float u = 0;
	float face_left = 0, face_fwd = 0; // the piece's +z in the segment frame (toward the road or the arriving traffic)
	float half_v = 0, half_u = 0; // half its footprint along and across the segment
};

// Where a profile's lamps stand (Roadcraft jobs/props.ts lamps, render/roads.ts pools): pole u on the right
// sidewalk (else the left), the head `reach` metres toward the carriageway; a second row on the left sidewalk when
// the profile has a median. Pools: du = |u - centre| - half_span.
struct LampRow {
	bool any = false, twin = false;
	float pole_u = 0, head_u = 0, pole_u2 = 0, head_u2 = 0;
	float centre = 0, half_span = 0;
};
LampRow lamp_row(const Profile &p, const LampDef &l);
// Arc of a segment's first lamp station: the night pools repeat every lamps.step_m from it.
double lamp_origin(const RoadSegment &g, const LampDef &l);

constexpr uint32_t furniture_bit(FurnitureKind k) { return 1u << uint32_t(k); }
constexpr uint32_t FURNITURE_ALL = (1u << FURNITURE_KINDS) - 1u;

// The pieces of segment `sid` whose kind is in `kinds`: the signal posts of its signalled ends (signalled[0] at a,
// [1] at b; the junction control is sim/traffic's), then by its furniture level the lamps, hydrants, bins, benches and
// trees, in that order. A hydrant, bin, bench or tree that would leave a walker no way past the pieces beside it is
// not placed (traffic.json ped_body_m and ped_margin_m: the walkers' own sizes).
void segment_furniture(const RoadNetwork &net, int sid, const bool signalled[2], uint32_t kinds, std::vector<FurnitureSpot> &out);
// The signal post on arm `sid` of signalled node `n`, on the footway at the right of the traffic arriving at `n`;
// false when that arm has no lanes into `n`.
bool signal_post(const RoadNetwork &net, int n, int sid, FurnitureSpot &out);

} // namespace city
