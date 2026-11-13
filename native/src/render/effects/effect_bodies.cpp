// The people a blow or a bullet struck, thrown as themselves: the walker's own body on a ragdoll (render/agents/body_renderer.h). A limb already lost is
// not there; a limb that comes off at this blow is thrown clear as a piece of the same body on a ragdoll of its own, with a spray of blood. Someone knocked
// down lies for as long as the simulation keeps them down (the walker is not drawn meanwhile) and the body goes when they get up. Also what a bullet draws:
// the tracer, the wound's spray and a car's bodywork hit.
#include "render/effects/effect_world.h"

#include "core/math.h"
#include "render/agents/ragdoll_parts.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
// An adult's trunk stands this high, its middle (NASA-STD-3000 vol. 1 table 3.3.1, the 50th percentile mean of both sexes): where a wound sprays.
constexpr float TRUNK_CENTRE_M = 1.2f;
constexpr float WALLS_REACH_M = 12.f; // farther than a body is thrown
constexpr float TOPPLE_MS = 2.2f; // the head of somebody going down, over their feet
constexpr float LIMB_SCATTER_MS = 1.5f; // each part leaves a little differently from its neighbour
constexpr float TORN_PUSH = 1.5f; // a body blown apart flies faster
constexpr float NO_PUSH_BELOW_M = 0.05f;
constexpr float LEAST_LIFE_S = 0.2f;
constexpr uint32_t ALL_PARTS = (1u << RAGDOLL_PARTS) - 1;
const Color TRACER_HOT = Color(1.f, 0.82f, 0.35f);
const Color GLASS_SHARD = Color(0.55f, 0.7f, 0.78f);
constexpr float TRACER_REACH_M = 30.f; // the line is drawn this far back along the shot
constexpr float WOUND_SPRAY = 8.f, WOUND_SIZE_M = 0.05f, WOUND_LIFE_S = 0.5f, WOUND_SPREAD_M = 0.1f;
constexpr float SPARK_COUNT = 10.f, SPARK_SPREAD_M = 0.15f, SPARK_SIZE_M = 0.1f, SPARK_LIFE_S = 0.5f;
constexpr int SHARDS = 10;
constexpr float SHARD_SIZE_M = 0.06f, SHARD_THICK_M = 0.01f, SHARD_SPEED_MS = 3.f, SHARD_LIFE_S = 6.f;
constexpr uint32_t LIMBS[] = { CAS_LIMB_HEAD, CAS_LIMB_ARM_L, CAS_LIMB_ARM_R, CAS_LIMB_LEG_L, CAS_LIMB_LEG_R }; // what can come off

// The ragdoll's parts a loss takes (ragdoll.ts LOST_PARTS: the arm from the forearm, the leg from the calf).
uint32_t parts_gone(uint32_t limbs) {
	constexpr uint32_t SKULL = 1u << 2, FOREARM_L = 1u << 4, FOREARM_R = 1u << 6, CALF_L = 3u << 8, CALF_R = 3u << 11;
	return ((limbs & CAS_LIMB_HEAD) != 0 ? SKULL : 0) | ((limbs & CAS_LIMB_ARM_L) != 0 ? FOREARM_L : 0) | ((limbs & CAS_LIMB_ARM_R) != 0 ? FOREARM_R : 0) |
			((limbs & CAS_LIMB_LEG_L) != 0 ? CALF_L : 0) | ((limbs & CAS_LIMB_LEG_R) != 0 ? CALF_R : 0);
}

constexpr uint32_t bit(ragdoll::Joint j) { return 1u << j; }
constexpr int STRUCK_KINDS = 7; // none, head, trunk, left arm, right arm, left leg, right leg (CAS_STRUCK_SHIFT)
constexpr uint32_t STRUCK_MASK = 7;

