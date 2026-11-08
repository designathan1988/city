// The blast's rigid pieces (debris.h).
#include "render/effects/debris.h"
#include "core/shader_contracts/effects.h"

#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/cylinder_mesh.hpp>
#include <godot_cpp/classes/sphere_mesh.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float CYL_SIDES = 9.f; // a thrown pole's or trunk's sides
constexpr int ROUND_SIDES = 10, ROUND_RINGS = 7; // an organ is a sphere of ten by seven
constexpr float UNIT_RADIUS = 0.5f; // a round one across, as the unit box
} // namespace

void Debris::init(const RID &scenario, const RID &material, const EffectParams &params) {
	params_ = params;
	Ref<BoxMesh> box;
	box.instantiate();
	box->set_size(Vector3(1, 1, 1));
	Ref<CylinderMesh> cyl;
	cyl.instantiate();
	cyl->set_height(1.f);
	cyl->set_top_radius(1.f);
	cyl->set_bottom_radius(1.f);
	cyl->set_radial_segments(int(CYL_SIDES));
	// debris.gdshader moves no vertex: a piece's box is its shape's, where it lies
	boxes_.init_shaped(scenario, box, material, params.debris_max, true, box->get_aabb(), 0.f);
	cylinders_.init_shaped(scenario, cyl, material, params.debris_max, true, cyl->get_aabb(), 0.f);
	Ref<SphereMesh> ball;
	ball.instantiate();
	ball->set_radius(UNIT_RADIUS);
	ball->set_height(1.f);
	ball->set_radial_segments(ROUND_SIDES);
	ball->set_rings(ROUND_RINGS);
	rounds_.init_shaped(scenario, ball, material, params.debris_max, true, ball->get_aabb(), 0.f);
	boxes_.count(0);
	cylinders_.count(0);
	rounds_.count(0);
}

void Debris::set_wrecks(const RID &scenario, const RID &material, const std::vector<Ref<Mesh>> &bodies) {
	const DebrisShape &K = params_.shapes.debris;
	wrecks_.resize(bodies.size());
	for (size_t i = 0; i < bodies.size(); i++) {
		wrecks_[i].init_shaped(scenario, bodies[i], material, K.wrecks_kept, true, bodies[i]->get_aabb(), 0.f);
		wrecks_[i].count(0);
	}
}

void Debris::reset() {
	pieces_.clear();
	dirty_ = true;
	drawn_ = -1;
}

void Debris::release() {
	boxes_.release();
	cylinders_.release();
	rounds_.release();
	for (MmLayer &l : wrecks_) l.release();
}

void Debris::add(const PieceSpec &s) {
	const DebrisShape &K = params_.shapes.debris;
	Piece b;
	b.p = s.at;
	b.v = s.velocity;
	b.w = s.random_spin ? Vector3((rng_.f() - 0.5f) * K.random_spin_range, (rng_.f() - 0.5f) * K.random_spin_range, (rng_.f() - 0.5f) * K.random_spin_range) : s.spin;
	if (!s.random_spin && s.spin == Vector3()) b.w = Vector3();
	b.size = s.size;
	b.q = s.turn;
	const float shade = K.tint_min + K.tint_span * rng_.f();
	b.tint = Color(std::min(1.f, s.tint.r * shade), std::min(1.f, s.tint.g * shade), std::min(1.f, s.tint.b * shade));
	b.cylinder = s.cylinder;
	b.round = s.round;
	b.still = s.still;
	b.asleep = s.still;
	b.burn_until = s.burn_s;
	b.life = s.life_s;
	b.charred = s.burn_s > 0;
	b.dusty = s.dusty;
	b.wreck = s.wreck;
	const Vector3 half = s.cylinder ? Vector3(s.size.x, s.size.y * 0.5f, s.size.z) : s.size * 0.5f;
	const float extent = s.cylinder ? s.size.y * s.size.y + 3.f * s.size.x * s.size.x : s.size.x * s.size.x + s.size.y * s.size.y + s.size.z * s.size.z;
	b.shape(half, extent);
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
	const DebrisShape &K = params_.shapes.debris;
	if (p.burn_until <= 0) return;
	p.burn_until -= dt;
	p.carry += params_.burn_fire_rate * dt;
	const float big = std::max(p.size.x, p.size.z) * 0.5f;
	while (p.carry >= 1.f) {
		p.carry -= 1.f;
		puffs.burst(p.p.x, p.p.y + K.flame_over_m, p.p.z, 1, params_.flame_kind, big, params_.flame_size + big * K.flame_follow_share, params_.flame_life);
		if (rng_.f() < params_.column_chance)
			puffs.burst(p.p.x, p.p.y + K.smoke_over_m, p.p.z, 1, params_.smoke_kind, K.smoke_spread_m, params_.smoke_size, params_.smoke_life);
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
	const DebrisShape &K = params_.shapes.debris;
	const bool burning = std::any_of(pieces_.begin(), pieces_.end(), [](const Piece &p) { return p.burn_until > 0; });
	if (!dirty_ && !moving() && !burning && drawn_ == int(pieces_.size())) return;
	int nb = 0, nc = 0, nr = 0;
	std::vector<int> nw(wrecks_.size(), 0);
	boxes_.clear_bounds(); // every piece is written again, where the steps left it: the boxes are theirs
	cylinders_.clear_bounds();
	rounds_.clear_bounds();
	for (MmLayer &l : wrecks_) l.clear_bounds();
	for (const Piece &p : pieces_) {
		const Basis rot(p.q);
		const float mark = p.charred ? DEBRIS_CHARRED : p.dusty ? DEBRIS_DUSTY : DEBRIS_PLAIN; // debris.gdshader
		if (p.wreck >= 0 && size_t(p.wreck) < wrecks_.size()) {
			int &k = nw[size_t(p.wreck)];
			if (k < K.wrecks_kept) wrecks_[size_t(p.wreck)].put(k++, rot.get_column(0), rot.get_column(1), rot.get_column(2), p.p, p.tint.r, p.tint.g, p.tint.b, mark);
			continue;
		}
		MmLayer &layer = p.round ? rounds_ : p.cylinder ? cylinders_ : boxes_;
		int &n = p.round ? nr : p.cylinder ? nc : nb;
		if (n >= params_.debris_max) continue;
		layer.put(n++, rot.get_column(0) * p.size.x, rot.get_column(1) * p.size.y, rot.get_column(2) * p.size.z, p.p, p.tint.r, p.tint.g, p.tint.b, mark);
	}
	for (size_t i = 0; i < wrecks_.size(); i++) {
		wrecks_[i].count(nw[i]);
		wrecks_[i].shown(nw[i] > 0);
		if (nw[i] > 0) wrecks_[i].upload();
	}
	for (auto [layer, n] : { std::pair<MmLayer *, int>{ &boxes_, nb }, std::pair<MmLayer *, int>{ &cylinders_, nc }, std::pair<MmLayer *, int>{ &rounds_, nr } }) {
		layer->count(n);
		layer->shown(n > 0);
		if (n > 0) layer->upload();
	}
	drawn_ = int(pieces_.size());
	dirty_ = false;
}

} // namespace city
