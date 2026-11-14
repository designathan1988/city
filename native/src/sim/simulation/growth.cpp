// What grows on a lot (sim/buildings/growth.h): whether a lot is free, the building tried on it (whole, then set back and narrower,
// then a density lower) with the candidate least like its neighbours kept, the quarter's era and the street side's character
// written in its seed, a civic building for a cell's biggest lot, and a building's age. The city filled at once: growth_fill.cpp.
#include "sim/buildings/growth.h"

#include "core/random.h"
#include "core/seeded.h"
#include "sim/buildings/b40/record.h"
#include "sim/citygen/lot_gen.h"
#include "sim/commands/lot_sites.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr uint32_t CIVIC_MUL = 2654435761U, CIVIC_MIX = 2246822519U;
constexpr uint32_t AGE_MIX = 0x9e3779b9U, AGE_MUL1 = 0x85ebca6bU, AGE_MUL2 = 0xc2b2ae35U;
constexpr uint32_t CIVIC_CHANCE_BITS = 1023, CIVIC_PICK_SHIFT = 10, CIVIC_PICK_STEPS = 1000;
constexpr double DECAY_TENTHS = 10;
constexpr uint32_t HASH_SHIFT_15 = 15, HASH_SHIFT_13 = 13, HASH_SHIFT_16 = 16;
constexpr float MEET_IN_M = 0.05f; // a try's rectangle is asked this far inside its own lines: lots that share a side do not meet
constexpr uint32_t CHARACTER_SALT = 0x51d7U, ERA_SALT = 0x3c6fU;
constexpr double HASH_UNIT = 4294967296.0, QUARTER_TURN = 1.5707963267948966;
static_assert(GrowthDef::ERAS == b40::ERAS, "a quarter's era is one of the generator's");

int biggest(const std::vector<Lot> &l) {
	size_t b = 0;
	for (size_t k = 0; k < l.size(); k++)
		if (l[k].w * l[k].d > l[b].w * l[b].d) b = k;
	return int(b);
}

// Whether two lots' turned rectangles lie over one another, the first taken `in` inside its own lines: a separating axis among the
// four the two rectangles' sides give, or none.
bool rects_meet(const Building &a, const Building &b, float in) {
	const float ca = std::cos(a.rot), sa = std::sin(a.rot), cb = std::cos(b.rot), sb = std::sin(b.rot);
	const float axes[4][2] = { { ca, -sa }, { sa, ca }, { cb, -sb }, { sb, cb } }; // each one's along and out (BuildingRecords::covers)
	const float ha[2] = { a.w / 2 - in, a.d / 2 - in }, hb[2] = { b.w / 2, b.d / 2 };
	const float dx = b.x - a.x, dz = b.z - a.z;
	for (const auto &n : axes) {
		const float ra = ha[0] * std::fabs(axes[0][0] * n[0] + axes[0][1] * n[1]) + ha[1] * std::fabs(axes[1][0] * n[0] + axes[1][1] * n[1]);
		const float rb = hb[0] * std::fabs(axes[2][0] * n[0] + axes[2][1] * n[1]) + hb[1] * std::fabs(axes[3][0] * n[0] + axes[3][1] * n[1]);
		if (std::fabs(dx * n[0] + dz * n[1]) >= ra + rb) return false;
	}
	return true;
}

// The lot as a record's rectangle: what the store's own tests read.
Building as_record(const Lot &l) {
	Building b;
	b.x = float(l.x); b.z = float(l.z); b.w = float(l.w); b.d = float(l.d); b.rot = float(l.rot);
	return b;
}

// Whether a standing building's lot, or a ruin, lies over the try's rectangle.
bool lots_over(const CityState &s, const Building &b) {
	if (s.damage.ruin_under(b.x, b.z, b.w, b.d, b.rot)) return true;
	float x0, z0, x1, z1;
	BuildingRecords::bounds(b, x0, z0, x1, z1);
	bool hit = false;
	s.buildings.meeting(x0, z0, x1, z1, [&](int id) { hit = hit || rects_meet(b, s.buildings.items[size_t(id)], MEET_IN_M); });
	return hit;
}

// One try's lot on lot `l`: set back from its front, in from its sides, a share of what is left about the front's middle. False
// when it leaves no room a building can use.
bool try_on(const GrowthDef &g, const Lot &l, const LotTry &t, Building &b) {
	const double w = (l.w - 2 * double(t.side_m)) * double(t.share), d = (l.d - double(t.front_m)) * double(t.share);
	if (w < double(g.least_room_m) || d < double(g.least_room_m)) return false;
	const double to_street = l.d / 2 - double(t.front_m) - d / 2; // from the lot's middle to the try's, along the way it faces
	b.x = float(l.x + std::sin(l.rot) * to_street);
	b.z = float(l.z + std::cos(l.rot) * to_street);
	b.w = float(w);
	b.d = float(d);
	b.rot = float(l.rot);
	return true;
}

