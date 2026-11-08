// Shared steps of the road commands (Roadcraft construction/execute.ts and journal.ts): splitting a segment, resolving
// an anchor to a node, clearing what a new road covers, settling the ground and closing an edit (node shapes, lots,
// growth queue, money). Internal to sim/commands.
#pragma once

#include "sim/commands/commands.h"
#include "sim/roads/road_network.h"

#include <algorithm>
#include <limits>
#include <map>
#include <vector>

namespace city {

constexpr size_t JOINT_ARMS = 2; // a road going on through a node

// Splits segment `seg` at t; returns the new node (height linear along the segment).
int split_segment(CityState &s, int seg, double t);
// The node an anchor stands for: its node, a split of its segment (searched again when an earlier split took it) or a
// new node on the ground.
int anchor_node(CityState &s, const Anchor &a, float lift = 0); // lift: of a new node
// How high over the grade line a road's end stands: a node's own, the road's it lands on, or the height being drawn at.
float anchor_lift(const CityState &s, const Anchor &a);
// Removes the buildings whose lot the segment's surface covers; returns how many.
int clear_under(CityState &s, int seg);
// Removes the segment and the nodes it leaves alone; the command settles what comes back (road_economy.h settle_road_edit).
void demolish_segment(CityState &s, int seg);
// The ground moved under the box: its roads, buildings and lots are drawn / derived again (brush.ts touchSamples).
void mark_ground(CityState &s, double minx, double minz, double maxx, double maxz);
// The painted ground of a sample rectangle is drawn again, its trees and covers placed again.
void mark_paint(CityState &s, const int rect[4]);
// The plants of the box are placed again.
void mark_nature(CityState &s, double minx, double minz, double maxx, double maxz);
// The segment's street furniture is placed and drawn again (its zones changed, or its square's hydrant may move).
void mark_furniture(CityState &s, int seg);
// What a road keeps when it is laid again (moved): its zones and flags.
struct RoadKeeps {
	int zl = 0, zr = 0;
	uint16_t flags = 0;
};
// A road laid as build_road lays it, with `keeps` on every piece, and no money moved: the command it is part of settles once.
EditResult lay_road(CityState &s, const Anchor &a, bool has_control, double cx, double cz, const Anchor &b, int profile, const RoadKeeps &keeps,
		std::vector<int> *laid = nullptr); // laid: the segments it leaves standing
// build_road without the money: laid and checked (a stretch squeezed between junctions refused); the command settles.
EditResult place_road(CityState &s, const Anchor &a, bool has_control, double cx, double cz, const Anchor &b, int profile);
// The curve through both ends of two pieces of one road, false where their tangents never meet.
bool joined_quad(const Quad &first, const Quad &second, Quad &out);
// The area an edit touches.
struct RoadBox {
	double minx = std::numeric_limits<double>::max(), minz = minx, maxx = std::numeric_limits<double>::lowest(), maxz = maxx;
	void add(double x, double z) {
		minx = std::min(minx, x); minz = std::min(minz, z);
		maxx = std::max(maxx, x); maxz = std::max(maxz, z);
	}
	void add(const RoadSegment &g) {
		add(double(g.minx), double(g.minz));
		add(double(g.maxx), double(g.maxz));
	}
};
// The two arms of `joint` made one road, when they run on and are the same road but for it; false, nothing done, when not. `past`: only if one of them is
// this arm (-1: either). What the arms were in `made` is the merged road there.
bool merge_joint(CityState &s, int joint, int past, RoadBox &box, std::vector<int> &made);
// How far from node `n` the junction wants the mouth of `seg`, before the road's length caps it (road_rules.cpp).
double wanted_retreat(const RoadNetwork &net, int n, int seg);
// The roads of the box whose two mouths overlap, and by how many metres (road_rules.cpp).
std::map<int, double> squeezes_in(const CityState &s, const RoadBox &box);
// Whether a road of the box overlaps more than `before` said.
bool squeezed_worse(const CityState &s, const std::map<int, double> &before, const RoadBox &box);
// The junction a new road's end would make, before it exists: how far back that road stops there and how far back each road
// it meets does (road_rules.cpp). A free end makes none.
struct EndMouth {
	int node = -1, seg = -1;
	double t = 0, own = 0;
	std::vector<std::pair<int, double>> met;
	// Whether the point at `at_t` of road `other` lies under this junction, or within `pad` metres of it.
	bool covers(const RoadNetwork &net, int other, double at_t, double pad) const;
};
EndMouth end_mouth(const CityState &s, const Anchor &at, const Quad &q, int end, int profile);
// What the player built at (x, z) changed: the chunk of its props and the walkers' bodies on the segments round it.
void mark_street(CityState &s, double x, double z);
// What a change to the network derives, settled where a command reads it before it ends (a road laid in pieces reads the
// mouths and heights of the pieces before it): the shapes of the nodes the changes reach (RoadNetwork::settle_shapes) and
// the heights of what they moved (sync_grade). A change that leaves the network alone settles nothing.
void settle_network(CityState &s);
// Ends a command, once per transaction (EditHistory::end, apply_entry): settles the network, places again the furniture of
// the hydrant squares of the nodes it changed (street_furniture.h hydrant_square), recomputes the dirty lot cells (queued
// for growth) and, when the command recorded a change or left derived work, bumps the edit revision and the map's.
void finish_command(CityState &s, bool recorded);

} // namespace city
