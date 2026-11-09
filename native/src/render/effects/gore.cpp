// The guts and bones of an opened body (gore.h).
#include "render/effects/gore.h"

#include "core/shader_contracts/effects.h"

#include <godot_cpp/classes/cylinder_mesh.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int CYL_SIDES = 7; // a gut's and a bone's sides (gore.ts CylinderGeometry(1, 1, 1, 7))
constexpr float FALL_MS2 = 9.81f; // gravity at the surface (9.80665 standard), to three figures
constexpr float NOTHING_M = 1e-6f; // a length this short has no direction
constexpr int RGB = 3; // a tint's channels in the data's flat lists

Color tint_at(const std::vector<float> &list, int k) {
	const size_t i = size_t(k) * RGB;
	return i + 2 < list.size() ? Color(list[i], list[i + 1], list[i + 2]) : Color(1, 1, 1);
}
} // namespace

void Gore::init(const RID &scenario, const RID &material, const EffectParams &params) {
	params_ = params;
	const GoreShape &K = params.shapes.gore;
	Ref<CylinderMesh> cyl;
	cyl.instantiate();
	cyl->set_height(1.f);
	cyl->set_top_radius(1.f);
	cyl->set_bottom_radius(1.f);
	cyl->set_radial_segments(CYL_SIDES);
	guts_.init_shaped(scenario, cyl, material, K.most_segments, true, cyl->get_aabb(), 0.f);
	bones_.init_shaped(scenario, cyl, material, K.most_bones, true, cyl->get_aabb(), 0.f);
	cuffs_.init_shaped(scenario, cyl, material, K.most_bones, true, cyl->get_aabb(), 0.f);
	for (MmLayer *l : { &guts_, &bones_, &cuffs_ }) {
		l->count(0);
		l->shown(false);
	}
}

void Gore::reset() {
	chains_.clear();
	stubs_.clear();
	dirty_ = true;
}

void Gore::release() {
	guts_.release();
	bones_.release();
	cuffs_.release();
}

void Gore::spill(uint32_t body, int joint, const Vector3 &at, const Vector3 &kick, Rng &rng) {
	const GoreShape &K = params_.shapes.gore;
	const int links = std::max(2, K.links);
	while (!chains_.empty() && int(chains_.size() + 1) * (links - 1) > K.most_segments) chains_.pop_front(); // the oldest goes
	Chain c;
	c.body = body;
	c.joint = joint;
	c.seg = K.seg_min_m + rng.f() * K.seg_span_m;
	c.radius = K.radius_min_m + rng.f() * K.radius_span_m;
	c.hold = rng.f() < K.hold_chance ? K.hold_min_s + rng.f() * K.hold_span_s : -1.f; // below 0: it holds
	const int tints = std::max(1, int(K.gut_tints.size()) / RGB);
	c.tint = tint_at(K.gut_tints, std::min(tints - 1, int(rng.f() * float(tints))));
	for (int i = 0; i < links; i++) { // coiled a little out of the wound, the way it was packed
		const float a = float(i) * K.coil_turn;
		const Vector3 v = at + Vector3(std::cos(a) * K.coil_m, -float(i) * c.seg * K.coil_drop, std::sin(a) * K.coil_m);
		c.p.push_back(v);
		c.o.push_back(v - kick * (K.step_s * float(i) / float(links)));
	}
	chains_.push_back(std::move(c));
	dirty_ = true;
}

void Gore::bone(uint32_t body, int from, int at, float len) {
	const GoreShape &K = params_.shapes.gore;
	if (int(stubs_.size()) >= K.most_bones) stubs_.pop_front();
	stubs_.push_back({ body, from, at, len });
	dirty_ = true;
}

bool Gore::moving() const {
	return std::any_of(chains_.begin(), chains_.end(), [&](const Chain &c) { return c.held || c.still <= params_.shapes.gore.still_s; });
}

