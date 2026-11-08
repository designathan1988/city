// The people a blow or a bullet struck, thrown as themselves: the walker's own body on a ragdoll (render/agents/body_renderer.h). A limb already lost is
// not there; a limb that comes off at this blow is the person's own piece, a body of its own thrown clear, now and then a bone out of
// the stump or the piece, with a spray of blood; a body a blast tears open spills its guts and organs. Someone knocked down lies for as long as
// the simulation keeps them down (the walker is not drawn meanwhile) and the body goes when they get up. Also what a bullet draws: the tracer, the
// wound's spray and a car's bodywork hit.
#include "render/effects/effect_world.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t STRUCK_MASK = 7; // the part a round struck (damage_event.h CAS_STRUCK_SHIFT): 0 none, a blast
// A tracer's hot core and a shard of a car's glass (sRGB).
const Color TRACER_HOT = Color(1.f, 0.82f, 0.35f);
const Color GLASS_SHARD = Color(0.55f, 0.7f, 0.78f);

// The ragdoll's parts a loss takes, every limb in `limbs` (CasLimb bits, RagdollLimb order).
uint32_t parts_gone(uint32_t limbs) {
	uint32_t parts = 0;
	for (int k = 0; k < LIMBS; k++)
		if ((limbs >> k & 1u) != 0) parts |= Ragdoll::parts_of(RagdollLimb(k));
	return parts;
}
} // namespace

// What was inside a body opened: flattened rounds of dark reds, thrown out of `at` by `kick` and their own scatter.
void EffectWorld::organs(const Vector3 &at, int count, const Vector3 &kick) {
	const GoreLook &g = params_.gore;
	for (int k = 0; k < count; k++) {
		const float r = g.organ_min_m + rng_.f() * g.organ_span_m;
		PieceSpec p;
		p.round = true;
		p.at = at;
		p.size = Vector3(r * (1.f + rng_.f() * g.organ_long_span), r * (g.organ_flat_min + rng_.f() * g.organ_flat_span), r);
		p.turn = Quaternion(Vector3(rng_.f(), rng_.f(), rng_.f()).normalized(), rng_.f() * PI_F);
		p.velocity = kick + Vector3((rng_.f() - 0.5f) * g.organ_scatter_ms, g.organ_lift_min_ms + rng_.f() * g.organ_lift_span_ms, (rng_.f() - 0.5f) * g.organ_scatter_ms);
		p.tint = g.organ_tints[size_t(rng_.next() % uint32_t(g.organ_tints.size()))];
		debris_.add(p);
	}
}

