// Areas of building plans, exact for any simple ring: a ring is cut into triangles (ears, Meisters 1975) and triangles are
// clipped by each other's edges (Sutherland and Hodgman 1974), so a concave plan needs no general polygon clipper.
#pragma once

#include "sim/buildings/b40/types.h"

#include <vector>

namespace city::b40 {

using Ring = std::vector<Point2>;

// The plan of a volume in the lot frame, counter-clockwise.
Ring ring_of(const Volume &v);
float ring_area(const Ring &ring); // signed: positive counter-clockwise
float overlap_area(const Ring &a, const Ring &b);
// The share (0..1) of `ring` lying over `supports`; where supports overlap each other their area counts once.
float support_share(const Ring &ring, const std::vector<Ring> &supports);
bool contains_point(const Ring &ring, Point2 p);

} // namespace city::b40
