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

// Whether lot `l` is free: nothing stands on it and this step's plan has not taken it.
bool lot_free(const CityState &s, const GrowthPlan &plan, const Lot &l) {
	if (lot_occupied(s, l)) return false;
	return std::none_of(plan.add.begin(), plan.add.end(), [&](const Building &b) { return b.x == float(l.x) && b.z == float(l.z); });
}

// The free lot the next building grows on (Roadcraft world/growth/zoning.ts pickLot): the one nearest the last that grew, when
// one lies within sim.grow_on_reach_m (the street the player zoned fills in, house after house); otherwise one drawn at random of
// all the free lots, by a stream of the city's seed and the count grown, so a city zoned the same way grows the same way. Cells
// found without a free lot go to the plan's drop list (they leave the queue when it is applied).
Pick pick_free(const CityState &s, GrowthPlan &plan, bool has_last, double last_x, double last_z, int grown) {
	const GrowthQueue &g = s.growth;
	const double reach = s.catalog.tune.sim_grow_on_reach_m;
	Pick best;
	best.d2 = reach * reach;
	int open = 0;
	auto dropped = [&](int64_t key) { return std::find(plan.drop.begin(), plan.drop.end(), key) != plan.drop.end(); };
	for (int64_t key : g.cells) {
		if (dropped(key)) continue;
		const std::vector<Lot> *lots = s.lots.lots_of(key);
		bool any = false;
		for (int i = 0; lots && i < int(lots->size()); i++) {
			const Lot &l = (*lots)[i];
			if (!lot_free(s, plan, l)) continue;
			any = true;
			open++;
			if (!has_last) continue;
			const double d2 = (l.x - last_x) * (l.x - last_x) + (l.z - last_z) * (l.z - last_z);
			if (d2 < best.d2) best = { key, i, d2 };
		}
		if (!any) plan.drop.push_back(key);
	}
	if (best.lot >= 0 || open == 0) return best;
	// None so near: the lot drawn, counted again over the cells that kept a free lot.
	int drawn = int(hash_combine(s.seed, uint32_t(grown)) % uint32_t(open));
	for (int64_t key : g.cells) {
		if (dropped(key)) continue;
		const std::vector<Lot> &lots = *s.lots.lots_of(key);
		for (int i = 0; i < int(lots.size()); i++) {
			if (!lot_free(s, plan, lots[i])) continue;
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

GrowthPlan plan_growth(CityState &s, double dt) {
	GrowthQueue &g = s.growth;
	GrowthPlan plan;
	plan.revision = s.revision;
	if (g.cells.empty() || s.damage.now < s.damage.growth_hold_until) return plan; // rubble lies: nothing is built over it yet
	g.acc = std::min(g.acc + dt * s.catalog.tune.sim_growth_per_s, MAX_OWED);
	bool has_last = g.has_last;
	double last_x = g.last_x, last_z = g.last_z, owed = g.acc;
	for (int tries = s.catalog.tune.sim_growth_tries; owed >= 1 && tries > 0; tries--) {
		const Pick p = pick_free(s, plan, has_last, last_x, last_z, g.grown + int(plan.add.size()));
		if (p.lot < 0) {
			plan.none_free = plan.add.empty();
			break;
		}
		plan.add.push_back(record_for(s, *s.lots.lots_of(p.key), p.lot));
		last_x = plan.add.back().x;
		last_z = plan.add.back().z;
		has_last = true;
		owed -= 1;
	}
	return plan;
}

void apply_growth(CityState &s, const GrowthPlan &plan) {
	GrowthQueue &g = s.growth;
	if (plan.revision != s.revision) return; // an edit came between: its lots and buildings may not be what the plan read
	for (int64_t key : plan.drop) {
		if (!g.members.erase(key)) continue;
		g.cells.erase(std::find(g.cells.begin(), g.cells.end(), key));
	}
	if (plan.none_free) g.acc = 0;
	for (Building b : plan.add) {
		b.built = s.clock.minutes.load();
		s.buildings.add(b);
		g.last_x = b.x;
		g.last_z = b.z;
		g.has_last = true;
		g.grown++;
		g.acc -= 1;
		s.touch_grown(); // a new building is a change to the map (saved), not to its streets
	}
}

} // namespace city