// The particles a casualty bleeds from (ragdoll_parts.h Joint): the struck part's, with none struck (a blow, a body blown apart) the
// trunk's; a limb that came off bleeds at its stump, where the part closes (BodyRenderer::pose), and what it took bleeds nowhere.
uint32_t bleeding(uint32_t struck, uint32_t gone) {
	using namespace ragdoll;
	constexpr uint32_t AT[STRUCK_KINDS] = { bit(PEL) | bit(CHE) | bit(BEL), bit(HEA) | bit(TOP), bit(CHE) | bit(BEL), bit(LE) | bit(LW), bit(RE) | bit(RW),
		bit(LK) | bit(LA), bit(RK) | bit(RA) };
	struct Loss {
		uint32_t limb, stump, taken;
	};
	constexpr Loss LOSSES[] = { { CAS_LIMB_HEAD, bit(HEA), bit(TOP) }, { CAS_LIMB_ARM_L, bit(LE), bit(LW) }, { CAS_LIMB_ARM_R, bit(RE), bit(RW) },
		{ CAS_LIMB_LEG_L, bit(LK), bit(LA) | bit(LT) }, { CAS_LIMB_LEG_R, bit(RK), bit(RA) | bit(RT) } };
	uint32_t at = AT[struck < uint32_t(STRUCK_KINDS) ? struck : 0], taken = 0;
	for (const Loss &l : LOSSES)
		if ((gone & l.limb) != 0) {
			at |= l.stump;
			taken |= l.taken;
		}
	return at & ~taken;
}
} // namespace

void EffectWorld::body(const DamageEvent &e, const CityState &s) {
	const uint32_t kind = e.v & CAS_KIND_MASK;
	const uint32_t lost = (e.v >> CAS_LOST_SHIFT) & CAS_LIMB_MASK, severed = (e.v >> CAS_SEVERED_SHIFT) & CAS_LIMB_MASK;
	const bool charred = (e.v & CAS_CHARRED) != 0, bled = (e.v & CAS_BLED) != 0, crawl = (e.v & CAS_CRAWL) != 0;
	const uint32_t struck = (e.v >> CAS_STRUCK_SHIFT) & STRUCK_MASK;
	const float ground = s.terrain.height_at(e.x, e.z);
	float px = e.x - e.b, pz = e.z - e.c;
	const float pl = std::hypot(px, pz);
	if (pl > NO_PUSH_BELOW_M) { px /= pl; pz /= pl; } else { px = std::cos(e.a); pz = std::sin(e.a); }
	// a round throws nobody: its knock-back never passes the recoil the shooter feels (https://en.wikipedia.org/wiki/Stopping_power), so
	// somebody shot (a struck part) goes down from where they stand; a blow throws, a trip pitches forward
	const float power = bled || struck != 0 ? 0.f : e.d * (kind == CAS_TORN ? TORN_PUSH : 1.f);
	// Someone knocked down gets up when the simulation says (lying hurt for good, the simulation's long while: dying there makes the
	// body a dead one, BodyRenderer::die); the effects clock runs a little late, so the body goes a moment early. The dead stay (0).
	const float life = kind == CAS_KNOCKED && e.dx > 0.f ? std::max(LEAST_LIFE_S, e.dx - params_.body_lead) : 0.f;
	const float yaw = e.a;
	if (people_) {
		BodySpec b;
		b.at = Vector3(e.x, std::max(ground, e.y), e.z);
		b.heading = yaw;
		b.velocity = Vector3(px, 0, pz) * (params_.push_ms * power) + Vector3(0, power > 0.f ? params_.lift_ms : 0.f, 0);
		b.tip = Vector3(px, 0, pz) * TOPPLE_MS;
		// the living put their hands out the way they go down, to take the fall (down.brace_ms, its source in effects.json); the dead do not
		if (kind == CAS_KNOCKED) b.brace = Vector3(px, -params_.brace_down, pz).normalized() * params_.brace_ms;
		b.scatter = power > 0.f ? LIMB_SCATTER_MS : 0.f;
		b.life_s = life;
		b.charred = charred;
		b.lost = parts_gone(lost | severed);
		b.rises = kind == CAS_KNOCKED;
		b.who = e.id;
		// the dead bleed where they come to rest, and so do the gravely hurt lying there for good (crawl); who only fell does not
		b.pool = kind == CAS_TORN ? params_.pool_torn : kind == CAS_DEAD || crawl ? 1.f : 0.f;
		b.bleeds = bleeding(struck, lost | severed);
		b.walls = walls_about(s, *street_look_, e.x, e.z, WALLS_REACH_M);
		const bool walker = e.id >= 0 && e.id < int(s.traffic.peds.size());
		b.variation = people_->variation_of(e.u, walker ? int(s.traffic.peds[size_t(e.id)].age) : -1);
		if (walker) people_->drawn_pose(e.id, b.clip, b.phase); // it falls from the pose it was drawn in
		people_->bodies().add(b);
		// what came off: a piece of the same body, in the same pose, thrown clear on a ragdoll of its own (Ragdoll::keep_parts); it stays where it
		// falls (0: for good), whether its owner gets up or not
		for (uint32_t limb : LIMBS) {
			if ((severed & limb) == 0) continue;
			BodySpec piece = b;
			piece.piece = parts_gone(limb);
			piece.lost = ALL_PARTS & ~piece.piece;
			piece.velocity = Vector3(px * params_.limb_push_ms + (rng_.f() - 0.5f) * LIMB_SCATTER_MS, params_.lift_ms * (0.5f + rng_.f()),
					pz * params_.limb_push_ms + (rng_.f() - 0.5f) * LIMB_SCATTER_MS);
			piece.tip = piece.brace = Vector3();
			piece.life_s = 0.f;
			piece.rises = false;
			piece.who = -1;
			piece.pool = 0.f;
			people_->bodies().add(piece);
		}
	}
	if (severed != 0) puffs_.burst(e.x, ground + TRUNK_CENTRE_M, e.z, int(WOUND_SPRAY * 2.f), PUFF_BLOOD, WOUND_SPREAD_M * 2.f, WOUND_SIZE_M, WOUND_LIFE_S);
}

