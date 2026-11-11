// A segment's centre line in plan: the one frame every reader of a segment's plan stands on (the footways, a node's outline, the
// road surfaces).
#pragma once

namespace city {

class RoadNetwork;

// Segment s's centre line at arc length `at` from node a, in plan, and its unit tangent from a to b there.
void centre_at(const RoadNetwork &net, int s, double at, double &x, double &z, double &fx, double &fz);

} // namespace city
