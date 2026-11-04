// Shared steps of the road commands (Roadcraft construction/execute.ts and journal.ts): splitting a segment, resolving
// an anchor to a node, clearing what a new road covers, settling the ground and closing an edit (node shapes, lots,
// growth queue, money). Internal to sim/commands.
#pragma once

#include "sim/commands/commands.h"

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
// The segment's street furniture is placed and drawn again (its zones changed, or its square's hydrant may move).
void mark_furniture(CityState &s, int seg);
// What a road keeps when it is laid again (moved): its zones and flags.
struct RoadKeeps {
	int zl = 0, zr = 0;
	uint16_t flags = 0;
};
// build_road with `keeps` on every piece.
EditResult lay_road(CityState &s, const Anchor &a, bool has_control, double cx, double cz, const Anchor &b, int profile, const RoadKeeps &keeps);
// Ends an edit over the box: refreshes node shapes there, places again the furniture of the hydrant squares
// of the nodes it changed (street_furniture.h hydrant_square), recomputes the dirty lot cells (queued for growth) and bumps
// the revision.
void finish_edit(CityState &s, double minx, double minz, double maxx, double maxz);

} // namespace city
