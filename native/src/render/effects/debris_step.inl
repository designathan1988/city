// The rigid step of one piece (debris.h), a template over the ground lookup: included at the end of debris.h.
#pragma once

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float SPIN_EPS = 1e-6f; // an angular velocity this small turns nothing
constexpr float SLOW_BASE_M = 0.15f; // Box2D's rest test: the farthest point moving slower than this plus a share of its reach
constexpr float SLOW_REACH = 0.55f;
constexpr float LOW_CLEAR_M = 0.05f; // a corner this close over the ground still counts as resting on it
constexpr float AIR_DRAG_GROUND = 0.995f; // horizontal speed kept per step while touching
} // namespace

template <typename F>
void Debris::step(Piece &b, F &&ground_of) {
	b.v.y -= params_.gravity * STEP_S;
	b.p += b.v * STEP_S;
	const float angle = b.w.length() * STEP_S;
	if (angle > SPIN_EPS) b.q = (godot::Quaternion(b.w.normalized(), angle) * b.q).normalized();
	bool touching = false;
	float deepest = 0, lowest = 1e9f;
	for (const godot::Vector3 &c : b.corners) {
		const godot::Vector3 r = b.q.xform(c);
		const godot::Vector3 wp = b.p + r;
		const float pen = ground_of(wp.x, wp.z) - wp.y;
		lowest = std::min(lowest, -pen);
		if (pen <= 0) continue;
		touching = true;
		deepest = std::max(deepest, pen);
		const godot::Vector3 vp = b.v + b.w.cross(r);
		if (vp.y >= 0) continue;
		const float k = 1.f + b.inv_inertia * (r.x * r.x + r.z * r.z);
		const float j = -(1.f + params_.restitution) * vp.y / k;
		b.v.y += j;
		b.w += godot::Vector3(-r.z, 0, r.x) * (b.inv_inertia * j);
		const float tl = std::hypot(vp.x, vp.z);
		if (tl <= SPIN_EPS) continue;
		const float f = std::min(tl / k, params_.friction * j);
		const float fx = -vp.x / tl * f, fz = -vp.z / tl * f;
		b.v.x += fx;
		b.v.z += fz;
		b.w += godot::Vector3(r.y * fz, r.z * fx - r.x * fz, -r.y * fx) * b.inv_inertia;
	}
	if (deepest > 0) b.p.y += deepest;
	if (touching) {
		b.w *= params_.damp_ground;
		b.v.x *= AIR_DRAG_GROUND;
		b.v.z *= AIR_DRAG_GROUND;
	}
	float reach2 = 0;
	for (const godot::Vector3 &c : b.corners) reach2 = std::max(reach2, c.length_squared());
	const float reach = std::sqrt(reach2);
	const bool slow = b.v.length() + b.w.length() * reach < SLOW_BASE_M + SLOW_REACH * reach;
	b.rest = (touching || lowest < LOW_CLEAR_M) && slow ? b.rest + STEP_S : 0.f;
	if (b.rest > params_.rest_s) {
		b.asleep = true;
		b.v = godot::Vector3();
		b.w = godot::Vector3();
		dirty_ = true;
	}
}

} // namespace city
