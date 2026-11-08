// One rigid piece over the ground: gravity, then each corner under the ground struck with an impulse (it bounces, slides, tumbles and
// settles; at rest it sleeps). What the thrown pieces (debris.h) and the pieces of a falling building (ruin_pieces.h) move by.
#pragma once

#include "render/effects/effect_params.h"

#include <godot_cpp/variant/quaternion.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <algorithm>
#include <cmath>

namespace city {

constexpr float RIGID_STEP_S = 1.f / 60.f;
constexpr int RIGID_CORNERS = 8; // of a piece's box

struct RigidPiece {
	godot::Vector3 p, v, w;
	godot::Quaternion q;
	godot::Vector3 corners[RIGID_CORNERS];
	float inv_inertia = 0, rest = 0;
	bool asleep = false;
	bool touching = false; // a corner was on the ground at the last step

	// Its box: `half` the sides, `extent` the sum of the squares its inertia goes by.
	void shape(const godot::Vector3 &half, float extent) {
		constexpr float INERTIA_NUM = 12.f, INERTIA_MIN = 1e-4f; // inverse inertia per unit mass of a box: 12 / (a2 + b2 + c2)
		int k = 0;
		for (int x = -1; x <= 1; x += 2)
			for (int y = -1; y <= 1; y += 2)
				for (int z = -1; z <= 1; z += 2) corners[k++] = godot::Vector3(float(x) * half.x, float(y) * half.y, float(z) * half.z);
		inv_inertia = INERTIA_NUM / std::max(INERTIA_MIN, extent);
	}
};

// One step of `b` over the ground `ground_of(x, z)`. True on the step it falls asleep.
template <typename F>
bool rigid_step(RigidPiece &b, F &&ground_of, const EffectParams &law) {
	constexpr float SPIN_EPS = 1e-6f; // an angular velocity this small turns nothing
	constexpr float SLOW_BASE_M = 0.15f; // Box2D's rest test: the farthest point moving slower than this plus a share of its reach
	constexpr float SLOW_REACH = 0.55f;
	constexpr float LOW_CLEAR_M = 0.05f; // a corner this close over the ground still counts as resting on it
	constexpr float AIR_DRAG_GROUND = 0.995f; // horizontal speed kept per step while touching
	constexpr float NOTHING_UNDER = 1e9f;
	b.v.y -= law.gravity * RIGID_STEP_S;
	b.p += b.v * RIGID_STEP_S;
	const float angle = b.w.length() * RIGID_STEP_S;
	if (angle > SPIN_EPS) b.q = (godot::Quaternion(b.w.normalized(), angle) * b.q).normalized();
	bool touching = false;
	float deepest = 0, lowest = NOTHING_UNDER;
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
		const float j = -(1.f + law.restitution) * vp.y / k;
		b.v.y += j;
		b.w += godot::Vector3(-r.z, 0, r.x) * (b.inv_inertia * j);
		const float tl = std::hypot(vp.x, vp.z);
		if (tl <= SPIN_EPS) continue;
		const float f = std::min(tl / k, law.friction * j);
		const float fx = -vp.x / tl * f, fz = -vp.z / tl * f;
		b.v.x += fx;
		b.v.z += fz;
		b.w += godot::Vector3(r.y * fz, r.z * fx - r.x * fz, -r.y * fx) * b.inv_inertia;
	}
	if (deepest > 0) b.p.y += deepest;
	b.touching = touching;
	if (touching) {
		b.w *= law.damp_ground;
		b.v.x *= AIR_DRAG_GROUND;
		b.v.z *= AIR_DRAG_GROUND;
	}
	float reach2 = 0;
	for (const godot::Vector3 &c : b.corners) reach2 = std::max(reach2, c.length_squared());
	const float reach = std::sqrt(reach2);
	const bool slow = b.v.length() + b.w.length() * reach < SLOW_BASE_M + SLOW_REACH * reach;
	b.rest = (touching || lowest < LOW_CLEAR_M) && slow ? b.rest + RIGID_STEP_S : 0.f;
	if (b.rest <= law.rest_s) return false;
	b.asleep = true;
	b.v = godot::Vector3();
	b.w = godot::Vector3();
	return true;
}

} // namespace city
