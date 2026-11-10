// What a road job copies of the city before it builds (sim/state/city_cut.h): the cut of its chunk, or superchunk, and of the
// segments it owns, grown by how far the road pieces read past a segment's own box. One owner for the road chunks, the
// structures and the far roads, so the three cut the same place.
#pragma once

#include "render/roads/road_look.h"
#include "sim/state/city_cut.h"

#include <cstdint>

namespace city {

// How far past a segment's box (its curve grown by half its section) the road pieces read the ground, the roads and the lots:
// the casing band, then the farthest of the ground a cutting's wall holds, a stayed bridge's legs, a portal's wings, a pier's
// foot and the car gates of the lots that face it; and a cell of the terrain round it all.
float road_reach(const Catalog &catalog, const StreetLook &look);

// The cut of the square `key` of side `size` (a chunk or a superchunk) and of the segments whose middle it holds, with `parts`.
// Takes the map shared for the copy alone: called by the job, on its worker.
CityCut cut_for_roads(const CityState &s, const StreetLook &look, int64_t key, float size, unsigned parts);

} // namespace city
