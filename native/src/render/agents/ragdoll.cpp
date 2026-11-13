// The particles of a fallen body and their joints (ragdoll.h): its dynamics. What it meets, ground, walls and other bodies, is
// ragdoll_contacts.cpp's.
#include "render/agents/ragdoll.h"

#include "core/math.h"
#include "render/agents/ragdoll_parts.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

using namespace ragdoll;

namespace {
constexpr int LIMB_OF[5][3] = { { 0, 0, 0 }, { LS, LE, LW }, { RS, RE, RW }, { LH, LK, LA }, { RH, RK, RA } };
constexpr float FALL_MS2 = 9.81f;
// No air drag: a body falling belly down tops out near 55 m/s (https://en.wikipedia.org/wiki/Terminal_velocity), so at a fall's few
// metres a second the air takes under a hundredth of its weight. What slows a limb is its joint (damp_joints).
// A passive knee's swing dies away at B / 2J a second: B 0.014 from the pendulum test of healthy knees (Valle et al., BMC Musculoskelet
// Disord 7:89, 2006, normalised by height^5), J 0.035 for the shank and foot about the knee (Winter's anthropometry: 6.1 % of the mass,
// 0.285 of the height long, its radius of gyration 0.735 of that; the same normalisation): 0.2 a second.
constexpr float JOINT_DAMP_PER_S = 0.2f;
constexpr float SINGULAR = 1e-9f; // an inertia this flat (the particles in a line: a limb torn off) has no spin to read
constexpr int ITERATIONS = 10;
constexpr float TOP_OVER_HEAD = 0.9f; // of the neck to the head, above it
constexpr float BELLY_OUT_M = 0.12f;
constexpr float KNEE_LEAST = 0.45f, ELBOW_LEAST = 0.35f, FOLD_LEAST = 0.62f; // shares of a straight limb, of the thigh to the chest
constexpr float RESTING_MS = 0.12f, RESTING_S = 0.6f;
constexpr float ONE_STEP_S = 1.f / 60.f;
constexpr float ELBOW_BRACE_SHARE = 0.5f; // the elbow is halfway along the arm the hand swings out on
constexpr float NO_LIMIT_M = 1e9f; // a stick that only keeps its ends apart
constexpr float DEG_TO_RAD = PI_F / 180.f;
// The normal range of motion (American Academy of Orthopaedic Surgeons): the neck bends 45 degrees either way, the ankle 20 up and 50 down.
constexpr float NECK_BEND_RAD = 45.f * DEG_TO_RAD, ANKLE_UP_RAD = 20.f * DEG_TO_RAD, ANKLE_DOWN_RAD = 50.f * DEG_TO_RAD;
constexpr int BONE_PASS_EVERY = 3; // the bones' pass on every third relaxation and the last
} // namespace

void Ragdoll::link(int a, int b, float share, bool least_only) {
	Stick &s = sticks_[sticks_used_++];
	s.a = a;
	s.b = b;
	s.rest = now_[a].distance_to(now_[b]) * share;
	s.most = least_only ? NO_LIMIT_M : s.rest;
}

void Ragdoll::link_angle(int a, int mid, int b, float less_rad, float more_rad) {
	const Vector3 u = now_[a] - now_[mid], v = now_[b] - now_[mid];
	const float l1 = u.length(), l2 = v.length();
	if (l1 < NOTHING_M || l2 < NOTHING_M) return;
	const float at = u.angle_to(v); // the inside angle at the joint now
	auto apart = [&](float angle) { return std::sqrt(l1 * l1 + l2 * l2 - 2.f * l1 * l2 * std::cos(std::clamp(angle, 0.f, PI_F))); };
	Stick &s = sticks_[sticks_used_++];
	s.a = a;
	s.b = b;
	s.rest = apart(at - less_rad);
	s.most = apart(at + more_rad);
}

void Ragdoll::begin(const Vector3 joints[RAGDOLL_JOINTS], const Vector3 &velocity, const Vector3 &tip, float scatter, const Vector3 &brace, Rng &rng) {
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
	// the neck and the ankles within a body's normal range of motion (AAOS: neck flexion and extension 45 degrees; ankle dorsiflexion 20,
	// plantarflexion 50): the head no longer folds into the chest, a foot no longer swings round its shin
	link_angle(CHE, NEC, HEA, NECK_BEND_RAD, NECK_BEND_RAD);
	link_angle(LK, LA, LT, ANKLE_UP_RAD, ANKLE_DOWN_RAD);
	link_angle(RK, RA, RT, ANKLE_UP_RAD, ANKLE_DOWN_RAD);
	for (int i = 0; i < PARTICLES; i++) {
		const float up = (now_[i].y - feet) / height;
		const float hand = i == LW || i == RW ? 1.f : i == LE || i == RE ? ELBOW_BRACE_SHARE : 0.f;
		const Vector3 own = velocity + tip * up + brace * hand + Vector3(rng.f() - 0.5f, rng.f() - 0.5f, rng.f() - 0.5f) * scatter;
		was_[i] = now_[i] - own * ONE_STEP_S;
	}
	for (Vector3 &s : last_side_) s = side;
	kept_ = ~0u;
	still_ = 0;
	asleep_ = false;
}

