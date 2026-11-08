// The commands of the Zones tool (lot_commands.cpp, zone_commands.cpp): the lots the player draws and edits in the Lots tab, the land
// lots (the generated city's and the ones the streets propose) taken as the player's, and the zone brush that zones whole lots.
// Refusals name the hint. Included by commands.h, the one header of the player's edits.
#pragma once

#include "sim/lots/lot_rings.h"

#include <vector>

namespace city {

struct CityState;
struct EditResult;
struct StrokePoint;

EditResult add_lot_rect(CityState &s, const LotPoint &a, const LotPoint &b, double angle);
EditResult add_lot_polygon(CityState &s, const std::vector<LotPoint> &points);
// A road edit cuts the lots it crosses, in its own step of undo (cutLotsByRoads): each is replaced by the pieces left on land, of its
// zone where they are big enough, and what stood on it goes.
void cut_lots_by_roads(CityState &s, double minx, double minz, double maxx, double maxz);
EditResult move_lot_corner(CityState &s, const LotPoint &from, const LotPoint &to);
EditResult curve_lot_side(CityState &s, const LotPoint &a, const LotPoint &b, const LotPoint &through);
EditResult set_lot_front(CityState &s, int id, int side);
EditResult split_lot(CityState &s, int id, const LotCut &cut);
EditResult join_lots(CityState &s, int a, int b);
EditResult delete_lot(CityState &s, int id);
// The lot zoned (0: the zone taken off, and what grew there with it; another zone keeps what stands).
EditResult zone_lot(CityState &s, int id, int zone);
// A lot of the land the player does not hold yet: a zoned lot of the streets' (its zone), or a lot the streets propose (zone 0).
struct LandLot {
	std::vector<LotPoint> ring;
	int zone = 0;
};
// The land lot under p: the zoned street lot there, else, with `propose` 0 or 1, the lot proposed there cut for works (1) or not
// (LotCache::proposed), left out where it would run into a lot of the player's or a zoned street lot; false on none.
bool land_lot_at(const CityState &s, const LotPoint &p, int propose, LandLot &out);
// That lot made the player's, of `zone` (0 takes what grew there).
EditResult take_land_lot(CityState &s, const LandLot &lot, int zone);
// The land lot under p made the player's with its own zone, to be edited as a lot (lotTool.ts adoptProposal); `placed` its id.
EditResult take_lot_at(CityState &s, const LotPoint &p);
// The Lots tab's delete stroke (lotTool.ts erase): the player's lots under its dabs, what stands on them, and every building whose
// footprint is under a dab, on a lot or not.
EditResult erase_lots_path(CityState &s, const std::vector<StrokePoint> &dabs);
// The zone brush (Zones › Zone): the dabs of one pointer move, in order, as one command, each zoning the lot under it whole: the
// player's lot there (zone_lot), else the land lot there (land_lot_at, proposed for the zone painted), which becomes the player's.
// Zone 0 takes the zone off and makes no lot of unzoned land. `zoned_sides` counts the lots zoned; `error` "hint.zone.empty"
// when no dab met a lot, "tools.toast.zoneSame" when every lot met had the zone already.
EditResult paint_zone_path(CityState &s, const std::vector<StrokePoint> &dabs, int zone);

} // namespace city
