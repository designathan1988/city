// A grown building made to its lot's own shape. The plan is laid on a rectangle (the lot's stand); on a lot of another shape (one the
// player drew: a polygon, a bent side, a lot that narrows at the back) the body takes the lot's outline: its side and back boundary
// leave the rectangle and run along the lot's own sides, all but the front; what the plan set down outside the lot is left out; and
// the outline is kept on the body (Body::lot_ring), where the render lays the lot's lawn to it. A lot that is its rectangle is left
// as planned.
#pragma once

#include "sim/buildings/b40/types.h"

#include <cstdint>
#include <vector>

namespace city::b40 {

// `ring`: the lot's corners in the lot frame (x from the front's middle along it, y away from the street), its first side the front.
void fit_outline(Body &b, const std::vector<Point2> &ring);
// What names a ring exactly, for a body's key (body_table.h BodyKey::ring): its corners' bits, hashed. 0 is no ring.
uint64_t ring_stamp(const std::vector<Point2> &ring);
// Whether a point of the lot frame lies in the ring.
bool in_ring(const std::vector<Point2> &ring, float x, float y);

} // namespace city::b40
