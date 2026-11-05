// Internal surface of the street furniture placement (sim/roads only): the sidewalk's own zones as both the segment
// pieces (street_furniture.cpp) and the posts (street_furniture_posts.cpp) read them.
#pragma once

#include "sim/roads/street_furniture.h"

namespace city {
namespace furniture {

inline const StreetFurnitureDef &defs(const RoadNetwork &net) { return net.cat->furniture; }

inline bool walk_on(const Profile &p, int side) { return (side == 0 ? p.edges.front() : p.edges.back()).strip.kind == StripKind::SIDEWALK; }
inline float walk_width(const Profile &p, int side) {
	const StripEdge &e = side == 0 ? p.edges.front() : p.edges.back();
	return e.u1 - e.u0;
}
// The u `from_kerb` metres in from the kerb's face of a sidewalk, and the direction toward the road from it.
inline float kerb_in(const Profile &p, int side, float from_kerb) { return side == 0 ? p.edges.front().u1 - from_kerb : p.edges.back().u0 + from_kerb; }
inline float toward_road(int side) { return side == 0 ? 1.f : -1.f; }

// A piece of kind `k` at (v, u) on segment `sid`, its +z toward (face_left, face_fwd) in the segment frame.
FurnitureSpot spot(const RoadNetwork &net, FurnitureKind k, int sid, const Profile &p, double v, float u, float face_left, float face_fwd);

} // namespace furniture
} // namespace city
