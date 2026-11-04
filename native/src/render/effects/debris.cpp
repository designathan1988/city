// The blast's rigid pieces (debris.h).
#include "render/effects/debris.h"

#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/cylinder_mesh.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float WORLD_HALF_M = 200000.f;
constexpr float WORLD_UP_M = 20000.f;
constexpr float INERTIA_NUM = 12.f; // inverse inertia per unit mass of a box: 12 / (a2 + b2 + c2)
constexpr float INERTIA_MIN = 1e-4f;
constexpr float RANDOM_SPIN_RANGE = 8.f; // rad/s of the random spin a piece is thrown with, either way
constexpr float TINT_MIN = 0.85f, TINT_SPAN = 0.3f; // each piece's shade varies a little
constexpr float CYL_SIDES = 9.f;
constexpr float FLAME_OVER_M = 0.3f; // flames rise from a burning piece
constexpr float SMOKE_OVER_M = 1.0f;
constexpr float SMOKE_SPREAD_M = 0.4f;
constexpr float FLAME_FOLLOW_SHARE = 0.4f; // the flame's size follows the piece's, to this share
} // namespace

void Debris::init(const RID &scenario, const RID &material, const EffectParams &params) {
	params_ = params;
	const AABB world(Vector3(-WORLD_HALF_M, -WORLD_UP_M, -WORLD_HALF_M), Vector3(2 * WORLD_HALF_M, 2 * WORLD_UP_M, 2 * WORLD_HALF_M));
	Ref<BoxMesh> box;
	box.instantiate();
	box->set_size(Vector3(1, 1, 1));
	Ref<CylinderMesh> cyl;
	cyl.instantiate();
	cyl->set_height(1.f);
	cyl->set_top_radius(1.f);
	cyl->set_bottom_radius(1.f);
	cyl->set_radial_segments(int(CYL_SIDES));
	boxes_.init(scenario, box, material, params.debris_max, true, world);
	cylinders_.init(scenario, cyl, material, params.debris_max, true, world);
	boxes_.count(0);
	cylinders_.count(0);
}

void Debris::reset() {
	pieces_.clear();
	dirty_ = true;
	drawn_ = -1;
}

void Debris::release() {
	boxes_.release();
	cylinders_.release();
}

void Debris::add(const PieceSpec &s) {
	Piece b;
	b.p = s.at;
	b.v = s.velocity;
	b.w = s.random_spin ? Vector3((rng_.f() - 0.5f) * RANDOM_SPIN_RANGE, (rng_.f() - 0.5f) * RANDOM_SPIN_RANGE, (rng_.f() - 0.5f) * RANDOM_SPIN_RANGE) : s.spin;
	if (!s.random_spin && s.spin == Vector3()) b.w = Vector3();
	b.size = s.size;
	b.q = s.turn;
	const float shade = TINT_MIN + TINT_SPAN * rng_.f();
	b.tint = Color(std::min(1.f, s.tint.r * shade), std::min(1.f, s.tint.g * shade), std::min(1.f, s.tint.b * shade));
	b.cylinder = s.cylinder;
	b.still = s.still;
	b.asleep = s.still;
	b.burn_until = s.burn_s;
	b.life = s.life_s;
	b.charred = s.burn_s > 0;
	const Vector3 half = s.cylinder ? Vector3(s.size.x, s.size.y * 0.5f, s.size.z) : s.size * 0.5f;
	int k = 0;
	for (int x = -1; x <= 1; x += 2)
		for (int y = -1; y <= 1; y += 2)
			for (int z = -1; z <= 1; z += 2) b.corners[k++] = Vector3(x * half.x, y * half.y, z * half.z);
	const float extent = s.cylinder ? s.size.y * s.size.y + 3.f * s.size.x * s.size.x : s.size.x * s.size.x + s.size.y * s.size.y + s.size.z * s.size.z;
	b.inv_inertia = INERTIA_NUM / std::max(INERTIA_MIN, extent);
	pieces_.push_back(b);
	if (int(pieces_.size()) > params_.debris_max) { // room: the oldest piece at rest that is not laid for good, else the oldest
		auto it = std::find_if(pieces_.begin(), pieces_.end(), [](const Piece &p) { return p.asleep && !p.still && p.burn_until <= 0; });
		pieces_.erase(it == pieces_.end() ? pieces_.begin() : it);
	}
	dirty_ = true;
}

bool Debris::moving() const {
	for (const Piece &p : pieces_)
		if (!p.asleep) return true;
	return false;
}

void Debris::burn(Piece &p, double, float dt, Puffs &puffs) {
	if (p.burn_until <= 0) return;
	p.burn_until -= dt;
	p.carry += params_.burn_fire_rate * dt;
	const float big = std::max(p.size.x, p.size.z) * 0.5f;
	while (p.carry >= 1.f) {
		p.carry -= 1.f;
		puffs.burst(p.p.x, p.p.y + FLAME_OVER_M, p.p.z, 1, params_.flame_kind, big, params_.flame_size + big * FLAME_FOLLOW_SHARE, params_.flame_life);
		if (rng_.f() < params_.column_chance)
			puffs.burst(p.p.x, p.p.y + SMOKE_OVER_M, p.p.z, 1, params_.smoke_kind, SMOKE_SPREAD_M, params_.smoke_size, params_.smoke_life);
	}
	if (p.burn_until <= 0) p.burn_until = 0;
}

void Debris::reap(float dt) {
	if (!std::any_of(pieces_.begin(), pieces_.end(), [](const Piece &p) { return p.life > 0; })) return;
	const size_t before = pieces_.size();
	for (Piece &p : pieces_)
		if (p.life > 0) p.life -= dt;
	pieces_.erase(std::remove_if(pieces_.begin(), pieces_.end(), [](const Piece &p) { return p.life < 0; }), pieces_.end());
	if (pieces_.size() != before) dirty_ = true;
}

void Debris::flush() {
	const bool burning = std::any_of(pieces_.begin(), pieces_.end(), [](const Piece &p) { return p.burn_until > 0; });
	if (!dirty_ && !moving() && !burning && drawn_ == int(pieces_.size())) return;
	int nb = 0, nc = 0;
	for (const Piece &p : pieces_) {
		MmLayer &layer = p.cylinder ? cylinders_ : boxes_;
		int &n = p.cylinder ? nc : nb;
		if (n >= params_.debris_max) continue;
		const Basis rot(p.q);
		layer.put(n++, rot.get_column(0) * p.size.x, rot.get_column(1) * p.size.y, rot.get_column(2) * p.size.z, p.p, p.tint.r, p.tint.g, p.tint.b,
				p.charred ? 1.f : 0.f);
	}
	boxes_.count(nb);
	cylinders_.count(nc);
	boxes_.shown(nb > 0);
	cylinders_.shown(nc > 0);
	if (nb > 0) boxes_.upload();
	if (nc > 0) cylinders_.upload();
	drawn_ = int(pieces_.size());
	dirty_ = false;
}

} // namespace city
