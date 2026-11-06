// The particles of a fallen body (ragdoll.h).
#include "render/agents/ragdoll.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
enum Joint { PEL, CHE, NEC, HEA, LS, LE, LW, RS, RE, RW, LH, LK, LA, LT, RH, RK, RA, RT, TOP, BEL };
enum Ref { REF_SIDE, REF_LARM, REF_RARM, REF_LLEG, REF_RLEG };
struct PartFrame {
	int origin, to, ref;
};
constexpr PartFrame PART_FRAMES[RAGDOLL_PARTS] = { // torso, neck, head; arms; legs (ragdoll.ts PARTS)
	{ PEL, CHE, REF_SIDE }, { NEC, HEA, REF_SIDE }, { HEA, TOP, REF_SIDE }, { LS, LE, REF_LARM }, { LE, LW, REF_LARM }, { RS, RE, REF_RARM }, { RE, RW, REF_RARM },
	{ LH, LK, REF_LLEG }, { LK, LA, REF_LLEG }, { LA, LT, REF_LLEG }, { RH, RK, REF_RLEG }, { RK, RA, REF_RLEG }, { RA, RT, REF_RLEG },
};
constexpr int LIMB_OF[5][3] = { { 0, 0, 0 }, { LS, LE, LW }, { RS, RE, RW }, { LH, LK, LA }, { RH, RK, RA } };
constexpr float FALL_MS2 = 9.81f;
constexpr float AIR_KEEP = 0.995f; // of a particle's speed a step
constexpr float LIMB_HALF_M = 0.06f; // a limb's half thickness: what stands off the ground
constexpr float SLIDE_KEEP = 0.25f; // of the speed along the ground a step on it: a body drags
constexpr int ITERATIONS = 10;
constexpr float TOP_OVER_HEAD = 0.9f; // of the neck to the head, above it
constexpr float BELLY_OUT_M = 0.12f;
constexpr float KNEE_LEAST = 0.45f, ELBOW_LEAST = 0.35f, FOLD_LEAST = 0.62f; // shares of a straight limb, of the thigh to the chest
constexpr float RESTING_MS = 0.12f, RESTING_S = 0.6f;
constexpr float NOTHING_M = 1e-5f;
constexpr float ONE_STEP_S = 1.f / 60.f;
} // namespace

void Ragdoll::link(int a, int b, float share, bool least_only) {
	Stick &s = sticks_[sticks_used_++];
	s.a = a;
	s.b = b;
	s.rest = now_[a].distance_to(now_[b]) * share;
	s.least_only = least_only;
}

void Ragdoll::begin(const Vector3 joints[RAGDOLL_JOINTS], const Vector3 &velocity, const Vector3 &tip, float scatter, Rng &rng) {
	for (int i = 0; i < RAGDOLL_JOINTS; i++) now_[i] = joints[i];
	now_[TOP] = now_[HEA] + (now_[HEA] - now_[NEC]) * TOP_OVER_HEAD;
	const Vector3 up = (now_[CHE] - now_[PEL]).normalized(), side = (now_[LS] - now_[RS]).normalized();
	now_[BEL] = now_[PEL] + side.cross(up).normalized() * BELLY_OUT_M;
	sticks_used_ = 0;
	static constexpr int TRUNK_BONDS[][2] = { { PEL, CHE }, { CHE, NEC }, { LS, RS }, { LH, RH }, { LS, CHE }, { RS, CHE }, { LS, NEC }, { RS, NEC }, { LH, PEL }, { RH, PEL }, { LS, LH },
		{ RS, RH }, { LS, RH }, { RS, LH }, { BEL, PEL }, { BEL, CHE }, { BEL, LH }, { BEL, RH }, { BEL, NEC }, { HEA, TOP }, { NEC, HEA }, { NEC, TOP }, { CHE, HEA } };
	static constexpr int LIMB_BONDS[][2] = { { LS, LE }, { LE, LW }, { RS, RE }, { RE, RW }, { LH, LK }, { LK, LA }, { LA, LT }, { LK, LT }, { RH, RK }, { RK, RA }, { RA, RT }, { RK, RT } };
	const float feet = std::min(now_[LT].y, now_[RT].y), height = std::max(now_[TOP].y - feet, NOTHING_M);
	for (const auto &p : TRUNK_BONDS) link(p[0], p[1]);
	for (const auto &p : LIMB_BONDS) link(p[0], p[1]);
	link(LS, LW, ELBOW_LEAST, true);
	link(RS, RW, ELBOW_LEAST, true);
	link(LH, LA, KNEE_LEAST, true);
	link(RH, RA, KNEE_LEAST, true);
	link(LK, CHE, FOLD_LEAST, true);
	link(RK, CHE, FOLD_LEAST, true);
	for (int i = 0; i < PARTICLES; i++) {
		const float up = (now_[i].y - feet) / height;
		const Vector3 own = velocity + tip * up + Vector3(rng.f() - 0.5f, rng.f() - 0.5f, rng.f() - 0.5f) * scatter;
		was_[i] = now_[i] - own * ONE_STEP_S;
	}
	for (Vector3 &s : last_side_) s = side;
	still_ = 0;
	asleep_ = false;
}