// The storeys of the tallest building within the organic reach of the lot (-1: none), the cell's own of this pass among them.
int tallest_near(const CityState &s, const Lot &l, const GrowthBeside *beside) {
	const float reach = s.catalog.growth.organic_reach_m;
	int tallest = -1;
	const auto weigh = [&](const Building &o) {
		if (std::hypot(o.x - float(l.x), o.z - float(l.z)) <= reach + std::max(o.w, o.d) / 2) tallest = std::max(tallest, int(o.floors));
	};
	s.buildings.query(float(l.x) - reach, float(l.z) - reach, float(l.x) + reach, float(l.z) + reach, [&](int id) { weigh(s.buildings.items[size_t(id)]); });
	if (beside && beside->made)
		for (const Building &o : *beside->made) weigh(o);
	return tallest;
}

// What stands within LOOK_REACH_M of the point, and how far: the store's records, and the cell's own of the pass under way.
std::vector<NearLook> looks_near(const CityState &s, float x, float z, const GrowthBeside *beside) {
	std::vector<NearLook> near;
	s.buildings.query(x - LOOK_REACH_M, z - LOOK_REACH_M, x + LOOK_REACH_M, z + LOOK_REACH_M, [&](int id) {
		const Building &o = s.buildings.items[size_t(id)];
		const float d = std::hypot(o.x - x, o.z - z);
		if (d >= LOOK_REACH_M) return;
		const bool known = beside && beside->looks && size_t(id) < beside->looks->size() && (*beside->looks)[size_t(id)].form != 0;
		near.push_back({ known ? (*beside->looks)[size_t(id)] : look_of(s, o), d });
	});
	for (size_t k = 0; beside && beside->made && k < beside->made->size(); k++) {
		const Building &o = (*beside->made)[k];
		const float d = std::hypot(o.x - x, o.z - z);
		if (d < LOOK_REACH_M) near.push_back({ (*beside->made_looks)[k], d });
	}
	return near;
}

// A candidate for a lot: the seed and the look its blocks have (b40 make_seed_body, blocks alone: no facade, no lot laid out).
struct Candidate {
	uint32_t seed = 0;
	BodyLook look;
};

Candidate draw_candidate(const CityState &s, const Building &lot, uint32_t seed) {
	Building rec = lot;
	rec.seed = seed;
	rec.style = uint16_t(seed & BUILDING_AUTO_STYLE_MASK);
	b40::Body blocks;
	b40::make_seed_body(s.buildings.designs, rec, blocks, true);
	return { seed, body_look(blocks, rec.fn) };
}

// The lot's building: the first candidate when nothing stands near to look like, else the one least like its neighbours. A
// candidate's seed is the lot's own, then hashes of it, each with its quarter written in: a city is the same every time.
Candidate best_candidate(const CityState &s, const Building &lot, const GrowthBeside *beside) {
	const GrowthDef &g = s.catalog.growth;
	const auto seed_of = [&](uint32_t k) { return quarter_seed(g, lot.x, lot.z, lot.rot, k == 0 ? lot.seed : hash_combine(lot.seed, k)); };
	const std::vector<NearLook> near = looks_near(s, lot.x, lot.z, beside);
	Candidate best = draw_candidate(s, lot, seed_of(0));
	if (near.empty()) return best;
	float score = unlikeness(best.look, near);
	for (uint32_t k = 1; k < uint32_t(LOOK_CANDIDATES); k++) {
		const Candidate c = draw_candidate(s, lot, seed_of(k));
		const float u = unlikeness(c.look, near);
		if (u > score) {
			score = u;
			best = c;
		}
	}
	return best;
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
	h = (h ^ (h >> HASH_SHIFT_13)) * AGE_MUL2;
	return double((h ^ (h >> HASH_SHIFT_16)) % uint32_t(g.decay_span_min / DECAY_TENTHS));
}

float decay_of(const GrowthDef &g, double built, double now) {
	if (g.decay_span_min <= 0) return 0;
	return float(std::clamp((now - built - double(g.decay_grace_min)) / double(g.decay_span_min), 0.0, 1.0));
}

