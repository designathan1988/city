// The zone brush (Zones › Zone): a stroke zones whole lots, never a stretch of street. Under each dab the player's lot is zoned
// (zone_lot); where the player holds none, the land lot there becomes the player's, of the zone painted: a zoned lot of the streets'
// (the generated city's), or the lot the streets propose there, cut for the zone painted (LotCache::proposed). Zone 0, the eraser,
// takes the zone off what it passes and lays no lot on unzoned land. Everything a stroke reaches derives once, when its command ends.
#include "sim/commands/commands.h"

#include "sim/lots/lot_cache.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr const char *ZONE_SAME = "tools.toast.zoneSame"; // every lot the stroke met already had the zone
constexpr const char *ZONE_EMPTY = "hint.zone.empty"; // the stroke met no lot, nor land a lot could be laid on
constexpr double SAME_CORNER_M = 0.01; // two land lots met by two dabs are one when their first corners meet

bool same_ring(const std::vector<LotPoint> &a, const std::vector<LotPoint> &b) {
	return std::hypot(a[0].x - b[0].x, a[0].z - b[0].z) < SAME_CORNER_M;
}
} // namespace

EditResult paint_zone_path(CityState &s, const std::vector<StrokePoint> &dabs, int zone) {
	EditResult all;
	if (zone < 0 || zone >= ZONE_CODES) {
		all.error = "tool.failed";
		return all;
	}
	// Erasing lays no lot; painting proposes the land cut for the zone painted.
	const int propose = zone == 0 ? -1 : int(cut_for_works(s.catalog, zone));
	std::vector<int> held; // the player's lots met, in order
	std::vector<LandLot> land; // the land lots met, in order
	bool met = false;
	for (const StrokePoint &p : dabs) {
		const int id = s.lot_book.at(p.x, p.z);
		if (id != 0) {
			met = true;
			if (std::find(held.begin(), held.end(), id) == held.end()) held.push_back(id);
			continue;
		}
		LandLot l;
		if (!land_lot_at(s, { p.x, p.z }, propose, l)) continue;
		met = true;
		if (l.zone == zone) continue; // a street lot of the zone already
		if (std::none_of(land.begin(), land.end(), [&](const LandLot &o) { return same_ring(o.ring, l.ring); })) land.push_back(std::move(l));
	}
	const auto add = [&](const EditResult &r) {
		if (!r.ok) return;
		all.ok = true;
		all.zoned_sides++;
		all.buildings_removed += r.buildings_removed;
	};
	for (const LandLot &l : land) add(take_land_lot(s, l, zone));
	for (int id : held) add(zone_lot(s, id, zone));
	if (!all.ok) all.error = met ? ZONE_SAME : ZONE_EMPTY;
	return all;
}

} // namespace city