// Holds a limb's middle joint to the side it bends to, `way` the front of the pelvis or its back.
void Ragdoll::hinge(int top, int middle, int end, float way) {
	const Vector3 front = (now_[BEL] - now_[PEL]).normalized() * way;
	const float out = (now_[middle] - (now_[top] + now_[end]) * 0.5f).dot(front);
	if (out < 0) now_[middle] -= front * out;
}

void Ragdoll::relax() {
	for (int i = 0; i < sticks_used_; i++) {
		const Stick &s = sticks_[i];
		const Vector3 d = now_[s.b] - now_[s.a];
		const float len = d.length();
		if (len < NOTHING_M || (s.least_only && len >= s.rest)) continue;
		const Vector3 move = d * (0.5f * (len - s.rest) / len);
		now_[s.a] += move;
		now_[s.b] -= move;
	}
	hinge(LH, LK, LA, 1.f);
	hinge(RH, RK, RA, 1.f);
	hinge(LS, LE, LW, -1.f);
	hinge(RS, RE, RW, -1.f);
}

void Ragdoll::step(float dt, const Ground &ground) {
	float floor_y[PARTICLES];
	float fastest = 0;
	for (int i = 0; i < PARTICLES; i++) {
		const Vector3 move = (now_[i] - was_[i]) * AIR_KEEP;
		fastest = std::max(fastest, move.length() / dt);
		was_[i] = now_[i];
		now_[i] += move + Vector3(0, -FALL_MS2 * dt * dt, 0);
		floor_y[i] = ground(now_[i].x, now_[i].z) + LIMB_HALF_M;
	}
	for (int k = 0; k < ITERATIONS; k++) {
		relax();
		for (int i = 0; i < PARTICLES; i++) {
			if (now_[i].y >= floor_y[i]) continue;
			now_[i].y = floor_y[i];
			was_[i].x = now_[i].x + (was_[i].x - now_[i].x) * SLIDE_KEEP;
			was_[i].z = now_[i].z + (was_[i].z - now_[i].z) * SLIDE_KEEP;
		}
	}
	still_ = fastest < RESTING_MS ? still_ + dt : 0.f;
	asleep_ = still_ > RESTING_S;
}

void Ragdoll::frames(Transform3D out[RAGDOLL_PARTS]) {
	for (int p = 0; p < RAGDOLL_PARTS; p++) {
		const PartFrame &f = PART_FRAMES[p];
		const Vector3 along = (now_[f.to] - now_[f.origin]).normalized();
		Vector3 side = now_[LS] - now_[RS];
		if (f.ref != REF_SIDE) { // the plane the limb bends in; straight, it keeps the last
			const int *limb = LIMB_OF[f.ref];
			side = (now_[limb[1]] - now_[limb[0]]).cross(now_[limb[2]] - now_[limb[1]]);
		}
		side -= along * side.dot(along);
		if (side.length() < NOTHING_M) side = last_side_[p] - along * last_side_[p].dot(along);
		side = side.normalized();
		last_side_[p] = side;
		Basis axes;
		axes.set_columns(along, side, along.cross(side));
		out[p] = Transform3D(axes, now_[f.origin]);
	}
}

Vector3 Ragdoll::pelvis() const { return now_[PEL]; }

} // namespace city
