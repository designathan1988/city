// Port of Roadcraft src/world/growth.ts (floorsFor, recordFor, squareCell, fillAll) and world/arch.ts civicFor.
#include "sim/buildings/growth.h"

#include "core/random.h"
#include "core/seeded.h"
#include "sim/buildings/b40/record.h"
#include "sim/citygen/lot_gen.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace city {

namespace {
// two integer mixes: the civic pick's (Knuth's 2^32 over the golden ratio, xxHash's PRIME32_2, xorshifts of 15 and 13) and the age's
// (the golden ratio, MurmurHash3's fmix32 multipliers, xorshifts of 13 and 16); a pick's chance in its low ten bits and its row in the
// rest, out of a thousand; a decay span in tenths
constexpr uint32_t CIVIC_MUL = 2654435761U, CIVIC_MIX = 2246822519U;
constexpr uint32_t AGE_MIX = 0x9e3779b9U, AGE_MUL1 = 0x85ebca6bU, AGE_MUL2 = 0xc2b2ae35U;
constexpr uint32_t CIVIC_CHANCE_BITS = 1023, CIVIC_PICK_SHIFT = 10, CIVIC_PICK_STEPS = 1000;
constexpr double DECAY_TENTHS = 10;
constexpr uint32_t HASH_SHIFT_15 = 15, HASH_SHIFT_13 = 13, HASH_SHIFT_16 = 16;

int biggest(const std::vector<Lot> &l) {
	size_t b = 0;
	for (size_t k = 0; k < l.size(); k++)
		if (l[k].w * l[k].d > l[b].w * l[b].d) b = k;
	return int(b);
}
} // namespace

MapCell map_cell_of(const CityState &s, double x, double z) {
	MapCell c;
	int cells = int(std::lround(s.map_half * 2 / CHUNK));
	c.x = int(std::floor((x + s.map_half) / CHUNK));
	c.z = int(std::floor((z + s.map_half) / CHUNK));
	c.id = c.z * cells + c.x;
	return c;
}

int body_floors(const b40::Data &data, const Building &rec) {
	const std::shared_ptr<const b40::Body> body_held = b40::record_body(data, rec);
	const b40::Body &body = *body_held;
	int top = 0;
	for (const b40::Volume &v : body.volumes) top = std::max(top, v.top());
	return top;
}

int civic_for(const GrowthDef &g, int cell_id, int zone, bool is_biggest, float area) {
	if (!is_biggest || area < g.civic_min_area) return FN_NONE;
	uint32_t h = uint32_t(cell_id) * CIVIC_MUL;
	h ^= h >> HASH_SHIFT_15;
	h *= CIVIC_MIX;
	h ^= h >> HASH_SHIFT_13;
	if (double(h & CIVIC_CHANCE_BITS) / (CIVIC_CHANCE_BITS + 1) > g.civic_chance) return FN_NONE;
	if (zone < 0 || zone >= BASE_COUNT || g.civic_by_zone[zone].empty()) return FN_NONE;
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
	h = (h ^ (h >> HASH_SHIFT_13)) * AGE_MUL2;
	return double((h ^ (h >> HASH_SHIFT_16)) % uint32_t(g.decay_span_min / DECAY_TENTHS));
}

bool lot_occupied(const CityState &s, const Lot &l) {
	float r = float(std::max(l.w, l.d));
	bool hit = false;
	s.buildings.query(float(l.x) - r, float(l.z) - r, float(l.x) + r, float(l.z) + r, [&](int id) {
		const Building &b = s.buildings.items[id];
		float reach = (float(std::min(l.w, l.d)) / 2 + std::min(b.w, b.d) / 2) * s.catalog.growth.occupied_share;
		// near enough its middle, or standing with its own middle on the lot (a body fitted to the lot stands back from its middle)
		if (std::hypot(b.x - float(l.x), b.z - float(l.z)) < reach || on_lot(l, { b.x, b.z })) hit = true;
	});
	// A square stands over many lots, its middle far from theirs.
	const float wide = r + std::sqrt(float(s.catalog.growth.plan.square_area_high));
	s.buildings.query(float(l.x) - wide, float(l.z) - wide, float(l.x) + wide, float(l.z) + wide, [&](int id) {
		const Building &b = s.buildings.items[id];
		hit = hit || (b.fn == FN_SQUARE && BuildingStore::covers(b, float(l.x), float(l.z), 0.f));
	});
	return hit;
}

