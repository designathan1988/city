// The people a blow or a bullet struck, thrown as themselves: the walker's own body on a ragdoll (render/agents/body_renderer.h). A limb already lost is
// not there; a limb that comes off at this blow is thrown clear as a piece tinted by the skin and clothes, with a spray of blood. Someone knocked down lies for
// as long as the simulation keeps them down (the walker is not drawn meanwhile) and the body goes when they get up. Also what a bullet draws: the tracer,
// the wound's spray and a car's bodywork hit.
#include "render/effects/effect_world.h"
#include "render/data_reader.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int PART_KINDS = 5; // the parts a body throws: the head, two arms, two legs
// An adult standing, in metres (NASA-STD-3000 vol. 1 table 3.3.1, the 50th percentile mean of both sexes, rounded to the
// box that holds the part): where the middle of each part is, and its box.
constexpr uint32_t LOOK_SKIN_SHIFT = 21, LOOK_TWO_BITS = 3, LOOK_TOP_SHIFT = 4, LOOK_BOTTOM_TEST = 2, LOOK_BOTTOM_SHIFT = 9; // bits of the walker's look hash (people_renderer.cpp)
// A charred body, a tracer's hot core and a shard of a car's glass (sRGB, the source game's); the salt of a body's look.
const Color BODY_CHAR = Color(0.05f, 0.045f, 0.04f);
const Color TRACER_HOT = Color(1.f, 0.82f, 0.35f);
const Color GLASS_SHARD = Color(0.55f, 0.7f, 0.78f);
constexpr uint32_t SALT_LOOK = 7;
constexpr uint32_t STRUCK_MASK = 7; // damage_event.h: the part a round struck, three bits from CAS_STRUCK_SHIFT (0 none: a blast)

Color parse_color(const DataValue &v) {
	v.colour(); // '#rrggbb', or an error
	return Color::html(v.text());
}

// The ragdoll's parts a loss takes (ragdoll.ts LOST_PARTS: the arm from the forearm, the leg from the calf).
uint32_t parts_gone(uint32_t limbs) {
	// The ragdoll's part bits a loss takes.
	constexpr uint32_t SKULL = 1u << 2, FOREARM_L = 1u << 4, FOREARM_R = 1u << 6, CALF_L = 3u << 8, CALF_R = 3u << 11;
	return ((limbs & CAS_LIMB_HEAD) != 0 ? SKULL : 0) | ((limbs & CAS_LIMB_ARM_L) != 0 ? FOREARM_L : 0) | ((limbs & CAS_LIMB_ARM_R) != 0 ? FOREARM_R : 0) |
			((limbs & CAS_LIMB_LEG_L) != 0 ? CALF_L : 0) | ((limbs & CAS_LIMB_LEG_R) != 0 ? CALF_R : 0);
}
} // namespace

bool EffectWorld::load_looks(const RenderContext &ctx, PackedStringArray &errors) {
	const Dictionary table = ctx.table("agents", errors);
	if (!errors.is_empty()) return false;
	const int64_t before = errors.size();
	const DataValue agents = DataReader{ errors, "agents.json" }.at(table); // render/data_reader.h
	const DataValue skin = agents["occupants"]["skin"], clothes = agents["people"]["clothes"];
	if (skin.size() == 0 || clothes.size() == 0) {
		agents.fail("occupants.skin and people.clothes are what the bodies wear");
		return false;
	}
	skins_.clear(); // a second read is the table again
	clothes_.clear();
	for (int k = 0; k < skin.size(); k++) skins_.push_back(parse_color(skin[k]));
	for (int k = 0; k < clothes.size(); k++) clothes_.push_back(parse_color(clothes[k]));
	return errors.size() == before;
}

