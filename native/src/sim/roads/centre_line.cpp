// A segment's centre line in plan (centre_line.h).
#include "sim/roads/centre_line.h"

#include "sim/roads/road_network.h"

#include <cmath>

namespace city {

void centre_at(const RoadNetwork &net, int s, double at, double &x, double &z, double &fx, double &fz) {
	const RoadSegment &g = net.segs[size_t(s)];
	const double t = t_at_length(g.table, at);
	double dx, dz;
	g.q.at(t, x, z);
	g.q.deriv(t, dx, dz);
	double l = std::hypot(dx, dz);
	if (l == 0) l = 1;
	fx = dx / l;
	fz = dz / l;
}

} // namespace city
