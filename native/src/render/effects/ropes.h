// The wires a blow tore loose: each pinned at its seat on the pole left standing, its loose end thrown
// out from the burst, a chain of points stepped by Verlet with its links kept to their length (a link is only pulled shorter,
// never pushed: a wire goes slack), lying on the ground where it falls. Its end, live while it lies near the ground, sparks and
// now and then flashes blue. Drawn as thin boxes, one a link, in a layer of the debris material, written only while one moves;
// a wire still past its settling time is never written again. Not saved: what the state holds is the poles gone, not their wires.
#pragma once

#include "core/random.h"
#include "core/shader_contracts/effects.h"
#include "render/effects/effect_params.h"
#include "render/effects/mm_layer.h"
#include "render/effects/puffs.h"
#include "render/effects/rigid_step.h"

#include <deque>
#include <vector>

namespace city {

class Ropes {
public:
	// `tint` (sRGB) and `radius`: a wire's (render/props/prop_kit.h WireSeats); `gravity` m/s2, the debris'.
	void init(const godot::RID &scenario, const godot::RID &material, const RopeLook &look, const godot::Color &tint, float radius, float gravity);
	void reset();
	void release();
	// A wire pinned at `from`, its loose end at `to`, thrown at `kick` (m/s, the end's; the points between in proportion).
	void add(const godot::Vector3 &from, const godot::Vector3 &to, const godot::Vector3 &kick);
	// One frame: `ground_of(x, z)` the ground's height; `flash(at)` asked for the blue flash of a live end.
	template <typename G, typename F>
	void update(float dt, G &&ground_of, Puffs &puffs, F &&flash) {
		clock_ += dt;
		while (clock_ >= STEP_S) {
			clock_ -= STEP_S;
			for (Rope &r : ropes_)
				if (r.age < look_.settle_s) {
					step(r, ground_of);
					dirty_ = true;
				}
		}
		for (Rope &r : ropes_) {
			r.age += dt;
			const godot::Vector3 &end = r.p.back();
			if (r.age >= r.live || end.y > ground_of(end.x, end.z) + look_.crackle_near_m || rng_.f() >= dt * look_.crackle_hz) continue;
			puffs.burst(end.x, end.y, end.z, look_.crackle_count, PUFF_SPARK, look_.crackle_spread_m, look_.crackle_size_m, look_.crackle_life_s);
			if (rng_.f() < look_.flash_chance) flash(end);
		}
		flush();
	}
	bool moving() const;
	int count() const { return int(ropes_.size()); }
	static constexpr float STEP_S = RIGID_STEP_S;

private:
	struct Rope {
		std::vector<godot::Vector3> p, o; // the points now and a step ago
		godot::Vector3 pin;
		float link = 0, age = 0, live = 0; // a link's length; seconds since it tore; how long its end stays live
	};
	template <typename G>
	void step(Rope &r, G &&ground_of) {
		const float drop = gravity_ * STEP_S * STEP_S;
		for (size_t k = 0; k < r.p.size(); k++) {
			const godot::Vector3 v = (r.p[k] - r.o[k]) * look_.damp;
			r.o[k] = r.p[k];
			r.p[k] += v - godot::Vector3(0, drop, 0);
		}
		for (int it = 0; it < look_.passes; it++) {
			r.p[0] = r.pin;
			for (size_t k = 1; k < r.p.size(); k++) pull(r.p[k - 1], r.p[k], r.link, k == 1);
			for (size_t k = 1; k < r.p.size(); k++) {
				godot::Vector3 &v = r.p[k];
				const float g = ground_of(v.x, v.z) + look_.lift_m;
				if (v.y >= g) continue;
				v.y = g;
				r.o[k].x = v.x - (v.x - r.o[k].x) * look_.friction;
				r.o[k].z = v.z - (v.z - r.o[k].z) * look_.friction;
			}
		}
	}
	// A link `a`-`b` longer than `link` pulled to it: the pinned end's neighbour alone, else both halfway.
	static void pull(godot::Vector3 &a, godot::Vector3 &b, float link, bool pinned);
	void flush();
	RopeLook look_;
	godot::Color tint_;
	float radius_ = 0, gravity_ = 0;
	std::deque<Rope> ropes_;
	MmLayer links_;
	static constexpr uint64_t WIRES_STREAM = 0x726f7065ULL; // the wires' own stream
	Rng rng_{ WIRES_STREAM };
	float clock_ = 0;
	bool dirty_ = false;
};

} // namespace city
