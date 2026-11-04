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
constexpr float SPARK_COUNT_PER_KPA = 0.4f; // sparks from a building hit grow with the load it felt
constexpr int HIT_DUST_MIN = 12;
constexpr float HIT_DUST_PER_KPA = 0.5f;
constexpr float HIT_SPREAD_M = 2.f;
constexpr float HIT_SIZE_M = 2.5f;
constexpr float HIT_LIFE_S = 4.f;
constexpr float SHELL_LIFT_M = 0.3f; // shrapnel starts this high at the ground
constexpr float SHELL_OUT_SHARE = 0.2f; // and this share of the fireball radius out
constexpr float SHELL_FLAT_MIN = 0.4f, SHELL_FLAT_SPAN = 0.5f; // pieces are flattened boxes
constexpr float HIT_SPARK_MAX = 30.f;
constexpr float DOWN_PAPER_HEIGHT_M = 3.f; // the papers of the offices start this high
constexpr float DOWN_SMOKE_SIZE_SCALE = 1.f;
constexpr float COLLAPSE_DUST_HIGH_M = 6.f;
constexpr float SOOT_SIZE_JITTER = 1.f;

// The shrapnel of the ground it tears up: the soil's own tints (sRGB).
const Color ROAD_TINT = Color(0.169f, 0.169f, 0.176f);
const Color CONCRETE = Color(0.604f, 0.588f, 0.557f);
const Color SOIL_TINT = Color(0.359f, 0.276f, 0.196f);
const Color BRICK = Color(0.541f, 0.29f, 0.208f);
const Color GLASS = Color(0.624f, 0.722f, 0.769f);
constexpr float HIT_SPARK_SIZE = 0.3f, HIT_SPARK_LIFE = 1.f, HIT_LIFT_M = 1.f;
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
	const float radius = params_.fireball_k * std::cbrt(e.b); // the fireball: from the charge
	const int soil = int(e.c);
	for (const BurstDef &b : params_.bursts) {
		const int kind = b.kind >= 0 ? b.kind : (soil == SOIL_SOIL_TINT ? PUFF_DUST : PUFF_CONCRETE);
		puffs_.burst(e.x, e.y + b.height, e.z, b.count, kind, b.spread_m + b.spread_r * radius, b.size_m + b.size_r * radius, b.life);
	}
	flash_at_ = Vector3(e.x, e.y + params_.flash_height, e.z);
	flash(radius);
	shake_ = std::max(shake_, params_.shake_base + params_.shake_per_r * radius);
	Mark ring;
	ring.kind = MARK_RING;
	ring.x = e.x; ring.z = e.z; ring.y = s.terrain.height_at(e.x, e.z) + params_.ring_height;
	ring.size = e.a * 2.f; // across: the damage reach
	ring.born = now_;
	ring.life = params_.ring_life;
	marks_.ring(ring);
	// Shrapnel of what the ground was.
	const Color kinds_ground[4] = { SOIL_TINT, SOIL_TINT, SOIL_TINT, CONCRETE }, kinds_road[4] = { ROAD_TINT, ROAD_TINT, CONCRETE, SOIL_TINT },
			kinds_building[4] = { CONCRETE, BRICK, GLASS, CONCRETE };
	const Color *palette = soil == SOIL_BUILDING ? kinds_building : soil == SOIL_SOIL_TINT ? kinds_ground : kinds_road;
	const int n = params_.shrap_base + int(params_.shrap_per_r * radius);
	for (int i = 0; i < n; i++) {
		const float a = rng_.f() * TAU_F, out = params_.shrap_speed_min + rng_.f() * params_.shrap_speed_span;
		const float sz = params_.shrap_size_min + rng_.f() * rng_.f() * params_.shrap_size_span;
		piece(Vector3(e.x + std::cos(a) * radius * SHELL_OUT_SHARE, e.y + SHELL_LIFT_M, e.z + std::sin(a) * radius * SHELL_OUT_SHARE),
				Vector3(sz * (1.f + rng_.f()), sz * (SHELL_FLAT_MIN + rng_.f() * SHELL_FLAT_SPAN), sz * (1.f + rng_.f())),
				Vector3(std::cos(a) * out, params_.shrap_lift_min + rng_.f() * params_.shrap_lift_span, std::sin(a) * out), palette[i % 4]);
	}
	// Soot and ash kept on the ground round it.
	const int blots = std::min(params_.soot_max, params_.soot_base + int(params_.soot_per_step * radius));
	for (int k = 0; k < blots; k++) {
		const float a = rng_.f() * TAU_F, r = std::sqrt(rng_.f()) * radius * params_.soot_reach_r;
		Mark m;
		m.kind = MARK_SOOT;
		m.x = e.x + std::cos(a) * r; m.z = e.z + std::sin(a) * r;
		m.y = s.terrain.height_at(m.x, m.z) + Marks::SURFACE_LIFT_M * SOOT_SIZE_JITTER;
		m.size = params_.soot_size_min + rng_.f() * params_.soot_size_span + radius * params_.soot_size_r;
		m.seed = rng_.f();
		marks_.add_soot(m);
	}
}

void EffectWorld::building_hit(const DamageEvent &e) {
	const int dust = HIT_DUST_MIN + int(e.a * HIT_DUST_PER_KPA);
	puffs_.burst(e.x, e.y + HIT_LIFT_M, e.z, dust, PUFF_CONCRETE, HIT_SPREAD_M, HIT_SIZE_M, HIT_LIFE_S);
	puffs_.burst(e.x, e.y + HIT_LIFT_M, e.z, std::min(int(e.a * SPARK_COUNT_PER_KPA), int(HIT_SPARK_MAX)), PUFF_SPARK, HIT_SPREAD_M, HIT_SPARK_SIZE, HIT_SPARK_LIFE);
}

void EffectWorld::building_down(const DamageEvent &e) {
	// The collapse: a cloud of concrete dust that billows out, and the papers of the offices and homes over the street.
	puffs_.burst(e.x, e.y, e.z, params_.down_dust_count, PUFF_DUST, params_.down_dust_spread, params_.down_dust_size * DOWN_SMOKE_SIZE_SCALE, params_.down_dust_life);
	puffs_.burst(e.x, e.y + COLLAPSE_DUST_HIGH_M, e.z, params_.down_dust_count, PUFF_CONCRETE, params_.down_dust_spread, params_.down_dust_size, params_.down_dust_life);
	puffs_.burst(e.x, e.y + DOWN_PAPER_HEIGHT_M, e.z, params_.paper_count, PUFF_PAPER, params_.paper_spread, params_.paper_size, params_.paper_life);
}

} // namespace city
