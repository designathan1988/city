// The events of a blow, played once (effect_world.h): the bomb itself (the fireball, the smoke, the dust, the shrapnel, the
// ring, the flash and the shake, the soot on the ground), the building struck and the building brought down.
#include "render/effects/effect_world.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int SOIL_BUILDING = 2; // BLAST event: the burst was on a building
constexpr int SOIL_SOIL_TINT = 0;

} // namespace

void EffectWorld::piece(const Vector3 &at, const Vector3 &size, const Vector3 &v, const Color &tint, bool cylinder, float burn) {
	PieceSpec s;
	s.at = at;
	s.size = size;
	s.velocity = v;
	s.tint = tint;
	s.cylinder = cylinder;
	s.burn_s = burn;
	debris_.add(s);
}

void EffectWorld::blast(const DamageEvent &e, const CityState &s) {
	const BlastShape &K = params_.shapes.effect_blast;
	const float radius = params_.fireball_base + params_.fireball_per_force * e.dx; // the fireball: from the force of the blow
	const int soil = int(e.c);
	for (const BurstDef &b : params_.bursts) {
		const int kind = b.kind >= 0 ? b.kind : (soil == SOIL_SOIL_TINT ? PUFF_DUST : PUFF_CONCRETE);
		puffs_.burst(e.x, e.y + b.height, e.z, b.count, kind, b.spread_m + b.spread_r * radius, b.size_m + b.size_r * radius, b.life);
	}
	for (const SourceDef &d : params_.sources) {
		Source src;
		src.x = e.x; src.y = e.y + d.height_m; src.z = e.z;
		const float big = d.size_k * radius;
		src.size = d.size_cap_m > 0.f ? std::min(d.size_cap_m, big) : big;
		src.rate = d.rate_hz;
		src.until = now_ + double(d.for_s + std::min(d.for_cap_s, d.for_k * radius));
		sources_.push_back(src);
	}
	flash_at_ = Vector3(e.x, e.y + params_.flash_height, e.z);
	flash(radius);
	shake_ = std::max(shake_, params_.shake_base + params_.shake_per_r * radius);
	blast_at_ = now_;
	Mark ring;
	ring.kind = MARK_RING;
	ring.x = e.x; ring.z = e.z;
	ring.size = radius * 2.f * params_.ring_reach_r; // across
	ring.y = Marks::top_ground(e.x, e.z, ring.size * 0.5f, [&](float x, float z) { return GroundCells::surface(s, x, z); }) + params_.ring_height;
	ring.born = now_;
	ring.life = params_.ring_life;
	marks_.ring(ring);
	// Shrapnel of what the ground was.
	const Tints &c = params_.tints; // shrapnel of the ground it tears up, in its own tints
	const Color kinds_ground[4] = { c.earth, c.earth, c.earth, c.concrete }, kinds_road[4] = { c.asphalt, c.asphalt, c.concrete, c.earth },
			kinds_building[4] = { c.concrete, c.brick, c.glass, c.concrete };
	const Color *palette = soil == SOIL_BUILDING ? kinds_building : soil == SOIL_SOIL_TINT ? kinds_ground : kinds_road;
	const int n = params_.shrap_base + int(params_.shrap_per_r * radius);
	for (int i = 0; i < n; i++) {
		const float a = rng_.f() * TAU_F, out = params_.shrap_speed_min + rng_.f() * params_.shrap_speed_span;
		const float sz = params_.shrap_size_min + rng_.f() * rng_.f() * params_.shrap_size_span;
		piece(Vector3(e.x + std::cos(a) * radius * K.shell_out_share, e.y + K.shell_lift_m, e.z + std::sin(a) * radius * K.shell_out_share),
				Vector3(sz * (1.f + rng_.f()), sz * (K.shell_flat_min + rng_.f() * K.shell_flat_span), sz * (1.f + rng_.f())),
				Vector3(std::cos(a) * out, params_.shrap_lift_min + rng_.f() * params_.shrap_lift_span, std::sin(a) * out), palette[i % 4]);
	}
	// Soot and ash kept on the ground round it.
	const int blots = std::min(params_.soot_max, params_.soot_base + int(params_.soot_per_step * radius));
	for (int k = 0; k < blots; k++) {
		const float a = rng_.f() * TAU_F, r = std::sqrt(rng_.f()) * radius * params_.soot_reach_r;
		Mark m;
		m.kind = MARK_SOOT;
		m.x = e.x + std::cos(a) * r; m.z = e.z + std::sin(a) * r;
		m.y = GroundCells::surface(s, m.x, m.z) + Marks::SURFACE_LIFT_M * K.soot_size_jitter;
		m.size = params_.soot_size_min + rng_.f() * params_.soot_size_span + radius * params_.soot_size_r;
		m.seed = rng_.f();
		marks_.add_soot(m);
	}
}

