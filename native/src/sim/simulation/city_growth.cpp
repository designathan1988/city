// Growth over time on the queued cells (Roadcraft world/growth.ts step / nearestFree / grow).
#include "sim/simulation/city_growth.h"

#include "sim/buildings/growth.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <mutex>
#include <shared_mutex>

namespace city {

namespace {
constexpr double MAX_OWED = 50; // growth.ts: acc is capped at 50 buildings

struct Pick {
	int64_t key = 0;
	int lot = -1;
	double d2 = 0;
};

// What a look at the queue decided: the building for the free lot nearest the last one grown, or that no lot is free; and the
// queue as it stands once the cells found with no free lot have left it.
struct Decided {
	bool grow = false;
	Building b;
	std::vector<int64_t> cells, dropped;
	uint64_t revision = 0; // the map's when it was decided: an edit since makes it void
};

// The free lot nearest the last one grown among `cells`; a cell with no free lot leaves them, the last taking its place (the
// order the queue has always been walked and kept in).
Pick nearest_free(const CityState &s, std::vector<int64_t> &cells, std::vector<int64_t> &dropped) {
	const GrowthQueue &g = s.growth;
	Pick best;
	for (size_t c = 0; c < cells.size();) {
		const std::vector<Lot> *lots = s.lots.lots_of(cells[c]);
		bool any = false;
		for (int i = 0; lots && i < int(lots->size()); i++) {
			const Lot &l = (*lots)[i];
			if (lot_occupied(s, l)) continue;
			any = true;
			double d2 = g.has_last ? (l.x - g.last_x) * (l.x - g.last_x) + (l.z - g.last_z) * (l.z - g.last_z) : 0;
			if (best.lot < 0 || d2 < best.d2) best = { cells[c], i, d2 };
		}
		if (any) {
			c++;
		} else {
			dropped.push_back(cells[c]);
			cells[c] = cells.back();
			cells.pop_back();
		}
	}
	return best;
}

// The next building, decided by reading the city (the caller holds the map shared).
Decided decide(const CityState &s) {
	Decided d;
	d.revision = s.revision;
	d.cells = s.growth.cells;
	const Pick p = nearest_free(s, d.cells, d.dropped);
	if (p.lot < 0) return d;
	d.b = record_for(s, *s.lots.lots_of(p.key), p.lot);
	d.b.built = s.clock.minutes.load();
	d.grow = true;
	return d;
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
	bool accrued = false;
	for (int tries = int(s.catalog.tune.sim_growth_tries); tries > 0; tries--) {
		Decided d;
		{
			std::shared_lock<std::shared_mutex> map(s.map_mutex);
			if (!accrued) {
				if (g.cells.empty() || s.damage.now < s.damage.growth_hold_until) return; // rubble lies: nothing is built over it yet
				g.acc = std::min(g.acc + dt * s.catalog.tune.sim_growth_per_s, MAX_OWED);
				accrued = true;
			}
			if (g.acc < 1) return;
			d = decide(s);
		}
		std::unique_lock<std::shared_mutex> map(s.map_mutex);
		if (s.revision != d.revision) return; // a command edited the map meanwhile: what is owed is decided again at the next step
		for (int64_t key : d.dropped) g.members.erase(key);
		g.cells = std::move(d.cells);
		if (!d.grow) {
			g.acc = 0;
			return;
		}
		s.buildings.add(d.b);
		g.last_x = d.b.x;
		g.last_z = d.b.z;
		g.has_last = true;
		g.grown++;
		g.acc -= 1;
		s.touch_grown(); // a new building is a change to the map (saved), not to its streets
	}
}

} // namespace city
