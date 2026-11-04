// The effects' owner (effect_world.h): init, the frame, the clock, the flash, the fires of the damage state.
#include "render/effects/effect_world.h"

#include "core/math.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float MAX_FRAME_S = 0.1f; // a stalled frame never advances the effects by more
constexpr float MARK_SLOTS = 1400.f; // craters, soot, blood and rings drawn at once
constexpr float ROOF_FLAME_BASE_M = 2.f; // a burning building flames from this high over its ground...
constexpr float ROOF_FLAME_SPAN_M = 10.f; // ...up to this much higher
constexpr float FLAME_SPAN_SHARE = 0.3f; // a flame's size is at most this share of the building's span
constexpr float ROOF_FLAME_CAP_M = 3.f;
constexpr float SHAKE_FLOOR = 1e-3f;
constexpr float FLASH_DAMP = 2.f; // the flash dies off with the square of its life left
constexpr float LIGHT_ATTEN = 2.f;
constexpr float GEYSER_LIFT_M = 0.4f, JET_SPREAD_M = 0.1f, JET_LIFE_S = 4.2f;
} // namespace

bool EffectWorld::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	if (!params_.load(ctx.table("effects", errors), errors) || !load_looks(ctx, errors)) return false;
	const RID puffs = ctx.material("puffs", errors), debris = ctx.material("debris", errors), marks = ctx.material("marks", errors);
	if (!errors.is_empty()) return false;
	puffs_.init(scenario_, puffs, params_.pool);
	puffs_.set_wind(params_.wind_x, params_.wind_z);
	marks_.init(scenario_, marks, int(MARK_SLOTS), params_.soot_kept, params_.blood_max);
	debris_.init(scenario_, debris, params_);
	RenderingServer *rs = RenderingServer::get_singleton();
	light_ = rs->omni_light_create();
	rs->light_set_shadow(light_, false);
	rs->light_set_color(light_, params_.flash_color);
	rs->light_set_param(light_, RenderingServer::LIGHT_PARAM_ENERGY, 0.f);
	rs->light_set_param(light_, RenderingServer::LIGHT_PARAM_ATTENUATION, LIGHT_ATTEN);
	light_inst_ = rs->instance_create2(light_, scenario_);
	return true;
}

void EffectWorld::reset() {
	puffs_.reset();
	marks_.reset();
	debris_.reset();
	geysers_.clear();
	sources_.clear();
	fire_carry_.clear();
	events_.clear();
	shake_ = 0;
	blast_at_ = -1;
	flash_left_ = 0;
	craters_seen_ = ~size_t(0);
}

void EffectWorld::release() {
	RenderingServer *rs = RenderingServer::get_singleton();
	if (light_inst_.is_valid()) rs->free_rid(light_inst_);
	if (light_.is_valid()) rs->free_rid(light_);
	light_inst_ = RID();
	light_ = RID();
	puffs_.release();
	marks_.release();
	debris_.release();
}

void EffectWorld::flash(float radius) {
	flash_left_ = params_.flash_life;
	flash_energy_ = params_.flash_energy;
	flash_range_ = std::max(params_.flash_range_min, params_.flash_range_r * radius);
}

void EffectWorld::light_update(float dt) {
	RenderingServer *rs = RenderingServer::get_singleton();
	flash_left_ = std::max(0.f, flash_left_ - dt);
	const float k = flash_left_ / std::max(params_.flash_life, SHAKE_FLOOR);
	rs->light_set_param(light_, RenderingServer::LIGHT_PARAM_ENERGY, flash_energy_ * std::pow(k, FLASH_DAMP));
	rs->light_set_param(light_, RenderingServer::LIGHT_PARAM_RANGE, flash_range_);
	rs->instance_set_transform(light_inst_, Transform3D(Basis(), flash_at_));
	rs->instance_set_visible(light_inst_, flash_left_ > 0);
}

