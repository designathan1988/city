// A lot's plants on the ground (see garden_place.h). The sizes are the 4.0's (render/gardenPlace.ts gardenPack), and they are its
// modelling: a shrub fills most of its element, a hedge is clipped pieces that overlap so it reads as one, a flower bed a scatter of
// low flowering clumps that throw no shadow.
#include "render/trees/garden_place.h"

#include "core/math.h"
#include "core/random.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t GARDEN_SALT = 0x6a4dU;
constexpr float PLANT_FOOT_BELOW = 0.05f; // a plant's foot under the ground
// A tree: how wide against its height, and how much deeper than wide.
constexpr float TREE_SPREAD = 0.82f, TREE_SPREAD_SPAN = 0.3f, TREE_DEEP = 0.9f, TREE_DEEP_SPAN = 0.2f;
// A shrub: the share of its element it fills, how tall it may be against that, and how many flower. The model's span is the crown's
// in streets.json (species shrub), its width for a height of one.
constexpr float SHRUB_FILL = 0.8f, SHRUB_TALL = 1.3f, SHRUB_FLOWERING = 0.25f, SHRUB_SPAN = 1.24f;
// A hedge: the most a clipped piece runs, against the least and its height, the overlap and the model's length and depth.
constexpr float HEDGE_LEAST_M = 2.2f, HEDGE_PER_H = 1.5f, HEDGE_LONG = 1.9f, HEDGE_OVERLAP = 1.25f, HEDGE_DEEP = 0.9f;
// A flower bed: the most one piece runs, the share of the bed it covers, how tall it is and how far it stands out of its soil.
constexpr float BED_PIECE_M = 2.4f, BED_SPREAD = 0.85f, CLUMP_H_M = 0.35f, CLUMP_H_SPAN_M = 0.25f, BED_LIFT_M = 0.12f;
constexpr float HALF = 0.5f;
// A near-white multiplier, so no two plants of a species are alike (plantKinds.ts foliageTint).
constexpr float TINT_R = 0.9f, TINT_R_SPAN = 0.18f, TINT_G = 0.93f, TINT_G_SPAN = 0.12f, TINT_B = 0.86f, TINT_B_SPAN = 0.14f;

// The draws of one plant: each a share, in order.
struct Draws {
	uint32_t h = 0, next = 0;
	float get() { return hash_float(h + next++); }
};

Color foliage_tint(Draws &r) {
	const float warm = r.get();
	return Color(TINT_R + warm * TINT_R_SPAN, TINT_G + r.get() * TINT_G_SPAN, TINT_B + (1.f - warm) * TINT_B_SPAN);
}

int flowering(const TreeLook &T, Draws &r) { return T.flowering_species[size_t(r.get() * float(FLOWERING_KINDS)) % FLOWERING_KINDS]; }

// Where a plant of the garden stands, on the ground under it.
struct Stand {
	const CityState &s;
	const b40::LotFrame f;
	std::vector<TreeInstance> &out;
	Vector3 ground(float x, float y) const {
		const V2 p = f.at(x, y);
		return Vector3(p.x, s.terrain.height_at(p.x, p.y) - PLANT_FOOT_BELOW, p.y);
	}
};

void plant_tree(const Stand &at, const TreeLook &T, const b40::LotElement &p, Draws &r) {
	const float spread = TREE_SPREAD + r.get() * TREE_SPREAD_SPAN;
	const int species = T.park.pick(r.get());
	const float yaw = r.get() * TAU_F, width = p.h * spread, deep = width * (TREE_DEEP + r.get() * TREE_DEEP_SPAN);
	at.out.push_back({ at.ground(p.x, p.y), p.h, width, yaw, 0, 0, species, foliage_tint(r), deep });
}

