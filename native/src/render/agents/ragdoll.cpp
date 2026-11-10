// The particles of a fallen body (ragdoll.h).
#include "render/agents/ragdoll.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
using namespace rag; // the particles (ragdoll.h)
enum Ref { REF_SIDE, REF_LARM, REF_RARM, REF_LLEG, REF_RLEG };
struct PartFrame {
	int origin, to, ref;
};
constexpr PartFrame PART_FRAMES[RAGDOLL_PARTS] = { // torso, neck, head; arms; legs (ragdoll.ts PARTS)
	{ PEL, CHE, REF_SIDE }, { NEC, HEA, REF_SIDE }, { HEA, TOP, REF_SIDE }, { LS, LE, REF_LARM }, { LE, LW, REF_LARM }, { RS, RE, REF_RARM }, { RE, RW, REF_RARM },
	{ LH, LK, REF_LLEG }, { LK, LA, REF_LLEG }, { LA, LT, REF_LLEG }, { RH, RK, REF_RLEG }, { RK, RA, REF_RLEG }, { RA, RT, REF_RLEG },
};
// a limb's three joints, by its number (none, the left and right arms, the left and right legs)
constexpr int LIMB_OF[5][3] = { { 0, 0, 0 }, { LS, LE, LW }, { RS, RE, RW }, { LH, LK, LA }, { RH, RK, RA } };
constexpr float FALL_MS2 = 9.81f; // gravity at the surface (9.80665 standard), to three figures
constexpr float NOTHING_M = 1e-5f; // a length this short has no direction
} // namespace

void Ragdoll::link(int a, int b, float share, bool least_only) {
	Stick &s = sticks_[sticks_used_++];
	s.a = a;
	s.b = b;
	s.rest = now_[a].distance_to(now_[b]) * share;
	s.least_only = least_only;
}

