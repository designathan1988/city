// The events about things a blow reached (effect_world.h): street furniture and the player's poles thrown, the wires torn off
// them, the woods' trees felled, vehicles blown, people killed (blood spray, pools and what a body torn apart leaves). Sizes of the
// pieces are the pieces' own (a pole, a bench, a car, a tree's height).
#include "render/effects/effect_world.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
// Tints (sRGB) of what is thrown.
const Color WOOD = Color(0.372f, 0.282f, 0.19f);
const Color METAL_TINT = Color(0.165f, 0.176f, 0.192f);
const Color LEAF = Color(0.239f, 0.42f, 0.165f);
const Color CHAR = Color(0.112f, 0.101f, 0.1f);
const Color FLESH = Color(0.43f, 0.05f, 0.063f);
const Color PAINT_GREY = Color(0.45f, 0.45f, 0.47f);
// The pieces' sizes in metres: a pole, a post, a hydrant, a bench, a bin, a trunk and a leaf.
constexpr uint32_t VEHICLE_TYPE_MASK = 255; // VEHICLE_BLOWN u: the vehicle's type in the low byte

Vector3 scatter(Rng &r, float k) { return Vector3((r.f() - 0.5f) * k, r.f() * k, (r.f() - 0.5f) * k); }
} // namespace

// The push of the last blast at (x, z), `speed` at its heart (the reference game's playEffects.ts `away`): out from it, less
// with the distance, to fall_least of it at fall_far_share fireball radii; lifted throw_lift as much.
Vector3 EffectWorld::away(float x, float z, float speed) const {
	const ThingsShape &K = params_.shapes.effect_things;
	const float dx = x - blast_x_, dz = z - blast_z_, d = std::hypot(dx, dz);
	const float share = blast_r_ > 0.f ? std::max(K.fall_least, 1.f - d / (K.fall_far_share * blast_r_)) : 1.f;
	const float ux = d > 0.f ? dx / d : 0.f, uz = d > 0.f ? dz / d : 0.f;
	return Vector3(ux * speed * share, K.throw_lift * share, uz * speed * share);
}