void Ragdoll::keep_parts(uint32_t parts) {
	uint32_t keep = 0;
	int root = -1;
	for (int p = 0; p < RAGDOLL_PARTS; p++) {
		if ((parts >> p & 1u) == 0) continue;
		keep |= 1u << PART_FRAMES[p].origin | 1u << PART_FRAMES[p].to;
		if (root < 0) root = PART_FRAMES[p].origin;
	}
	if (root < 0) return;
	kept_ = keep;
	root_ = root;
	int n = 0; // the sticks with both ends on the piece, the rest let go
	for (int i = 0; i < sticks_used_; i++)
		if ((kept_ >> sticks_[i].a & 1u) != 0 && (kept_ >> sticks_[i].b & 1u) != 0) sticks_[n++] = sticks_[i];
	sticks_used_ = n;
	for (int i = 0; i < PARTICLES; i++)
		if ((kept_ >> i & 1u) == 0) {
			was_[i] = was_[root_];
			now_[i] = now_[root_];
		}
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
		if (len < NOTHING_M || (len >= s.rest && len <= s.most)) continue;
		const Vector3 move = d * (0.5f * (len - (len < s.rest ? s.rest : s.most)) / len);
		now_[s.a] += move;
		now_[s.b] -= move;
	}
	if (kept_ != ~0u) return; // a piece has no pelvis to bend its joints by
	hinge(LH, LK, LA, 1.f);
	hinge(RH, RK, RA, 1.f);
	hinge(LS, LE, LW, -1.f);
	hinge(RS, RE, RW, -1.f);
}

// Joint friction (Mueller et al., Position Based Dynamics, 2007, 3.5): the body's rigid motion is read from its particles (its centre's
// step and its spin, the angular momentum through the inertia), and each particle's step less that motion, what its joints do, is
// damped at JOINT_DAMP_PER_S; the fall and the spin of the whole are kept.
void Ragdoll::damp_joints(float dt) {
	Vector3 centre, step;
	int n = 0;
	for (int i = 0; i < PARTICLES; i++)
		if ((kept_ >> i & 1u) != 0) {
			centre += now_[i];
			step += now_[i] - was_[i];
			n++;
		}
	if (n < 2) return;
	centre /= float(n);
	step /= float(n);
	Vector3 momentum;
	Basis inertia{ Vector3(), Vector3(), Vector3() };
	for (int i = 0; i < PARTICLES; i++) {
		if ((kept_ >> i & 1u) == 0) continue;
		const Vector3 r = now_[i] - centre;
		momentum += r.cross(now_[i] - was_[i]);
		const float r2 = r.length_squared();
		inertia.rows[0] += Vector3(r2, 0, 0) - r * r.x;
		inertia.rows[1] += Vector3(0, r2, 0) - r * r.y;
		inertia.rows[2] += Vector3(0, 0, r2) - r * r.z;
	}
	const Vector3 spin = std::fabs(inertia.determinant()) > SINGULAR ? inertia.inverse().xform(momentum) : Vector3();
	const float share = 1.f - std::exp(-JOINT_DAMP_PER_S * dt);
	for (int i = 0; i < PARTICLES; i++) {
		if ((kept_ >> i & 1u) == 0) continue;
		const Vector3 own = now_[i] - was_[i], rigid = step + spin.cross(now_[i] - centre);
		was_[i] = now_[i] - (own + (rigid - own) * share);
	}
}

int Ragdoll::step(float dt, const Ground &ground, float hard_ms, Vector3 hits[RAGDOLL_HITS]) {
	float floor_y[PARTICLES];
	float fastest = 0;
	int hit = 0;
	damp_joints(dt);
	for (int i = 0; i < PARTICLES; i++) {
		const Vector3 move = now_[i] - was_[i];
		fastest = std::max(fastest, move.length() / dt);
		was_[i] = now_[i];
		now_[i] += move + Vector3(0, -FALL_MS2 * dt * dt, 0);
		floor_y[i] = ground(now_[i].x, now_[i].z) + LIMB_HALF_M;
	}
	for (int k = 0; k < ITERATIONS; k++) {
		relax();
		for (int i = 0; i < PARTICLES; i++) {
			off_walls(i);
			if (now_[i].y >= floor_y[i]) continue;
			// coming down on the ground faster than hard_ms (the step's own fall, before the sticks' first pass corrects it)
			if (k == 0 && hit < RAGDOLL_HITS && now_[i].y - was_[i].y < -hard_ms * dt) hits[hit++] = Vector3(now_[i].x, floor_y[i] - LIMB_HALF_M, now_[i].z);
			now_[i].y = floor_y[i];
			was_[i].x = now_[i].x + (was_[i].x - now_[i].x) * SLIDE_KEEP;
			was_[i].z = now_[i].z + (was_[i].z - now_[i].z) * SLIDE_KEEP;
		}
		if (k % BONE_PASS_EVERY == BONE_PASS_EVERY - 1 || k == ITERATIONS - 1) {
			bones_on_ground(ground);
			bones_off_walls();
		}
	}
	if (kept_ != ~0u) // a piece: what is not on it stays at its root
		for (int i = 0; i < PARTICLES; i++)
			if ((kept_ >> i & 1u) == 0) {
				now_[i] = now_[root_];
				was_[i] = was_[root_];
			}
	still_ = fastest < RESTING_MS ? still_ + dt : 0.f;
	asleep_ = still_ > RESTING_S;
	return hit;
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
