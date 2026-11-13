// What a fallen body meets (ragdoll.h): the ground under its bones, the walls of the lots round it, roofs it comes down on, and the other
// bodies it falls among. Its dynamics, the particles and their joints, are ragdoll.cpp's.
#include "render/agents/ragdoll.h"

#include "render/agents/ragdoll_parts.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

using namespace ragdoll;

namespace {
constexpr float BONE_SAMPLES[] = { 1.f / 3.f, 2.f / 3.f }; // where along a bone it is held out of the ground, besides its ends
constexpr float BANK_M = 0.08f; // deeper than this under a bone's middle: a bank beside it (a kerb's face), left to its ends
// Two joints of two bodies touch nearer than this: two limbs' half thicknesses, surface to surface.
constexpr float TOUCH_M = 2.f * LIMB_HALF_M;
// Clothes on clothes, static and kinetic (Babaarslan and Kalebek, Tekstil ve Konfeksiyon 3/2010, fabric on fabric under the heaviest load
// measured, 20 N: both 0.7; they fall as the load grows, and a body is heavier).
constexpr float CLOTH_STATIC = 0.7f, CLOTH_KINETIC = 0.7f;
} // namespace

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

// A contact as Position Based Dynamics resolves it: the joint moved out along the line between the two, the speed it had along that
// line towards the other taken away (Mueller et al. 2007, 3.5), then friction at the position level (Macklin et al., Unified Particle
// Physics, SIGGRAPH 2014, 6.1): its step across the line, against the other's, taken away whole while under the static share of how deep
// it sank, else by the kinetic share. Damping the speed alone lets a pile slide apart; this holds a body lying on another.
void Ragdoll::push_out_of(const Ragdoll &other) {
	constexpr float TOUCH2 = TOUCH_M * TOUCH_M;
	for (int i = 0; i < PARTICLES; i++) {
		if ((kept_ >> i & 1u) == 0) continue;
		for (int j = 0; j < PARTICLES; j++) {
			if ((other.kept_ >> j & 1u) == 0) continue;
			const Vector3 d = now_[i] - other.now_[j];
			const float d2 = d.length_squared();
			if (d2 >= TOUCH2) continue;
			const float len = std::sqrt(d2);
			const Vector3 out = len > NOTHING_M ? d / len : Vector3(0, 1, 0); // atop the other when they coincide
			const float deep = TOUCH_M - len;
			const float towards = std::min((now_[i] - was_[i]).dot(out), 0.f); // its step along the line, when it closes in
			now_[i] += out * deep;
			was_[i] += out * (deep + towards); // moved, not thrown, and no longer closing in
			const Vector3 rel = (now_[i] - was_[i]) - (other.now_[j] - other.was_[j]);
			const Vector3 slide = rel - out * rel.dot(out);
			const float slid = slide.length();
			now_[i] -= slid < CLOTH_STATIC * deep ? slide : slide * std::min(CLOTH_KINETIC * deep / std::max(slid, NOTHING_M), 1.f);
		}
	}
}

float Ragdoll::overlap(const Ragdoll &other) const {
	float most = 0.f;
	for (int i = 0; i < PARTICLES; i++) {
		if ((kept_ >> i & 1u) == 0) continue;
		for (int j = 0; j < PARTICLES; j++)
			if ((other.kept_ >> j & 1u) != 0) most = std::max(most, TOUCH_M - now_[i].distance_to(other.now_[j]));
	}
	return most;
}

} // namespace city
