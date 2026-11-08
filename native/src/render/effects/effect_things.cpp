// The events about things a blow reached (effect_world.h): street furniture thrown, vehicles blown, people killed (blood
// spray, pools and what a body torn apart leaves). Sizes of the pieces are the pieces' own (a pole, a bench, a car).
#include "render/effects/effect_world.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
// The pieces' sizes in metres: a pole, a post, a hydrant, a bench, a bin, a trunk and a leaf.
constexpr float STUMP_WIDEN = 1.1f; // a snapped pole's stump, a little wider than its top
constexpr float SNAP_GAP_M = 0.1f; // the top starts this far over the break
constexpr float SNAP_THROW = 0.7f, LAMP_THROW = 1.2f; // of the push: a snapped top, a lamp's head
// A lighting column (streets.json props: about 9 m, 0.12 m across at its top), a shrub and a small box.
constexpr float COLUMN_H = 9.f, COLUMN_R = 0.12f, COLUMN_SPIN = 1.6f, COLUMN_HEAD_LIFT = 4.f;
constexpr float COLUMN_HEAD_L = 0.78f, COLUMN_HEAD_T = 0.14f, COLUMN_HEAD_W = 0.38f;
constexpr float BUSH_TALL = 1.2f, BUSH_STEM = 0.14f, BUSH_LEAF = 0.3f, BUSH_LEAF_DEPTH = 0.05f, BUSH_TOP_SHARE = 0.8f;
constexpr int BUSH_LEAVES = 14, SCRAP_PIECES = 4;
constexpr float SCRAP_FROM_M = 0.5f;
constexpr float SCRAP_L_LEAST = 0.2f, SCRAP_T_LEAST = 0.06f, SCRAP_W_LEAST = 0.1f, SCRAP_L_MORE = 0.5f, SCRAP_T_MORE = 0.2f, SCRAP_W_MORE = 0.3f;
constexpr uint32_t VEHICLE_TYPE_MASK = 255; // VEHICLE_BLOWN u: the vehicle's type in the low byte

Vector3 scatter(Rng &r, float k) { return Vector3((r.f() - 0.5f) * k, r.f() * k, (r.f() - 0.5f) * k); }
const Vector3 UP(0, 1, 0), ACROSS_X(1, 0, 0);

// A piece that turns about `axis` at `rate` rad/s as it is thrown (a pole falling over), not at random.
PieceSpec turning(const Vector3 &at, const Vector3 &size, const Vector3 &v, const Color &tint, const Vector3 &spin) {
	PieceSpec s;
	s.at = at;
	s.size = size;
	s.velocity = v;
	s.tint = tint;
	s.cylinder = true;
	s.random_spin = false;
	s.spin = spin;
	return s;
}
} // namespace

// A pole or a signal post knocked out: whole it falls over away from the burst, snapped its stump stays and its top falls, burst it
// flies in splinters; a pole's arm and lamp are knocked off its top and the line arcs there, a post's head is thrown and arcs.
void EffectWorld::pole(const DamageEvent &e) {
	const ThingsShape &K = params_.shapes.effect_things;
	const PoleLook &L = params_.pole;
	const bool pole = GoneKind(e.u) == GoneKind::POLE;
	const float h = pole ? seats_.height + L.crown_m : K.post_h, r = pole ? K.mast_r : K.post_rad;
	const Color tint = pole ? params_.tints.wood : params_.tints.metal;
	const Vector3 at(e.x, e.y, e.z), push(e.dx * K.throw_speed, K.throw_lift, e.dz * K.throw_speed), way(e.dx, 0, e.dz);
	const Vector3 axis = way.length_squared() > 0.f ? UP.cross(way).normalized() : ACROSS_X; // the top falls the way it is thrown
	if (e.a == FURNITURE_SPLINTERS) {
		for (float y = 0; y < h; y += K.splinter_step_min + rng_.f() * K.splinter_step_span)
			piece(at + Vector3(0, y + K.splinter_base_m, 0), Vector3(r, K.splinter_step_min + rng_.f(), r), push * (K.splinter_push_min + rng_.f()) + scatter(rng_, K.throw_scatter), tint, true);
	} else if (e.a == FURNITURE_SNAPPED) {
		const float cut = h * (L.cut_min + rng_.f() * L.cut_span);
		PieceSpec stump = turning(at + Vector3(0, cut * K.trunk_half, 0), Vector3(r * STUMP_WIDEN, cut, r * STUMP_WIDEN), Vector3(), tint, Vector3());
		stump.still = true;
		debris_.add(stump);
		debris_.add(turning(at + Vector3(0, cut + (h - cut) * K.trunk_half + SNAP_GAP_M, 0), Vector3(r, h - cut, r), push * SNAP_THROW, tint,
				axis * (L.snap_spin_min + rng_.f() * L.snap_spin_span)));
	} else {
		debris_.add(turning(at + Vector3(0, h * K.trunk_half, 0), Vector3(r, h, r), push * K.thrown_slow, tint, axis * (L.fall_spin_min + rng_.f() * L.fall_spin_span)));
	}
	if (!pole) {
		const Vector3 head = at + Vector3(0, h - L.post_head_m.y * K.trunk_half, 0);
		piece(head, L.post_head_m, push + Vector3(0, K.throw_lift, 0), params_.tints.metal);
		arc(head, params_.arc.signal_arc_min_s + rng_.f() * params_.arc.signal_arc_span_s);
		return;
	}
	const Vector3 top = at + Vector3(0, seats_.height, 0);
	const float arm = seats_.urban.back() + L.arm_over_m;
	PieceSpec bar = turning(top, Vector3(arm, L.arm_section_m, L.arm_section_m), push, tint, Vector3());
	bar.cylinder = false;
	bar.random_spin = true;
	bar.turn = Quaternion(UP, std::atan2(-axis.z, axis.x)); // lying across the throw, as it stood across the line
	debris_.add(bar);
	if (e.v != 0) piece(top + way * (arm * L.lamp_reach_share), L.lamp_head_m, push * LAMP_THROW, params_.tints.metal);
	arc(top, params_.arc.pole_arc_min_s + rng_.f() * params_.arc.pole_arc_span_s); // a flash and sparks off the line as it goes
}