void Ragdoll::begin(const RagdollShape &law, const Vector3 joints[RAGDOLL_JOINTS], const Vector3 &velocity, const Vector3 &tip, float scatter, Rng &rng) {
	law_ = &law;
	const RagdollShape &K = law;
	for (int i = 0; i < RAGDOLL_JOINTS; i++) now_[i] = joints[i];
	now_[TOP] = now_[HEA] + (now_[HEA] - now_[NEC]) * K.top_over_head;
	const Vector3 up = (now_[CHE] - now_[PEL]).normalized(), side = (now_[LS] - now_[RS]).normalized();
	now_[BEL] = now_[PEL] + side.cross(up).normalized() * K.belly_out_m;
	sticks_used_ = 0;
	static constexpr int TRUNK_BONDS[][2] = { { PEL, CHE }, { CHE, NEC }, { LS, RS }, { LH, RH }, { LS, CHE }, { RS, CHE }, { LS, NEC }, { RS, NEC }, { LH, PEL }, { RH, PEL }, { LS, LH },
		{ RS, RH }, { LS, RH }, { RS, LH }, { BEL, PEL }, { BEL, CHE }, { BEL, LH }, { BEL, RH }, { BEL, NEC }, { HEA, TOP }, { NEC, HEA }, { NEC, TOP }, { CHE, HEA } };
	static constexpr int LIMB_BONDS[][2] = { { LS, LE }, { LE, LW }, { RS, RE }, { RE, RW }, { LH, LK }, { LK, LA }, { LA, LT }, { LK, LT }, { RH, RK }, { RK, RA }, { RA, RT }, { RK, RT } };
	const float feet = std::min(now_[LT].y, now_[RT].y), height = std::max(now_[TOP].y - feet, NOTHING_M);
	for (const auto &p : TRUNK_BONDS) link(p[0], p[1]);
	for (const auto &p : LIMB_BONDS) link(p[0], p[1]);
	link(LS, LW, K.elbow_least, true);
	link(RS, RW, K.elbow_least, true);
	link(LH, LA, K.knee_least, true);
	link(RH, RA, K.knee_least, true);
	link(LK, CHE, K.fold_least, true);
	link(RK, CHE, K.fold_least, true);
	for (int i = 0; i < PARTICLES; i++) {
		const float up = (now_[i].y - feet) / height;
		const Vector3 own = velocity + tip * up + Vector3(rng.f() - 0.5f, rng.f() - 0.5f, rng.f() - 0.5f) * scatter;
		was_[i] = now_[i] - own * K.step_s;
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

namespace {
constexpr int SIDES = 4; // of a wall's box on the ground plan
// Whether a point on the ground plan stands inside a wall's box, under its top.
bool in_wall(const BodyWall &w, const Vector3 &p) {
	if (p.y >= w.top) return false;
	const Vector2 rel = Vector2(p.x, p.z) - w.at;
	const float lx = rel.dot(w.along), ly = rel.dot(w.deep);
	return lx > w.x0 && lx < w.x1 && ly > w.y0 && ly < w.y1;
}
} // namespace

// Out of a wall: the static collision of Position Based Dynamics, the point projected to the surface and the speed along it damped.
// By the nearest side that leads out of every wall: buildings stand against one another, and the nearest side of one alone put the
// point inside its neighbour, which put it back, for ever (a body that never came to rest). With no side leading out it stays.
void Ragdoll::off_walls(int i) {
	const RagdollShape &K = *law_;
	Vector3 &p = now_[i];
	for (const BodyWall &w : walls_) {
		if (!in_wall(w, p)) continue;
		const Vector2 rel = Vector2(p.x, p.z) - w.at;
		const float lx = rel.dot(w.along), ly = rel.dot(w.deep);
		const float depth[SIDES] = { lx - w.x0, w.x1 - lx, ly - w.y0, w.y1 - ly };
		const Vector2 way[SIDES] = { -w.along, w.along, -w.deep, w.deep };
		int best = -1;
		for (int side = 0; side < SIDES; side++) {
			if (best >= 0 && depth[side] >= depth[best]) continue;
			const Vector2 move = way[side] * (depth[side] + K.limb_half_m);
			const Vector3 there(p.x + move.x, p.y, p.z + move.y);
			bool free = true;
			for (const BodyWall &other : walls_) free = free && (&other == &w || !in_wall(other, there));
			if (free) best = side;
		}
		if (best < 0) continue;
		const Vector2 move = way[best] * (depth[best] + K.limb_half_m);
		p.x += move.x;
		p.z += move.y;
		was_[i].x = p.x + (was_[i].x - p.x) * K.slide_keep;
		was_[i].z = p.z + (was_[i].z - p.z) * K.slide_keep;
	}
}

void Ragdoll::step(float dt, const Ground &ground) {
	const RagdollShape &K = *law_;
	float floor_y[PARTICLES];
	float fastest = 0;
	for (int i = 0; i < PARTICLES; i++) {
		const Vector3 move = (now_[i] - was_[i]) * K.air_keep;
		fastest = std::max(fastest, move.length() / dt);
		was_[i] = now_[i];
		now_[i] += move + Vector3(0, -FALL_MS2 * dt * dt, 0);
		floor_y[i] = ground(now_[i].x, now_[i].z) + K.limb_half_m;
	}
	for (int k = 0; k < K.iterations; k++) {
		relax();
		for (int i = 0; i < PARTICLES; i++) {
			off_walls(i);
			if (now_[i].y >= floor_y[i]) continue;
			now_[i].y = floor_y[i];
			was_[i].x = now_[i].x + (was_[i].x - now_[i].x) * K.slide_keep;
			was_[i].z = now_[i].z + (was_[i].z - now_[i].z) * K.slide_keep;
		}
	}
	fastest_ = fastest;
	still_ = fastest < K.resting_ms ? still_ + dt : 0.f;
	asleep_ = still_ > K.resting_s;
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

void Ragdoll::writhe(float t, float dt) {
	const RagdollShape &K = *law_;
	static constexpr int LEGS[2][2] = { { LK, LH }, { RK, RH } }; // each knee and its hip, the right a phase behind
	const float strength = std::min(1.f, t / K.writhe_rise_s) * std::max(K.writhe_floor, 1.f - t / K.writhe_fade_s);
	const Vector3 spine = (now_[CHE] - now_[PEL]) * K.writhe_spine_share;
	for (int leg = 0; leg < 2; leg++) {
		const float s = std::sin(t * K.writhe_rate + float(leg) * K.writhe_phase);
		if (s <= 0.f) continue;
		const int knee = LEGS[leg][0], hip = LEGS[leg][1];
		const Vector3 to = now_[hip].lerp(now_[BEL], K.writhe_hip_share) + spine;
		now_[knee] = now_[knee].lerp(to, std::min(1.f, K.writhe_pull * dt * s * strength));
	}
	still_ = 0; // it is moving: stepped every frame, never asleep while it writhes
	asleep_ = false;
}

} // namespace city
