// The torn wires of felled poles (wires.h).
#include "render/effects/wires.h"

#include "core/shader_contracts/effects.h"

#include <godot_cpp/classes/box_mesh.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float FALL_MS2 = 9.81f; // gravity at the surface (9.80665 standard), to three figures
constexpr float NOTHING_M = 1e-6f; // a length this short has no direction
constexpr int RGB = 3; // a tint's channels
} // namespace

void Wires::init(const RID &scenario, const RID &material, const EffectParams &params) {
	params_ = params;
	const WiresShape &K = params.shapes.wires;
	Ref<BoxMesh> box;
	box.instantiate();
	box->set_size(Vector3(1, 1, 1));
	links_.init_shaped(scenario, box, material, K.most * std::max(1, K.points - 1), true, box->get_aabb(), 0.f);
	links_.count(0);
	links_.shown(false);
}

void Wires::reset() {
	ropes_.clear();
	dirty_ = true;
}

void Wires::release() { links_.release(); }

void Wires::add(const Vector3 &pin, const Vector3 &loose, const Vector3 &kick) {
	const WiresShape &K = params_.shapes.wires;
	const int n = std::max(2, K.points);
	Rope r;
	r.pin = pin;
	for (int k = 0; k < n; k++) { // along the span, the loose end moving fastest: it is thrown, the pinned end held
		const float t = float(k) / float(n - 1);
		const Vector3 v = pin.lerp(loose, t);
		r.p.push_back(v);
		r.o.push_back(v - kick * (t * K.step_s));
	}
	r.seg = pin.distance_to(loose) / float(n - 1) * K.slack;
	r.sparks = K.sparks_min_s + rng_.f() * K.sparks_span_s;
	ropes_.push_back(std::move(r));
	if (int(ropes_.size()) > K.most) ropes_.pop_front();
	dirty_ = true;
}

bool Wires::moving() const {
	return std::any_of(ropes_.begin(), ropes_.end(), [&](const Rope &r) { return r.age < params_.shapes.wires.life_s; });
}

void Wires::step(Rope &r, float dt, const std::function<float(float, float)> &ground_of) {
	const WiresShape &K = params_.shapes.wires;
	for (size_t k = 0; k < r.p.size(); k++) {
		const Vector3 v = (r.p[k] - r.o[k]) * K.damp;
		r.o[k] = r.p[k];
		r.p[k] += v + Vector3(0, -FALL_MS2 * dt * dt, 0);
	}
	for (int it = 0; it < K.iterations; it++) {
		r.p[0] = r.pin;
		for (size_t k = 1; k < r.p.size(); k++) { // a wire does not stretch; slack, it is free
			const Vector3 d = r.p[k] - r.p[k - 1];
			const float l = std::max(d.length(), NOTHING_M);
			if (l <= r.seg) continue;
			const float diff = (l - r.seg) / l;
			if (k == 1) r.p[k] -= d * diff;
			else {
				r.p[k - 1] += d * (diff * 0.5f);
				r.p[k] -= d * (diff * 0.5f);
			}
		}
		for (size_t k = 1; k < r.p.size(); k++) {
			const float g = ground_of(r.p[k].x, r.p[k].z) + K.ground_lift_m;
			if (r.p[k].y >= g) continue;
			r.p[k].y = g;
			r.o[k].x = r.p[k].x - (r.p[k].x - r.o[k].x) * K.ground_keep;
			r.o[k].z = r.p[k].z - (r.p[k].z - r.o[k].z) * K.ground_keep;
		}
	}
}

void Wires::update(float dt, const std::function<float(float, float)> &ground_of, Puffs &puffs) {
	const WiresShape &K = params_.shapes.wires;
	if (ropes_.empty()) {
		if (dirty_) flush();
		return;
	}
	clock_ += dt;
	bool moved = false;
	while (clock_ >= K.step_s) {
		clock_ -= K.step_s;
		for (Rope &r : ropes_)
			if (r.age < K.life_s) {
				step(r, K.step_s, ground_of);
				moved = true;
			}
	}
	for (Rope &r : ropes_) {
		r.age += dt;
		const Vector3 &end = r.p.back(); // the loose end live on the ground: sparks for a while
		if (r.age >= r.sparks || end.y > ground_of(end.x, end.z) + K.spark_near_m) continue;
		r.carry += K.spark_hz * dt;
		while (r.carry >= 1.f) {
			r.carry -= 1.f;
			puffs.burst(end.x, end.y, end.z, K.spark_count, PUFF_SPARK, K.spark_spread_m, K.spark_size_m, K.spark_life_s);
		}
	}
	if (moved || dirty_) flush();
}

void Wires::flush() {
	const WiresShape &K = params_.shapes.wires;
	const Color tint = K.tint.size() >= size_t(RGB) ? Color(K.tint[0], K.tint[1], K.tint[2]) : Color();
	links_.clear_bounds();
	int n = 0;
	for (const Rope &r : ropes_)
		for (size_t k = 1; k < r.p.size(); k++) links_.put_link(n++, r.p[k - 1], r.p[k], K.thick_m, tint.r, tint.g, tint.b, DEBRIS_PLAIN);
	links_.count(n);
	links_.shown(n > 0);
	if (n > 0) links_.upload();
	dirty_ = false;
}

} // namespace city
