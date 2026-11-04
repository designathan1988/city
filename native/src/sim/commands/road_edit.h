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

// Arc length at curve parameter t from an arc_table (plan.ts arcAtT).
double arc_at(const std::vector<double> &table, double t);
// Value of `len` metres of a profile built on the ground (Roadcraft roads/economy.ts segmentPrice, kind ground).
double road_value(const CityState &s, int profile, double len);
// Splits segment `seg` at t; returns the new node (height linear along the segment).
int split_segment(CityState &s, int seg, double t);
// The node an anchor stands for: its node, a split of its segment (searched again when an earlier split took it) or a
// new node on the ground.
int anchor_node(CityState &s, const Anchor &a);
// Removes the buildings whose lot the segment's surface covers; returns how many.
int clear_under(CityState &s, int seg);
// The ground follows the new segment (a terrain corridor modifier, as the generated city's streets).
void conform_under(CityState &s, int seg);
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
// build_road with `keeps` on every piece.
EditResult lay_road(CityState &s, const Anchor &a, bool has_control, double cx, double cz, const Anchor &b, int profile, const RoadKeeps &keeps);
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
// Ends an edit over the box: refreshes node shapes there, places again the furniture of the hydrant squares
// of the nodes it changed (street_furniture.h hydrant_square), recomputes the dirty lot cells (queued for growth) and bumps
// the revision.
void finish_edit(CityState &s, double minx, double minz, double maxx, double maxz);

} // namespace city
