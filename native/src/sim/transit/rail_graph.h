// The tracks of one mode as a graph (Roadcraft 4.0 world/transit.ts railGraph, railPath): every point of every track
// a node (ends and corners within join_m of one another are one), each run between two a stretch; and the way along
// the tracks from one point on them to another (Dijkstra from the two ends of the start's stretch).
#pragma once

#include "sim/transit/transit_plan.h"

#include <vector>

namespace city {

struct RailEdge {
	int a = 0, b = 0;
	float length = 0;
};

struct RailGraph {
	TransitMode mode = TransitMode::TRAIN;
	std::vector<V2> points;
	std::vector<RailEdge> edges;
	std::vector<std::vector<int>> at; // per point: the edges meeting it
};

namespace transit {

void rail_graph(const TransitPlan &p, TransitMode mode, float join_m, RailGraph &out);
// The points to pass from `from` to `to` along the tracks (both put on their nearest stretch); false when the
// tracks do not join them.
bool rail_path(const RailGraph &g, V2 from, V2 to, std::vector<V2> &out);

} // namespace transit
} // namespace city
