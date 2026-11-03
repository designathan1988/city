// Port of Roadcraft src/world/growth.ts (floorsFor, recordFor, squareCell, fillAll) and world/arch.ts civicFor.
#include "sim/buildings/growth.h"

#include "core/seeded.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float OCCUPIED_SHARE = 0.8f;
constexpr int NEIGHBOURHOOD_CELLS = 2; // a neighbourhood is 2 x 2 cells and holds one square
constexpr uint32_t SQUARE_SALT = 4242, CIVIC_MUL = 2654435761U, CIVIC_MIX = 2246822519U;
constexpr uint32_t AGE_MIX = 0x9e3779b9U, AGE_MUL1 = 0x85ebca6bU, AGE_MUL2 = 0xc2b2ae35U;
constexpr uint32_t CIVIC_CHANCE_BITS = 1023, CIVIC_PICK_SHIFT = 10, CIVIC_PICK_STEPS = 1000;
constexpr double DECAY_TENTHS = 10;

int draw(Mulberry32 &r, const IntRange &x) { return r.irange_excl(x.lo, x.hi); }

int biggest(const std::vector<Lot> &l) {
	size_t b = 0;
	for (size_t k = 0; k < l.size(); k++)
		if (l[k].w * l[k].d > l[b].w * l[b].d) b = k;
	return int(b);
}
} // namespace

int base_zone(int z) { return z == 5 || z == 6 ? 1 : z == 7 || z == 8 ? 2 : z; }
int zone_density(int z) { return z == 5 || z == 7 ? 0 : z == 6 || z == 8 ? 2 : 1; }

MapCell map_cell_of(const CityState &s, double x, double z) {
	MapCell c;
	int cells = int(std::lround(s.map_half * 2 / CHUNK));
	c.x = int(std::floor((x + s.map_half) / CHUNK));
	c.z = int(std::floor((z + s.map_half) / CHUNK));
	c.id = c.z * cells + c.x;
	return c;
}

int floors_for(const GrowthDef &g, int zone_id, const Profile &street, uint32_t seed) {
	Mulberry32 r(seed);
	bool avenue = street.priority >= g.avenue_priority;
	int zone = base_zone(zone_id), dens = zone_density(zone_id);
	if (dens == 0) return draw(r, zone == 1 ? g.low_res : g.low_other);
	if (dens == 2) return draw(r, zone == 1 ? (avenue ? g.high_res_avenue : g.high_res) : (avenue ? g.high_other_avenue : g.high_other));
	switch (zone) {
		case 1: {
			if (avenue) return draw(r, g.res_avenue);
			double x = r.next();
			return draw(r, x < g.res_split[0] ? g.res_bands[0] : x < g.res_split[1] ? g.res_bands[1] : g.res_bands[2]);
		}
		case 2: return draw(r, avenue ? g.commercial_avenue : g.commercial);
		case 3: return draw(r, g.industrial);
		case 4: return draw(r, avenue ? g.mixed_avenue : g.mixed);
		default: return 1;
	}
}

int civic_for(const GrowthDef &g, int cell_id, int zone, bool is_biggest, float area) {
	if (!is_biggest || area < g.civic_min_area) return FN_NONE;
	uint32_t h = uint32_t(cell_id) * CIVIC_MUL;
	h ^= h >> 15;
	h *= CIVIC_MIX;
	h ^= h >> 13;
	if (double(h & CIVIC_CHANCE_BITS) / (CIVIC_CHANCE_BITS + 1) > g.civic_chance) return FN_NONE;
	if (zone < 0 || zone > 4 || g.civic_by_zone[zone].empty()) return FN_NONE;
	const auto &list = g.civic_by_zone[zone];
	int total = 0;
	for (const auto &e : list) total += e.second;
	double k = double((h >> CIVIC_PICK_SHIFT) % CIVIC_PICK_STEPS) / CIVIC_PICK_STEPS * total;
	for (const auto &e : list)
		if ((k -= e.second) < 0) return e.first;
	return list[0].first;
}

double generated_age(const GrowthDef &g, uint32_t seed) {
	uint32_t h = (seed ^ AGE_MIX) * AGE_MUL1;
	h = (h ^ (h >> 13)) * AGE_MUL2;
	return double((h ^ (h >> 16)) % uint32_t(g.decay_span_min / DECAY_TENTHS));
}

bool lot_occupied(const CityState &s, const Lot &l) {
	float r = float(std::max(l.w, l.d));
	bool hit = false;
	s.buildings.query(float(l.x) - r, float(l.z) - r, float(l.x) + r, float(l.z) + r, [&](int id) {
		const Building &b = s.buildings.items[id];
		float reach = (float(std::min(l.w, l.d)) / 2 + std::min(b.w, b.d) / 2) * OCCUPIED_SHARE;
		if (std::hypot(b.x - float(l.x), b.z - float(l.z)) < reach) hit = true;
	});
	return hit;
}

