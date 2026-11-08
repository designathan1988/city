// The way a line of poles goes (Roadcraft poleLines.ts and poleRoutes.ts): the guide lines just behind the kerbs, one per
// sidewalk of each segment, joined round the corners, with spans over the streets. A run is the cheapest way through them,
// a span costing poles.crossing_cost_m over its width so that the line keeps to one side unless crossing saves it.
#pragma once

#include "core/math.h"
#include "sim/roads/road_network.h"

#include <vector>

namespace city {

class RouteGraph {
public:
	// The guide lines of the segments meeting the box.
	void build(const RoadNetwork &net, double x0, double z0, double x1, double z1);
	// The station nearest (x, z) within `reach`, or -1; `at` is the nearest point of the guide line there.
	int nearest(double x, double z, double reach) const;
	XZ point(int station) const { return { x_[size_t(station)], z_[size_t(station)] }; }
	bool empty() const { return x_.empty(); }
	// The stations from `from` to `to` by the cheapest way, or empty when no line joins them.
	std::vector<int> route(int from, int to, double &cost) const;

private:
	struct Edge {
		int to = -1;
		double cost = 0;
	};
	std::vector<double> x_, z_;
	std::vector<std::vector<Edge>> edges_;
	int add(double x, double z);
	void link(int a, int b, double extra);
};

} // namespace city
