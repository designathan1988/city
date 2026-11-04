// The events about things a blow reached (effect_world.h): street furniture thrown, vehicles blown, people killed (blood
// spray, pools and what a body torn apart leaves). Sizes of the pieces are the pieces' own (a pole, a bench, a car).
#include "render/effects/effect_world.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
// Tints (sRGB) of what is thrown.
const Color WOOD = Color(0.372f, 0.282f, 0.19f);
const Color METAL = Color(0.165f, 0.176f, 0.192f);
const Color LEAF = Color(0.239f, 0.42f, 0.165f);
const Color CHAR = Color(0.112f, 0.101f, 0.1f);
const Color FLESH = Color(0.43f, 0.05f, 0.063f);
const Color PAINT_GREY = Color(0.45f, 0.45f, 0.47f);
// The pieces' sizes in metres: a pole, a post, a hydrant, a bench, a bin, a trunk and a leaf.
constexpr float MAST_R = 0.15f, MAST_H = 7.f, POST_RAD = 0.1f, POST_H = 6.2f, HYDRANT_R = 0.2f, HYDRANT_H = 0.7f;
constexpr float BENCH_L = 1.6f, BENCH_W = 0.5f, BENCH_H = 0.5f, BIN_L = 0.5f, BIN_H = 0.9f;
constexpr float BENCH_SLAT_SHARE = 0.3f; // a bench thrown flat: its thickness over its height
constexpr float TRUNK_R = 0.2f, TRUNK_H = 3.f, LEAF_SIZE = 0.5f, LEAF_THICK = 0.1f;
constexpr float TRUNK_TOP_SHARE = 0.9f, TRUNK_STUMP_SHARE = 0.25f, TRUNK_HALF = 0.5f, CROWN_HEIGHT_K = 1.7f;
constexpr float SPLINTER_R = 0.05f, SPLINTER_LEN_MIN = 0.3f, SPLINTER_LEN_SPAN = 0.5f;
constexpr int CROWN_LEAVES = 24, SPLINTERS = 6;
constexpr float THROW_SPEED = 5.f, THROW_LIFT = 3.f, THROW_SCATTER = 4.f, THROWN_SLOW = 0.4f, THROWN_TREE = 0.6f;
constexpr float SPLINTER_PUSH_MIN = 0.8f, SPLINTER_STEP_MIN = 1.f, SPLINTER_STEP_SPAN = 2.5f, SPLINTER_BASE_M = 0.5f;
constexpr float SPARK_PUFFS = 18.f, SPARK_SPREAD = 0.3f, SPARK_SIZE = 0.12f, SPARK_LIFE = 0.8f;
constexpr float WRECK_L = 4.4f, WRECK_H = 1.4f, WRECK_W = 1.8f, CAR_BURN_S = 40.f, CAR_THROW_SPEED = 9.f, WRECK_LIFT = 3.f;
constexpr float TWO_WHEELER_BURN_S = 10.f; // a moped or a bike burns less
constexpr float TWO_WHEELER_L = 0.45f, TWO_WHEELER_H = 0.7f, TWO_WHEELER_W = 0.3f; // its box as shares of a car's
constexpr float ROLL_R = 0.32f, ROLL_W = 0.22f;
constexpr float FIRE_PUFFS_CAR = 40.f, FIRE_PUFFS_BIKE = 15.f, FIRE_SPREAD_CAR = 1.2f, FIRE_SPREAD_BIKE = 0.6f, FIRE_SIZE = 1.4f, FIRE_LIFE = 1.f;
constexpr float BLOOD_PUFFS = 40.f, BLOOD_SPREAD_M = 0.2f, BLOOD_SIZE_M = 0.04f, BLOOD_LIFE_S = 0.8f;
constexpr float ORGAN_MIN_M = 0.04f, ORGAN_SPAN_M = 0.07f, ORGAN_SPEED = 3.f, ORGAN_FLAT_MIN = 0.6f, ORGAN_THIN = 0.7f, ORGAN_THIN_SPAN = 0.6f;
constexpr float ORGAN_ROUND = 0.7f; // the share of organs drawn as short cylinders
constexpr int ORGANS_MIN = 4, ORGANS_SPAN = 4;
constexpr float BODY_HEIGHT_M = 0.5f; // blood sprays from the chest of someone falling
constexpr float SCATTER_BLOOD_M = 1.2f, POOL_MIN = 0.6f, POOL_SPAN = 0.8f, POOL_TORN = 1.4f;
constexpr int TORN_POOLS = 3;
constexpr uint32_t VEHICLE_TYPE_MASK = 255; // VEHICLE_BLOWN u: the vehicle's type in the low byte
constexpr uint32_t VEHICLE_MOTO = 5; // VehicleType::MOTO