BodyLook look_of(CityState &s, const Building &rec) {
	const b40::BodyKey key = b40::body_key(rec);
	auto it = s.buildings.looks.find(key);
	if (it != s.buildings.looks.end()) return it->second;
	const std::shared_ptr<const b40::Body> body_held = b40::record_body(s.buildings.designs, rec);
	const b40::Body &body = *body_held;
	return s.buildings.looks[key] = body_look(s.catalog.tune, body, rec.fn);
}

namespace {
// What stands within LOOK_REACH_M of the point, and how far.
std::vector<NearLook> looks_near(CityState &s, float x, float z) {
	std::vector<int> ids;
	s.buildings.query(x - LOOK_REACH_M, z - LOOK_REACH_M, x + LOOK_REACH_M, z + LOOK_REACH_M, [&](int id) { ids.push_back(id); });
	std::vector<NearLook> near;
	for (int id : ids) {
		const Building &o = s.buildings.items[size_t(id)];
		const float d = std::hypot(o.x - x, o.z - z);
		if (d < LOOK_REACH_M) near.push_back({ look_of(s, o), d });
	}
	return near;
}

// The body a seed gives on the lot, as the candidates compare, and whether it stands inside the lot's ring.
struct Candidate {
	uint32_t seed = 0;
	BodyLook look;
	int storeys = 0;
	bool fits = false;
};

// Whether every solid ground block of the body lies inside the ring.
bool body_fits(const GrowthDef &g, const b40::Body &body, const Building &rec, const std::vector<XZ> &ring) {
	const std::vector<V2> corners = b40::ground_corners(body, rec, g.fit_inset_m);
	std::vector<XZ> block(b40::BLOCK_CORNERS);
	for (size_t k = 0; k + b40::BLOCK_CORNERS <= corners.size(); k += b40::BLOCK_CORNERS) {
		for (size_t i = 0; i < b40::BLOCK_CORNERS; i++) block[i] = { corners[k + i].x, corners[k + i].y };
		if (!ring_holds(ring, block)) return false;
	}
	return true;
}

Candidate draw_candidate(const CityState &s, const Building &lot, uint32_t seed, const std::vector<XZ> &ring) {
	Building rec = lot;
	rec.seed = seed;
	rec.style = uint16_t(seed & BUILDING_AUTO_STYLE_MASK);
	const std::shared_ptr<const b40::Body> body_held = b40::record_body(s.buildings.designs, rec);
	const b40::Body &body = *body_held;
	Candidate c;
	c.seed = seed;
	c.look = body_look(s.catalog.tune, body, rec.fn);
	for (const b40::Volume &v : body.volumes) c.storeys = std::max(c.storeys, v.top());
	c.fits = body_fits(s.catalog.growth, body, rec, ring);
	return c;
}

// The lot's body: the lot's own seed when it fits and nothing stands near to look like, else of the candidates that fit the one least
// like its neighbours (the first that fits when none stands near), and of all of them when none fits. A candidate's seed is the lot's
// own, then hashes of it, so a city is the same every time.
Candidate best_candidate(CityState &s, const Building &lot, const std::vector<XZ> &ring) {
	const std::vector<NearLook> near = looks_near(s, lot.x, lot.z);
	const auto seed_of = [&](size_t k) { return k == 0 ? lot.seed : hash_combine(lot.seed, uint32_t(k)); };
	std::array<Candidate, LOOK_CANDIDATES> c;
	size_t drawn = 0; // with nothing near, the lot's own seed alone unless it does not fit
	if (near.empty()) {
		c[0] = draw_candidate(s, lot, lot.seed, ring);
		if (c[0].fits) return c[0];
		drawn = 1;
	}
	s.jobs->parallel_for(c.size() - drawn, [&](size_t k) { c[k + drawn] = draw_candidate(s, lot, seed_of(k + drawn), ring); });
	const bool any_fits = std::any_of(c.begin(), c.end(), [](const Candidate &k) { return k.fits; });
	size_t best = c.size();
	float score = 0;
	for (size_t k = 0; k < c.size(); k++) {
		if (any_fits && !c[k].fits) continue;
		const float u = near.empty() ? 0.f : unlikeness(s.catalog.tune, c[k].look, near);
		if (best == c.size() || u > score) {
			score = u;
			best = k;
		}
	}
	return c[best];
}

// The zone `drop` densities under `zone` of the same base, the lowest there is when fewer are; the zone itself when it has none.
int lower_zone(const Catalog &cat, int zone, int drop) {
	const ZoneDef &z = cat.zone(zone);
	for (int want = std::max(0, z.density - drop); want < z.density; want++)
		for (int k = 0; k < ZONE_CODES; k++)
			if (cat.zones[k].base == z.base && cat.zones[k].density == want) return k;
	return zone;
}

// The record of one fitting try: its depth set back from the street's side (the back kept where it was), its width narrowed about its
// middle, its zone lowered.
Building fitted(const Catalog &cat, const Building &b, const FitTry &t) {
	Building out = b;
	const float setback = b.d * t.setback_share;
	out.d = b.d - setback;
	out.w = b.w * t.width_share;
	out.x = b.x - setback / 2 * std::sin(b.rot); // the record's +z faces the street (b40::lot_frame)
	out.z = b.z - setback / 2 * std::cos(b.rot);
	out.zone = uint8_t(lower_zone(cat, b.zone, t.density_drop));
	return out;
}

// The ring a lot stands in: the player's lot round its middle (a zoned one's stand stands in it), else the lot's own rectangle.
std::vector<XZ> ring_of(const CityState &s, const Lot &l) {
	if (const PlayerLot *p = s.lot_book.find(s.lot_book.at(l.x, l.z))) return p->corners;
	return lot_ring(l);
}
} // namespace

