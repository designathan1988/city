// The streets' lines as one network, its one owner. Each segment's main row of utility poles runs along its street on one face
// (line_face); what the lines do at their nodes is decided here, once, for every reader (the placement of a segment's pieces,
// the render's wires, the census): at a joint (two arms, the street going on), and on the straight side of a T where two arms go
// straight on into each other and the footway runs straight past the lots' gates, the line goes on through one pole, the lower
// segment's own end pole (placed by its row, clear of the gates); at a junction each corner a line reaches holds one pole of its
// own, standing on the corner between its two crosswalks, which every line reaching that corner ends on; and a line is strung across the junction to the line of the arm it
// goes straight on into, as a city's distribution network runs on through its crossings. A corner pole is placed once: it is
// listed on the corner's lower arm (street_furniture.cpp lamps), and every reader of that arm's kept placement sees it.
#pragma once

#include "sim/roads/road_network.h"
#include "sim/roads/street_furniture.h"

#include <functional>
#include <vector>

namespace city {

// A pole of the lines: station `station` of segment `seg`'s main row, or (corner >= 0) the corner pole `corner` of node `node`,
// listed on `seg`.
struct LinePole {
	int seg = -1, station = -1, node = -1, corner = -1;
	bool operator==(const LinePole &o) const { return seg == o.seg && station == o.station && node == o.node && corner == o.corner; }
};
struct LineSpan {
	LinePole a, b;
	bool across = false; // strung over a junction, from a corner pole to the corner pole of the line going straight on
};

// The face a street's lamps stand on: 0 its right sidewalk (the profile's edges.front()), 1 its left; the east or south face
// whichever way the segment was drawn (data lamps._doc, NPIP-1 5.2), so a street keeps one face through its joints and the lines
// of crossing streets meet on corners; the other when only it has a sidewalk; -1 with no sidewalk, a ring or a rail.
int street_face(const RoadNetwork &net, int sid);
// The face a segment's line of poles stands on: its street_face, -1 when its furniture level lays none.
int line_face(const RoadNetwork &net, int sid);

// A junction's corner pole: where it stands in the world, and on which arm it is listed with its place in that arm's frame.
struct CornerPole {
	bool stands = false;
	int seg = -1; // the corner's lower arm, whose placement lists it
	double x = 0, z = 0;
	double v = 0;
	float u = 0;
	float face_left = 0, face_fwd = 0; // toward the junction, in the listing arm's frame
};
// Corner `k` of node `n` (between its end k and the next anticlockwise, node_outline.h node_ends): it stands when a line reaches
// the corner and its footway has room for a pole kerb_inset_m in from the kerb at the middle of its kerb line; never between two
// arms in line, where the kerb runs straight (a line runs on through there, line_end_at's joint).
CornerPole corner_pole(const RoadNetwork &net, int n, int k);

// The corner poles of segment `sid`'s nodes whose listing arm is `sid`, appended to its pieces: lit, each with its node and corner
// (street_furniture.cpp segment_furniture lists them before the rows that end on them).
void list_corner_poles(const RoadNetwork &net, int sid, std::vector<FurnitureSpot> &out);
// How far from a mouth a line's own end pole stands: past the crosswalk and past the place a signal post may take, a hand's gap
// (items.gap_m) clear of it, to the middle of the pole. A joint's pole stands there too, on the walkers' sidewalk, never on their
// way round the node.
double line_end_setback(const RoadNetwork &net);
// How segment `sid`'s line meets its end `end` (0 at a, 1 at b): on a corner pole (`corner` >= 0, the place along the segment
// it is projected to), through a joint's one pole (`joint`, a joint or the straight side of a T; `own` when this segment places it,
// past its mouth; else `v` is that pole carried past this segment's end by its straight distance from where this row's own end
// pole would stand, the row spaced from it), or on an end pole of its own past the mouth (neither).
struct LineEndAt {
	int corner = -1;
	bool joint = false, own = true;
	double v = 0;
};
LineEndAt line_end_at(const RoadNetwork &net, int sid, int end);
// Where the pole segment `sid`'s line ends on at its end `end` stands in the world, as placed before the lots' gates move a row's pole
// (its own end pole, the joint's pole whichever arm places it, the corner pole); false when none stands there.
bool line_end_point(const RoadNetwork &net, int sid, int end, double &x, double &z);

// The spans of node `n`'s lines that no segment's row strings by itself: a joint's (its pole to the other segment's first station),
// a corner's (the corner pole to the nearest station of each line ending on it) and across the junction. `first_station(seg, end)`:
// the station of seg's row standing nearest its end `end` (0 at a, 1 at b), or -1 when none stands.
using FirstStation = std::function<int(int seg, int end)>;
void node_line_spans(const RoadNetwork &net, int n, const FirstStation &first_station, std::vector<LineSpan> &out);

// The segments whose placement node `n`'s line spans read (a reader working on a copy of the city keeps their furniture): the
// node's arms, and the arms of each arm's far node (a row with no pole of its own runs on to the pole its far end stands on).
void line_reads(const RoadNetwork &net, int n, std::vector<int> &out);
// The arm of node `n` that a line arriving on arm `seg` goes straight on into, or -1 (within the traffic's straight_dot).
int line_straight_on(const RoadNetwork &net, int n, int seg);
// Corner k of node n a line on segment `sid`'s face reaches at that node, or -1.
int line_corner_at(const RoadNetwork &net, int n, int sid);

} // namespace city
