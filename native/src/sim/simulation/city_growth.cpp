// Growth over time on the queued cells (Roadcraft world/growth.ts step / nearestFree / grow).
#include "sim/simulation/city_growth.h"

#include "sim/buildings/growth.h"
#include "sim/state/city_state.h"

#include <algorithm>

namespace city {

namespace {
constexpr double MAX_OWED = 50; // growth.ts: acc is capped at 50 buildings

struct Pick {
	int64_t key = 0;
	int lot = -1;
	double d2 = 0;
};

// The free lot nearest the last one grown among the queued cells; cells left without a free lot leave the queue.
Pick nearest_free(CityState &s) {
	GrowthQueue &g = s.growth;
	Pick best;
	for (size_t c = 0; c < g.cells.size();) {
		const std::vector<Lot> *lots = s.lots.lots_of(g.cells[c]);
		bool any = false;
		for (int i = 0; lots && i < int(lots->size()); i++) {
			const Lot &l = (*lots)[i];
			if (lot_occupied(s, l)) continue;
			any = true;
			double d2 = g.has_last ? (l.x - g.last_x) * (l.x - g.last_x) + (l.z - g.last_z) * (l.z - g.last_z) : 0;
			if (best.lot < 0 || d2 < best.d2) best = { g.cells[c], i, d2 };
		}
		if (any) {
			c++;
		} else {
			g.members.erase(g.cells[c]);
			g.cells[c] = g.cells.back();
			g.cells.pop_back();
		}
	}
	return best;
}
} // namespace

void GrowthQueue::push(int64_t key) {
	if (members.insert(key).second) cells.push_back(key);
}

void GrowthQueue::assign(std::vector<int64_t> keys) {
	cells.clear();
	members.clear();
	for (int64_t k : keys) push(k);
}

void grow_step(CityState &s, double dt) {
	GrowthQueue &g = s.growth;
	if (g.cells.empty() || s.damage.now < s.damage.growth_hold_until) return; // rubble lies: nothing is built over it yet
	g.acc = std::min(g.acc + dt * s.catalog.tune.sim_growth_per_s, MAX_OWED);
	for (int tries = int(s.catalog.tune.sim_growth_tries); g.acc >= 1 && tries > 0; tries--) {
		Pick p = nearest_free(s);
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