void EffectWorld::furniture(const DamageEvent &e, const CityState &) {
	const ThingsShape &K = params_.shapes.effect_things;
	const Vector3 push(e.dx * K.throw_speed, K.throw_lift, e.dz * K.throw_speed);
	const Vector3 at(e.x, e.y, e.z);
	const Tints &c = params_.tints;
	switch (GoneKind(e.u)) {
		case GoneKind::POLE:
		case GoneKind::POST: pole(e); break;
		case GoneKind::HYDRANT: piece(at + Vector3(0, K.hydrant_h * K.trunk_half, 0), Vector3(K.hydrant_r, K.hydrant_h, K.hydrant_r), push, c.metal, true); break;
		case GoneKind::BENCH: piece(at + Vector3(0, K.bench_h, 0), Vector3(K.bench_l, K.bench_h * K.bench_slat_share, K.bench_w), push, c.wood); break;
		case GoneKind::BIN: piece(at + Vector3(0, K.bin_h * K.trunk_half, 0), Vector3(K.bin_l, K.bin_h, K.bin_l), push, c.metal); break;
		case GoneKind::COLUMN: { // a lighting column bent over and thrown, its head flung off, sparks from its lamp
			const Vector3 way(e.dx, 0, e.dz);
			const Vector3 axis = way.length_squared() > 0.f ? UP.cross(way).normalized() : ACROSS_X;
			debris_.add(turning(at + Vector3(0, COLUMN_H * K.trunk_half, 0), Vector3(COLUMN_R, COLUMN_H, COLUMN_R), push * K.thrown_slow, c.metal, axis * COLUMN_SPIN));
			piece(at + Vector3(0, COLUMN_H, 0), Vector3(COLUMN_HEAD_L, COLUMN_HEAD_T, COLUMN_HEAD_W), push + Vector3(0, COLUMN_HEAD_LIFT, 0), c.metal);
			arc(at + Vector3(0, COLUMN_H, 0), params_.arc.gap_min_s);
			break;
		}
		case GoneKind::SHRUB: // its stem thrown over, its leaves scattered
			piece(at + Vector3(0, BUSH_TALL * K.trunk_half, 0), Vector3(BUSH_STEM, BUSH_TALL, BUSH_STEM), push * K.thrown_slow, c.wood, true);
			for (int k = 0; k < BUSH_LEAVES; k++)
				piece(at + Vector3(0, BUSH_TALL * BUSH_TOP_SHARE, 0), Vector3(BUSH_LEAF, BUSH_LEAF_DEPTH, BUSH_LEAF), push + scatter(rng_, K.throw_scatter * 2.f), c.leaf);
			break;
		case GoneKind::BOX: // a post box, a phone or a sign: in a few pieces of metal
			for (int k = 0; k < SCRAP_PIECES; k++)
				piece(at + Vector3(0, SCRAP_FROM_M, 0), Vector3(SCRAP_L_LEAST + rng_.f() * SCRAP_L_MORE, SCRAP_T_LEAST + rng_.f() * SCRAP_T_MORE, SCRAP_W_LEAST + rng_.f() * SCRAP_W_MORE),
						push + scatter(rng_, K.throw_scatter), c.metal);
			break;
		default: fell(at, params_.tree.street_height_m, params_.tree.street_width_m, Vector3(e.dx, 0, e.dz)); break; // a street tree
	}
}

