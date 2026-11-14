// Every building's lot graded to the street it faces (sim/terrain/lot_shape.h): its level the footway's top along its front less the
// lot's plate, the sidewalk in front of it graded with it, so the lot's covers (render/buildings/lot_emit.cpp, a plate over the
// ground) meet the footway's top and the floor (the highest ground + the plinth) stands at the street. Beside road_grade_sync.h, the
// one place the network and the ground are seen together, as the lots read the buildings, the street and the ground at once:
// settled after every edit (sync_grade), the city's generation and growth, and the load.
#pragma once

#include "sim/buildings/b40/types.h"

#include <memory>
#include <vector>

namespace city {

struct Building;
struct CityState;

// The lots whose ground is due (the buildings that came, went or changed: BuildingStore::sites_due) and those facing a segment in
// `streets` (its heights moved, or it went), graded again under one held composition; what stands on that ground is marked. Each
// one's edges (lot_edges) are settled with it: a record whose edges changed is drawn again, its body made to them.
void settle_sites(CityState &s, const std::vector<int> &streets);
// What the edges of building `rec`'s lot meet (b40::LotEdges), read from the streets: the stretches of its front where the street
// outside it has no kerb a car may stand at (sim/roads/parking_layout.h kerb_standing: a junction's corner, its crosswalks, the place
// of its signal posts) or where the front faces a junction ahead of every street's end; and the sides a street runs along. The one
// reading of them: settle_sites keeps every record's own (Building::edges) so, a record made from the seed reads them before its
// body is made (growth, a drawn building), and the probes hold the bodies against it. Read from the record's rectangle alone: one not
// yet in the store is read the same.
b40::LotEdges lot_edges(const CityState &s, const Building &rec);
// The outline of the lot building `rec` stands on, in the record's lot frame (b40::LotFrame: x from the front's middle along it, y
// away from the street), where a lot the player drew holds its middle; none on a lot the streets cut. The one reading of it:
// growth reads it for the record it makes, settle_sites keeps every record's own (Building::ring) so after an edit and a load.
std::shared_ptr<const std::vector<b40::Point2>> lot_ring(const CityState &s, const Building &rec);

} // namespace city
