// Growth over time on the queued cells (Roadcraft world/growth.ts step and grow, world/growth/zoning.ts pickLot).
#include "sim/simulation/city_growth.h"

#include "core/random.h"
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

// The free lot the next building grows on (Roadcraft world/growth/zoning.ts pickLot): the one nearest the last that grew, when
// one lies within sim.grow_on_reach_m (the street the player zoned fills in, house after house); otherwise one drawn at random of
// all the free lots, by a stream of the city's seed and the count grown, so a city zoned the same way grows the same way. Cells
// left without a free lot leave the queue.
Pick pick_free(CityState &s) {
	GrowthQueue &g = s.growth;
	const double reach = s.catalog.tune.sim_grow_on_reach_m;
	Pick best;
	best.d2 = reach * reach;
	int open = 0;
	for (size_t c = 0; c < g.cells.size();) {
		const std::vector<Lot> *lots = s.lots.lots_of(g.cells[c]);
		bool any = false;
		for (int i = 0; lots && i < int(lots->size()); i++) {
			const Lot &l = (*lots)[i];
			if (lot_occupied(s, l)) continue;
			any = true;
			open++;
			if (!g.has_last) continue;
			const double d2 = (l.x - g.last_x) * (l.x - g.last_x) + (l.z - g.last_z) * (l.z - g.last_z);
			if (d2 < best.d2) best = { g.cells[c], i, d2 };
		}
		if (any) {
			c++;
		} else {
			g.members.erase(g.cells[c]);
			g.cells[c] = g.cells.back();
			g.cells.pop_back();
		}
	}
	if (best.lot >= 0 || open == 0) return best;
	// None so near: the lot drawn, counted again over the cells that kept a free lot (the walk above settled the queue).
	int drawn = int(hash_combine(s.seed, uint32_t(g.grown)) % uint32_t(open));
	for (int64_t key : g.cells) {
		const std::vector<Lot> &lots = *s.lots.lots_of(key);
		for (int i = 0; i < int(lots.size()); i++) {
			if (lot_occupied(s, lots[i])) continue;
			if (drawn-- == 0) return { key, i, 0 };
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
	for (int tries = s.catalog.tune.sim_growth_tries; g.acc >= 1 && tries > 0; tries--) {
		Pick p = pick_free(s);
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
