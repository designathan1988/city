// The people a blow or a bullet struck, thrown as themselves: the walker's own body on a ragdoll (render/agents/body_renderer.h). A limb already lost is
// not there; a limb that comes off at this blow is thrown clear as a piece tinted by the skin and clothes, with a spray of blood. Someone knocked down lies for
// as long as the simulation keeps them down (the walker is not drawn meanwhile) and the body goes when they get up. Also what a bullet draws: the tracer,
// the wound's spray and a car's bodywork hit.
#include "render/effects/effect_world.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
// An adult standing, in metres (NASA-STD-3000 vol. 1 table 3.3.1, the 50th percentile mean of both sexes, rounded to the
// box that holds the part): where the middle of each part is, and its box.
constexpr float SKULL_CENTRE_M = 1.62f, TRUNK_CENTRE_M = 1.2f, ARM_CENTRE_M = 1.2f, LEG_CENTRE_M = 0.45f, ARM_SIDE_M = 0.24f, LEG_SIDE_M = 0.1f;
constexpr float SKULL_W = 0.18f, SKULL_H = 0.23f, SKULL_D = 0.2f;
constexpr float SLEEVE_W = 0.09f, SLEEVE_H = 0.6f, LEG_W = 0.13f, LEG_H = 0.9f, LEG_D = 0.14f;
const Vector3 HEAD_BOX(SKULL_W, SKULL_H, SKULL_D), ARM_BOX(SLEEVE_W, SLEEVE_H, SLEEVE_W), LEG_BOX(LEG_W, LEG_H, LEG_D);
constexpr float WALLS_REACH_M = 12.f; // farther than a body is thrown
constexpr float TOPPLE_MS = 2.2f; // the head of somebody going down, over their feet
constexpr float LIMB_SCATTER_MS = 1.5f; // each part leaves a little differently from its neighbour
constexpr float TORN_PUSH = 1.5f; // a body blown apart flies faster
constexpr float NO_PUSH_BELOW_M = 0.05f;
constexpr float LEAST_LIFE_S = 0.2f;
constexpr uint32_t LOOK_SKIN_SHIFT = 21, LOOK_TWO_BITS = 3, LOOK_TOP_SHIFT = 4, LOOK_BOTTOM_TEST = 2, LOOK_BOTTOM_SHIFT = 9; // bits of the walker's look hash (people_renderer.cpp)
const Color BODY_CHAR = Color(0.05f, 0.045f, 0.04f);
const Color TRACER_HOT = Color(1.f, 0.82f, 0.35f);
const Color GLASS_SHARD = Color(0.55f, 0.7f, 0.78f);
constexpr float TRACER_REACH_M = 30.f; // the line is drawn this far back along the shot
constexpr float WOUND_SPRAY = 8.f, WOUND_SIZE_M = 0.05f, WOUND_LIFE_S = 0.5f, WOUND_SPREAD_M = 0.1f;
constexpr float SPARK_COUNT = 10.f, SPARK_SPREAD_M = 0.15f, SPARK_SIZE_M = 0.1f, SPARK_LIFE_S = 0.5f;
constexpr int SHARDS = 10;
constexpr float SHARD_SIZE_M = 0.06f, SHARD_THICK_M = 0.01f, SHARD_SPEED_MS = 3.f, SHARD_LIFE_S = 6.f;
constexpr int PART_KINDS = 5;
constexpr uint32_t SALT_LOOK = 7;

Color parse_color(const Variant &v) { return Color::html(String(v)); }

// The ragdoll's parts a loss takes (ragdoll.ts LOST_PARTS: the arm from the forearm, the leg from the calf).
uint32_t parts_gone(uint32_t limbs) {
	constexpr uint32_t SKULL = 1u << 2, FOREARM_L = 1u << 4, FOREARM_R = 1u << 6, CALF_L = 3u << 8, CALF_R = 3u << 11;
	return ((limbs & CAS_LIMB_HEAD) != 0 ? SKULL : 0) | ((limbs & CAS_LIMB_ARM_L) != 0 ? FOREARM_L : 0) | ((limbs & CAS_LIMB_ARM_R) != 0 ? FOREARM_R : 0) |
			((limbs & CAS_LIMB_LEG_L) != 0 ? CALF_L : 0) | ((limbs & CAS_LIMB_LEG_R) != 0 ? CALF_R : 0);
}
} // namespace

bool EffectWorld::load_looks(const RenderContext &ctx, PackedStringArray &errors) {
	const Dictionary agents = ctx.table("agents", errors);
	const Array skin = Dictionary(agents.get("occupants", Dictionary())).get("skin", Array());
	const Array clothes = Dictionary(agents.get("people", Dictionary())).get("clothes", Array());
	if (skin.is_empty() || clothes.is_empty()) {
		errors.push_back("agents.json: occupants.skin and people.clothes are needed for the bodies");
		return false;
	}
	for (int k = 0; k < int(skin.size()); k++) skins_.push_back(parse_color(skin[k]));
	for (int k = 0; k < int(clothes.size()); k++) clothes_.push_back(parse_color(clothes[k]));
	return true;
}

