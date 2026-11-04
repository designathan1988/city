// Point queries on the road network (Roadcraft 4.0 world/carriageway.ts onCarriageway, world/footwayPath.ts
// nearestFootway): whether a point is on a carriageway (a segment's asphalt band or a junction's plate) and the road's
// height there, and the nearest point of a footway's walking line. Only the segments near the point are read.
#pragma once

#include "sim/roads/road_network.h"

#include <functional>

namespace city {

struct FootwayHit {
	bool ok = false;
	float x = 0, z = 0, d = 0, arc = 0;
	int seg = -1, side = 0; // side 0 the right footway (walk_right), 1 the left one
};

namespace roads {

// On a road's carriageway; `y` gets the road's height there when it is.
bool on_carriageway(const RoadNetwork &net, double x, double z, double *y = nullptr);
// The nearest point within `reach` of a footway's walking line; a point `rejects` turns down is replaced by the
// nearest one of that footway it accepts, sampled every `step` metres along it.
FootwayHit nearest_footway(const RoadNetwork &net, double x, double z, double reach, double step,
		const std::function<bool(double, double)> &rejects = nullptr);

} // namespace roads
} // namespace city
