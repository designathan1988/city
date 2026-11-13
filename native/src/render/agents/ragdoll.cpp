// The particles of a fallen body (ragdoll.h).
#include "render/agents/ragdoll.h"

#include "core/math.h"

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
constexpr float ELBOW_BRACE_SHARE = 0.5f; // the elbow is halfway along the arm the hand swings out on
constexpr float NO_LIMIT_M = 1e9f; // a stick that only keeps its ends apart
constexpr float DEG_TO_RAD = PI_F / 180.f;
// The normal range of motion (American Academy of Orthopaedic Surgeons): the neck bends 45 degrees either way, the ankle 20 up and 50 down.
constexpr float NECK_BEND_RAD = 45.f * DEG_TO_RAD, ANKLE_UP_RAD = 20.f * DEG_TO_RAD, ANKLE_DOWN_RAD = 50.f * DEG_TO_RAD;
constexpr float BONE_SAMPLES[] = { 1.f / 3.f, 2.f / 3.f }; // where along a bone it is held out of the ground, besides its ends
constexpr float BANK_M = 0.08f; // deeper than this under a bone's middle: a bank beside it (a kerb's face), left to its ends
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

// The bones as capsules on the ground (Jakobsen, Advanced Character Physics, GDC 2001: a stick with a thickness): points along each are
// held out of it as its ends are, the move shared by the ends by how near each is, and their old places moved alike so it adds no
// speed. A shin no longer sinks over a kerb's edge; a point deeper than a bank's face is beside it, not on it.
void Ragdoll::bones_on_ground(const Ground &ground) {
	for (const PartFrame &f : PART_FRAMES) {
		const int a = f.origin, b = f.to;
		if ((kept_ >> a & 1u) == 0 || (kept_ >> b & 1u) == 0) continue;
		for (float t : BONE_SAMPLES) {
			const Vector3 q = now_[a].lerp(now_[b], t);
			const float deep = ground(q.x, q.z) + LIMB_HALF_M - q.y;
			if (deep <= 0.f || deep > BANK_M) continue;
			const float wa = 1.f - t, wb = t, lift = deep / (wa * wa + wb * wb);
			now_[a].y += wa * lift;
			was_[a].y += wa * lift;
			now_[b].y += wb * lift;
			was_[b].y += wb * lift;
		}
	}
}

// A bone with its ends on the two sides of a wall (its plane crossed within the wall's length and under its top) has the end nearer the
// wall brought to the other end's side, its old place moved alike (no speed added): a body lies against a fence, not astride it.
bool Ragdoll::uncross(int a, int b, const BodyWall &w, bool across_deep) {
	const Vector2 axis = across_deep ? w.deep : w.along, other = across_deep ? w.along : w.deep;
	const float lo = across_deep ? w.y0 : w.x0, hi = across_deep ? w.y1 : w.x1;
	const float olo = across_deep ? w.x0 : w.y0, ohi = across_deep ? w.x1 : w.y1;
	const Vector2 ra = Vector2(now_[a].x, now_[a].z) - w.at, rb = Vector2(now_[b].x, now_[b].z) - w.at;
	const float ka = ra.dot(axis), kb = rb.dot(axis), mid = (lo + hi) * 0.5f;
	if (!((ka < lo && kb > hi) || (ka > hi && kb < lo))) return false;
	const float t = (mid - ka) / (kb - ka);
	const float along_t = ra.dot(other) + (rb.dot(other) - ra.dot(other)) * t, y_t = now_[a].y + (now_[b].y - now_[a].y) * t;
	if (along_t <= olo || along_t >= ohi || y_t >= w.top) return false;
	const bool move_a = std::fabs(ka - mid) < std::fabs(kb - mid);
	const int i = move_a ? a : b;
	const float from = move_a ? ka : kb, to_side = (move_a ? kb : ka) > mid ? hi + LIMB_HALF_M : lo - LIMB_HALF_M;
	const Vector2 shift = axis * (to_side - from);
	now_[i].x += shift.x;
	now_[i].z += shift.y;
	was_[i].x += shift.x;
	was_[i].z += shift.y;
	return true;
}

void Ragdoll::bones_off_walls() {
	for (const PartFrame &f : PART_FRAMES) {
		const int a = f.origin, b = f.to;
		if ((kept_ >> a & 1u) == 0 || (kept_ >> b & 1u) == 0) continue;
		for (const BodyWall &w : walls_)
			if (!uncross(a, b, w, true)) uncross(a, b, w, false);
	}
}

float Ragdoll::deepest(const Ground &ground) const {
	float deep = 0.f;
	for (int i = 0; i < PARTICLES; i++)
		if ((kept_ >> i & 1u) != 0) deep = std::max(deep, ground(now_[i].x, now_[i].z) - now_[i].y);
	for (const PartFrame &f : PART_FRAMES) {
		if ((kept_ >> f.origin & 1u) == 0 || (kept_ >> f.to & 1u) == 0) continue;
		for (float t : BONE_SAMPLES) {
			const Vector3 q = now_[f.origin].lerp(now_[f.to], t);
			deep = std::max(deep, ground(q.x, q.z) - q.y);
		}
	}
	return deep;
}

// Out of a wall by the nearest side: the static collision of Position Based Dynamics, the point projected to the closest of the surface, and the speed along it damped.
void Ragdoll::off_walls(int i) {
	Vector3 &p = now_[i];
	for (const BodyWall &w : walls_) {
		if (p.y >= w.top) continue;
		const Vector2 rel = Vector2(p.x, p.z) - w.at;
		const float lx = rel.dot(w.along), ly = rel.dot(w.deep);
		if (lx <= w.x0 || lx >= w.x1 || ly <= w.y0 || ly >= w.y1) continue;
		if (was_[i].y >= w.top) { // come down on it from above: onto its roof, as onto the ground
			p.y = w.top + LIMB_HALF_M;
			was_[i].x = p.x + (was_[i].x - p.x) * SLIDE_KEEP;
			was_[i].z = p.z + (was_[i].z - p.z) * SLIDE_KEEP;
			continue;
		}
		const float west = lx - w.x0, east = w.x1 - lx, south = ly - w.y0, north = w.y1 - ly;
		const float least = std::min({ west, east, south, north });
		const Vector2 out = least == west ? -w.along : least == east ? w.along : least == south ? -w.deep : w.deep;
		const Vector2 move = out * (least + LIMB_HALF_M);
		p.x += move.x;
		p.z += move.y;
		was_[i].x = p.x + (was_[i].x - p.x) * SLIDE_KEEP;
		was_[i].z = p.z + (was_[i].z - p.z) * SLIDE_KEEP;
	}
}

int Ragdoll::step(float dt, const Ground &ground, float hard_ms, Vector3 hits[RAGDOLL_HITS]) {
	float floor_y[PARTICLES];
	float fastest = 0;
	int hit = 0;
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
