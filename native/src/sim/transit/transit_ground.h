// Where the transport meets the streets: whether a point is on a carriageway (a track there is laid flush, a metro entrance moves off it,
// a train's track may not run down one). Read through a CityView: the live city (its caller holds the map) or a job's cut of it.
#pragma once

#include "core/math.h"
#include "sim/state/city_view.h"

namespace city {

bool transit_on_road(const CityView &s, const V2 &p);
// The street's surface at a point, the road's own height and `kerb` more on a footway; false off the street.
bool transit_paved(const CityView &s, const V2 &p, float kerb, float &y, bool &carriageway);
// A stop's frame: along its street or track, and out (from the kerb to the back of the footway; on a track, to the side of its platform).
// `track`: the track the stop stands on (TransitStop::track), null when it stands on none.
void transit_stop_frame(const CityView &s, const TransitStop &q, const RailTrack *track, V2 &along, V2 &out);
// The same in the live city, its track found in the plan.
void transit_stop_frame(const CityState &s, const TransitStop &q, V2 &along, V2 &out);
// The track of the live plan a stop stands on, or null.
const RailTrack *transit_track_of(const CityState &s, const TransitStop &q);
// Whether a building stands within `pad` of a point.
bool transit_built(const CityView &s, const V2 &p, float pad);

} // namespace city
