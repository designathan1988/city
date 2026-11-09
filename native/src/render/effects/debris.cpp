// The blast's rigid pieces (debris.h).
#include "render/effects/debris.h"
#include "core/shader_contracts/effects.h"

#include "core/math.h"

#include "render/surface_data.h"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/cylinder_mesh.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float CYL_SIDES = 9.f; // a thrown pole's or trunk's sides

// A broken plate of the ground (effects.json shapes.crater_slabs): a jagged outline in the xz plane, its sides a unit thick
// along y and pushed out by the bevel's width, its faces bevelled back to the outline: flat-shaded triangles.
Array slab_mesh(const CraterSlabsShape &K) {
	const int n = std::max(3, K.outline_corners);
	std::vector<Vector3> rim(static_cast<size_t>(n)); // the outline, at unit radius scale
	for (int k = 0; k < n; k++) {
		const float a = float(k) / float(n) * TAU_F;
		const float r = K.outline_r + K.outline_wave * std::sin(float(k) * K.outline_wave_k + K.outline_wave_phase) +
				(K.outline_notch_every > 0 && k % K.outline_notch_every == 0 ? -K.outline_notch : K.outline_bulge);
		rim[size_t(k)] = Vector3(std::cos(a) * r, 0.f, std::sin(a) * r);
	}
	const float side = 0.5f, face = side + K.bevel_thick; // the sides' half height and the faces' (the bevel stands past the sides)
	PackedVector3Array verts, normals;
	const auto tri = [&](const Vector3 &a, const Vector3 &b, const Vector3 &c) {
		Vector3 nrm = (b - a).cross(c - a).normalized();
		if (nrm.dot((a + b + c) / 3.f) < 0.f) nrm = -nrm; // outward: the plate is star-shaped about its centre
		for (const Vector3 &v : { a, b, c }) {
			verts.push_back(v);
			normals.push_back(nrm);
		}
	};
	const auto quad = [&](const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d) {
		tri(a, b, c);
		tri(a, c, d);
	};
	for (int k = 0; k < n; k++) {
		const Vector3 p0 = rim[size_t(k)], p1 = rim[size_t((k + 1) % n)];
		const Vector3 q0 = p0 * (1.f + K.bevel_size / std::max(p0.length(), K.bevel_size)), q1 = p1 * (1.f + K.bevel_size / std::max(p1.length(), K.bevel_size));
		const Vector3 up(0, face, 0), down(0, -face, 0), hi(0, side, 0), lo(0, -side, 0);
		tri(up, p0 + up, p1 + up); // the top face
		quad(p0 + up, q0 + hi, q1 + hi, p1 + up); // its bevel
		quad(q0 + hi, q0 + lo, q1 + lo, q1 + hi); // the broken side
		quad(q0 + lo, p0 + down, p1 + down, q1 + lo); // the lower bevel
		tri(down, p1 + down, p0 + down); // the bottom face
	}
	Array arrays;
	arrays.resize(Mesh::ARRAY_MAX);
	arrays[Mesh::ARRAY_VERTEX] = verts;
	arrays[Mesh::ARRAY_NORMAL] = normals;
	return arrays;
}
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
	// each shape made a mesh with the material (surface_data.h): the engine has its pipelines before a blast throws the first piece
	boxes_.init_shaped(scenario, OwnedMesh::of(mesh_of_arrays(box->get_mesh_arrays(), material)), params.debris_max, true, box->get_aabb(), 0.f);
	cylinders_.init_shaped(scenario, OwnedMesh::of(mesh_of_arrays(cyl->get_mesh_arrays(), material)), params.debris_max, true, cyl->get_aabb(), 0.f);
	const Array slab = slab_mesh(params.shapes.crater_slabs);
	slabs_.init_shaped(scenario, OwnedMesh::of(mesh_of_arrays(slab, material)), params.debris_max, true, arrays_box(slab), 0.f);
	boxes_.count(0);
	cylinders_.count(0);
	slabs_.count(0);
}

void Debris::set_wrecks(const RID &scenario, const RID &material, const std::vector<const MeshBuilder *> &bodies) {
	const DebrisShape &K = params_.shapes.debris;
	wrecks_.resize(bodies.size());
	for (size_t i = 0; i < bodies.size(); i++) {
		wrecks_[i].init_shaped(scenario, bodies[i]->bake(material), K.wrecks_kept, true, bodies[i]->bounds(), 0.f);
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
	slabs_.release();
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
	b.slab = s.slab && !s.cylinder;
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

int Debris::still_near(float x, float z, float reach) const {
	int n = 0;
	for (const Piece &p : pieces_)
		if (p.still && std::hypot(p.p.x - x, p.p.z - z) < reach) n++;
	return n;
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
	int nb = 0, nc = 0, ns = 0;
	std::vector<int> nw(wrecks_.size(), 0);
	boxes_.clear_bounds(); // every piece is written again, where the steps left it: the boxes are theirs
	cylinders_.clear_bounds();
	slabs_.clear_bounds();
	for (MmLayer &l : wrecks_) l.clear_bounds();
	for (const Piece &p : pieces_) {
		const Basis rot(p.q);
		const float mark = p.charred ? DEBRIS_CHARRED : p.dusty ? DEBRIS_DUSTY : DEBRIS_PLAIN; // debris.gdshader
		if (p.wreck >= 0 && size_t(p.wreck) < wrecks_.size()) {
			int &k = nw[size_t(p.wreck)];
			if (k < K.wrecks_kept) wrecks_[size_t(p.wreck)].put(k++, rot.get_column(0), rot.get_column(1), rot.get_column(2), p.p, p.tint.r, p.tint.g, p.tint.b, mark);
			continue;
		}
		MmLayer &layer = p.cylinder ? cylinders_ : p.slab ? slabs_ : boxes_;
		int &n = p.cylinder ? nc : p.slab ? ns : nb;
		if (n >= params_.debris_max) continue;
		layer.put(n++, rot.get_column(0) * p.size.x, rot.get_column(1) * p.size.y, rot.get_column(2) * p.size.z, p.p, p.tint.r, p.tint.g, p.tint.b, mark);
	}
	for (size_t i = 0; i < wrecks_.size(); i++) {
		wrecks_[i].count(nw[i]);
		wrecks_[i].shown(nw[i] > 0);
		if (nw[i] > 0) wrecks_[i].upload();
	}
	boxes_.count(nb);
	cylinders_.count(nc);
	slabs_.count(ns);
	boxes_.shown(nb > 0);
	cylinders_.shown(nc > 0);
	slabs_.shown(ns > 0);
	if (nb > 0) boxes_.upload();
	if (nc > 0) cylinders_.upload();
	if (ns > 0) slabs_.upload();
	drawn_ = int(pieces_.size());
	dirty_ = false;
}

} // namespace city