void EffectWorld::furniture(const DamageEvent &e, const CityState &) {
	const ThingsShape &K = params_.shapes.effect_things;
	const Vector3 push = away(e.x, e.z, K.throw_speed);
	const Vector3 at(e.x, e.y, e.z);
	switch (GoneKind(e.u)) {
		case GoneKind::POLE:
		case GoneKind::POST: {
			const bool pole = GoneKind(e.u) == GoneKind::POLE;
			const float h = e.b > 0.f ? e.b : pole ? K.mast_h : K.post_h, r = pole ? K.mast_r : K.post_rad;
			const Color tint = pole ? WOOD : METAL_TINT;
			const int broke = int(std::lround(e.a));
			if (broke == FURNITURE_SPLINTERS) { // to splinters: pieces of it flung out
				for (float y = 0; y < h; y += K.splinter_step_min + rng_.f() * K.splinter_step_span)
					piece(at + Vector3(0, y + K.splinter_base_m, 0), Vector3(r, K.splinter_step_min + rng_.f(), r), push * (K.splinter_push_min + rng_.f()) + scatter(rng_, K.throw_scatter), tint, true);
			} else if (broke == FURNITURE_SNAPPED) { // snapped: the stump left standing, the top thrown over
				const float cut = h * (K.snap_min + rng_.f() * K.snap_span);
				PieceSpec stump;
				stump.at = at + Vector3(0, cut * K.trunk_half, 0);
				stump.size = Vector3(r, cut, r);
				stump.cylinder = true;
				stump.tint = tint;
				stump.still = true;
				stump.random_spin = false;
				debris_.add(stump);
				piece(at + Vector3(0, cut + (h - cut) * K.trunk_half, 0), Vector3(r, h - cut, r), push * K.thrown_tree, tint, true);
			} else { // whole: it falls the way it is thrown
				piece(at + Vector3(0, h * K.trunk_half, 0), Vector3(r, h, r), push * K.thrown_slow, tint, true);
			}
			if (pole) {
				const Vector3 top = at + Vector3(0, h, 0);
				piece(top, Vector3(K.arm_l_m, K.arm_t_m, K.arm_t_m), push + Vector3(0, K.throw_lift, 0), WOOD); // the cross-arm knocked off
				if (e.c > 0.f) piece(top + Vector3(K.lamp_reach_m * K.trunk_half, 0, 0), Vector3(K.lamp_l_m, K.lamp_h_m, K.lamp_w_m), push, METAL_TINT); // its lamp
				puffs_.burst(e.x, e.y + h, e.z, int(K.spark_puffs), PUFF_SPARK, K.spark_spread, K.spark_size, K.spark_life); // a flash and sparks off the line
			}
			break;
		}
		case GoneKind::HYDRANT: piece(at + Vector3(0, K.hydrant_h * K.trunk_half, 0), Vector3(K.hydrant_r, K.hydrant_h, K.hydrant_r), push, METAL_TINT, true); break;
		case GoneKind::BENCH: piece(at + Vector3(0, K.bench_h, 0), Vector3(K.bench_l, K.bench_h * K.bench_slat_share, K.bench_w), push, WOOD); break;
		case GoneKind::BIN: piece(at + Vector3(0, K.bin_h * K.trunk_half, 0), Vector3(K.bin_l, K.bin_h, K.bin_l), push, METAL_TINT); break;
		default: { // a tree, a shrub, or another piece of furniture the gone record files with them: each its own look
			const int was = int(e.v) - 1; // FurnitureKind, -1 not street furniture (a planted tree)
			if (was < 0 || was == int(FurnitureKind::TREE)) fell_tree(at, push, e.b > 0.f ? e.b : K.trunk_h);
			else if (was == int(FurnitureKind::SHRUB)) fell_tree(at, push, K.shrub_h);
			else if (was == int(FurnitureKind::POLE)) piece(at + Vector3(0, K.mast_h * K.trunk_half, 0), Vector3(K.mast_r, K.mast_h, K.mast_r), push * K.thrown_slow, WOOD, true);
			else
				for (int k = 0; k < K.bits; k++) // burst into bits (a drain, a postbox, a sign...)
					piece(at + Vector3(0, K.bit_lift_m, 0), Vector3(K.bit_l_min + rng_.f() * K.bit_l_span, K.bit_h_min + rng_.f() * K.bit_h_span, K.bit_w_min + rng_.f() * K.bit_w_span),
							push + scatter(rng_, K.throw_scatter), METAL_TINT);
			break;
		}
	}
}

// A tree felled (playEffects.ts explode, a tree): a charred stump of cut_min plus cut_span of its height left standing, the trunk
// above thrown burning, splinters at the break and the crown in clumps of leaves.
void EffectWorld::fell_tree(const Vector3 &at, const Vector3 &push, float height) {
	const ThingsShape &K = params_.shapes.effect_things;
	const float cut = height * (K.cut_min + rng_.f() * K.cut_span);
	PieceSpec stump;
	stump.at = at + Vector3(0, cut * K.trunk_half, 0);
	stump.size = Vector3(K.trunk_r, cut, K.trunk_r);
	stump.cylinder = true;
	stump.tint = CHAR;
	stump.still = true;
	stump.random_spin = false;
	debris_.add(stump);
	piece(at + Vector3(0, cut + (height - cut) * K.trunk_half, 0), Vector3(K.trunk_r * K.trunk_top_share, height - cut, K.trunk_r * K.trunk_top_share), push * K.thrown_tree, WOOD, true,
			K.trunk_burn_s);
	for (int k = 0; k < K.splinters; k++)
		piece(at + Vector3(0, cut, 0), Vector3(K.splinter_r, K.splinter_len_min + rng_.f() * K.splinter_len_span, K.splinter_r), push + scatter(rng_, K.throw_scatter), WOOD);
	for (int k = 0; k < K.crown_leaves; k++)
		piece(at + Vector3(0, height * K.crown_share, 0), Vector3(K.leaf_size * (K.splinter_push_min + rng_.f()), K.leaf_thick, K.leaf_size * (K.splinter_push_min + rng_.f())),
				push + scatter(rng_, K.throw_scatter * 2.f), LEAF);
}

