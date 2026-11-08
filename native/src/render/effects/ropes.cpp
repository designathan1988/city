// The wires a blow tore loose (ropes.h).
#include "render/effects/ropes.h"

#include <godot_cpp/classes/box_mesh.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int FIRST_ROOM = 64; // links the layer holds before it grows
constexpr float LINK_EPS = 1e-6f;
constexpr float HALF = 0.5f;
const Vector3 UP(0, 1, 0), ACROSS(1, 0, 0);
constexpr float NEAR_VERTICAL = 0.99f; // a link this upright takes its side from x, not from up
} // namespace

void Ropes::init(const RID &scenario, const RID &material, const RopeLook &look, const Color &tint, float radius, float gravity) {
	look_ = look;
	tint_ = tint;
	radius_ = radius;
	gravity_ = gravity;
	Ref<BoxMesh> box;
	box.instantiate();
	box->set_size(Vector3(1, 1, 1));
	links_.init_shaped(scenario, box, material, FIRST_ROOM, false, box->get_aabb(), 0.f); // a wire's shadow is under a texel
	links_.count(0);
	links_.shown(false);
}

void Ropes::reset() {
	ropes_.clear();
	dirty_ = true;
	flush();
}

void Ropes::release() { links_.release(); }

void Ropes::add(const Vector3 &from, const Vector3 &to, const Vector3 &kick) {
	Rope r;
	const int n = look_.points;
	r.pin = from;
	for (int k = 0; k < n; k++) {
		const float t = float(k) / float(n - 1);
		const Vector3 v = from.lerp(to, t);
		r.p.push_back(v);
		r.o.push_back(v - kick * (t * STEP_S)); // a step ago: thrown in proportion along it, the loose end the most
	}
	r.link = from.distance_to(to) / float(n - 1) * look_.slack;
	r.live = look_.crackle_min_s + rng_.f() * look_.crackle_span_s;
	ropes_.push_back(std::move(r));
	if (int(ropes_.size()) > look_.most) ropes_.pop_front();
	dirty_ = true;
}

void Ropes::pull(Vector3 &a, Vector3 &b, float link, bool pinned) {
	const Vector3 d = b - a;
	const float l = std::max(d.length(), LINK_EPS);
	if (l <= link) return;
	const float over = (l - link) / l;
	if (pinned) {
		b -= d * over;
		return;
	}
	a += d * (over * HALF);
	b -= d * (over * HALF);
}

bool Ropes::moving() const {
	return std::any_of(ropes_.begin(), ropes_.end(), [&](const Rope &r) { return r.age < look_.settle_s; });
}

void Ropes::flush() {
	if (!dirty_) return;
	dirty_ = false;
	int n = 0;
	for (const Rope &r : ropes_) n += int(r.p.size()) - 1;
	links_.reserve(n);
	links_.clear_bounds();
	const float thick = radius_ * 2.f;
	int i = 0;
	for (const Rope &r : ropes_)
		for (size_t k = 1; k < r.p.size(); k++) {
			const Vector3 a = r.p[k - 1], b = r.p[k], along = b - a;
			const float l = along.length();
			const Vector3 dir = l > LINK_EPS ? along / l : UP;
			const Vector3 side = (std::fabs(dir.dot(UP)) > NEAR_VERTICAL ? ACROSS : UP).cross(dir).normalized(), up = dir.cross(side);
			links_.put(i++, along, up * thick, side * thick, (a + b) * HALF, tint_.r, tint_.g, tint_.b, DEBRIS_PLAIN);
		}
	links_.count(i);
	links_.shown(i > 0);
	if (i > 0) links_.upload();
}

} // namespace city
