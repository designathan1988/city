// Growth over time on the queued cells (Roadcraft world/growth.ts step / nearestFree / grow).
#include "sim/simulation/city_growth.h"

#include "sim/buildings/growth.h"
#include "sim/state/city_state.h"

#include <algorithm>

namespace city {

namespace {
constexpr double MAX_OWED = 50; // growth.ts: acc is capped at 50 buildings

} // namespace

void grow_step(CityState &s, double dt) {
	GrowthQueue &g = s.growth;
	if (g.cells.empty() || s.damage.now < s.damage.growth_hold_until) return; // rubble lies: nothing is built over it yet
	g.acc = std::min(g.acc + dt * s.catalog.tune.sim_growth_per_s, MAX_OWED);
	for (int tries = int(s.catalog.tune.sim_growth_tries); g.acc >= 1 && tries > 0; tries--) {
		GrowthPick p = g.nearest(s);
		if (p.lot < 0) {
			g.acc = 0;
			return;
		}
		const std::vector<Lot> &lots = *s.lots.lots_of(p.key);
		Building b = record_for(s, lots, p.lot);
		b.built = s.clock.minutes.load();
		s.buildings.add(b);
		g.last_x = b.x;
		g.last_z = b.z;
		g.has_last = true;
		g.grown++;
		g.acc -= 1;
		s.touch(); // a new building is a change to the map (saved)
	}
}

} // namespace city