void EffectWorld::tracer(const DamageEvent &e) {
	Vector3 dir(e.dx, e.a, e.dz);
	if (dir.length_squared() < NO_PUSH_BELOW_M) return;
	dir = dir.normalized();
	const Vector3 end(e.x, e.y, e.z), start = end - dir * TRACER_REACH_M;
	PieceSpec t;
	t.size = Vector3(params_.tracer_m, params_.tracer_m, TRACER_REACH_M);
	t.at = (start + end) * 0.5f;
	t.turn = Basis::looking_at(dir, Vector3(0, 1, 0)).get_rotation_quaternion();
	t.tint = TRACER_HOT;
	t.still = true;
	t.life_s = params_.tracer_life;
	debris_.add(t);
	puffs_.burst(e.x, e.y, e.z, int(SPARK_COUNT), PUFF_SPARK, SPARK_SPREAD_M, SPARK_SIZE_M, SPARK_LIFE_S); // where it ends
}

void EffectWorld::wound(const DamageEvent &e) {
	puffs_.burst(e.x, e.y, e.z, int(WOUND_SPRAY), PUFF_BLOOD, WOUND_SPREAD_M, WOUND_SIZE_M, WOUND_LIFE_S);
}

void EffectWorld::vehicle_hit(const DamageEvent &e) {
	puffs_.burst(e.x, e.y, e.z, int(SPARK_COUNT), PUFF_SPARK, SPARK_SPREAD_M, SPARK_SIZE_M, SPARK_LIFE_S);
	if (e.u == 0) return; // metal: sparks only; glass: shards too
	for (int k = 0; k < SHARDS; k++) {
		PieceSpec p;
		p.size = Vector3(SHARD_SIZE_M * (0.5f + rng_.f()), SHARD_THICK_M, SHARD_SIZE_M * (0.5f + rng_.f()));
		p.at = Vector3(e.x, e.y, e.z);
		p.turn = Quaternion(Vector3(0, 1, 0), rng_.f() * TAU_F);
		p.tint = GLASS_SHARD;
		p.velocity = Vector3((rng_.f() - 0.5f + e.dx) * SHARD_SPEED_MS, rng_.f() * SHARD_SPEED_MS, (rng_.f() - 0.5f + e.dz) * SHARD_SPEED_MS);
		p.life_s = SHARD_LIFE_S;
		debris_.add(p);
	}
}

} // namespace city