void plant_shrub(const Stand &at, const TreeLook &T, const b40::LotElement &p, Draws &r) {
	const float across = std::min(p.w, p.d) * SHRUB_FILL / SHRUB_SPAN;
	const float tall = std::min(p.h, across * SHRUB_TALL), wide = std::max(across, tall / SHRUB_TALL);
	const int species = r.get() < SHRUB_FLOWERING ? flowering(T, r) : T.shrub_species;
	const float yaw = r.get() * TAU_F, deep = wide * (TREE_DEEP + r.get() * TREE_DEEP_SPAN);
	at.out.push_back({ at.ground(p.x, p.y), tall, wide, yaw, 0, 0, species, foliage_tint(r), deep });
}

// Clipped pieces along the hedge's run, each on the ground under it.
void plant_hedge(const Stand &at, const TreeLook &T, const b40::LotElement &p, Draws &r) {
	const bool along_x = p.w >= p.d;
	const float run = along_x ? p.w : p.d, thick = along_x ? p.d : p.w;
	const int n = std::max(1, int(std::lround(run / std::max(HEDGE_LEAST_M, p.h * HEDGE_PER_H))));
	const float step = run / float(n);
	const V2 way = along_x ? at.f.e0 : at.f.e1;
	const float yaw = std::atan2(-way.y, way.x); // the instance's x axis (cos, 0, -sin) along the run
	const Color tint = foliage_tint(r);
	for (int i = 0; i < n; i++) {
		const float t = (float(i) + HALF) * step - run * HALF;
		at.out.push_back({ at.ground(p.x + (along_x ? t : 0), p.y + (along_x ? 0 : t)), p.h, step / HEDGE_LONG * HEDGE_OVERLAP, yaw, 0, 0, T.hedge_species, tint, thick / HEDGE_DEEP });
	}
}

// A flower bed in pieces of about a clump's run, each one instance of a bed's model on the ground under its middle: the 4.0's scatter of
// clumps, 24 models a bed, threw 3 million triangles of shadow on the town and tripled the frame's GPU time (2026-11-05).
void plant_bed(const Stand &at, const TreeLook &T, const b40::LotElement &p, Draws &r) {
	const bool along_x = p.w >= p.d;
	const float run = along_x ? p.w : p.d, thick = along_x ? p.d : p.w;
	const int n = std::max(1, int(std::ceil(run / BED_PIECE_M)));
	const float step = run / float(n);
	const V2 way = along_x ? at.f.e0 : at.f.e1;
	const float yaw = std::atan2(-way.y, way.x);
	for (int i = 0; i < n; i++) {
		const float t = (float(i) + HALF) * step - run * HALF;
		Vector3 foot = at.ground(p.x + (along_x ? t : 0), p.y + (along_x ? 0 : t));
		foot.y += PLANT_FOOT_BELOW + BED_LIFT_M;
		const int species = T.bed_species[size_t(r.get() * float(FLOWERING_KINDS)) % FLOWERING_KINDS];
		at.out.push_back({ foot, CLUMP_H_M + r.get() * CLUMP_H_SPAN_M, step * BED_SPREAD, yaw, 0, 0, species, Color(1, 1, 1, 1), thick * BED_SPREAD });
	}
}

bool is_plant(b40::Element kind) { return kind == b40::EL_TREE || kind == b40::EL_SHRUB || kind == b40::EL_HEDGE || kind == b40::EL_FLOWERS; }
} // namespace

void place_garden(const CityState &s, const TreeLook &T, const Building &rec, GardenView view, std::vector<TreeInstance> &out) {
	const std::shared_ptr<const b40::Body> body = b40::record_body(s.buildings.designs, rec);
	const Stand at{ s, b40::lot_frame(rec), out };
	uint32_t k = 0; // the plant's index among the body's plants: its draws
	for (const b40::LotElement &p : body->elements) {
		if (!is_plant(p.kind)) continue;
		Draws r{ hash_combine(rec.seed, k++ + GARDEN_SALT) };
		switch (p.kind) {
			case b40::EL_TREE: plant_tree(at, T, p, r); break;
			case b40::EL_SHRUB: plant_shrub(at, T, p, r); break;
			case b40::EL_HEDGE: plant_hedge(at, T, p, r); break;
			default:
				if (view == GardenView::NEAR) plant_bed(at, T, p, r);
				break;
		}
	}
}

} // namespace city
