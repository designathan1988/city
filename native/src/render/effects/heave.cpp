// The ground a crater heaved (heave.h): its plates and slabs drawn from the crater's seed, laid at rest.
#include "render/effects/heave.h"

#include "core/math.h"
#include "core/random.h"
#include "core/shader_contracts/effects.h"
#include "render/effects/ground_cells.h"
#include "render/effects/marks.h"
#include "render/mesh_builder.h"

#include <godot_cpp/classes/box_mesh.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int FIRST_ROOM = 256; // pieces a layer holds before it grows
constexpr uint64_t HEAVE_SALT = 0x68656176ULL; // the crater's seed, as this stream's
constexpr int SALT_SHIFT = 32; // the salt over the seed's 32 bits
constexpr float HALF = 0.5f;
const Vector3 UP(0, 1, 0);
const Color WHITE(1, 1, 1);

// The plate: its jagged outline (`radii`, a corner each, evenly round) extruded one unit along y about its middle, the edges of
// both faces bevelled `bevel` in. Flat faces, white: the instance tints it.
Ref<Mesh> plate_mesh(const std::vector<float> &radii, float bevel) {
	MeshBuilder b;
	const int n = int(radii.size());
	std::vector<Vector3> outer(radii.size()), inner(radii.size());
	for (int k = 0; k < n; k++) {
		const float a = TAU_F * float(k) / float(n);
		const Vector3 d(std::cos(a), 0, std::sin(a));
		outer[size_t(k)] = d * radii[size_t(k)];
		inner[size_t(k)] = d * std::max(0.f, radii[size_t(k)] - bevel);
	}
	const Vector3 lo(0, -HALF + bevel, 0), hi(0, HALF - bevel, 0), bottom(0, -HALF, 0), top(0, HALF, 0);
	for (int k = 0; k < n; k++) {
		const size_t i = size_t(k), j = size_t((k + 1) % n);
		const Vector3 side = (outer[i] + outer[j]).normalized();
		b.quad(outer[i] + lo, outer[j] + lo, outer[j] + hi, outer[i] + hi, WHITE, side);
		b.quad(outer[i] + hi, outer[j] + hi, inner[j] + top, inner[i] + top, WHITE, (side + UP).normalized());
		b.quad(outer[i] + lo, outer[j] + lo, inner[j] + bottom, inner[i] + bottom, WHITE, (side - UP).normalized());
	}
	for (int face = 0; face < 2; face++) {
		const Vector3 out = face == 0 ? UP : -UP, at = face == 0 ? top : bottom;
		const int centre = b.add(at, out, WHITE);
		std::vector<int> ring(radii.size());
		for (int k = 0; k < n; k++) ring[size_t(k)] = b.add(inner[size_t(k)] + at, out, WHITE);
		for (int k = 0; k < n; k++) b.tri(centre, ring[size_t(k)], ring[size_t((k + 1) % n)], out);
	}
	return b.commit();
}

Vector3 radial(float a) { return Vector3(std::cos(a), 0, std::sin(a)); }
// Tipped `angle` about the horizontal way round the crater at the turn `a`: up leans out, away from the hole.
Quaternion tipped(float a, float angle) { return Quaternion(UP.cross(radial(a)).normalized(), angle); }

Color shaded(const Color &c, float k) { return Color(std::min(1.f, c.r * k), std::min(1.f, c.g * k), std::min(1.f, c.b * k)); }
} // namespace

void Heave::init(const RID &scenario, const RID &material, const HeaveLook &look) {
	look_ = look;
	const Ref<Mesh> plate = plate_mesh(look.outline_r, look.bevel_m);
	Ref<BoxMesh> box;
	box.instantiate();
	box->set_size(Vector3(1, 1, 1));
	plates_.init_shaped(scenario, plate, material, FIRST_ROOM, true, plate->get_aabb(), 0.f);
	boxes_.init_shaped(scenario, box, material, FIRST_ROOM, true, box->get_aabb(), 0.f);
	plates_.count(0);
	boxes_.count(0);
	plates_.shown(false);
	boxes_.shown(false);
}

// The state only: the layers are written at the next set_craters (a reset lays every crater again, EffectWorld::reset), and a reset may come
// before init, with no layer to write.
void Heave::reset() {
	pieces_.clear();
	laid_.clear();
	stale_ = false;
}

void Heave::release() {
	plates_.release();
	boxes_.release();
}

void Heave::set_craters(const std::vector<Crater> &list, size_t kept, const CityState &s) {
	if (stale_) kept = 0;
	kept = std::min({ kept, laid_.size(), list.size() });
	if (kept < laid_.size()) {
		pieces_.resize(laid_[kept].first);
		laid_.resize(kept);
	}
	for (size_t i = kept; i < list.size(); i++) lay(list[i], s);
	stale_ = false;
	flush();
}

