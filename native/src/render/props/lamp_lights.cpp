// The nearest lamps the view can reach, each with a spot of its own (lamp_lights.h).
#include "render/props/lamp_lights.h"

#include "core/grid.h"
#include "core/math.h"
#include "render/render_layers.h"

#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/transform3d.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float REACH_LAG_S = 0.4f; // the lit radius follows the count of lamps this slowly, or a lamp at its rim pops
constexpr float SENT_SHARE = 0.01f; // of a lamp's energy or shadow: smaller changes are not sent
constexpr float PLACE_GRAIN = 16.f; // of a metre: the grid a lamp's key is read on
constexpr float PUBLISHED_M = 0.25f; // a smaller change of the lit radius is not published
constexpr float CHUNK_HALF_DIAGONAL = 0.7072f;
constexpr double STRAIGHT_DOWN = -PI_D / 2; // a spot shines along its -Z
constexpr int HALF_KEY_BITS = 32;
constexpr float LEAST_SPAN = 1e-3f;
constexpr uint32_t CASTERS = LAYER_FURNITURE | LAYER_VEHICLES | LAYER_PEOPLE; // a wall in every lamp's map is most of the cost

const StringName &lit_name() {
	static const StringName name("lamp_lit");
	return name;
}

uint64_t lamp_key(const Vector3 &at) {
	const uint32_t x = uint32_t(int32_t(std::lround(at.x * PLACE_GRAIN))), z = uint32_t(int32_t(std::lround(at.z * PLACE_GRAIN)));
	return (uint64_t(x) << HALF_KEY_BITS | z) + 1;
}

float ease(float from, float to, float x) {
	const float t = std::clamp((x - from) / std::max(to - from, LEAST_SPAN), 0.f, 1.f);
	return t * t * (3.f - 2.f * t);
}

float flat_distance(const Vector3 &a, const Vector3 &b) { return std::hypot(a.x - b.x, a.z - b.z); }
} // namespace

LampLightLook LampLightLook::of(const Tuning &t, const Color &glass) {
	LampLightLook l;
	l.color = glass;
	l.energy = float(t.render_lamp_energy);
	l.range = float(t.render_lamp_range_m);
	l.angle_deg = float(t.render_lamp_cone_deg);
	l.cone_fall = float(t.render_lamp_cone_fall);
	l.decay = float(t.render_lamp_decay);
	l.specular = float(t.render_lamp_specular);
	l.reach = float(t.render_lamp_reach_m);
	l.fade_from = float(t.render_lamp_gives_way_from);
	l.cast_reach = float(t.render_lamp_cast_reach_m);
	return l;
}

void LampLights::init(const RID &scenario, const LampLightLook &look) {
	scenario_ = scenario;
	look_ = look;
	publish(Vector2());
}

LampLights::Slot LampLights::make_slot() const {
	RenderingServer *rs = RenderingServer::get_singleton();
	Slot s;
	s.light = rs->spot_light_create();
	rs->light_set_color(s.light, look_.color);
	rs->light_set_param(s.light, RenderingServer::LIGHT_PARAM_RANGE, look_.range);
	rs->light_set_param(s.light, RenderingServer::LIGHT_PARAM_ATTENUATION, look_.decay);
	rs->light_set_param(s.light, RenderingServer::LIGHT_PARAM_SPOT_ANGLE, look_.angle_deg);
	rs->light_set_param(s.light, RenderingServer::LIGHT_PARAM_SPOT_ATTENUATION, look_.cone_fall);
	rs->light_set_param(s.light, RenderingServer::LIGHT_PARAM_SPECULAR, look_.specular);
	rs->light_set_param(s.light, RenderingServer::LIGHT_PARAM_ENERGY, 0);
	rs->light_set_shadow(s.light, false);
	rs->light_set_shadow_caster_mask(s.light, CASTERS);
	s.instance = rs->instance_create2(s.light, scenario_);
	rs->instance_set_layer_mask(s.instance, LAYER_FURNITURE);
	rs->instance_set_visible(s.instance, false);
	return s;
}

void LampLights::free_slot(Slot &s) const {
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->free_rid(s.instance);
	rs->free_rid(s.light);
	s = Slot();
}

// As many lights as the tier pays for, made when they are first asked for: a reset frees them all.
void LampLights::fit() {
	while (int(slots_.size()) > budget_) {
		free_slot(slots_.back());
		slots_.pop_back();
	}
	while (int(slots_.size()) < budget_) slots_.push_back(make_slot());
}

void LampLights::reset() {
	chunks_.clear();
	wanted_.clear();
	for (Slot &s : slots_) free_slot(s);
	slots_.clear();
	lit_ = 0;
}

void LampLights::set_chunk(int64_t key, std::vector<Vector3> &&lamps) {
	if (lamps.empty()) chunks_.erase(key);
	else chunks_[key] = std::move(lamps);
}

void LampLights::drop_chunk(int64_t key) { chunks_.erase(key); }

