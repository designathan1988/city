// The parking invariants, over the traffic's parked side (sim/traffic/parking_state.h), checked once the traffic has drained the
// change log (before that its tables are legitimately behind the city):
// - parking.lot_sources_live: every lot read (ParkingState::lots) is an alive building, and each of its bays is alive, its own and
//   within the building's reach (half its lot's diagonal from its centre). A record: the building's id.
// - parking.yard_docks_live: every yard planned is an alive building's, and while its lot is read each dock with a way names, by its
//   place among the lot's bays (yard.h), an alive dock bay of that lot. A lot not read keeps its plan (the camera left). A record:
//   the building's id.
// - parking.lorry_target_live: every lorry bound to its dock (before it starts out: lorry_bound) has an alive building and an alive
//   bay of that building. A record: the lorry's vehicle id.
// Whole or scoped alike: the tables are bounded by the parking window (park_window_m) and the lorry count (lorry_most), so the pass
// is O(lot sources + their bays + planned docks + lorries), each a direct lookup; nothing allocated.
#include "sim/checks/records.h"

#include "sim/state/city_state.h"
#include "sim/traffic/yard.h"

namespace city::checks {

namespace {
constexpr const char *FEATURE = "parking";
constexpr const char *LOT_SOURCES_LIVE = "lot_sources_live";
constexpr const char *YARD_DOCKS_LIVE = "yard_docks_live";
constexpr const char *LORRY_TARGET_LIVE = "lorry_target_live";

const Building *alive_building(const CityState &s, int id) {
	if (id < 0 || size_t(id) >= s.buildings.items.size()) return nullptr;
	const Building &b = s.buildings.items[size_t(id)];
	return b.alive ? &b : nullptr;
}

const Bay *alive_bay(const ParkingState &p, int b) {
	if (b < 0 || size_t(b) >= p.bays.size()) return nullptr;
	return p.bays[size_t(b)].alive ? &p.bays[size_t(b)] : nullptr;
}

bool lot_sound(const CityState &s, int id, const ParkSource &src) {
	const Building *rec = alive_building(s, id);
	if (!rec) return false;
	const float reach2 = (rec->w * rec->w + rec->d * rec->d) * 0.25f; // half the diagonal, squared: every point of the lot is within it
	for (int b : src.bays) {
		const Bay *bay = alive_bay(s.traffic.park, b);
		if (!bay || bay->kerb || bay->source != id) return false;
		const float dx = bay->at.x - rec->x, dz = bay->at.y - rec->z;
		if (dx * dx + dz * dz > reach2) return false;
	}
	return true;
}

bool yard_sound(const CityState &s, int id, const traffic::YardPlan &plan) {
	if (!alive_building(s, id)) return false;
	const ParkingState &p = s.traffic.park;
	if (p.lots.count(id) == 0) return true; // its lot let go behind the camera: the plan waits for it, its places unresolved
	for (const traffic::DockWays &w : plan.docks) {
		if (w.gate < 0) continue;
		const Bay *bay = alive_bay(p, traffic::lot_bay(p, id, w.bay));
		if (!bay || !bay->dock || bay->source != id) return false;
	}
	return true;
}

bool lorry_sound(const CityState &s, const Lorry &l) {
	if (!traffic::lorry_bound(l)) return true; // driving out: it names neither any more
	if (!alive_building(s, l.building)) return false;
	const Bay *bay = alive_bay(s.traffic.park, l.bay);
	return bay && !bay->kerb && bay->source == l.building;
}
} // namespace

void check_parking(const CityState &s, const CheckScope &, Broken &broken) {
	const ParkingState &p = s.traffic.park;
	for (const auto &lot : p.lots)
		if (!lot_sound(s, lot.first, lot.second)) broken.add(FEATURE, LOT_SOURCES_LIVE, lot.first);
	for (const auto &yard : p.yards)
		if (yard.second && !yard_sound(s, yard.first, *yard.second)) broken.add(FEATURE, YARD_DOCKS_LIVE, yard.first);
	for (const Lorry &l : p.lorries)
		if (!lorry_sound(s, l)) broken.add(FEATURE, LORRY_TARGET_LIVE, l.id);
}

} // namespace city::checks
