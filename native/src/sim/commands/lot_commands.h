// The lots the player draws (Zones > Lots, lot_commands.cpp): added, cut by a road, shaped, split, joined, deleted, zoned. Refusals
// name the hint. Callers hold map_mutex exclusively, as for every command (commands.h).
#pragma once

#include "sim/commands/commands.h"

namespace city {

EditResult add_lot_rect(CityState &s, const XZ &a, const XZ &b, double angle);
EditResult add_lot_polygon(CityState &s, const std::vector<XZ> &points);
// A road edit cuts the lots it crosses, in its own step of undo (cutLotsByRoads): each is replaced by the pieces left on land, of its
// zone where they are big enough, and what stood on it goes.
void cut_lots_by_roads(CityState &s, double minx, double minz, double maxx, double maxz);
EditResult move_lot_corner(CityState &s, const XZ &from, const XZ &to);
EditResult curve_lot_side(CityState &s, const XZ &a, const XZ &b, const XZ &through);
EditResult set_lot_front(CityState &s, int id, int side);
EditResult split_lot(CityState &s, int id, const LotCut &cut);
EditResult join_lots(CityState &s, int a, int b);
EditResult delete_lot(CityState &s, int id);
// The delete stroke (lotTool.ts eraseUnder and up): every lot the way passes over goes with what stands on it, and every building
// whose ground blocks it passes over, on a lot or not; one undo step. Refused when it passed over nothing.
EditResult erase_lots(CityState &s, const std::vector<XZ> &stroke);
EditResult zone_lot(CityState &s, int id, int zone);

} // namespace city