void EffectWorld::body(const DamageEvent &e, const CityState &s) {
	const BodiesShape &K = params_.shapes.effect_bodies;
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
	if (pl > K.no_push_below_m) { px /= pl; pz /= pl; } else { px = std::cos(e.a); pz = std::sin(e.a); }
	const float power = bled ? 0.f : e.d * (kind == CAS_TORN ? K.torn_push : 1.f);
	// Someone knocked down gets up when the simulation says; the effects clock runs a little late, so the body goes a moment early.
	const float life = kind == CAS_KNOCKED && e.dx > 0.f ? std::max(K.least_life_s, std::min(e.dx, params_.body_life) - params_.body_lead) : params_.body_life;
	const float yaw = e.a;
	const float side_x = -std::sin(yaw), side_z = std::cos(yaw); // to the walker's left
	struct Part { Vector3 box, at; Color tint; uint32_t bit; };
	const Vector3 HEAD_BOX(K.skull_w, K.skull_h, K.skull_d), ARM_BOX(K.sleeve_w, K.sleeve_h, K.sleeve_w), LEG_BOX(K.leg_w, K.leg_h, K.leg_d);
	// A limb or the head as a cylinder: (radius, length, radius) of the box that holds it.
	auto round_of = [](const Vector3 &box) { return Vector3(std::min(box.x, box.z) * 0.5f, box.y, std::min(box.x, box.z) * 0.5f); };
	const Part parts[PART_KINDS] = {
		{ HEAD_BOX, Vector3(0, K.skull_centre_m, 0), skin, CAS_LIMB_HEAD },
		{ ARM_BOX, Vector3(side_x * K.arm_side_m, K.arm_centre_m, side_z * K.arm_side_m), top, CAS_LIMB_ARM_L },
		{ ARM_BOX, Vector3(-side_x * K.arm_side_m, K.arm_centre_m, -side_z * K.arm_side_m), top, CAS_LIMB_ARM_R },
		{ LEG_BOX, Vector3(side_x * K.leg_side_m, K.leg_centre_m, side_z * K.leg_side_m), bottom, CAS_LIMB_LEG_L },
		{ LEG_BOX, Vector3(-side_x * K.leg_side_m, K.leg_centre_m, -side_z * K.leg_side_m), bottom, CAS_LIMB_LEG_R },
	};
	auto lay = [&](const Vector3 &box, const Vector3 &at, const Color &tint, float push, float keep_life, bool round) {
		PieceSpec p;
		p.size = round ? round_of(box) : box;
		p.cylinder = round;
		p.at = Vector3(e.x + at.x, ground + at.y, e.z + at.z);
		p.turn = Quaternion(Vector3(0, 1, 0), -yaw);
		p.tint = charred ? BODY_CHAR : tint;
		p.velocity = Vector3(px * push + (rng_.f() - 0.5f) * K.limb_scatter_ms, push > 0.f ? params_.lift_ms * (0.5f + rng_.f()) : 0.f, pz * push + (rng_.f() - 0.5f) * K.limb_scatter_ms);
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
		b.tip = Vector3(px, 0, pz) * K.topple_ms;
		b.scatter = power > 0.f ? K.limb_scatter_ms : 0.f;
		b.life_s = life;
		b.charred = charred;
		b.lost = parts_gone(lost | severed);
		b.rises = kind == CAS_KNOCKED;
		b.writhes = (e.v & CAS_CRAWL) != 0; // down for good and hurt: it writhes where it comes to rest
		b.walls = walls_near(s, e.x, e.z, K.walls_reach_m);
		b.variation = people_->variation_of(e.u, cas_age_of(e.v)); // the age the walker had (damage_event.h)
		const uint32_t serial = people_->bodies().add(b);
		if (serial != 0) gore(e, serial, Vector3(px, 0, pz), severed);
		for (int k = 1; k < PART_KINDS; k++)
			if ((severed & parts[k].bit) != 0) lay(parts[k].box, parts[k].at, parts[k].tint, params_.limb_push_ms, life, true);
		if ((severed & CAS_LIMB_HEAD) != 0) lay(HEAD_BOX, parts[0].at, skin, params_.limb_push_ms, life, true);
	}
	if (severed != 0) puffs_.burst(e.x, ground + K.trunk_centre_m, e.z, int(K.wound_spray * 2.f), PUFF_BLOOD, K.wound_spread_m * 2.f, K.wound_size_m, K.wound_life_s);
}

