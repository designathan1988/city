// The effects' owner (effect_world.h): init, the frame, the clock, the flash, the fires of the damage state.
#include "render/effects/effect_world.h"

#include "core/math.h"
#include "render/data_reader.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float JET_LEAST = 0.25f; // a hydrant's jet dying down keeps this share of its size (a modelling choice)
constexpr float FLASH_YIELD = 0.6f; // a weaker flash of another colour never takes the light from one brighter than this share of it
} // namespace

bool EffectWorld::init(const RenderContext &ctx, PackedStringArray &errors) {
	const WorldShape &K = params_.shapes.effect_world;
	scenario_ = ctx.scenario;
	if (!params_.load(ctx.table("effects", errors), ctx.table("aftermath", errors), ctx.table("gore", errors), errors)) return false;
	if (!ctx.catalog) {
		errors.push_back("effects without a catalogue");
		return false;
	}
	params_.fireball_base = ctx.catalog->tune.strike_scar_radius_m; // the fireball is as wide as the scar it leaves: one law
	params_.fireball_per_force = ctx.catalog->tune.strike_scar_radius_per_force;
	const RID puffs = ctx.material("puffs", errors), debris = ctx.material("debris", errors), marks = ctx.material("marks", errors), rubble = ctx.material("rubble", errors);
	if (!errors.is_empty()) return false;
	puffs_.init(scenario_, puffs, params_);
	puffs_.set_wind(params_.wind_x, params_.wind_z);
	marks_.init(scenario_, marks, int(K.mark_slots), params_.soot_kept, params_.blood_max);
	debris_.init(scenario_, debris, params_);
	heave_.init(scenario_, debris, params_.heave);
	DataReader streets{ errors, "streets.json" };
	if (!seats_.load(streets.at(ctx.table("streets", errors))["wires"])) return false;
	ropes_.init(scenario_, debris, params_.rope, seats_.colour, seats_.radius, params_.gravity);
	debris_material_ = debris;
	mounds_.init(scenario_, rubble, K.mound_slots, params_.shapes.mounds);
	const Ref<ShaderMaterial> ruin = ctx.material_objects.get("building_ruin", Variant());
	if (ruin.is_null()) errors.push_back("material ausente: building_ruin");
	ruins_.init(scenario_, ruin, params_);
	RenderingServer *rs = RenderingServer::get_singleton();
	light_ = rs->omni_light_create();
	rs->light_set_shadow(light_, false);
	rs->light_set_color(light_, params_.flash_color);
	flash_colour_ = flash_sent_ = params_.flash_color;
	rs->light_set_param(light_, RenderingServer::LIGHT_PARAM_ENERGY, 0.f);
	rs->light_set_param(light_, RenderingServer::LIGHT_PARAM_ATTENUATION, K.light_atten);
	light_inst_ = rs->instance_create2(light_, scenario_);
	return true;
}

void EffectWorld::set_wrecks(const std::vector<Ref<Mesh>> &bodies, const std::vector<Vector3> &sizes) {
	debris_.set_wrecks(scenario_, debris_material_, bodies);
	wreck_sizes_ = sizes;
}

void EffectWorld::reset() {
	ruins_.reset();
	if (people_) people_->bodies().reset();
	puffs_.reset();
	marks_.reset();
	debris_.reset();
	heave_.reset();
	ropes_.reset();
	mounds_.reset();
	geysers_.clear();
	arcs_.clear();
	sources_.clear();
	fire_carry_.clear();
	events_.clear();
	shake_ = 0;
	blast_at_ = -1;
	flash_left_ = 0;
	craters_seen_ = crater_layout_seen_ = ruins_seen_ = scars_seen_ = UNSEEN; // everything the state holds is drawn again
}

void EffectWorld::touch_terrain(const CityState &s, const std::vector<int> &rects) {
	if (rects.empty() || (marks_.craters() == 0 && heave_.craters() == 0)) return;
	const Heightfield &t = s.terrain;
	// a decal reads the ground at the nodes round each of its points (grid_sample: floor and floor + 1): a node more each side;
	// a heaved piece stands where it was laid, on the node under it and the next
	const auto moved = [&](float x, float z, float half) {
		const int x0 = t.ix_of(x - half) - 1, x1 = t.ix_of(x + half) + 2, z0 = t.ix_of(z - half) - 1, z1 = t.ix_of(z + half) + 2;
		for (size_t k = 0; k + 3 < rects.size(); k += 4)
			if (rects[k] <= x1 && rects[k + 2] >= x0 && rects[k + 1] <= z1 && rects[k + 3] >= z0) return true;
		return false;
	};
	marks_.ground_moved(moved);
	heave_.ground_moved(moved);
}