// The fires the damage state holds: flames from a few points of each burning building and a chimney of smoke.
void EffectWorld::burn_fires(const CityState &s, float dt) {
	for (const Fire &f : s.damage.fires) {
		if (uint32_t(f.building) >= s.buildings.items.size()) continue;
		const Building &b = s.buildings.items[size_t(f.building)];
		if (!b.alive) continue;
		float &carry = fire_carry_[f.building];
		carry += params_.burn_building_rate * dt;
		const float span = std::max(b.w, b.d);
		const float ground = s.terrain.height_at(b.x, b.z);
		while (carry >= 1.f) {
			carry -= 1.f;
			const float a = rng_.f() * TAU_F, r = std::sqrt(rng_.f()) * span * 0.5f;
			const float x = b.x + std::cos(a) * r, z = b.z + std::sin(a) * r, y = ground + ROOF_FLAME_BASE_M + rng_.f() * ROOF_FLAME_SPAN_M;
			puffs_.burst(x, y, z, 1, params_.flame_kind, 0.5f, std::min(span * FLAME_SPAN_SHARE, ROOF_FLAME_CAP_M), params_.flame_life);
			if (rng_.f() < params_.column_chance) puffs_.burst(x, y, z, 1, params_.smoke_kind, 0.5f, params_.smoke_size, params_.smoke_life);
		}
	}
	if (s.damage.fires.empty() && !fire_carry_.empty()) fire_carry_.clear();
}

void EffectWorld::step_geysers(float dt) {
	for (Geyser &g : geysers_) {
		const float strength = std::min(1.f, float(g.until - now_) / params_.geyser_fade);
		g.carry += params_.geyser_rate * strength * dt;
		const int n = int(g.carry);
		g.carry -= float(n);
		if (n > 0) puffs_.burst(g.x, g.y + GEYSER_LIFT_M, g.z, n, PUFF_WATER, JET_SPREAD_M, params_.geyser_size * std::max(0.25f, strength), JET_LIFE_S);
	}
	geysers_.erase(std::remove_if(geysers_.begin(), geysers_.end(), [&](const Geyser &g) { return now_ > g.until; }), geysers_.end());
}

void EffectWorld::sync(CityState &s) {
	const auto t = std::chrono::steady_clock::now();
	const float dt = started_ ? std::min(MAX_FRAME_S, std::chrono::duration<float>(t - last_).count()) : 0.f;
	started_ = true;
	last_ = t;
	now_ += dt;
	{
		std::lock_guard<std::mutex> lk(s.log.mutex);
		events_.swap(s.log.damage_events);
	}
	puffs_.update(now_); // the clock the puffs about to be born are stamped with
	for (const DamageEvent &e : events_) apply(e, s);
	played_ += events_.size();
	events_played_.swap(events_);
	events_.clear();
	if (craters_seen_ != s.damage.craters.size()) {
		craters_seen_ = s.damage.craters.size();
		marks_.set_craters(s.damage.craters, [&](float x, float z) { return s.terrain.base_height_at(x, z); });
	}
	if (ruins_seen_ != s.damage.ruins.size()) {
		// a building brought down in this frame already fell into its heap (its event); any other change (open, undo, redo) lays them all again
		const bool fell = std::any_of(events_played_.begin(), events_played_.end(), [](const DamageEvent &e) { return e.kind == DamageEvent::BUILDING_DOWN; });
		if (!fell) rebuild_ruins(s);
		ruins_seen_ = s.damage.ruins.size();
	}
	burn_fires(s, dt);
	step_geysers(dt);
	step_sources(dt);
	debris_.update(now_, dt, [&](float x, float z) { return s.terrain.height_at(x, z); }, puffs_);
	shake_ *= std::exp(-params_.shake_decay * dt);
	if (shake_ < SHAKE_FLOOR) shake_ = 0;
	light_update(dt);
	marks_.update(now_);
	puffs_.update(now_);
}

} // namespace city