// The cell of the neighbourhood that holds its square: the first, in an order drawn by the neighbourhood, whose
// biggest lot is big enough and not industrial (-1 when none).
static int square_cell(const CityState &s, const MapCell &c) {
	int cells = int(std::lround(s.map_half * 2 / CHUNK));
	int bx = c.x / NEIGHBOURHOOD_CELLS, bz = c.z / NEIGHBOURHOOD_CELLS;
	uint32_t h = hash2(bx, bz, SQUARE_SALT);
	for (uint32_t k = 0; k < 4; k++) {
		uint32_t q = (h + k) & 3;
		int x = bx * NEIGHBOURHOOD_CELLS + int(q & 1), z = bz * NEIGHBOURHOOD_CELLS + int(q >> 1);
		if (x >= cells || z >= cells) continue;
		int64_t key = cell_key(int(std::floor((x * CHUNK - s.map_half) / CHUNK)), int(std::floor((z * CHUNK - s.map_half) / CHUNK)));
		const std::vector<Lot> *l = s.lots.lots_of(key);
		if (!l || l->empty()) continue;
		const Lot &b = (*l)[biggest(*l)];
		if (b.w * b.d >= s.catalog.growth.square_min_area && base_zone(b.zone) != 3) return z * cells + x;
	}
	return -1;
}

static bool neighbourhood_has_square(const CityState &s, double x, double z) {
	float span = CHUNK * NEIGHBOURHOOD_CELLS;
	float x0 = std::floor((float(x) + s.map_half) / span) * span - s.map_half, z0 = std::floor((float(z) + s.map_half) / span) * span - s.map_half;
	bool found = false;
	s.buildings.query(x0, z0, x0 + span, z0 + span, [&](int id) { found = found || s.buildings.items[id].fn == FN_SQUARE; });
	return found;
}

Building record_for(const CityState &s, const std::vector<Lot> &cell_lots, int i, bool organic) {
	const Lot &l = cell_lots[i];
	const GrowthDef &g = s.catalog.growth;
	MapCell cell = map_cell_of(s, l.x, l.z);
	bool big = biggest(cell_lots) == i;
	bool square_here = big && square_cell(s, cell) == cell.id;
	int fn = square_here ? int(FN_SQUARE) : civic_for(g, cell.id, base_zone(l.zone), big, float(l.w * l.d));
	if (fn == FN_SQUARE && !square_here) fn = FN_NONE;
	if (fn == FN_SQUARE && neighbourhood_has_square(s, l.x, l.z)) fn = FN_NONE;
	int floors = floors_for(g, l.zone, s.catalog.profile(l.profile), l.seed);
	if (organic) {
		int tallest = -1;
		float reach = g.organic_reach_m;
		s.buildings.query(float(l.x) - reach, float(l.z) - reach, float(l.x) + reach, float(l.z) + reach, [&](int id) {
			const Building &b = s.buildings.items[id];
			if (std::hypot(b.x - float(l.x), b.z - float(l.z)) <= reach + std::max(b.w, b.d) / 2) tallest = std::max(tallest, int(b.floors));
		});
		if (tallest >= 0) floors = std::min(floors, std::max(1, tallest) + g.organic_extra_floors);
	}
	Building b;
	b.x = float(l.x); b.z = float(l.z); b.w = float(l.w); b.d = float(l.d); b.rot = float(l.rot);
	b.zone = uint8_t(base_zone(l.zone));
	b.fn = uint8_t(fn);
	b.floors = uint16_t(floors);
	b.seed = l.seed;
	b.style = uint16_t(l.seed & BUILDING_AUTO_STYLE_MASK);
	b.profile = uint16_t(l.profile);
	return b;
}

int fill_all(CityState &s) {
	int n = 0;
	std::vector<int64_t> keys;
	for (const auto &kv : s.lots.by_cell) keys.push_back(kv.first);
	std::sort(keys.begin(), keys.end());
	for (int64_t key : keys) {
		const std::vector<Lot> &lots = s.lots.by_cell[key];
		for (int i = 0; i < int(lots.size()); i++) {
			if (lot_occupied(s, lots[i])) continue;
			Building b = record_for(s, lots, i, false);
			b.built = s.clock.minutes.load() - generated_age(s.catalog.growth, b.seed);
			s.buildings.add(b);
			n++;
		}
	}
	return n;
}

} // namespace city
