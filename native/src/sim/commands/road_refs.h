// What other records name a road by, carried when the road is taken up and laid again (split_segment, move_node, set_road_bend,
// reverse_road, merge_joint, the profile change's flip, the roundabout's cut; a crater's cut splits through split_segment): the hand
// links and bans its nodes hold (RoadNode::links) and the bus stops on it (TransitStop::seg). They are taken before the road
// goes (j_remove_segment drops them, road_network.h ids being reused), and each is written again, through the journal, on the
// piece that now meets the same node (a stop: the nearest piece in its reach). What finds no such piece stays dropped. Internal
// to sim/commands.
#pragma once

#include "core/math.h"
#include "sim/roads/road_network.h"

#include <vector>

namespace city {

struct CityState;

// A hand link or ban that named a road taken up. A road it names is `due` until that road is laid again: the field keeps the
// old id until then, and only the flag says so (an old id may already be a new piece's).
struct HeldLink {
	int node = -1; // the node that held it
	LaneLink link; // its roads, those carried already re-pointed
	bool from_due = false, to_due = false;
	bool lost = false; // a road of it came back meeting no node it met: it stays dropped
	bool written = false; // back on its node
};

// A bus stop on a road taken up.
struct HeldStop {
	int id = 0; // TransitStop::id (the plan's index is read when it is written)
	int seg = -1; // the road it was on
	V2 at;
	bool done = false;
};

struct RoadRefs {
	std::vector<HeldLink> links;
	std::vector<HeldStop> stops;
	int moved = -1, to = -1; // move_node: the node moved, and the node it ends as (the links it held go there)
};

// What names any of `segs`, taken while they still stand.
RoadRefs refs_of(const CityState &s, const std::vector<int> &segs);
// The links held at `node` die with it: the node goes in the re-laying (a joint merged away, a bent road's inner joints, a
// junction a ring replaces). Its id may be a new node's by the time the road stands again, so no piece meeting that id takes them.
void forget_refs_at(RoadRefs &refs, int node);
// Road `old` stands again as `pieces`: every held link naming it is re-pointed to the piece meeting the node that holds it (the
// moved node's links: the node it ended as), and is written back on that node once none of its roads is due, if both its roads
// are that node's arms then. Every stop it held goes to the nearest piece it stands by (transit_stop_by_road), or stays on none.
void carry_refs(CityState &s, RoadRefs &refs, int old, const std::vector<int> &pieces);

} // namespace city