uint32_t quarter_seed(const GrowthDef &g, float x, float z, float rot, uint32_t draw) {
	const auto cell = [](float v, float side) { return int32_t(std::floor(v / std::max(side, 1.f))); };
	const uint32_t facing = uint32_t(int32_t(std::lround(double(rot) / QUARTER_TURN))); // the way its front looks, to a quarter turn
	const uint32_t character = hash2(cell(x, g.frontage_cell_m), cell(z, g.frontage_cell_m), facing + CHARACTER_SALT) & b40::SEED_CHARACTER_MASK;
	double at = double(hash2(cell(x, g.era_cell_m), cell(z, g.era_cell_m), ERA_SALT)) / HASH_UNIT;
	const float *shares = std::hypot(x, z) < g.old_core_m ? g.core_eras : g.edge_eras; // a town is older at its heart
	uint32_t era = GrowthDef::ERAS - 1;
	for (uint32_t e = 0; e < uint32_t(GrowthDef::ERAS); e++)
		if ((at -= double(shares[e])) < 0) {
			era = e;
			break;
		}
	// the draw's upper half moved to the nearest value that says the era: the lots of a quarter still differ by it
	uint32_t upper = draw >> b40::SEED_ERA_SHIFT;
	upper = upper - upper % uint32_t(b40::ERAS) + era;
	if (upper > b40::SEED_CHARACTER_MASK) upper -= uint32_t(b40::ERAS);
	return upper << b40::SEED_ERA_SHIFT | character;
}

bool lot_occupied(const CityState &s, const Lot &l) {
	// A ruin keeps its lot until the player clears it (damage_state.h Ruin): nothing grows over rubble, whatever the hold of the blow.
	if (s.damage.ruin_under(float(l.x), float(l.z), float(l.w), float(l.d), float(l.rot))) return true;
	const Building lot = as_record(l);
	float x0, z0, x1, z1;
	BuildingRecords::bounds(lot, x0, z0, x1, z1);
	bool hit = false;
	s.buildings.meeting(x0, z0, x1, z1, [&](int id) { // every lot whose box meets this one's: a square stands over many, its middle far off
		const Building &b = s.buildings.items[size_t(id)];
		hit = hit || BuildingRecords::covers(b, lot.x, lot.z, 0.f) || BuildingRecords::covers(lot, b.x, b.z, 0.f);
	});
	return hit;
}

BodyLook look_of(const CityState &s, const Building &rec) { return *s.buildings.designs.made->look(s.buildings.designs, rec); }

std::optional<Building> record_for(const CityState &s, const std::vector<Lot> &cell_lots, int i, const GrowthBeside *beside, BodyLook *look) {
	const Lot &l = cell_lots[size_t(i)];
	const GrowthDef &g = s.catalog.growth;
	const int use = zone_use(l.zone);
	int fn = civic_for(g, map_cell_of(s, l.x, l.z).id, use, biggest(cell_lots) == i, float(l.w * l.d));
	if (fn == FN_SQUARE) fn = FN_NONE; // the squares are the plan's (sim/citygen/squares.h)
	const int tallest = tallest_near(s, l, beside);
	for (int step = zone_step(l.zone); step >= 0 && zone_step(zone_of(use, step)) == step; step--) { // its density, then each lower one the use has
		for (const LotTry &t : g.tries) {
			Building b;
			if (!try_on(g, l, t, b) || lots_over(s, b)) continue;
			b.zone = uint8_t(zone_of(use, step));
			b.fn = uint8_t(fn);
			b.seed = l.seed;
			b.style = uint16_t(l.seed & BUILDING_AUTO_STYLE_MASK);
			b.profile = uint16_t(l.profile);
			b.edges = lot_edges(s, b); // what its lot's edges meet, as the streets stand: the candidates are the bodies that will stand
			b.edges_read = true;
			// what the body is made under, kept: the record always gives this body again, whose storeys `floors` says
			b.cap = uint16_t(fn == FN_NONE && tallest >= 0 ? std::max(1, tallest) + g.organic_extra_floors : 0);
			const Candidate kept = fn == FN_NONE ? best_candidate(s, b, beside) : draw_candidate(s, b, b.seed);
			if (fn == FN_NONE && kept.look.room < g.least_room_m) continue; // its plan leaves the blocks no room: the next try, or a density lower
			b.seed = kept.seed;
			b.style = uint16_t(kept.seed & BUILDING_AUTO_STYLE_MASK);
			b.floors = uint16_t(kept.look.storeys); // the blocks are the height: a record never says another
			if (fn == FN_NONE) b.ring = lot_ring(s, b); // on a lot the player drew, the body takes the lot's own outline
			if (look) *look = kept.look;
			return b;
		}
		if (fn != FN_NONE) break; // a civic building has no density to step down
	}
	return std::nullopt;
}

} // namespace city