Vector3 scatter(Rng &r, float k) { return Vector3((r.f() - 0.5f) * k, r.f() * k, (r.f() - 0.5f) * k); }
} // namespace

void EffectWorld::furniture(const DamageEvent &e, const CityState &) {
	const Vector3 push(e.dx * THROW_SPEED, THROW_LIFT, e.dz * THROW_SPEED);
	const Vector3 at(e.x, e.y, e.z);
	switch (GoneKind(e.u)) {
		case GoneKind::POLE:
		case GoneKind::POST: {
			const bool pole = GoneKind(e.u) == GoneKind::POLE;
			const float h = pole ? MAST_H : POST_H, r = pole ? MAST_R : POST_RAD;
			if (e.a > 0.f) { // to splinters: pieces of it flung out
				for (float y = 0; y < h; y += SPLINTER_STEP_MIN + rng_.f() * SPLINTER_STEP_SPAN)
					piece(at + Vector3(0, y + SPLINTER_BASE_M, 0), Vector3(r, SPLINTER_STEP_MIN + rng_.f(), r), push * (SPLINTER_PUSH_MIN + rng_.f()) + scatter(rng_, THROW_SCATTER), pole ? WOOD : METAL, true);
			} else { // whole: it falls the way it is thrown
				piece(at + Vector3(0, h * TRUNK_HALF, 0), Vector3(r, h, r), push * THROWN_SLOW, pole ? WOOD : METAL, true);
			}
			if (pole) puffs_.burst(e.x, e.y + h, e.z, int(SPARK_PUFFS), PUFF_SPARK, SPARK_SPREAD, SPARK_SIZE, SPARK_LIFE); // a flash and sparks off the line
			break;
		}
		case GoneKind::HYDRANT: piece(at + Vector3(0, HYDRANT_H * TRUNK_HALF, 0), Vector3(HYDRANT_R, HYDRANT_H, HYDRANT_R), push, METAL, true); break;
		case GoneKind::BENCH: piece(at + Vector3(0, BENCH_H, 0), Vector3(BENCH_L, BENCH_H * BENCH_SLAT_SHARE, BENCH_W), push, WOOD); break;
		case GoneKind::BIN: piece(at + Vector3(0, BIN_H * TRUNK_HALF, 0), Vector3(BIN_L, BIN_H, BIN_L), push, METAL); break;
		default: { // a tree: a charred stump, the trunk thrown, splinters and the crown in clumps of leaves
			piece(at + Vector3(0, TRUNK_H * TRUNK_STUMP_SHARE, 0), Vector3(TRUNK_R, TRUNK_H * TRUNK_HALF, TRUNK_R), Vector3(), CHAR, true);
			piece(at + Vector3(0, TRUNK_H, 0), Vector3(TRUNK_R * TRUNK_TOP_SHARE, TRUNK_H * TRUNK_HALF, TRUNK_R * TRUNK_TOP_SHARE), push * THROWN_TREE, WOOD, true, TWO_WHEELER_BURN_S);
			for (int k = 0; k < SPLINTERS; k++)
				piece(at + Vector3(0, TRUNK_H * TRUNK_HALF, 0), Vector3(SPLINTER_R, SPLINTER_LEN_MIN + rng_.f() * SPLINTER_LEN_SPAN, SPLINTER_R), push + scatter(rng_, THROW_SCATTER), WOOD);
			for (int k = 0; k < CROWN_LEAVES; k++)
				piece(at + Vector3(0, TRUNK_H * CROWN_HEIGHT_K, 0), Vector3(LEAF_SIZE * (SPLINTER_PUSH_MIN + rng_.f()), LEAF_THICK, LEAF_SIZE * (SPLINTER_PUSH_MIN + rng_.f())), push + scatter(rng_, THROW_SCATTER * 2.f), LEAF);
			break;
		}
	}
}

