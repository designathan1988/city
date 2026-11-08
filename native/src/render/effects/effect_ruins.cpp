// What a building brought down leaves (DamageState::ruins): a heap of broken pieces over its lot. When it falls the
// pieces start inside its volume and drop (the rigid step of debris.h settles them where they land, some roll off); a
// heap read from the state (open, undo, redo) is laid directly, the same pieces every time (they come from the
// record's seed). Pieces are boxes of the wall's tints, the roof's and glass, for what cannot be cut: a building comes down as its own walls, and lies as
// they came to rest (ruin_pieces.h), with only the mound laid here.
#include "render/effects/effect_world.h"
#include "render/effects/mounds.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
// The pieces' tints (sRGB, the source game's): four walls, a roof tile and glass; a building's wall is one of the first four.
constexpr uint32_t PALETTE = 6, WALLS = 4;
constexpr uint64_t SEED_MUL = 2654435761ULL; // Knuth's multiplicative hash of the record's seed
constexpr uint32_t SEED_BITS = 0xFFFu; // a mound's seed: twelve bits of the record's hash
const Color WALL_CREAM = Color(0.78f, 0.72f, 0.6f);
const Color WALL_BRICK = Color(0.54f, 0.29f, 0.21f);
const Color WALL_GREY = Color(0.6f, 0.6f, 0.58f);
const Color WALL_WHITE = Color(0.86f, 0.86f, 0.84f);
const Color ROOF_TILE = Color(0.62f, 0.27f, 0.17f);
const Color GLASS_PANE = Color(0.45f, 0.58f, 0.68f);
const Color PALETTE_TINTS[PALETTE] = { WALL_CREAM, WALL_BRICK, WALL_GREY, WALL_WHITE, ROOF_TILE, GLASS_PANE };
} // namespace

void EffectWorld::pile(const Ruin &r, bool falling, bool mound, bool loose) {
	const RuinsShape &K = params_.shapes.effect_ruins;
	Rng rng(uint64_t(r.seed) * SEED_MUL + 1);
	const float storeys = float(r.record.floors);
	const int n = std::clamp(int(r.w * r.d * K.pieces_per_m2 + storeys * r.w * K.pieces_per_storey), K.heap_fewest, K.heap_most);
	const float height = storeys / K.storeys_per_m, heap = heap_of(r);
	const float c = std::cos(r.rot), sn = std::sin(r.rot);
	const Color wall = PALETTE_TINTS[hash_u32(r.seed) % WALLS]; // each building its own wall; the roof and glass are common to all
	if (mound) mounds_.add(Vector3(r.x, ground_here_, r.z), r.rot, r.w * MOUND_SPREAD, r.d * MOUND_SPREAD, heap, wall, float(hash_u32(r.seed) & SEED_BITS) / K.seed_span, falling ? now_ : K.mound_long_ago_s);
	for (int i = 0; loose && i < n; i++) {
		const float u = (rng.f() - 0.5f) * r.w, v = (rng.f() - 0.5f) * r.d;
		const float x = r.x + u * c + v * sn, z = r.z - u * sn + v * c;
		const float edge = 1.f - std::max(std::fabs(u) / (r.w * 0.5f), std::fabs(v) / (r.d * 0.5f)); // 1 at the middle, 0 at the walls
		const float sz = K.slab_min_m + rng.f() * K.piece_span_m;
		const uint32_t pick = rng.next() % (PALETTE + 2);
		const Color tint = pick < 3 ? wall : PALETTE_TINTS[std::min(pick - 2, PALETTE - 1)];
		PieceSpec s;
		s.dusty = true;
		s.size = Vector3(sz, sz * (K.piece_flat_min + rng.f() * K.piece_flat_span), sz * (K.piece_flat_min + rng.f()));
		s.turn = Quaternion(Vector3(0, 1, 0), rng.f() * TAU_F);
		s.tint = tint;
		const float ground = ground_here_;
		if (falling) {
			s.at = Vector3(x, ground + rng.f() * height + K.fall_down_m, z);
			s.velocity = Vector3((rng.f() - 0.5f) * K.fall_out_m, 0, (rng.f() - 0.5f) * K.fall_out_m);
		} else {
			s.at = Vector3(x, ground + s.size.y * 0.5f + heap * std::min(1.f, edge * 2.f), z); // on the mound's top
			s.still = true;
		}
		debris_.add(s);
	}
}

namespace {
Ruin standing_ruin(const Building &b) {
	Ruin r;
	r.x = b.x; r.z = b.z; r.w = b.w; r.d = b.d; r.rot = b.rot; r.seed = b.seed;
	r.record = b;
	return r;
}

std::vector<Scar> scars_of(const CityState &s, const Building &b) {
	std::vector<Scar> out;
	s.damage.each_scar_of(b.seed, b.x, b.z, [&](const Scar &c) { out.push_back(c); });
	return out;
}
} // namespace

void EffectWorld::building_scar(const DamageEvent &e, const CityState &s) {
	const Building &b = s.buildings.items[size_t(e.id)];
	if (b.alive) ruins_.scar(s, standing_ruin(b), scars_of(s, b), true);
}

void EffectWorld::rebuild_ruins(const CityState &s) {
	debris_.reset();
	mounds_.reset();
	std::map<uint32_t, std::vector<Scar>> standing;
	std::map<uint32_t, Ruin> whole;
	for (const Scar &c : s.damage.scars) // those of a building that fell later name nothing now
		s.buildings.query(c.bx, c.bz, c.bx, c.bz, [&](int id) {
			const Building &b = s.buildings.items[size_t(id)];
			if (!b.alive || b.seed != c.seed || b.x != c.bx || b.z != c.bz) return;
			standing[b.seed].push_back(c);
			whole.emplace(b.seed, standing_ruin(b));
		});
	std::map<uint32_t, size_t> counts;
	for (const auto &[seed, scars] : standing) counts[seed] = scars.size();
	ruins_.keep_only(s.damage.ruins.items(), counts);
	for (const auto &[seed, scars] : standing) ruins_.scar(s, whole[seed], scars, false);
	for (const Ruin &r : s.damage.ruins) {
		ground_here_ = s.terrain.height_at(r.x, r.z);
		pile(r, false, true, !ruins_.holds(r.seed) && !ruins_.add(s, r, heap_of(r), false));
	}
}

float EffectWorld::heap_of(const Ruin &r) const {
	const RuinsShape &K = params_.shapes.effect_ruins;
	return std::clamp(float(r.record.floors) / K.storeys_per_m * K.heap_height_k, K.heap_min_m, K.heap_max_m);
}

} // namespace city