void Heave::lay(const Crater &c, const CityState &s) {
	const HeaveLook &h = look_;
	const float reach = c.radius * Marks::CRATER_SCALE;
	const bool hard = c.hard != 0;
	Rng rng((HEAVE_SALT << SALT_SHIFT) | uint64_t(c.seed));
	Laid laid;
	laid.x = c.x;
	laid.z = c.z;
	laid.reach = reach * (h.plate_in + h.plate_span) + (h.plate_w_min + h.plate_w_span) * h.scale_most;
	laid.first = pieces_.size();
	const auto place = [&](float a, float r) { return Vector3(c.x + std::cos(a) * r, 0, c.z + std::sin(a) * r); };
	// The slabs of the surface broken at the lip, tipped, sunk a little where they came down.
	const int slabs = int(std::lround(float(h.slabs_base) + reach * h.slabs_per_m));
	for (int i = 0; i < slabs; i++) {
		const float a = TAU_F * float(i) / float(slabs) + rng.f() * h.slab_jitter, r = reach * (h.slab_in + rng.f() * h.slab_span);
		const float side = (h.slab_min_m + rng.f() * h.slab_span_m) * (hard ? 1.f : h.slab_earth_scale);
		Piece p;
		p.size = Vector3(side, hard ? h.slab_thick_hard : h.slab_thick_earth, side * (h.slab_depth_min + rng.f() * h.slab_depth_span));
		p.turn = tipped(a, h.slab_tilt_min + rng.f() * h.slab_tilt_span);
		p.tint = shaded(hard ? (i % h.slab_concrete_every == 0 ? h.concrete : h.asphalt) : h.earth, h.shade_min + rng.f() * h.shade_span);
		p.at = place(a, r);
		const Basis turn(p.turn);
		const float rise = (std::fabs(turn.rows[1][0]) * p.size.x + std::fabs(turn.rows[1][1]) * p.size.y + std::fabs(turn.rows[1][2]) * p.size.z) * HALF;
		p.at.y = GroundCells::surface(s, p.at.x, p.at.z) + rise * (1.f - h.slab_embed);
		pieces_.push_back(p);
	}
	// The plates of the ground thrown up round it, none where earlier craters' plates already crowd.
	int crowding = 0;
	for (size_t i = 0; i < laid.first; i++) {
		const Piece &o = pieces_[i];
		if (o.plate && std::hypot(o.at.x - c.x, o.at.z - c.z) < reach * h.crowd_r) crowding++;
	}
	const int plates = crowding > h.crowded ? 0 : std::min(h.plates_most, int(std::lround(float(h.plates_base) + reach / h.plates_per_m)));
	const float scale = std::min(h.scale_most, h.scale_base + reach / h.scale_grow_m);
	for (int i = 0; i < plates; i++) {
		const float a = rng.f() * TAU_F, r = reach * (h.plate_in + std::sqrt(rng.f()) * h.plate_span);
		const float w = (h.plate_w_min + rng.f() * h.plate_w_span) * scale;
		Piece p;
		p.plate = true;
		p.size = Vector3(w, hard ? h.plate_thick_hard : h.plate_thick_earth, w * (h.plate_depth_min + rng.f() * h.plate_depth_span));
		p.turn = tipped(a, h.tilt_min + rng.f() * h.tilt_span) * Quaternion(UP, rng.f() * PI_F);
		const Color base = hard ? (rng.f() < h.concrete_share ? h.concrete : h.asphalt) : h.earth;
		p.tint = shaded(base.lerp(h.dirt, h.dirt_min + rng.f() * h.dirt_span), h.shade_min + rng.f() * h.shade_span);
		p.at = place(a, r);
		const float g = GroundCells::surface(s, p.at.x, p.at.z);
		p.at.y = g + w * h.plate_lift;
		pieces_.push_back(p);
		if (i % h.clod_every != 0) continue;
		Piece clod; // earth and grit thrown up beside it
		clod.size = Vector3(h.clod_min_m + rng.f() * h.clod_span_m, h.clod_h_m, h.clod_min_m + rng.f() * h.clod_span_m);
		clod.tint = shaded(h.earth, h.shade_min + rng.f() * h.shade_span);
		clod.at = Vector3(p.at.x, g + h.clod_h_m * HALF, p.at.z);
		pieces_.push_back(clod);
	}
	laid_.push_back(laid);
}

void Heave::flush() {
	int np = 0, nb = 0;
	for (const Piece &p : pieces_) (p.plate ? np : nb)++;
	plates_.reserve(np);
	boxes_.reserve(nb);
	plates_.clear_bounds();
	boxes_.clear_bounds();
	np = nb = 0;
	for (const Piece &p : pieces_) {
		const Basis turn(p.turn);
		MmLayer &layer = p.plate ? plates_ : boxes_;
		int &n = p.plate ? np : nb;
		layer.put(n++, turn.get_column(0) * p.size.x, turn.get_column(1) * p.size.y, turn.get_column(2) * p.size.z, p.at, p.tint.r, p.tint.g, p.tint.b, DEBRIS_PLAIN);
	}
	for (auto [layer, n] : { std::pair<MmLayer *, int>{ &plates_, np }, std::pair<MmLayer *, int>{ &boxes_, nb } }) {
		layer->count(n);
		layer->shown(n > 0);
		if (n > 0) layer->upload();
	}
}

} // namespace city
