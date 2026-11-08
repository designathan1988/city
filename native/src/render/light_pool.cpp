// The nearest of a list, each with a spot of its own (light_pool.h).
#include "render/light_pool.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float REACH_LAG_S = 0.4f; // the lit radius follows the count of lights this slowly, or one at its rim pops
constexpr float SENT_SHARE = 0.01f; // of a light's energy or shadow: smaller changes are not sent
constexpr float LEAST_SPAN = 1e-3f; // a span this short has no inside: the least a share divides by

float ease(float from, float to, float x) {
	const float t = std::clamp((x - from) / std::max(to - from, LEAST_SPAN), 0.f, 1.f);
	return t * t * (3.f - 2.f * t);
}

std::vector<LightWanted>::const_iterator find_holder(const std::vector<LightWanted> &list, uint64_t key) {
	return std::lower_bound(list.begin(), list.end(), key, [](const LightWanted &w, uint64_t k) { return w.key < k; });
}
} // namespace

void LightPool::init(const RID &scenario, const LightLook &look) {
	scenario_ = scenario;
	look_ = look;
}

LightPool::Slot LightPool::make_slot() const {
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
	rs->light_set_shadow_caster_mask(s.light, look_.casters);
	s.instance = rs->instance_create2(s.light, scenario_);
	rs->instance_set_layer_mask(s.instance, look_.layer);
	rs->instance_set_visible(s.instance, false);
	return s;
}

void LightPool::free_slot(Slot &s) const {
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->free_rid(s.instance);
	rs->free_rid(s.light);
	s = Slot();
}

// As many lights as the tier pays for, made when they are first asked for: a reset frees them all.
void LightPool::fit() {
	while (int(slots_.size()) > budget_) {
		free_slot(slots_.back());
		slots_.pop_back();
	}
	while (int(slots_.size()) < budget_) slots_.push_back(make_slot());
}

void LightPool::reset() {
	for (Slot &s : slots_) free_slot(s);
	slots_.clear();
	lit_ = 0;
}

// The nearest ones, and how far they go.
void LightPool::pick(std::vector<LightWanted> &list, float most_m) {
	target_ = most_m;
	if (list.size() > slots_.size()) {
		std::nth_element(list.begin(), list.begin() + std::ptrdiff_t(slots_.size()), list.end(), [](const LightWanted &a, const LightWanted &b) { return a.eye_m < b.eye_m; });
		target_ = list[slots_.size()].eye_m;
		list.resize(slots_.size());
	}
	cast_target_ = 0;
	if (cast_budget_ > 0) {
		cast_target_ = look_.cast_reach;
		if (int(list.size()) > cast_budget_) {
			std::nth_element(list.begin(), list.begin() + cast_budget_, list.end(), [](const LightWanted &a, const LightWanted &b) { return a.sight_m < b.sight_m; });
			cast_target_ = std::min(cast_target_, list[size_t(cast_budget_)].sight_m);
		}
	}
	std::sort(list.begin(), list.end(), [](const LightWanted &a, const LightWanted &b) { return a.key < b.key; });
}

// A light stays with the one it has; those whose holder left take the ones that came.
void LightPool::place(const std::vector<LightWanted> &list, bool moving) {
	RenderingServer *rs = RenderingServer::get_singleton();
	std::vector<char> taken(list.size(), 0);
	std::vector<Slot *> open;
	for (Slot &s : slots_) {
		const auto it = find_holder(list, s.holder);
		if (s.holder == 0 || it == list.end() || it->key != s.holder) {
			open.push_back(&s);
			continue;
		}
		taken[size_t(it - list.begin())] = 1;
		if (moving) rs->instance_set_transform(s.instance, it->at);
	}
	size_t next = 0;
	for (size_t i = 0; i < list.size(); i++) {
		if (taken[i]) continue;
		Slot &s = *open[next++];
		s.holder = list[i].key;
		rs->instance_set_transform(s.instance, list[i].at);
		rs->instance_set_visible(s.instance, true);
	}
	for (; next < open.size(); next++) {
		Slot &s = *open[next];
		if (s.holder != 0) rs->instance_set_visible(s.instance, false);
		if (s.casts) rs->light_set_shadow(s.light, false);
		s.holder = 0;
		s.energy = -1;
		s.casts = false;
	}
}

void LightPool::shine(const std::vector<LightWanted> &list) {
	RenderingServer *rs = RenderingServer::get_singleton();
	const float from = reach_ * look_.fade_from;
	lit_ = 0;
	for (Slot &s : slots_) {
		if (s.holder == 0) continue;
		const auto it = find_holder(list, s.holder);
		const float e = look_.energy * night_ * (1.f - ease(from, reach_, it->eye_m));
		lit_ += e > 0 ? 1 : 0;
		cast(s, it->sight_m);
		if (std::fabs(e - s.energy) <= look_.energy * SENT_SHARE && (e > 0) == (s.energy > 0)) continue;
		s.energy = e;
		rs->light_set_param(s.light, RenderingServer::LIGHT_PARAM_ENERGY, e);
	}
}

// The nearest few cast: what stands under them has a shadow. One that stops being of them lets its shadow fade first.
void LightPool::cast(Slot &s, float sight_m) {
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

void LightPool::put_out() {
	if (lit_ > 0) place({}, false);
	lit_ = 0;
	reach_ = 0;
}

void LightPool::update(std::vector<LightWanted> &list, float most_m, float frame_s, bool moving) {
	fit();
	if (!wanted()) {
		put_out();
		return;
	}
	pick(list, most_m);
	place(list, moving);
	const float follow = 1.f - std::exp(-frame_s / REACH_LAG_S);
	reach_ += (target_ - reach_) * follow;
	cast_reach_ += (cast_target_ - cast_reach_) * follow;
	shine(list);
}

Vector2 LightPool::lit_span() const { return Vector2(reach_ * look_.fade_from, reach_); }

} // namespace city