void EffectWorld::release() {
	ruins_.release();
	RenderingServer *rs = RenderingServer::get_singleton();
	if (light_inst_.is_valid()) rs->free_rid(light_inst_);
	if (light_.is_valid()) rs->free_rid(light_);
	light_inst_ = RID();
	light_ = RID();
	puffs_.release();
	marks_.release();
	mounds_.release();
	debris_.release();
	heave_.release();
	ropes_.release();
}

void EffectWorld::flash(float radius) {
	flash_at(flash_at_, params_.flash_color, params_.flash_energy, std::max(params_.flash_range_min, params_.flash_range_r * radius), params_.flash_life);
}

void EffectWorld::flash_at(const Vector3 &at, const Color &colour, float energy, float range, float life) {
	const float lit = flash_left_ > 0 ? flash_energy_ * std::pow(flash_left_ / std::max(flash_life_, params_.shapes.effect_world.shake_floor), params_.shapes.effect_world.flash_damp) : 0.f;
	const bool same = colour == flash_colour_;
	if (lit > energy * FLASH_YIELD && !same) return;
	flash_at_ = at;
	flash_colour_ = colour;
	flash_energy_ = std::max(energy, same ? lit : 0.f);
	flash_range_ = range;
	flash_life_ = life;
	flash_left_ = life;
}

void EffectWorld::light_update(float dt) {
	const WorldShape &K = params_.shapes.effect_world;
	RenderingServer *rs = RenderingServer::get_singleton();
	flash_left_ = std::max(0.f, flash_left_ - dt);
	const float k = flash_left_ / std::max(flash_life_, K.shake_floor);
	if (flash_colour_ != flash_sent_) {
		rs->light_set_color(light_, flash_colour_);
		flash_sent_ = flash_colour_;
	}
	rs->light_set_param(light_, RenderingServer::LIGHT_PARAM_ENERGY, flash_energy_ * std::pow(k, K.flash_damp));
	rs->light_set_param(light_, RenderingServer::LIGHT_PARAM_RANGE, flash_range_);
	rs->instance_set_transform(light_inst_, Transform3D(Basis(), flash_at_));
	rs->instance_set_visible(light_inst_, flash_left_ > 0);
}

// The fires the damage state holds: flames from a few points of each burning building and a chimney of smoke.
void EffectWorld::burn_fires(const CityState &s, float dt) {
	const WorldShape &K = params_.shapes.effect_world;
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
			const float x = b.x + std::cos(a) * r, z = b.z + std::sin(a) * r, y = ground + K.roof_flame_base_m + rng_.f() * K.roof_flame_span_m;
			puffs_.burst(x, y, z, 1, params_.flame_kind, 0.5f, std::min(span * K.flame_span_share, K.roof_flame_cap_m), params_.flame_life);
			if (rng_.f() < params_.column_chance) puffs_.burst(x, y, z, 1, params_.smoke_kind, 0.5f, params_.smoke_size, params_.smoke_life);
		}
	}
	if (s.damage.fires.empty() && !fire_carry_.empty()) fire_carry_.clear();
}

void EffectWorld::step_geysers(float dt) {
	const WorldShape &K = params_.shapes.effect_world;
	for (Geyser &g : geysers_) {
		const float strength = g.weak ? 1.f : std::min(1.f, float(g.until - now_) / params_.geyser_fade);
		g.carry += (g.weak ? params_.leak_rate : params_.geyser_rate) * strength * dt;
		const int n = int(g.carry);
		g.carry -= float(n);
		if (n == 0) continue;
		if (g.weak) puffs_.burst(g.x, g.y, g.z, n, PUFF_WATER, K.jet_spread_m, params_.leak_size, params_.leak_life);
		else puffs_.burst(g.x, g.y + K.geyser_lift_m, g.z, n, PUFF_WATER, K.jet_spread_m, params_.geyser_size * std::max(JET_LEAST, strength), K.jet_life_s);
	}
	geysers_.erase(std::remove_if(geysers_.begin(), geysers_.end(), [&](const Geyser &g) { return now_ > g.until; }), geysers_.end());
}