// What an opened body shows (ragdoll.ts openBelly and sever's boneOut): shot open by its trunk wounds, or torn open by a blast (one
// torn apart most often, one killed now and then; a bullet never opens a body at once), the guts out of the belly and, now and
// then, the pelvis; a bone's end out of each stump left at this blow.
void EffectWorld::gore(const DamageEvent &e, uint32_t serial, const Vector3 &way, uint32_t severed) {
	const GoreShape &K = params_.shapes.gore;
	const uint32_t kind = e.v & CAS_KIND_MASK;
	const bool struck = ((e.v >> CAS_STRUCK_SHIFT) & STRUCK_MASK) != 0;
	const bool torn_open = !struck && ((kind == CAS_TORN && rng_.f() < K.torn_open_chance) || (kind == CAS_DEAD && rng_.f() < K.dead_open_chance));
	if ((e.v & CAS_OPENED) != 0 || torn_open) open_body(serial, way * ((e.v & CAS_OPENED) != 0 ? K.shot_kick_ms : K.torn_kick_ms), rng_.f() < K.pelvis_chance);
	struct Stump { uint32_t limb; int from, at; bool head; };
	static constexpr Stump STUMPS[] = { { CAS_LIMB_ARM_L, rag::LS, rag::LE, false }, { CAS_LIMB_ARM_R, rag::RS, rag::RE, false },
		{ CAS_LIMB_LEG_L, rag::LH, rag::LK, false }, { CAS_LIMB_LEG_R, rag::RH, rag::RK, false }, { CAS_LIMB_HEAD, rag::CHE, rag::NEC, true } };
	for (const Stump &s : STUMPS)
		if ((severed & s.limb) != 0 && rng_.f() < K.bone_chance) gore_.bone(serial, s.from, s.at, s.head ? K.bone_head_len_m : K.bone_len_m);
}

void EffectWorld::open_body(uint32_t body, const Vector3 &kick, bool pelvis_too) {
	const GoreShape &K = params_.shapes.gore;
	Vector3 at;
	if (!people_ || !people_->bodies().joint(body, rag::BEL, at)) return;
	gore_.spill(body, rag::BEL, at, kick, rng_);
	if (pelvis_too && people_->bodies().joint(body, rag::PEL, at)) gore_.spill(body, rag::PEL, at, kick * K.pelvis_kick, rng_);
}

void EffectWorld::tracer(const DamageEvent &e) {
	const BodiesShape &K = params_.shapes.effect_bodies;
	Vector3 dir(e.dx, e.a, e.dz);
	if (dir.length_squared() < K.no_push_below_m) return;
	dir = dir.normalized();
	const Vector3 end(e.x, e.y, e.z), start = end - dir * K.tracer_reach_m;
	PieceSpec t;
	t.size = Vector3(params_.tracer_m, params_.tracer_m, K.tracer_reach_m);
	t.at = (start + end) * 0.5f;
	t.turn = Basis::looking_at(dir, Vector3(0, 1, 0)).get_rotation_quaternion();
	t.tint = TRACER_HOT;
	t.still = true;
	t.life_s = params_.tracer_life;
	debris_.add(t);
	puffs_.burst(e.x, e.y, e.z, int(K.spark_count), PUFF_SPARK, K.spark_spread_m, K.spark_size_m, K.spark_life_s); // where it ends
}

void EffectWorld::wound(const DamageEvent &e) {
	const BodiesShape &K = params_.shapes.effect_bodies;
	puffs_.burst(e.x, e.y, e.z, int(K.wound_spray), PUFF_BLOOD, K.wound_spread_m, K.wound_size_m, K.wound_life_s);
}

void EffectWorld::vehicle_hit(const DamageEvent &e) {
	const BodiesShape &K = params_.shapes.effect_bodies;
	puffs_.burst(e.x, e.y, e.z, int(K.spark_count), PUFF_SPARK, K.spark_spread_m, K.spark_size_m, K.spark_life_s);
	if (e.u == 0) return; // metal: sparks only; glass: shards too
	for (int k = 0; k < K.shards; k++) {
		PieceSpec p;
		p.size = Vector3(K.shard_size_m * (0.5f + rng_.f()), K.shard_thick_m, K.shard_size_m * (0.5f + rng_.f()));
		p.at = Vector3(e.x, e.y, e.z);
		p.turn = Quaternion(Vector3(0, 1, 0), rng_.f() * TAU_F);
		p.tint = GLASS_SHARD;
		p.velocity = Vector3((rng_.f() - 0.5f + e.dx) * K.shard_speed_ms, rng_.f() * K.shard_speed_ms, (rng_.f() - 0.5f + e.dz) * K.shard_speed_ms);
		p.life_s = K.shard_life_s;
		debris_.add(p);
	}
}

} // namespace city