// The nearest lamps whose light can reach the view, and how far they go. A lamp behind the eye still counts when its light lies before it.
void LampLights::pick(const Vector3 &eye, const ViewCull &view) {
	wanted_.clear();
	const float chunk_pad = float(CHUNK) * CHUNK_HALF_DIAGONAL, half = float(CHUNK) * 0.5f;
	for (const auto &kv : chunks_) {
		const Vector3 middle(float(key_x(kv.first)) * float(CHUNK) + half, eye.y, float(key_y(kv.first)) * float(CHUNK) + half);
		if (flat_distance(middle, eye) > look_.reach + chunk_pad) continue;
		for (const Vector3 &at : kv.second) {
			const float far = flat_distance(at, eye);
			if (far <= look_.reach && view.sees(at, look_.range)) wanted_.push_back({ far, at.distance_to(eye), lamp_key(at), at });
		}
	}
	const auto nearer = [](const Wanted &a, const Wanted &b) { return a.eye_m < b.eye_m; };
	target_ = look_.reach;
	if (wanted_.size() > slots_.size()) {
		std::nth_element(wanted_.begin(), wanted_.begin() + std::ptrdiff_t(slots_.size()), wanted_.end(), nearer);
		target_ = wanted_[slots_.size()].eye_m;
		wanted_.resize(slots_.size());
	}
	cast_target_ = 0;
	if (cast_budget_ > 0) {
		cast_target_ = look_.cast_reach;
		if (int(wanted_.size()) > cast_budget_) {
			std::nth_element(wanted_.begin(), wanted_.begin() + cast_budget_, wanted_.end(), [](const Wanted &a, const Wanted &b) { return a.sight_m < b.sight_m; });
			cast_target_ = std::min(cast_target_, wanted_[size_t(cast_budget_)].sight_m);
		}
	}
}

// A light stays on the lamp it has; those whose lamp left take the lamps that came.
void LampLights::place() {
	RenderingServer *rs = RenderingServer::get_singleton();
	std::sort(wanted_.begin(), wanted_.end(), [](const Wanted &a, const Wanted &b) { return a.key < b.key; });
	std::vector<char> taken(wanted_.size(), 0);
	std::vector<Slot *> open;
	for (Slot &s : slots_) {
		const auto it = std::lower_bound(wanted_.begin(), wanted_.end(), s.lamp, [](const Wanted &w, uint64_t k) { return w.key < k; });
		if (s.lamp != 0 && it != wanted_.end() && it->key == s.lamp) taken[size_t(it - wanted_.begin())] = 1;
		else open.push_back(&s);
	}
	const Basis down(Vector3(1, 0, 0), real_t(STRAIGHT_DOWN));
	size_t next = 0;
	for (size_t i = 0; i < wanted_.size(); i++) {
		if (taken[i]) continue;
		Slot &s = *open[next++];
		s.lamp = wanted_[i].key;
		rs->instance_set_transform(s.instance, Transform3D(down, wanted_[i].at));
		rs->instance_set_visible(s.instance, true);
	}
	for (; next < open.size(); next++) {
		if (open[next]->lamp != 0) rs->instance_set_visible(open[next]->instance, false);
		open[next]->lamp = 0;
		open[next]->energy = -1;
		if (open[next]->casts) rs->light_set_shadow(open[next]->light, false);
		open[next]->casts = false;
	}
}

void LampLights::shine() {
	RenderingServer *rs = RenderingServer::get_singleton();
	const float from = reach_ * look_.fade_from;
	lit_ = 0;
	for (Slot &s : slots_) {
		if (s.lamp == 0) continue;
		const auto it = std::lower_bound(wanted_.begin(), wanted_.end(), s.lamp, [](const Wanted &w, uint64_t k) { return w.key < k; });
		const float e = look_.energy * night_ * (1.f - ease(from, reach_, it->eye_m));
		lit_ += e > 0 ? 1 : 0;
		cast(s, it->sight_m);
		if (std::fabs(e - s.energy) <= look_.energy * SENT_SHARE && (e > 0) == (s.energy > 0)) continue;
		s.energy = e;
		rs->light_set_param(s.light, RenderingServer::LIGHT_PARAM_ENERGY, e);
	}
}

// The nearest few cast: what stands in the street has a shadow under it. A lamp that stops being one of them lets its shadow fade first.
void LampLights::cast(Slot &s, float sight_m) {
	RenderingServer *rs = RenderingServer::get_singleton();
	const bool casts = sight_m < cast_reach_;
	if (casts != s.casts) {
		s.casts = casts;
		rs->light_set_shadow(s.light, casts);
		s.opacity = -1;
	}
	if (!casts) return;
	const float o = 1.f - ease(cast_reach_ * look_.fade_from, cast_reach_, sight_m);
	if (std::fabs(o - s.opacity) <= SENT_SHARE) return;
	s.opacity = o;
	rs->light_set_param(s.light, RenderingServer::LIGHT_PARAM_SHADOW_OPACITY, o);
}

void LampLights::publish(const Vector2 &lit) {
	if (std::fabs(lit.x - published_.x) < PUBLISHED_M && std::fabs(lit.y - published_.y) < PUBLISHED_M) return;
	published_ = lit;
	RenderingServer::get_singleton()->global_shader_parameter_set(lit_name(), lit);
}

void LampLights::update(const Vector3 &eye, const ViewCull &view, float frame_s) {
	if (budget_ == 0 || night_ <= 0) {
		if (lit_ > 0 || !wanted_.empty()) {
			wanted_.clear();
			place();
			lit_ = 0;
		}
		fit();
		reach_ = 0;
		publish(Vector2());
		return;
	}
	fit();
	pick(eye, view);
	place();
	const float follow = 1.f - std::exp(-frame_s / REACH_LAG_S);
	reach_ += (target_ - reach_) * follow;
	cast_reach_ += (cast_target_ - cast_reach_) * follow;
	shine();
	publish(Vector2(reach_ * look_.fade_from, reach_));
}

} // namespace city