void EffectWorld::vehicle(const DamageEvent &e, const CityState &) {
	const bool two_wheeler = (e.u & VEHICLE_TYPE_MASK) == VEHICLE_MOTO;
	PieceSpec shell;
	shell.at = Vector3(e.x, e.y + WRECK_H * TRUNK_HALF, e.z);
	shell.size = two_wheeler ? Vector3(WRECK_L * TWO_WHEELER_L, WRECK_H * TWO_WHEELER_H, WRECK_W * TWO_WHEELER_W) : Vector3(WRECK_L, WRECK_H, WRECK_W);
	shell.turn = Quaternion(Vector3(0, 1, 0), -e.a);
	shell.velocity = Vector3((rng_.f() - 0.5f) * CAR_THROW_SPEED, WRECK_LIFT, (rng_.f() - 0.5f) * CAR_THROW_SPEED);
	shell.tint = PAINT_GREY;
	shell.burn_s = two_wheeler ? TWO_WHEELER_BURN_S : CAR_BURN_S;
	debris_.add(shell);
	for (int k = 0; k < 2; k++) { // two wheels thrown clear
		PieceSpec w;
		w.at = Vector3(e.x, e.y + ROLL_R, e.z);
		w.size = Vector3(ROLL_R, ROLL_W, ROLL_R);
		w.cylinder = true;
		w.tint = CHAR;
		w.velocity = Vector3((rng_.f() - 0.5f) * CAR_THROW_SPEED, WRECK_LIFT + rng_.f() * THROW_SCATTER, (rng_.f() - 0.5f) * CAR_THROW_SPEED);
		debris_.add(w);
	}
	puffs_.burst(e.x, e.y + 1.f, e.z, int(two_wheeler ? FIRE_PUFFS_BIKE : FIRE_PUFFS_CAR), PUFF_FLAME, two_wheeler ? FIRE_SPREAD_BIKE : FIRE_SPREAD_CAR, FIRE_SIZE, FIRE_LIFE);
}

void EffectWorld::casualty(const DamageEvent &e, const CityState &s) {
	const uint32_t kind = e.v & CAS_KIND_MASK;
	if (kind == CAS_KNOCKED) return; // knocked down: the body falls (the ragdoll, effect_ragdoll.cpp)
	puffs_.burst(e.x, e.y + BODY_HEIGHT_M, e.z, int(BLOOD_PUFFS), PUFF_BLOOD, BLOOD_SPREAD_M, BLOOD_SIZE_M, BLOOD_LIFE_S);
	const bool torn = kind == CAS_TORN;
	for (int k = 0; k < (torn ? TORN_POOLS : 1); k++) { // the pool the body leaves, spreading
		Mark m;
		m.kind = MARK_POOL;
		const float a = rng_.f() * TAU_F, r = k == 0 ? 0.f : rng_.f() * SCATTER_BLOOD_M;
		m.x = e.x + std::cos(a) * r;
		m.z = e.z + std::sin(a) * r;
		m.y = s.terrain.height_at(m.x, m.z) + Marks::SURFACE_LIFT_M;
		m.size = params_.pool_size * (POOL_MIN + rng_.f() * POOL_SPAN) * (torn ? POOL_TORN : 1.f);
		m.seed = rng_.f();
		m.born = now_;
		marks_.add_blood(m);
	}
	if (!torn) return;
	const int organs = ORGANS_MIN + int(rng_.f() * float(ORGANS_SPAN));
	for (int k = 0; k < organs; k++) { // a body torn apart: what was inside, thrown over the street
		const float s0 = ORGAN_MIN_M + rng_.f() * ORGAN_SPAN_M, a = rng_.f() * TAU_F;
		piece(Vector3(e.x, e.y + BODY_HEIGHT_M, e.z), Vector3(s0, s0 * (ORGAN_FLAT_MIN + rng_.f()), s0 * (ORGAN_THIN + rng_.f() * ORGAN_THIN_SPAN)),
				Vector3(std::cos(a) * ORGAN_SPEED, THROW_LIFT, std::sin(a) * ORGAN_SPEED), FLESH, rng_.f() < ORGAN_ROUND);
	}
}

void EffectWorld::apply(const DamageEvent &e, const CityState &s) {
	switch (e.kind) {
		case DamageEvent::BLAST: blast(e, s); break;
		case DamageEvent::BUILDING_HIT: building_hit(e); break;
		case DamageEvent::BUILDING_DOWN: building_down(e); break;
		case DamageEvent::FURNITURE: furniture(e, s); break;
		case DamageEvent::VEHICLE_BLOWN: vehicle(e, s); break;
		case DamageEvent::CASUALTY: casualty(e, s); break;
		case DamageEvent::GEYSER: geysers_.push_back({ e.x, e.y, e.z, now_ + double(params_.geyser_s), 0.f }); break;
		default: break;
	}
}

} // namespace city
