// What hangs off a body opened or cut (body_gore.h).
#include "render/agents/body_gore.h"

#include "core/shader_contracts/effects.h"

#include <godot_cpp/classes/cylinder_mesh.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

using namespace godot;

namespace city {

namespace {
constexpr int FIRST_ROOM = 64;
constexpr int SIDES = 7; // gore.ts: a gut and a bone are seven-sided
constexpr float STEP_S = 1.f / 60.f;
constexpr float STALL_CAP_S = 0.05f; // a stalled frame steps them no more than this
constexpr float MM_TO_M = 0.001f;
constexpr float LINK_EPS = 1e-6f;
constexpr float HALF = 0.5f;
constexpr float NEAR_VERTICAL = 0.99f;
const Vector3 UP(0, 1, 0), ACROSS(1, 0, 0);
constexpr float HOLDS = std::numeric_limits<float>::infinity();

// A unit cylinder's instance from `a` to `b`, `r` round: its axis (y) along the segment.
void put_between(MmLayer &layer, int i, const Vector3 &a, const Vector3 &b, float r, const Color &tint) {
	const Vector3 along = b - a;
	const float l = std::max(along.length(), LINK_EPS);
	const Vector3 dir = along / l;
	const Vector3 side = (std::fabs(dir.dot(UP)) > NEAR_VERTICAL ? ACROSS : UP).cross(dir).normalized(), other = dir.cross(side);
	layer.put(i, side * r, dir * l, other * r, (a + b) * HALF, tint.r, tint.g, tint.b, DEBRIS_PLAIN);
}
} // namespace

void BodyGore::init(const RID &scenario, const RID &material, const GoreLook &look, float gravity) {
	look_ = look;
	gravity_ = gravity;
	Ref<CylinderMesh> cyl;
	cyl.instantiate();
	cyl->set_height(1.f);
	cyl->set_top_radius(1.f);
	cyl->set_bottom_radius(1.f);
	cyl->set_radial_segments(SIDES);
	for (MmLayer *l : { &links_, &sticks_, &cuffs_ }) {
		l->init_shaped(scenario, cyl, material, FIRST_ROOM, true, cyl->get_aabb(), 0.f);
		l->count(0);
		l->shown(false);
	}
}

void BodyGore::reset() {
	chains_.clear();
	bones_.clear();
	dirty_ = true;
}

void BodyGore::release() {
	for (MmLayer *l : { &links_, &sticks_, &cuffs_ }) l->release();
}

void BodyGore::spill(int slot, int particle, const Vector3 &at, const Vector3 &kick) {
	const GoreLook &g = look_;
	Chain c;
	c.slot = slot;
	c.particle = particle;
	c.link = g.link_min_m + rng_.f() * g.link_span_m;
	c.radius = g.gut_r_min_m + rng_.f() * g.gut_r_span_m;
	c.tint = g.gut_tints[size_t(rng_.next() % uint32_t(g.gut_tints.size()))];
	c.hold_s = rng_.f() < g.hold_chance ? g.hold_min_s + rng_.f() * g.hold_span_s : HOLDS;
	for (int i = 0; i < g.links; i++) { // coiled a little out of the wound, the way it was packed
		const float a = float(i) * g.coil_turn;
		const Vector3 v = at + Vector3(std::cos(a) * g.coil_m, -float(i) * c.link * g.drop_share, std::sin(a) * g.coil_m);
		c.p.push_back(v);
		c.o.push_back(v - kick * (STEP_S * float(i) / float(g.links)));
	}
	chains_.push_back(std::move(c));
	dirty_ = true;
}

void BodyGore::bone(int slot, RagdollStump stump, float length) {
	bones_.push_back({ slot, stump, length });
	dirty_ = true;
}

void BodyGore::forget(int slot) {
	const auto mine = [slot](const auto &x) { return x.slot == slot; };
	const size_t before = chains_.size() + bones_.size();
	chains_.erase(std::remove_if(chains_.begin(), chains_.end(), mine), chains_.end());
	bones_.erase(std::remove_if(bones_.begin(), bones_.end(), mine), bones_.end());
	dirty_ = dirty_ || chains_.size() + bones_.size() != before;
}

void BodyGore::step(Chain &c, float dt, const Anchor &anchor, const std::function<float(float, float)> &ground) {
	const GoreLook &g = look_;
	c.age += dt;
	Vector3 held;
	if (c.held && (c.age > c.hold_s || !anchor(c.slot, c.particle, held))) c.held = false;
	if (!c.held && c.still > g.still_s) return;
	float moved = 0;
	for (size_t i = 0; i < c.p.size(); i++) {
		const Vector3 v = (c.p[i] - c.o[i]) * g.damp;
		c.o[i] = c.p[i];
		c.p[i] += v + Vector3(0, -gravity_ * dt * dt, 0);
		moved = std::max(moved, std::fabs(v.x) + std::fabs(v.y) + std::fabs(v.z));
	}
	for (int it = 0; it < g.passes; it++) {
		if (c.held) c.p[0] = held;
		for (size_t i = 1; i < c.p.size(); i++) {
			const Vector3 d = c.p[i] - c.p[i - 1];
			const float l = std::max(d.length(), LINK_EPS), diff = (l - c.link) / l;
			if (i == 1 && c.held) c.p[i] -= d * diff;
			else {
				c.p[i - 1] += d * (diff * HALF);
				c.p[i] -= d * (diff * HALF);
			}
		}
		for (size_t i = 0; i < c.p.size(); i++) {
			const float floor = ground(c.p[i].x, c.p[i].z) + c.radius;
			if (c.p[i].y >= floor) continue;
			c.p[i].y = floor;
			c.o[i].x = c.p[i].x - (c.p[i].x - c.o[i].x) * g.friction;
			c.o[i].z = c.p[i].z - (c.p[i].z - c.o[i].z) * g.friction;
		}
	}
	c.still = moved < g.still_mm * MM_TO_M ? c.still + dt : 0.f;
	dirty_ = true;
}

void BodyGore::update(float dt, const Anchor &anchor, const std::function<float(float, float)> &ground, bool bodies_moved) {
	const float span = std::min(STALL_CAP_S, std::max(0.f, dt));
	for (Chain &c : chains_) step(c, span, anchor, ground);
	const size_t before = bones_.size();
	bones_.erase(std::remove_if(bones_.begin(), bones_.end(), [&](const Bone &b) {
		Vector3 x;
		return !anchor(b.slot, b.stump.at, x);
	}), bones_.end());
	if (bones_.size() != before || (bodies_moved && !bones_.empty())) dirty_ = true; // a bone goes with its body
	if (dirty_) flush(anchor);
}

void BodyGore::flush(const Anchor &anchor) {
	if (!ready()) return;
	dirty_ = false;
	int n = 0;
	for (const Chain &c : chains_) n += int(c.p.size()) - 1;
	links_.reserve(n);
	sticks_.reserve(int(bones_.size()));
	cuffs_.reserve(int(bones_.size()));
	for (MmLayer *l : { &links_, &sticks_, &cuffs_ }) l->clear_bounds();
	int i = 0;
	for (const Chain &c : chains_)
		for (size_t k = 1; k < c.p.size(); k++) put_between(links_, i++, c.p[k - 1], c.p[k], c.radius, c.tint);
	int b = 0;
	for (const Bone &bone : bones_) {
		Vector3 from, at;
		if (!anchor(bone.slot, bone.stump.from, from) || !anchor(bone.slot, bone.stump.at, at)) continue;
		const Vector3 d = at - from;
		if (d.length() < LINK_EPS) continue;
		const Vector3 dir = d.normalized();
		put_between(sticks_, b, at, at + dir * bone.length, look_.bone_r_m, look_.bone_tint);
		put_between(cuffs_, b, at, at + dir * look_.cuff_len_m, look_.cuff_r_m, look_.cuff_tint); // the raw flesh round it where it leaves
		b++;
	}
	for (auto [layer, count] : { std::pair<MmLayer *, int>{ &links_, i }, std::pair<MmLayer *, int>{ &sticks_, b }, std::pair<MmLayer *, int>{ &cuffs_, b } }) {
		layer->count(count);
		layer->shown(count > 0);
		if (count > 0) layer->upload();
	}
}

} // namespace city
