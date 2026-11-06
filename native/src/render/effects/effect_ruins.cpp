// What a building brought down leaves (DamageState::ruins): a heap of broken pieces over its lot. When it falls the
// pieces start inside its volume and drop (the rigid step of debris.h settles them where they land, some roll off); a
// heap read from the state (open, undo, redo) is laid directly, the same pieces every time (they come from the
// record's seed). Pieces are boxes of the wall's tints, the roof's and glass; a building falling in view comes down as its own walls instead
// (ruin_pieces.h), and only the mound is laid here.
#include "render/effects/effect_world.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float PIECES_PER_M2 = 0.35f; // heap pieces over the lot's area, and what each floor adds to the share
constexpr float PIECES_PER_STOREY = 0.25f;
constexpr float SEED_SPAN = 4096.f; // the relief of a heap, from its seed
constexpr int HEAP_FEWEST = 14, HEAP_MOST = 60;
constexpr float STOREYS_PER_M = 0.3125f; // a storey is about 3.2 m: a building's height for the fall
constexpr float SLAB_MIN_M = 0.35f, PIECE_SPAN_M = 1.6f, PIECE_FLAT_MIN = 0.08f, PIECE_FLAT_SPAN = 0.22f; // slabs of wall, roof and glass
constexpr float HEAP_HEIGHT_K = 0.07f; // a heap rises this share of the building's height: most of a building is air (a design choice, no measured source)
constexpr float HEAP_MIN_M = 0.8f, HEAP_MAX_M = 4.5f;
constexpr float MOUND_SPREAD = 1.15f; // it slumps past the walls it fell from
constexpr float FALL_OUT_M = 2.5f; // the pieces of a falling building are thrown out a little
constexpr float FALL_DOWN_M = 1.5f;
constexpr double MOUND_LONG_AGO_S = -1e6; // a heap read from the state stands whole
constexpr uint32_t PALETTE = 6;
const Color WALL_CREAM = Color(0.78f, 0.72f, 0.6f);
const Color WALL_BRICK = Color(0.54f, 0.29f, 0.21f);
const Color WALL_GREY = Color(0.6f, 0.6f, 0.58f);
const Color WALL_WHITE = Color(0.86f, 0.86f, 0.84f);
const Color ROOF_TILE = Color(0.62f, 0.27f, 0.17f);
const Color GLASS_PANE = Color(0.45f, 0.58f, 0.68f);
const Color PALETTE_TINTS[6] = { WALL_CREAM, WALL_BRICK, WALL_GREY, WALL_WHITE, ROOF_TILE, GLASS_PANE };
} // namespace

void EffectWorld::pile(const Ruin &r, bool falling, bool mound, bool loose) {
	Rng rng(uint64_t(r.seed) * 2654435761ULL + 1);
	const float storeys = float(r.record.floors);
	const int n = std::clamp(int(r.w * r.d * PIECES_PER_M2 + storeys * r.w * PIECES_PER_STOREY), HEAP_FEWEST, HEAP_MOST);
	const float height = storeys / STOREYS_PER_M, heap = heap_of(r);
	const float c = std::cos(r.rot), sn = std::sin(r.rot);
	const Color wall = PALETTE_TINTS[hash_u32(r.seed) % 4]; // each building its own wall; the roof and glass are common to all
	if (mound) mounds_.add(Vector3(r.x, ground_here_, r.z), r.rot, r.w * MOUND_SPREAD, r.d * MOUND_SPREAD, heap, wall, float(hash_u32(r.seed) & 0xFFFu) / SEED_SPAN, falling ? now_ : MOUND_LONG_AGO_S);
	for (int i = 0; loose && i < n; i++) {
		const float u = (rng.f() - 0.5f) * r.w, v = (rng.f() - 0.5f) * r.d;
		const float x = r.x + u * c + v * sn, z = r.z - u * sn + v * c;
		const float edge = 1.f - std::max(std::fabs(u) / (r.w * 0.5f), std::fabs(v) / (r.d * 0.5f)); // 1 at the middle, 0 at the walls
		const float sz = SLAB_MIN_M + rng.f() * PIECE_SPAN_M;
		const uint32_t pick = rng.next() % (PALETTE + 2);
		const Color tint = pick < 3 ? wall : PALETTE_TINTS[std::min(pick - 2, PALETTE - 1)];
		PieceSpec s;
		s.dusty = true;
		s.size = Vector3(sz, sz * (PIECE_FLAT_MIN + rng.f() * PIECE_FLAT_SPAN), sz * (PIECE_FLAT_MIN + rng.f()));
		s.turn = Quaternion(Vector3(0, 1, 0), rng.f() * TAU_F);
		s.tint = tint;
		const float ground = ground_here_;
		if (falling) {
			s.at = Vector3(x, ground + rng.f() * height + FALL_DOWN_M, z);
			s.velocity = Vector3((rng.f() - 0.5f) * FALL_OUT_M, 0, (rng.f() - 0.5f) * FALL_OUT_M);
		} else {
			s.at = Vector3(x, ground + s.size.y * 0.5f + heap * std::min(1.f, edge * 2.f), z); // on the mound's top
			s.still = true;
		}
		debris_.add(s);
	}
}

void EffectWorld::rebuild_ruins(const CityState &s) {
	debris_.reset();
	mounds_.reset();
	ruins_.keep_only(s.damage.ruins);
	for (const Ruin &r : s.damage.ruins) {
		ground_here_ = s.terrain.height_at(r.x, r.z);
		pile(r, false, true, !ruins_.holds(r.seed));
	}
}

float EffectWorld::heap_of(const Ruin &r) {
	return std::clamp(float(r.record.floors) / STOREYS_PER_M * HEAP_HEIGHT_K, HEAP_MIN_M, HEAP_MAX_M);
}

} // namespace city
