// A lot against the streets (Roadcraft world/lots.ts onLand, cutLotsByRoads, lotSnapper): the paving round a box as polygons, what of
// a ring stands off it, which of its sides faces the street and where a point of a lot lands by a footway.
#pragma once

#include "sim/lots/lot_book.h"

#include <vector>

namespace city {

class RoadNetwork;

// What a lot's point landed on.
enum class LotSnapKind : uint8_t { NONE, CORNER, STREET, LOT };
struct LotSnap {
	LotPoint p;
	LotSnapKind kind = LotSnapKind::NONE;
};

// A ring drawn over the street cut back to the land: its biggest piece, corners squared where two streets make them. Empty when
// less than a lot is left; with no road on the map, the ring as it came.
std::vector<LotPoint> lot_on_land(const RoadNetwork &net, const std::vector<LotPoint> &ring);
// The pieces of a lot a road crossed, big enough to be lots; `crossed` false where the paving only meets its outline.
std::vector<std::vector<LotPoint>> lot_pieces(const RoadNetwork &net, const std::vector<LotPoint> &ring, bool &crossed);
// The longest of the sides against a street.
int lot_front_side(const RoadNetwork &net, const std::vector<LotPoint> &ring);
// A point near a street put on the outer edge of its footway, on the corner of two footways, or on a corner of another lot; `skip`
// leaves one lot out and `apart` the corners at one point (the corner being dragged).
LotSnap lot_snap(const LotBook &book, const RoadNetwork &net, const LotPoint &p, double reach, int skip, const LotPoint *apart);

} // namespace city
