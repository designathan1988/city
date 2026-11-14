// Growth over time on the queued cells (city_growth.h): the queue's open lots, the look that picks the next lot, and the step.
#include "sim/simulation/city_growth.h"

#include "core/random.h"
#include "sim/buildings/growth.h"
#include "sim/commands/lot_sites.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <optional>
#include <shared_mutex>

namespace city {

namespace {
constexpr double MAX_OWED = 50; // the most buildings a step may owe: a long pause does not burst into a district at once
constexpr int FILED_A_LOOK = 16; // queued cells whose open lots one look finds: a queue read from a save is filed over a few steps
constexpr int PICKS_A_LOOK = 8; // lots a look may find taken, or with nothing that fits, before it leaves the rest to the next
constexpr double NEARER_M = 0.001; // a lot takes the place of one found before only when nearer by this: two as near keep the lists' order
constexpr uint32_t DRAW_SALT = 0x6d0fU;

using LotRef = std::pair<int64_t, int>; // a cell and the index of one of its lots
constexpr LotRef NO_LOT{ 0, -1 };

// What a look at the queue decided, all of it written under the map's exclusive hold: the open lots it found for cells that had no
// list, the lots it found taken or unfit, and the building for the lot it picked, if any.
struct Decided {
	bool grow = false;
	Building b;
	LotRef lot = NO_LOT;
	std::vector<std::pair<int64_t, std::vector<int>>> filed;
	std::vector<LotRef> shut;
	uint64_t revision = 0; // the map's when it was decided: an edit since makes it void
};

// The open lots a look reads: the queue's lists and those the look itself just found, less the lots it found shut.
struct OpenView {
	const GrowthQueue &g;
	const Decided &d;
	const std::vector<int> *of(int64_t key) const {
		for (const auto &f : d.filed)
			if (f.first == key) return &f.second;
		const auto it = g.open.find(key);
		return it == g.open.end() ? nullptr : &it->second;
	}
	bool shut(const LotRef &l) const { return std::find(d.shut.begin(), d.shut.end(), l) != d.shut.end(); }
};

// The queued cells a square of `reach` round the last building grown touches, row by row.
template <typename F>
void cells_in_reach(const GrowthQueue &g, double reach, F &&fn) {
	for (int j = int(std::floor((g.last_z - reach) / CHUNK)); j <= int(std::floor((g.last_z + reach) / CHUNK)); j++)
		for (int i = int(std::floor((g.last_x - reach) / CHUNK)); i <= int(std::floor((g.last_x + reach) / CHUNK)); i++)
			if (g.members.count(cell_key(i, j)) != 0) fn(cell_key(i, j));
}

// The open lots of the queued cells that have no list yet, a few a look: those within reach of the last building grown first (the
// street it stands on fills along), then in the queue's order.
void find_open(const CityState &s, Decided &d) {
	const GrowthQueue &g = s.growth;
	if (g.open.size() >= g.cells.size()) return; // every queued cell has its list
	const auto file = [&](int64_t key) {
		if (int(d.filed.size()) >= FILED_A_LOOK || g.open.count(key) != 0) return;
		for (const auto &f : d.filed)
			if (f.first == key) return;
		std::vector<int> open;
		const std::vector<Lot> *lots = s.lots.lots_of(key);
		for (int i = 0; lots && i < int(lots->size()); i++)
			if (!lot_occupied(s, (*lots)[size_t(i)])) open.push_back(i);
		d.filed.emplace_back(key, std::move(open));
	};
	if (g.has_last) cells_in_reach(g, s.catalog.tune.sim_growth_reach_m, file);
	for (size_t c = 0; c < g.cells.size() && int(d.filed.size()) < FILED_A_LOOK; c++) file(g.cells[c]);
}

// The open lot nearest the last building grown, within the reach: none when the street it stands on has no open lot so near.
LotRef nearest_open(const CityState &s, const OpenView &v) {
	const GrowthQueue &g = s.growth;
	const double reach = s.catalog.tune.sim_growth_reach_m;
	LotRef best = NO_LOT;
	double least = reach;
	cells_in_reach(g, reach, [&](int64_t key) {
		const std::vector<int> *open = v.of(key);
		const std::vector<Lot> *lots = s.lots.lots_of(key);
		if (!open || !lots) return;
		for (int i : *open) {
			if (size_t(i) >= lots->size() || v.shut({ key, i })) continue;
			const Lot &l = (*lots)[size_t(i)];
			const double far = std::hypot(l.x - g.last_x, l.z - g.last_z);
			if (far < least - NEARER_M) {
				least = far;
				best = { key, i };
			}
		}
	});
	return best;
}

// An open lot drawn from every filed cell of the queue, each lot as likely as another. The draw is the city's seed's, the count of
// buildings grown and the lots this look already found shut: the same city zoned the same way grows the same way.
LotRef drawn_open(const CityState &s, const OpenView &v) {
	const GrowthQueue &g = s.growth;
	size_t total = 0;
	for (int64_t key : g.cells)
		if (const std::vector<int> *open = v.of(key)) total += open->size();
	if (total == 0) return NO_LOT;
	size_t at = size_t(hash_combine(hash_combine(s.seed, DRAW_SALT), uint32_t(g.grown) + uint32_t(v.d.shut.size()))) % total;
	for (int64_t key : g.cells) {
		const std::vector<int> *open = v.of(key);
		if (!open) continue;
		if (at < open->size()) return { key, (*open)[at] };
		at -= open->size();
	}
	return NO_LOT;
}

// The next building, decided by reading the city (the caller holds the map shared): the lot picked is looked at once more, as a
// building may have come there by another hand since its cell was filed, and a lot nothing fits is shut with the taken ones.
Decided look(const CityState &s) {
	Decided d;
	d.revision = s.revision;
	find_open(s, d);
	const OpenView view{ s.growth, d };
	for (int picks = PICKS_A_LOOK; picks > 0; picks--) {
		LotRef pick = s.growth.has_last ? nearest_open(s, view) : NO_LOT;
		if (pick.second < 0) pick = drawn_open(s, view);
		if (pick.second < 0) return d; // nothing open among the cells filed so far
		if (view.shut(pick)) continue; // drawn again: the next draw counts the lots shut
		const std::vector<Lot> *lots = s.lots.lots_of(pick.first);
		std::optional<Building> made;
		if (lots && size_t(pick.second) < lots->size() && !lot_occupied(s, (*lots)[size_t(pick.second)])) made = record_for(s, *lots, pick.second);
		if (!made) {
			d.shut.push_back(pick);
			continue;
		}
		d.b = *made;
		d.b.built = s.clock.minutes.load();
		d.lot = pick;
		d.grow = true;
		return d;
	}
	return d;
}
} // namespace

void GrowthQueue::push(int64_t key) {
	open.erase(key);
	if (members.insert(key).second) cells.push_back(key);
}

void GrowthQueue::drop(int64_t key) {
	open.erase(key);
	if (members.erase(key) == 0) return;
	const auto it = std::find(cells.begin(), cells.end(), key);
	if (it == cells.end()) return;
	*it = cells.back(); // the last takes its place: the order the queue has always been kept in
	cells.pop_back();
}

void GrowthQueue::assign(std::vector<int64_t> keys) {
	cells.clear();
	members.clear();
	open.clear();
	for (int64_t k : keys) push(k);
}

void GrowthQueue::file(int64_t key, std::vector<int> lots) {
	if (members.count(key) == 0) return;
	if (lots.empty()) drop(key);
	else open[key] = std::move(lots);
}

void GrowthQueue::shut(int64_t key, int lot) {
	const auto it = open.find(key);
	if (it == open.end()) return;
	std::vector<int> &lots = it->second;
	lots.erase(std::remove(lots.begin(), lots.end(), lot), lots.end());
	if (lots.empty()) drop(key);
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
			d = look(s);
		}
		map_hold::WriterHold map(s.map_mutex, map_hold::Writer::TICK);
		if (s.revision != d.revision) return; // a command edited the map meanwhile: what is owed is decided again at the next step
		const bool learnt = !d.filed.empty() || !d.shut.empty();
		for (auto &f : d.filed) g.file(f.first, std::move(f.second));
		for (const LotRef &l : d.shut) g.shut(l.first, l.second);
		if (!d.grow) {
			if (g.cells.empty()) g.acc = 0; // nothing is open anywhere: nothing is owed
			if (g.cells.empty() || !learnt) return;
			continue; // cells were filed or lots shut: the next try looks at the queue as it now stands
		}
		s.buildings.add(d.b);
		settle_sites(s, {}); // its lot graded to its street, as an edit's are
		g.shut(d.lot.first, d.lot.second);
		g.last_x = d.b.x;
		g.last_z = d.b.z;
		g.has_last = true;
		g.grown++;
		g.acc -= 1;
		s.touch_grown(); // a new building is a change to the map (saved), not to its streets
	}
}

} // namespace city
