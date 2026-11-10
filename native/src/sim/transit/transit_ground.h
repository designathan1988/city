// Where the transport meets the streets: whether a point is on a carriageway (a track there is laid flush, a metro entrance moves off it,
// a train's track may not run down one).
#pragma once

#include "core/math.h"

namespace city {

struct CityState;

struct TransitStop;

bool transit_on_road(const CityState &s, const V2 &p);
// The street's surface at a point, the road's own height and `kerb` more on a footway; false off the street.
bool transit_paved(const CityState &s, const V2 &p, float kerb, float &y, bool &carriageway);
// A stop's frame: along its street or track, and out (from the kerb to the back of the footway; on a track, to the side of its platform).
void transit_stop_frame(const CityState &s, const TransitStop &q, V2 &along, V2 &out);
// Whether a building stands within `pad` of a point.
bool transit_built(const CityState &s, const V2 &p, float pad);

} // namespace city