void EffectWorld::building_hit(const DamageEvent &e) {
	const BlastShape &K = params_.shapes.effect_blast;
	const int dust = K.hit_dust_min + int(e.a * K.hit_dust_per_kpa);
	puffs_.burst(e.x, e.y + K.hit_lift_m, e.z, dust, PUFF_CONCRETE, K.hit_spread_m, K.hit_size_m, K.hit_life_s);
	puffs_.burst(e.x, e.y + K.hit_lift_m, e.z, std::min(int(e.a * K.spark_count_per_kpa), int(K.hit_spark_max)), PUFF_SPARK, K.hit_spread_m, K.hit_spark_size, K.hit_spark_life);
}

void EffectWorld::building_down(const DamageEvent &e, const CityState &s) {
	const BlastShape &K = params_.shapes.effect_blast;
	// The collapse: a cloud of concrete dust that billows out, and the papers of the offices and homes over the street.
	puffs_.burst(e.x, e.y, e.z, params_.down_dust_count, PUFF_DUST, params_.down_dust_spread, params_.down_dust_size * K.down_smoke_size_scale, params_.down_dust_life);
	puffs_.burst(e.x, e.y + K.collapse_dust_high_m, e.z, params_.down_dust_count, PUFF_CONCRETE, params_.down_dust_spread, params_.down_dust_size, params_.down_dust_life);
	// The heap it left, which the building falls into: the ruin the event names (its index when it fell); the list searched by place
	// only when that one stands elsewhere now (a ruin before it cleared since, in the same frame).
	const Revised<Ruin> &ruins = s.damage.ruins;
	const auto here = [&](const Ruin &r) { return std::fabs(r.x - e.x) < K.ruin_match_m && std::fabs(r.z - e.z) < K.ruin_match_m; };
	const auto fall = [&](const Ruin &r) {
		ground_here_ = s.terrain.height_at(r.x, r.z);
		pile(r, true, true, !ruins_.add(s, r, heap_of(r), true));
	};
	if (e.u < ruins.size() && here(ruins[e.u])) fall(ruins[e.u]);
	else
		for (const Ruin &r : ruins)
			if (here(r)) fall(r);
	puffs_.burst(e.x, e.y + K.down_paper_height_m, e.z, params_.paper_count, PUFF_PAPER, params_.paper_spread, params_.paper_size, params_.paper_life);
}

// What the blast leaves burning: each source puffs flames at its rate (smaller as it dies down), now and then a chimney of dark
// smoke and a light smoke (render/blast.ts, the loop over `fires`).
void EffectWorld::step_sources(float dt) {
	for (Source &f : sources_) {
		f.carry += f.rate * dt;
		const float dying = std::min(1.f, float(f.until - now_) / params_.dying_s + params_.dying_floor);
		while (f.carry >= 1.f) {
			f.carry -= 1.f;
			puffs_.burst(f.x, f.y + params_.puff_height, f.z, 1, params_.flame_kind, f.size, (params_.flame_base + f.size * params_.flame_k) * dying, params_.flame_life);
			if (rng_.f() < params_.column_chance)
				puffs_.burst(f.x, f.y + params_.chimney_height, f.z, 1, params_.smoke_kind, params_.chimney_spread, params_.smoke_size, params_.smoke_life);
			if (rng_.f() < params_.light_chance)
				puffs_.burst(f.x, f.y + params_.light_height, f.z, 1, PUFF_EXHAUST, f.size * params_.light_spread_k, params_.light_size, params_.light_life);
		}
	}
	sources_.erase(std::remove_if(sources_.begin(), sources_.end(), [&](const Source &f) { return now_ > f.until; }), sources_.end());
}

} // namespace city
