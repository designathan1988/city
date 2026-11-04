// The people a blow or a bullet struck, as bodies that fall: the walker's own cooked mesh and look as the rigid bodies and limited
// joints of ragdoll_world.h, thrown by the blow. A part lost earlier is not there; a part that comes off at this blow leaves its
// joint and flies with a spray of blood. Someone knocked down lies for as long as the simulation keeps them down (the walker is
// not drawn meanwhile) and the body goes when they get up. Also what a bullet draws: the tracer, the wound's spray and a car's
// bodywork hit.
#include "render/effects/effect_world.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float LIMB_SCATTER_MS = 1.5f; // each body of a person leaves a little differently from its neighbour
constexpr float TRUNK_PUSH_SHARE = 0.7f; // the heavy body leaves slower than a limb
constexpr float TORN_PUSH = 1.5f; // a body blown apart flies faster
constexpr float NO_PUSH_BELOW_M = 0.05f;
constexpr float LEAST_LIFE_S = 0.2f;
const Color TRACER_HOT = Color(1.f, 0.82f, 0.35f);
const Color GLASS_SHARD = Color(0.55f, 0.7f, 0.78f);
constexpr float TRACER_REACH_M = 30.f; // the line is drawn this far back along the shot
constexpr float WOUND_SPRAY = 8.f, WOUND_SIZE_M = 0.05f, WOUND_LIFE_S = 0.5f, WOUND_SPREAD_M = 0.1f;
constexpr float SPARK_COUNT = 10.f, SPARK_SPREAD_M = 0.15f, SPARK_SIZE_M = 0.1f, SPARK_LIFE_S = 0.5f;
constexpr int SHARDS = 10;
constexpr float SHARD_SIZE_M = 0.06f, SHARD_THICK_M = 0.01f, SHARD_SPEED_MS = 3.f, SHARD_LIFE_S = 6.f;
constexpr float TRUNK_HEIGHT_M = 1.2f; // where a torn-off part's blood sprays from
} // namespace

void EffectWorld::body(const DamageEvent &e, const CityState &s) {
	const uint32_t kind = e.v & CAS_KIND_MASK;
	const uint32_t lost = (e.v >> CAS_LOST_SHIFT) & CAS_LIMB_MASK, severed = (e.v >> CAS_SEVERED_SHIFT) & CAS_LIMB_MASK;
	const bool bled = (e.v & CAS_BLED) != 0;
	const float ground = s.terrain.height_at(e.x, e.z);
	float px = e.x - e.b, pz = e.z - e.c;
	const float pl = std::hypot(px, pz);
	if (pl > NO_PUSH_BELOW_M) { px /= pl; pz /= pl; } else { px = std::cos(e.a); pz = std::sin(e.a); }
	const float power = bled ? 0.f : e.d * (kind == CAS_TORN ? TORN_PUSH : 1.f);
	RagdollSpawn r;
	r.look = e.u;
	r.feet = Vector3(e.x, ground, e.z);
	r.heading = e.a;
	const float body_push = params_.push_ms * power * TRUNK_PUSH_SHARE, limb_push = params_.limb_push_ms;
	r.velocity = Vector3(px * body_push, body_push > 0.f ? params_.lift_ms * (0.5f + rng_.f()) : 0.f, pz * body_push);
	r.limb_velocity = Vector3(px * limb_push, params_.lift_ms * (0.5f + rng_.f()), pz * limb_push);
	r.scatter = LIMB_SCATTER_MS;
	r.lost = lost;
	r.severed = severed;
	r.charred = (e.v & CAS_CHARRED) != 0;
	// Someone knocked down gets up when the simulation says; the effects clock runs a little late, so the body goes a moment early.
	r.life_s = kind == CAS_KNOCKED && e.dx > 0.f ? std::max(LEAST_LIFE_S, std::min(e.dx, params_.body_life) - params_.body_lead) : params_.body_life;
	ragdolls_.spawn(r, rng_);
	if (severed != 0) puffs_.burst(e.x, ground + TRUNK_HEIGHT_M, e.z, int(WOUND_SPRAY * 2.f), PUFF_BLOOD, WOUND_SPREAD_M * 2.f, WOUND_SIZE_M, WOUND_LIFE_S);
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