void EffectWorld::body(const DamageEvent &e, const CityState &s) {
	const BodiesShape &K = params_.shapes.effect_bodies;
	const uint32_t kind = e.v & CAS_KIND_MASK;
	const uint32_t lost = (e.v >> CAS_LOST_SHIFT) & CAS_LIMB_MASK, severed = (e.v >> CAS_SEVERED_SHIFT) & CAS_LIMB_MASK;
	const bool charred = (e.v & CAS_CHARRED) != 0, bled = (e.v & CAS_BLED) != 0, struck = ((e.v >> CAS_STRUCK_SHIFT) & STRUCK_MASK) != 0;
	const float ground = GroundCells::surface(s, e.x, e.z);
	float px = e.x - e.b, pz = e.z - e.c;
	const float pl = std::hypot(px, pz);
	if (pl > K.no_push_below_m) { px /= pl; pz /= pl; } else { px = std::cos(e.a); pz = std::sin(e.a); }
	const float power = bled ? 0.f : e.d * (kind == CAS_TORN ? K.torn_push : 1.f);
	// Someone knocked down gets up when the simulation stands its walker again (dx of its seconds after the blow), by its clock.
	const bool rises = kind == CAS_KNOCKED && e.dx > 0.f;
	const float life = rises ? std::max(K.least_life_s, e.dx) : params_.body_life;
	const GoreLook &g = params_.gore;
	if (people_) {
		BodySpec b;
		b.at = Vector3(e.x, std::max(ground, e.y), e.z);
		b.heading = e.a;
		b.velocity = Vector3(px, 0, pz) * (params_.push_ms * power) + Vector3(0, power > 0.f ? params_.lift_ms : 0.f, 0);
		b.tip = Vector3(px, 0, pz) * K.topple_ms;
		b.scatter = power > 0.f ? K.limb_scatter_ms : 0.f;
		b.life_s = life;
		b.charred = charred;
		b.lost = parts_gone(lost | severed);
		b.up_s = rises ? e.sim_s + double(e.dx) : -1.0;
		b.walls = walls_near(s, e.x, e.z, K.walls_reach_m);
		b.variation = people_->variation_of(e.u, cas_age_of(e.v)); // the age the walker had (damage_event.h)
		// Now and then the bone shows at a cut: out of the stump, out of the piece, or both (one draw for the two).
		float bone_roll[LIMBS] = {};
		for (int k = 0; k < LIMBS; k++) {
			bone_roll[k] = rng_.f();
			if ((severed >> k & 1u) != 0 && bone_roll[k] < g.stump_bone_chance)
				b.bones.push_back({ Ragdoll::stump(RagdollLimb(k), false), k == LIMB_HEAD ? g.head_bone_len_m : g.bone_len_m });
		}
		// A blast tears a body open (one torn apart, most often); a round opens one only when the simulation says (CAS_OPENED).
		const float open_chance = struck ? 0.f : kind == CAS_TORN ? g.open_torn_chance : kind == CAS_DEAD ? g.open_dead_chance : 0.f;
		const bool opened = (e.v & CAS_OPENED) != 0 || rng_.f() < open_chance;
		if (opened) {
			b.guts = rng_.f() < g.pelvis_chance ? 2 : 1;
			b.guts_kick = Vector3(px, 0, pz) * g.belly_kick_ms;
			b.second_kick = b.guts_kick * g.pelvis_kick_share;
		}
		people_->bodies().add(b);
		for (int k = 0; k < LIMBS; k++) { // what came off at this blow: the person's own piece, a body of its own from here
			if ((severed >> k & 1u) == 0) continue;
			BodySpec piece = b;
			piece.piece = Ragdoll::parts_of(RagdollLimb(k));
			piece.up_s = -1.0;
			piece.guts = 0;
			piece.bones.clear();
			piece.tip = Vector3();
			const float speed = (struck ? g.shot_speed_min_ms + rng_.f() * g.shot_speed_span_ms : g.piece_speed_min_ms + g.piece_speed_power_ms * std::max(g.power_least, e.d)) *
					(g.speed_jitter_min + rng_.f() * g.speed_jitter_span);
			const float spread = struck ? g.shot_spread_ms : g.piece_spread_ms;
			const float lift = struck ? g.shot_lift_min_ms + rng_.f() * g.shot_lift_span_ms : (g.piece_lift_min_ms + rng_.f() * g.piece_lift_span_ms) * (g.lift_base + e.d);
			piece.velocity = Vector3(px * speed + (rng_.f() - 0.5f) * spread, lift, pz * speed + (rng_.f() - 0.5f) * spread);
			if (bone_roll[k] > g.piece_bone_from && bone_roll[k] < g.piece_bone_to)
				piece.bones.push_back({ Ragdoll::stump(RagdollLimb(k), true), k == LIMB_HEAD ? g.head_piece_bone_len_m : g.piece_bone_len_m });
			people_->bodies().add(piece);
		}
		if (opened) organs(Vector3(e.x, ground + K.trunk_centre_m, e.z), g.opened_organs_min + int(rng_.f() * float(g.opened_organs_span)), b.guts_kick);
	}
	if (severed != 0) puffs_.burst(e.x, ground + K.trunk_centre_m, e.z, int(K.wound_spray * 2.f), PUFF_BLOOD, K.wound_spread_m * 2.f, K.wound_size_m, K.wound_life_s);
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