void Gore::cylinder(MmLayer &layer, int slot, const Vector3 &a, const Vector3 &b, float r, const Color &tint) {
	layer.put_link(slot, a, b, r, tint.r, tint.g, tint.b, DEBRIS_PLAIN);
}

void Gore::update(float dt, const std::function<float(float, float)> &ground_of, const Anchor &anchor) {
	const GoreShape &K = params_.shapes.gore;
	const float step = std::min(K.step_most_s, std::max(0.f, dt));
	for (Chain &c : chains_) {
		c.age += step;
		Vector3 held_at;
		if (c.held && ((c.hold >= 0.f && c.age > c.hold) || !anchor(c.body, c.joint, held_at))) c.held = false;
		if (!c.held && c.still > K.still_s) continue;
		float moved = 0;
		for (size_t i = 0; i < c.p.size(); i++) {
			const Vector3 v = (c.p[i] - c.o[i]) * K.damp;
			c.o[i] = c.p[i];
			c.p[i] += v + Vector3(0, -FALL_MS2 * step * step, 0);
			moved = std::max(moved, std::fabs(v.x) + std::fabs(v.y) + std::fabs(v.z));
		}
		for (int it = 0; it < K.iterations; it++) {
			if (c.held) c.p[0] = held_at;
			for (size_t i = 1; i < c.p.size(); i++) {
				const Vector3 d = c.p[i] - c.p[i - 1];
				const float l = std::max(d.length(), NOTHING_M), diff = (l - c.seg) / l;
				if (i == 1 && c.held) c.p[i] -= d * diff;
				else {
					c.p[i - 1] += d * (diff * 0.5f);
					c.p[i] -= d * (diff * 0.5f);
				}
			}
			for (size_t i = 0; i < c.p.size(); i++) {
				const float g = ground_of(c.p[i].x, c.p[i].z) + c.radius;
				if (c.p[i].y >= g) continue;
				c.p[i].y = g;
				c.o[i].x = c.p[i].x - (c.p[i].x - c.o[i].x) * K.ground_keep;
				c.o[i].z = c.p[i].z - (c.p[i].z - c.o[i].z) * K.ground_keep;
			}
		}
		c.still = moved < K.still_m ? c.still + step : 0.f;
		dirty_ = true;
	}
	if (!stubs_.empty()) dirty_ = true; // a bone follows its body
	if (!dirty_) return;
	dirty_ = false;
	guts_.clear_bounds();
	int n = 0;
	for (const Chain &c : chains_)
		for (size_t i = 1; i < c.p.size() && n < K.most_segments; i++) cylinder(guts_, n++, c.p[i - 1], c.p[i], c.radius, c.tint);
	bones_.clear_bounds();
	cuffs_.clear_bounds();
	int b = 0;
	const Color bone_tint = tint_at(K.bone_tint, 0), cuff_tint = tint_at(K.cuff_tint, 0);
	for (auto it = stubs_.begin(); it != stubs_.end();) {
		Vector3 from, at;
		if (!anchor(it->body, it->from, from) || !anchor(it->body, it->at, at)) { // the body is gone: its bone with it
			it = stubs_.erase(it);
			continue;
		}
		const Vector3 d = at - from;
		const float l = d.length();
		if (l > NOTHING_M && b < K.most_bones) {
			const Vector3 dir = d / l;
			cylinder(bones_, b, at, at + dir * it->len, K.bone_r_m, bone_tint);
			cylinder(cuffs_, b, at, at + dir * K.cuff_len_m, K.cuff_r_m, cuff_tint); // the raw flesh round it where it leaves the stump
			b++;
		}
		++it;
	}
	for (auto &[layer, count] : { std::pair<MmLayer *, int>{ &guts_, n }, { &bones_, b }, { &cuffs_, b } }) {
		layer->count(count);
		layer->shown(count > 0);
		if (count > 0) layer->upload();
	}
}

} // namespace city