void EffectWorld::fell(const Vector3 &at, float height, float width, const Vector3 &way) {
	const ThingsShape &K = params_.shapes.effect_things;
	const auto &L = params_.tree;
	const Tints &c = params_.tints;
	const Vector3 push(way.x * K.throw_speed, K.throw_lift, way.z * K.throw_speed);
	const Vector3 axis = way.length_squared() > 0.f ? UP.cross(way).normalized() : ACROSS_X; // the crown falls the way it is thrown
	const float r = height * L.trunk_r_share, cut = height * (L.stump_min + rng_.f() * L.stump_span);
	PieceSpec stump = turning(at + Vector3(0, cut * K.trunk_half, 0), Vector3(r, cut, r), Vector3(), c.charred, Vector3());
	stump.still = true;
	debris_.add(stump);
	PieceSpec trunk = turning(at + Vector3(0, cut + (height - cut) * K.trunk_half, 0), Vector3(r, height - cut, r), push * L.throw_share, c.wood,
			axis * (L.spin_min + rng_.f() * L.spin_span));
	trunk.burn_s = L.burn_min_s + rng_.f() * L.burn_span_s;
	debris_.add(trunk);
	for (int k = 0; k < L.chips; k++)
		piece(at + Vector3(0, cut, 0), Vector3(L.chip_r_m, L.chip_len_min_m + rng_.f() * L.chip_len_span_m, L.chip_r_m), push + scatter(rng_, K.throw_scatter), c.wood);
	const int leaves = std::max(1, int(width * L.leaves_per_m));
	for (int k = 0; k < leaves; k++) {
		const float a = L.leaf_min_m + rng_.f() * L.leaf_span_m, b = L.leaf_min_m + rng_.f() * L.leaf_span_m;
		piece(at + Vector3(0, height * L.crown_at, 0), Vector3(a, L.leaf_thick_m, b), push + scatter(rng_, L.leaf_scatter_ms), c.leaf);
	}
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
	shell.tint = params_.tints.paint;
	const bool drop = e.kind == DamageEvent::VEHICLE_DROP; // a rider shot off: the machine falls over, it does not burn
	shell.burn_s = drop ? 0.f : rider ? K.moped_burn_s : K.car_burn_s;
	debris_.add(shell);
	for (int k = 0; k < 2; k++) { // two wheels thrown clear
		PieceSpec w;
		w.at = Vector3(e.x, e.y + K.roll_r, e.z);
		w.size = Vector3(K.roll_r, K.roll_w, K.roll_r);
		w.cylinder = true;
		w.tint = params_.tints.charred;
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
		m.y = GroundCells::surface(s, m.x, m.z) + Marks::SURFACE_LIFT_M;
		m.size = params_.pool_size * (K.pool_min + rng_.f() * K.pool_span) * (torn ? K.pool_torn : 1.f);
		m.seed = rng_.f();
		m.born = now_;
		marks_.add_blood(m);
	}
	if (!torn) return;
	const GoreLook &g = params_.gore; // a body torn apart: what was inside, thrown
	organs(Vector3(e.x, e.y + K.chest_rise_m, e.z), g.torn_organs_min + int(rng_.f() * float(g.torn_organs_span)), Vector3());
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
		case DamageEvent::GEYSER: geysers_.push_back({ e.x, e.y, e.z, now_ + double(params_.geyser_s), 0.f, false }); break;
		case DamageEvent::LEAK: geysers_.push_back({ e.x, e.y, e.z, now_ + double(e.a), 0.f, true }); break;
		case DamageEvent::SPARK: arc(Vector3(e.x, e.y, e.z), e.a); break;
		case DamageEvent::WIRE: wire(e); break;
		case DamageEvent::TREE_FELL: fell(Vector3(e.x, e.y, e.z), e.a, e.a * params_.tree.street_width_m / params_.tree.street_height_m, Vector3(e.dx, 0, e.dz)); break;
		default: break;
	}
}

} // namespace city