// The woods' trees the blow's clearing took, found where they stood (the tree renderer's own placement, the clearing left out),
// each felled away from the blast.
void EffectWorld::trees(const DamageEvent &e, const CityState &s) {
	if (!tree_finder_) return;
	const ThingsShape &K = params_.shapes.effect_things;
	felled_.clear();
	tree_finder_(s, e.x, e.z, e.a, int(e.u), felled_);
	for (const TreeInstance &t : felled_) fell_tree(t.p, away(t.p.x, t.p.z, K.throw_speed), t.height);
}

// A wire torn off a felled pole: one rope a primary wire of the span, pinned at the standing pole's arm, loose where the felled
// pole's arm fell to, thrown away from the blast (blast.ts wire; playEffects.ts explode over hit.wires).
void EffectWorld::wire(const DamageEvent &e) {
	const WiresShape &K = params_.shapes.wires;
	const Vector3 pin(e.x, e.y, e.z), loose(e.b, e.c, e.d);
	Vector3 span = loose - pin;
	span.y = 0;
	const float l = span.length();
	if (l <= 0.f) return;
	const Vector3 across = Vector3(-span.z, 0, span.x) / l; // the arm reaches across the span
	const Vector3 kick = away(loose.x, loose.z, K.kick_ms);
	for (float along : K.primary_m)
		wires_.add(pin + Vector3(0, e.a - K.arm_drop_m, 0) + across * along, loose + Vector3(0, e.a * K.fall_share, 0) + across * along, kick);
}

void EffectWorld::vehicle(const DamageEvent &e, const CityState &) {
	const ThingsShape &K = params_.shapes.effect_things;
	const bool rider = two_wheeler(VehicleType(e.u & VEHICLE_TYPE_MASK));
	PieceSpec shell;
	const size_t type = e.u & VEHICLE_TYPE_MASK;
	shell.wreck = type < wreck_sizes_.size() ? int(type) : -1;
	shell.size = shell.wreck >= 0 ? wreck_sizes_[type] : Vector3(K.wreck_w, K.wreck_h, K.wreck_l);
	shell.at = Vector3(e.x, e.y + shell.size.y * K.trunk_half, e.z);
	shell.turn = Quaternion(Vector3(0, 1, 0), -e.a);
	shell.velocity = Vector3((rng_.f() - 0.5f) * K.car_throw_speed, K.wreck_lift, (rng_.f() - 0.5f) * K.car_throw_speed);
	shell.tint = PAINT_GREY;
	const bool drop = e.kind == DamageEvent::VEHICLE_DROP; // a rider shot off: the machine falls over, it does not burn
	shell.burn_s = drop ? 0.f : rider ? K.moped_burn_s : K.car_burn_s;
	debris_.add(shell);
	for (int k = 0; k < 2; k++) { // two wheels thrown clear
		PieceSpec w;
		w.at = Vector3(e.x, e.y + K.roll_r, e.z);
		w.size = Vector3(K.roll_r, K.roll_w, K.roll_r);
		w.cylinder = true;
		w.tint = CHAR;
		w.velocity = Vector3((rng_.f() - 0.5f) * K.car_throw_speed, K.wreck_lift + rng_.f() * K.throw_scatter, (rng_.f() - 0.5f) * K.car_throw_speed);
		debris_.add(w);
	}
	if (!drop) puffs_.burst(e.x, e.y + 1.f, e.z, int(rider ? K.fire_puffs_bike : K.fire_puffs_car), PUFF_FLAME, rider ? K.fire_spread_bike : K.fire_spread_car, K.fire_size, K.fire_life);
}

