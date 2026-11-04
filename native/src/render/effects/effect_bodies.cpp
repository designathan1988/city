// The people a blow or a bullet struck, as rigid pieces: head, trunk, arms and legs, each its own box tinted by the
// walker's skin and clothes, thrown by the blow and settled by the rigid step of debris.h (no joints: the pieces of a
// body fall apart on the ground instead of folding at the joints). A limb already lost is not there; a limb that comes off
// at this blow is thrown clear with a spray of blood. Someone knocked down lies for as long as the simulation keeps them
// down (the walker is not drawn meanwhile) and the pieces go when they get up. Also what a bullet draws: the tracer, the
// wound's spray and a car's bodywork hit.
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
constexpr float CHEST_W = 0.36f, CHEST_H = 0.58f, CHEST_D = 0.2f;
constexpr float SLEEVE_W = 0.09f, SLEEVE_H = 0.6f, LEG_W = 0.13f, LEG_H = 0.9f, LEG_D = 0.14f;
const Vector3 HEAD_BOX(SKULL_W, SKULL_H, SKULL_D), TRUNK_BOX(CHEST_W, CHEST_H, CHEST_D), ARM_BOX(SLEEVE_W, SLEEVE_H, SLEEVE_W), LEG_BOX(LEG_W, LEG_H, LEG_D);
constexpr float LIMB_SCATTER_MS = 1.5f; // each part leaves a little differently from its neighbour
constexpr float TRUNK_PUSH_SHARE = 0.7f; // the heavy trunk leaves slower than the limbs
constexpr float TORN_PUSH = 1.5f; // a body blown apart flies faster
constexpr float NO_PUSH_BELOW_M = 0.05f;
constexpr uint32_t LOOK_SKIN_SHIFT = 21, CLOTHES_BOTTOM_SHIFT = 8; // bits of the walker's look hash (people_renderer.cpp)
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
} // namespace

bool EffectWorld::load_looks(const RenderContext &ctx, PackedStringArray &errors) {
	const Dictionary agents = ctx.table("agents", errors);
	const Array skin = Dictionary(agents.get("occupants", Dictionary())).get("skin", Array());
	const Array clothes = Dictionary(agents.get("people", Dictionary())).get("clothes", Array());
	if (skin.is_empty() || clothes.is_empty()) {
		errors.push_back("agents.json: occupants.skin e people.clothes são necessários para os corpos");
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
	const Color top = clothes_[h % clothes_.size()], bottom = clothes_[(h >> CLOTHES_BOTTOM_SHIFT) % clothes_.size()];
	const float ground = s.terrain.height_at(e.x, e.z);
	float px = e.x - e.b, pz = e.z - e.c;
	const float pl = std::hypot(px, pz);
	if (pl > NO_PUSH_BELOW_M) { px /= pl; pz /= pl; } else { px = std::cos(e.a); pz = std::sin(e.a); }
	const float power = bled ? 0.f : e.d * (kind == CAS_TORN ? TORN_PUSH : 1.f);
	const float life = kind == CAS_KNOCKED && e.dx > 0.f ? std::min(e.dx, params_.body_life) : params_.body_life;
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
	if ((lost & CAS_LIMB_HEAD) == 0) lay(HEAD_BOX, parts[0].at, skin, params_.push_ms * power, life, true);
	lay(TRUNK_BOX, Vector3(0, TRUNK_CENTRE_M, 0), top, params_.push_ms * power * TRUNK_PUSH_SHARE, life, false);
	for (int k = 1; k < PART_KINDS; k++) {
		if ((lost & parts[k].bit) != 0 && (severed & parts[k].bit) == 0) continue;
		const bool off = (severed & parts[k].bit) != 0;
		lay(parts[k].box, parts[k].at, parts[k].tint, off ? params_.limb_push_ms : params_.push_ms * power, life, true);
	}
	if ((severed & CAS_LIMB_HEAD) != 0) lay(HEAD_BOX, parts[0].at, skin, params_.limb_push_ms, life, true);
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