void EffectWorld::step_arcs() {
	const ArcLook &a = params_.arc;
	for (Arc &c : arcs_) {
		if (now_ < c.next) continue;
		c.next = now_ + double(a.gap_min_s + rng_.f() * a.gap_span_s);
		puffs_.burst(c.at.x, c.at.y, c.at.z, a.sparks, PUFF_SPARK, a.spread_m, a.size_m, a.life_s);
		flash_at(c.at, a.color, a.energy, a.range_m, a.flash_s);
	}
	arcs_.erase(std::remove_if(arcs_.begin(), arcs_.end(), [&](const Arc &c) { return now_ > c.until; }), arcs_.end());
}

void EffectWorld::wire(const DamageEvent &e) {
	const RopeLook &r = params_.rope;
	const bool rural = e.u != 0;
	const Vector3 arm(e.dx, 0, e.dz), out(std::cos(e.a), r.kick_lift, std::sin(e.a));
	const std::vector<float> &seats = rural ? seats_.rural : seats_.urban;
	for (float along : seats) {
		const Vector3 from = Vector3(e.x, e.y + seats_.height, e.z) + arm * along, to = Vector3(e.b, e.d + seats_.height * r.drop_share, e.c) + arm * along;
		ropes_.add(from, to, out * r.kick_m);
	}
}

void EffectWorld::sync(CityState &s, FrameAdmission &admission) {
	const WorldShape &K = params_.shapes.effect_world;
	const auto t = std::chrono::steady_clock::now();
	const float dt = started_ ? std::min(K.max_frame_s, std::chrono::duration<float>(t - last_).count()) : 0.f;
	started_ = true;
	last_ = t;
	now_ += dt;
	{
		std::lock_guard<std::mutex> lk(s.log.mutex);
		events_.swap(s.log.damage_events);
	}
	puffs_.stamp(now_); // the clock the puffs about to be born are stamped with
	for (const DamageEvent &e : events_) apply(e, s);
	played_ += events_.size();
	events_played_.swap(events_);
	events_.clear();
	const Revised<Crater> &craters = s.damage.craters;
	if (craters_seen_ != craters.revision()) {
		// only blows since (the layout held): the craters laid are still the list's first, and only the new ones are laid
		const bool grew = crater_layout_seen_ == craters.layout_revision();
		marks_.set_craters(craters.items(), grew ? marks_.craters() : 0, [&](float x, float z) { return GroundCells::laid(s, x, z); }); // none where its deck is gone
		heave_.set_craters(craters.items(), grew ? heave_.craters() : 0, s);
		craters_seen_ = craters.revision();
		crater_layout_seen_ = craters.layout_revision();
	} else if (heave_.craters() < craters.size()) {
		heave_.set_craters(craters.items(), heave_.craters(), s); // the ground under them moved: laid again on it
	}
	if (ruins_seen_ != s.damage.ruins.revision() || scars_seen_ != s.damage.scars.revision()) {
		// a building brought down in this frame already fell into its heap (its event); any other change (open, undo, redo) lays them all again
		const bool fell = std::any_of(events_played_.begin(), events_played_.end(), [](const DamageEvent &e) { return e.kind == DamageEvent::BUILDING_DOWN || e.kind == DamageEvent::BUILDING_SCAR; });
		if (!fell) rebuild_ruins(s);
		ruins_seen_ = s.damage.ruins.revision();
		scars_seen_ = s.damage.scars.revision();
	}
	burn_fires(s, dt);
	step_geysers(dt);
	step_arcs();
	step_sources(dt);
	const auto ground = [&](float x, float z) { return ground_.at(s, x, z); };
	ropes_.update(dt, ground, puffs_,
			[&](const Vector3 &at) { flash_at(at, params_.arc.color, params_.rope.flash_energy, params_.arc.range_m, params_.rope.flash_s); });
	debris_.update(now_, dt, ground, puffs_);
	ruins_.update(dt, puffs_, admission);
	if (people_) people_->bodies().update(dt, ground);
	for (const RuinPieces::Uncut &u : ruins_.take_uncut()) {
		ground_here_ = s.terrain.height_at(u.ruin.x, u.ruin.z);
		pile(u.ruin, u.falling, false, true);
	}
	shake_ *= std::exp(-params_.shake_decay * dt);
	if (shake_ < K.shake_floor) shake_ = 0;
	light_update(dt);
	marks_.update(now_);
	mounds_.update(now_);
	puffs_.flush();
}

} // namespace city
