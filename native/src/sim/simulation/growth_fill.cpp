// A generated city filled at once (sim/buildings/growth.h fill_all): a building on every free lot, the cells filled side by side
// over the job pool in four passes of a 2 x 2 checkerboard. No two cells of a pass touch, and nothing a lot reads of its
// neighbours reaches a cell away (their looks 100 m, their height the organic reach; a cell is 256 m), so a cell reads only what
// the passes before it added and what it made itself, and the city is the same for the same seed whatever the pool. Nothing is
// written while a pass runs: each cell's buildings are added after it, in the cells' order.
#include "sim/buildings/growth.h"

#include "sim/citygen/lot_gen.h"
#include "sim/state/city_state.h"

#include <algorithm>

namespace city {

namespace {
constexpr int PASSES = 4; // the cells by the parity of their two coordinates

// What one cell made in a pass: its buildings and their looks, by the same index.
struct Filled {
	std::vector<Building> made;
	std::vector<BodyLook> looks;
};

int pass_of(int64_t key) { return (key_x(key) & 1) | ((key_y(key) & 1) << 1); }

void fill_cell(const CityState &s, int64_t key, const std::vector<BodyLook> &looks, Filled &out) {
	const std::vector<Lot> *lots = s.lots.lots_of(key);
	const GrowthBeside beside{ &out.made, &out.looks, &looks };
	for (int i = 0; lots && i < int(lots->size()); i++) {
		if (lot_occupied(s, (*lots)[size_t(i)])) continue;
		BodyLook look;
		const std::optional<Building> b = record_for(s, *lots, i, &beside, &look);
		if (!b) continue; // nothing fits it: it waits, as a lot of growth over time does
		out.made.push_back(*b);
		out.looks.push_back(look);
	}
}
} // namespace

int fill_all(CityState &s) {
	std::vector<int64_t> keys;
	for (const auto &kv : s.lots.by_cell) keys.push_back(kv.first);
	std::sort(keys.begin(), keys.end());
	std::vector<BodyLook> looks; // of the records added, by id: a later pass reads its neighbours' here, never off their bodies
	const double now = s.clock.minutes.load();
	int n = 0;
	for (int pass = 0; pass < PASSES; pass++) {
		std::vector<int64_t> cells;
		for (int64_t key : keys)
			if (pass_of(key) == pass) cells.push_back(key);
		std::vector<Filled> filled(cells.size());
		const auto fill = [&](size_t c) { fill_cell(s, cells[c], looks, filled[c]); };
		if (s.jobs) s.jobs->parallel_for(cells.size(), fill);
		else
			for (size_t c = 0; c < cells.size(); c++) fill(c);
		for (Filled &f : filled)
			for (size_t k = 0; k < f.made.size(); k++) {
				f.made[k].built = now - generated_age(s.catalog.growth, f.made[k].seed);
				const size_t id = size_t(s.buildings.add(f.made[k]));
				if (id >= looks.size()) looks.resize(id + 1);
				looks[id] = f.looks[k];
				n++;
			}
	}
	return n;
}

} // namespace city