void EffectWorld::body(const DamageEvent &e, const CityState &s) {
	const uint32_t kind = e.v & CAS_KIND_MASK;
	const uint32_t lost = (e.v >> CAS_LOST_SHIFT) & CAS_LIMB_MASK, severed = (e.v >> CAS_SEVERED_SHIFT) & CAS_LIMB_MASK;
	const bool charred = (e.v & CAS_CHARRED) != 0, bled = (e.v & CAS_BLED) != 0;
	const uint32_t h = hash_u32(e.u ^ SALT_LOOK);
	const Color skin = skins_[(e.u >> LOOK_SKIN_SHIFT) % skins_.size()];
	// The walker's own clothes as the people renderer decodes the look hash (a top or bottom tinted by the hash; one drawn in
	// the texture's own colour has no tint to copy: a clothes colour from the hash stands in).
	const Color top = clothes_[((e.u & LOOK_TWO_BITS) != 0 ? e.u >> LOOK_TOP_SHIFT : h) % clothes_.size()];
	const Color bottom = clothes_[(((e.u >> LOOK_BOTTOM_TEST) & LOOK_TWO_BITS) != 0 ? e.u >> LOOK_BOTTOM_SHIFT : h >> LOOK_BOTTOM_SHIFT) % clothes_.size()];
	const float ground = s.terrain.height_at(e.x, e.z);
	float px = e.x - e.b, pz = e.z - e.c;
	const float pl = std::hypot(px, pz);
	if (pl > NO_PUSH_BELOW_M) { px /= pl; pz /= pl; } else { px = std::cos(e.a); pz = std::sin(e.a); }
	const float power = bled ? 0.f : e.d * (kind == CAS_TORN ? TORN_PUSH : 1.f);
	// Someone knocked down gets up when the simulation says (lying hurt for good, the simulation's long while: dying there makes the
	// body a dead one, BodyRenderer::die); the effects clock runs a little late, so the body goes a moment early. The dead stay (0).
	const float life = kind == CAS_KNOCKED && e.dx > 0.f ? std::max(LEAST_LIFE_S, e.dx - params_.body_lead) : 0.f;
	const float yaw = e.a;
	const float side_x = -std::sin(yaw), side_z = std::cos(yaw); // to the walker's left
	struct Part { Vector3 box, at; Color tint; uint32_t bit; };
	// A limb or the head as a cylinder: (radius, length, radius) of the box that holds it.
	auto round_of = [](const Vector3 &box) { return Vector3(std::min(box.x, box.z) * 0.5f, box.y, std::min(box.x, box.z) * 0.5f); };
	const Part parts[PART_KINDS] = {
		{ HEAD_BOX, Vector3(0, SKULL_CENTRE_M, 0), skin, CAS_LIMB_HEAD },
		{ ARM_BOX, Vector3(side_x * ARM_SIDE_M, ARM_CENTRE_M, side_z * ARM_SIDE_M), top, CAS_LIMB_ARM_L },
		{ ARM_BOX, Vector3(-side_x * ARM_SIDE_M, ARM_CENTRE_M, -side_z * ARM_SIDE_M), top, CAS_LIMB_ARM_R },
		{ LEG_BOX, Vector3(side_x * LEG_SIDE_M, LEG_CENTRE_M, side_z * LEG_SIDE_M), bottom, CAS_LIMB_LEG_L },
		{ LEG_BOX, Vector3(-side_x * LEG_SIDE_M, LEG_CENTRE_M, -side_z * LEG_SIDE_M), bottom, CAS_LIMB_LEG_R },
	};
	auto lay = [&](const Vector3 &box, const Vector3 &at, const Color &tint, float push, float keep_life, bool round) {
		PieceSpec p;
		p.size = round ? round_of(box) : box;
		p.cylinder = round;
		p.at = Vector3(e.x + at.x, ground + at.y, e.z + at.z);
		p.turn = Quaternion(Vector3(0, 1, 0), -yaw);
		p.tint = charred ? BODY_CHAR : tint;
		p.velocity = Vector3(px * push + (rng_.f() - 0.5f) * LIMB_SCATTER_MS, push > 0.f ? params_.lift_ms * (0.5f + rng_.f()) : 0.f, pz * push + (rng_.f() - 0.5f) * LIMB_SCATTER_MS);
		p.life_s = keep_life;
		p.random_spin = push > 0.f;
		p.spin = Vector3();
		debris_.add(p);
	};
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
		b.pool = kind == CAS_KNOCKED ? 0.f : kind == CAS_TORN ? params_.pool_torn : 1.f; // the dead bleed where they come to rest
		b.walls = walls_near(s, e.x, e.z, WALLS_REACH_M);
		const bool walker = e.id >= 0 && e.id < int(s.traffic.peds.size());
		b.variation = people_->variation_of(e.u, walker ? int(s.traffic.peds[size_t(e.id)].age) : -1);
		if (walker) people_->drawn_pose(e.id, b.clip, b.phase); // it falls from the pose it was drawn in
		people_->bodies().add(b);
		// what came off stays where it fell (0: for good), whether its owner gets up or not
		for (int k = 1; k < PART_KINDS; k++)
			if ((severed & parts[k].bit) != 0) lay(parts[k].box, parts[k].at, parts[k].tint, params_.limb_push_ms, 0.f, true);
		if ((severed & CAS_LIMB_HEAD) != 0) lay(HEAD_BOX, parts[0].at, skin, params_.limb_push_ms, 0.f, true);
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