Building record_for(CityState &s, const std::vector<Lot> &cell_lots, int i) {
	const Lot &l = cell_lots[i];
	const GrowthDef &g = s.catalog.growth;
	MapCell cell = map_cell_of(s, l.x, l.z);
	int fn = civic_for(g, cell.id, s.catalog.zone(l.zone).base, biggest(cell_lots) == i, float(l.w * l.d));
	if (fn == FN_SQUARE) fn = FN_NONE; // the squares are the plan's (sim/citygen/squares.h)
	int tallest = -1;
	const float reach = g.organic_reach_m;
	s.buildings.query(float(l.x) - reach, float(l.z) - reach, float(l.x) + reach, float(l.z) + reach, [&](int id) {
		const Building &o = s.buildings.items[id];
		if (std::hypot(o.x - float(l.x), o.z - float(l.z)) <= reach + std::max(o.w, o.d) / 2) tallest = std::max(tallest, int(o.floors));
	});
	Building b;
	b.x = float(l.x); b.z = float(l.z); b.w = float(l.w); b.d = float(l.d); b.rot = float(l.rot);
	b.zone = uint8_t(l.zone);
	b.fn = uint8_t(fn);
	b.seed = l.seed;
	b.style = uint16_t(l.seed & BUILDING_AUTO_STYLE_MASK);
	b.profile = uint16_t(l.profile);
	b.floors = uint16_t(fn == FN_NONE && tallest >= 0 ? std::max(1, tallest) + g.organic_extra_floors : 0);
	// The tries in order, the first whose body fits the lot's ring kept; the last stands when none does, so no lot stays empty.
	const std::vector<XZ> ring = ring_of(s, l);
	const Building stand = b;
	Candidate kept;
	for (const FitTry &t : g.fit_tries) {
		b = fitted(s.catalog, stand, t);
		kept = fn == FN_NONE ? best_candidate(s, b, ring) : draw_candidate(s, b, b.seed, ring);
		if (kept.fits) break;
	}
	b.seed = kept.seed;
	b.style = uint16_t(kept.seed & BUILDING_AUTO_STYLE_MASK);
	b.floors = uint16_t(kept.storeys); // the body is the height: a record never says another
	s.buildings.looks[b40::body_key(b)] = kept.look;
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
			Building b = record_for(s, lots, i);
			b.built = s.clock.minutes.load() - generated_age(s.catalog.growth, b.seed);
			s.buildings.add(b);
			n++;
		}
	}
	return n;
}

} // namespace city
