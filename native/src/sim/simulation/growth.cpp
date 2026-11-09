// Port of Roadcraft src/world/growth.ts (floorsFor, recordFor, squareCell, fillAll) and world/arch.ts civicFor.
#include "sim/buildings/growth.h"

#include "core/random.h"
#include "core/seeded.h"
#include "sim/buildings/b40/record.h"
#include "sim/citygen/lot_gen.h"
#include "sim/roads/geometry.h"
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
		if (std::hypot(b.x - float(l.x), b.z - float(l.z)) < reach) hit = true;
	});
	// A square stands over many lots, its middle far from theirs.
	const float wide = r + std::sqrt(float(s.catalog.growth.plan.square_area_high));
	s.buildings.query(float(l.x) - wide, float(l.z) - wide, float(l.x) + wide, float(l.z) + wide, [&](int id) {
		const Building &b = s.buildings.items[id];
		hit = hit || (b.fn == FN_SQUARE && BuildingStore::covers(b, float(l.x), float(l.z), 0.f));
	});
	return hit;
}

BodyLook look_of(const CityState &s, const Building &rec) { return *s.buildings.designs.made->look(s.buildings.designs, rec, s.catalog.tune); }

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

// The body a seed gives on the lot, as the candidates compare.
struct Candidate {
	uint32_t seed = 0;
	BodyLook look;
	int storeys = 0;
};

Candidate draw_candidate(const CityState &s, const Building &lot, uint32_t seed) {
	Building rec = lot;
	rec.seed = seed;
	rec.style = uint16_t(seed & BUILDING_AUTO_STYLE_MASK);
	const std::shared_ptr<const b40::Body> body_held = b40::record_body(s.buildings.designs, rec);
	const b40::Body &body = *body_held;
	Candidate c;
	c.seed = seed;
	c.look = body_look(s.catalog.tune, body, rec.fn);
	for (const b40::Volume &v : body.volumes) c.storeys = std::max(c.storeys, v.top());
	return c;
}

// The lot's body: the first when nothing stands near to look like, else the candidate least like its neighbours. A candidate's seed is
// the lot's own, then hashes of it, so a city is the same every time.
Candidate best_candidate(CityState &s, const Building &lot) {
	const std::vector<NearLook> near = looks_near(s, lot.x, lot.z);
	if (near.empty()) return draw_candidate(s, lot, lot.seed);
	std::array<Candidate, LOOK_CANDIDATES> c;
	s.jobs->parallel_for(c.size(), [&](size_t k) { c[k] = draw_candidate(s, lot, k == 0 ? lot.seed : hash_combine(lot.seed, uint32_t(k))); });
	size_t best = 0;
	float score = unlikeness(s.catalog.tune, c[0].look, near);
	for (size_t k = 1; k < c.size(); k++) {
		const float u = unlikeness(s.catalog.tune, c[k].look, near);
		if (u > score) {
			score = u;
			best = k;
		}
	}
	return c[best];
}
} // namespace

namespace {
// Whether a point lies on a street: within corner_margin_m of the paving of a road on the ground.
bool on_street(const CityState &s, double x, double z) {
	const double margin = s.catalog.tune.lot_corner_margin_m;
	bool hit = false;
	s.roads.for_segments_near(x - margin, z - margin, x + margin, z + margin, [&](int sg) {
		const RoadSegment &g = s.roads.segs[size_t(sg)];
		if (hit || structure_of(g.flags) != Structure::GROUND) return;
		double t = 0, dist = 0;
		closest_t(g.q, x, z, t, dist);
		hit = dist < s.roads.profile_of(sg).half() + margin;
	});
	return hit;
}
} // namespace

// The outline of the player's lot a lot of the growth stands for (lot_cache.cpp with_player_lots gives it the lot's stand): its
// corners in the world, or none for a lot the streets proposed.
std::vector<V2> player_ring(const CityState &s, const Lot &l) {
	std::vector<V2> ring;
	for (int i : s.lot_book.near(l.x, l.z, l.x, l.z)) {
		const LotStand &st = s.lot_book.stand(i);
		const PlayerLot &p = s.lot_book.all()[size_t(i)];
		if (p.zone == 0 || st.x != l.x || st.z != l.z || st.rot != l.rot) continue;
		for (const XZ &c : p.corners) ring.push_back(V2(float(c.x), float(c.z)));
		break;
	}
	return ring;
}

b40::SiteRef site_of(const CityState &s, const Building &b, const Lot &l) {
	const b40::LotFrame f = b40::lot_frame(b);
	const b40::LotSlopeShape &K = s.buildings.designs.shapes.slope;
	return b40::read_site(f.o, f.e0, f.e1, b.w, b.d, [&](float x, float z) { return s.terrain.height_at(x, z); }, K.street_threshold,
			player_ring(s, l), K.front_points);
}

uint8_t corner_of(const CityState &s, const Lot &l) {
	const Tuning &tn = s.catalog.tune;
	Building rec;
	rec.x = float(l.x); rec.z = float(l.z); rec.w = float(l.w); rec.d = float(l.d); rec.rot = float(l.rot);
	const b40::LotFrame f = b40::lot_frame(rec);
	for (const b40::LotCorner side : { b40::CORNER_LEFT, b40::CORNER_RIGHT }) {
		const double along = (side == b40::CORNER_LEFT ? -1 : 1) * (l.w / 2 + tn.lot_corner_beyond_m);
		bool both = true;
		for (const double k : { tn.lot_corner_near, tn.lot_corner_far }) {
			const V2 p = f.at(float(along), float(l.d * k));
			both = both && on_street(s, p.x, p.y);
		}
		if (both) return side;
	}
	return b40::CORNER_NONE;
}

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
	b.corner = corner_of(s, l); // before the candidates: a corner's plan is its own
	b.site = site_of(s, b, l); // and the ground and outline each candidate is made on (b40/lot_site.h)
	b.floors = uint16_t(fn == FN_NONE && tallest >= 0 ? std::max(1, tallest) + g.organic_extra_floors : 0);
	const Candidate kept = fn == FN_NONE ? best_candidate(s, b) : draw_candidate(s, b, b.seed);
	b.seed = kept.seed;
	b.style = uint16_t(kept.seed & BUILDING_AUTO_STYLE_MASK);
	b.floors = uint16_t(kept.storeys); // the body is the height: a record never says another
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