// A few drops where somebody bleeding walks.
void EffectWorld::drip(const DamageEvent &e) {
	const ThingsShape &K = params_.shapes.effect_things;
	Mark m;
	m.kind = MARK_SPLAT;
	m.x = e.x + (rng_.f() - 0.5f) * K.drip_scatter_m;
	m.z = e.z + (rng_.f() - 0.5f) * K.drip_scatter_m;
	m.y = e.y + Marks::SURFACE_LIFT_M;
	m.size = K.drip_min_m + rng_.f() * K.drip_span_m;
	m.seed = rng_.f();
	m.born = now_;
	marks_.add_blood(m);
}

void EffectWorld::casualty(const DamageEvent &e, const CityState &s) {
	const ThingsShape &K = params_.shapes.effect_things;
	const uint32_t kind = e.v & CAS_KIND_MASK;
	if ((e.v & CAS_BLED) == 0) body(e, s); // the walker is drawn no more: the body takes its place (one that bled out is already lying there)
	if (kind == CAS_KNOCKED) return; // a knock bleeds no more than the wound that came with it
	puffs_.burst(e.x, e.y + K.chest_rise_m, e.z, int(K.blood_puffs), PUFF_BLOOD, K.blood_spread_m, K.blood_size_m, K.blood_life_s);
	const bool torn = kind == CAS_TORN;
	for (int k = 0; k < (torn ? K.torn_pools : 1); k++) { // the pool the body leaves, spreading
		Mark m;
		m.kind = MARK_POOL;
		const float a = rng_.f() * TAU_F, r = k == 0 ? 0.f : rng_.f() * K.scatter_blood_m;
		m.x = e.x + std::cos(a) * r;
		m.z = e.z + std::sin(a) * r;
		m.y = s.terrain.height_at(m.x, m.z) + Marks::SURFACE_LIFT_M;
		m.size = params_.pool_size * (K.pool_min + rng_.f() * K.pool_span) * (torn ? K.pool_torn : 1.f);
		m.seed = rng_.f();
		m.born = now_;
		marks_.add_blood(m);
	}
	if (!torn) return;
	const int organs = K.organs_min + int(rng_.f() * float(K.organs_span));
	for (int k = 0; k < organs; k++) { // a body torn apart: what was inside, thrown over the street
		const float s0 = K.organ_min_m + rng_.f() * K.organ_span_m, a = rng_.f() * TAU_F;
		piece(Vector3(e.x, e.y + K.chest_rise_m, e.z), Vector3(s0, s0 * (K.organ_flat_min + rng_.f()), s0 * (K.organ_thin + rng_.f() * K.organ_thin_span)),
				Vector3(std::cos(a) * K.organ_speed, K.throw_lift, std::sin(a) * K.organ_speed), FLESH, rng_.f() < K.organ_round);
	}
}

void EffectWorld::apply(const DamageEvent &e, const CityState &s) {
	switch (e.kind) {
		case DamageEvent::BLAST: blast(e, s); break;
		case DamageEvent::BUILDING_HIT: building_hit(e); break;
		case DamageEvent::BUILDING_DOWN: building_down(e, s); break;
		case DamageEvent::BUILDING_SCAR: building_scar(e, s); break;
		case DamageEvent::FURNITURE: furniture(e, s); break;
		case DamageEvent::VEHICLE_BLOWN: vehicle(e, s); break;
		case DamageEvent::CASUALTY: casualty(e, s); break;
		case DamageEvent::SHOT: tracer(e); break;
		case DamageEvent::WOUND: wound(e); break;
		case DamageEvent::DRIP: drip(e); break;
		case DamageEvent::VEHICLE_HIT: vehicle_hit(e); break;
		case DamageEvent::VEHICLE_DROP: vehicle(e, s); break;
		case DamageEvent::GEYSER: geysers_.push_back({ e.x, e.y, e.z, now_ + double(params_.geyser_s), 0.f }); break;
		case DamageEvent::WIRE: wire(e); break;
		case DamageEvent::TREES: trees(e, s); break;
		default: break;
	}
}

} // namespace city
