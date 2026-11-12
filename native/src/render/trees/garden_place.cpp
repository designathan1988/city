// A lot's plants on the ground (see garden_place.h). Every plant is sized by what its species' model draws (the kit's measure,
// drawn_body.h), so the body drawn is the element's: a tree's crown and a shrub stand inside their room whatever their turn; a
// hedge is clipped pieces from end to end of its run, as thick and as tall as it; a flower bed is pieces of low flowering clumps
// over its whole patch, which throw no shadow.
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
// A tree: the share of its room its crown takes, and its depth against its width.
constexpr float TREE_FILL = 0.86f, TREE_FILL_SPAN = 0.14f, TREE_DEEP = 0.88f, TREE_DEEP_SPAN = 0.12f;
// A shrub: the share of its room it takes, how tall it may stand against how wide it is drawn, and how many flower.
constexpr float SHRUB_FILL = 0.8f, SHRUB_TALL = 1.f, SHRUB_FLOWERING = 0.25f;
// A run's pieces (a hedge, a flower bed): a piece's faces reach this share of their length into the next piece's, so the run is one
// body; and the least part of a step that asks for one more piece.
constexpr float PIECE_JOIN = 0.96f, PIECE_ROUND = 1e-3f;
// A flower bed: its soil's top over the ground (the clumps stand on it), the share of the bed's height a clump reaches, and the
// flattest a clump lies, its height against its width (where its flowers alone are as tall as the bed).
constexpr float BED_LIFT_M = 0.12f, BED_TALL = 0.75f, BED_TALL_SPAN = 0.25f, BED_FLAT = 0.15f;
constexpr float HALF = 0.5f;
// A near-white multiplier, so no two plants of a species are alike.
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

// Where a plant of the garden stands, on the ground under it, and what its species draws.
struct Stand {
	const CityView &s;
	const TreeKit &kit;
	const b40::LotFrame f;
	std::vector<TreeInstance> &out;
	Vector3 ground(float x, float y) const {
		const V2 p = f.at(x, y);
		return Vector3(p.x, s.terrain.height_at(p.x, p.y) - PLANT_FOOT_BELOW, p.y);
	}
	// A plant's foot: on the ground under its middle, or as far over it as its element stands (a plant in a planter, on the planter's soil).
	Vector3 foot(const b40::LotElement &p) const {
		Vector3 at = ground(p.x, p.y);
		at.y += p.z;
		return at;
	}
	const DrawnBody &drawn(int species) const { return kit.species(species).drawn; }
	// An instance with no size along one of its axes draws nothing and gives the shader no basis: it is not made.
	void add(const TreeInstance &t) const {
		if (t.width > 0 && t.height > 0 && t.depth > 0) out.push_back(t);
	}
};

// The width at which a model, `flat` times as deep as wide, reaches `reach` from its axis: what a plant that stands alone in its
// room, whatever its turn, is as wide as.
float round_width(const DrawnBody &m, float reach, float flat) { return reach / m.around(1.f, flat); }

// A tree stands in the room the lot's layout gave it: its crown, leaves and all, within half the room of its trunk, so it keeps
// off the fence, the footway and the walls the layout kept the room clear of; no taller than its element.
void plant_tree(const Stand &at, const TreeLook &T, const b40::LotElement &p, Draws &r) {
	const float fill = TREE_FILL + r.get() * TREE_FILL_SPAN;
	const int species = T.park.pick(r.get());
	const float yaw = r.get() * TAU_F, flat = TREE_DEEP + r.get() * TREE_DEEP_SPAN;
	const DrawnBody &m = at.drawn(species);
	const float width = round_width(m, std::min(p.w, p.d) * HALF * fill, flat);
	at.add({ at.foot(p), std::min(p.h, m.tall_within(width, p.h)), width, yaw, 0, 0, species, foliage_tint(r), width * flat });
}

void plant_shrub(const Stand &at, const TreeLook &T, const b40::LotElement &p, Draws &r) {
	const int species = r.get() < SHRUB_FLOWERING ? flowering(T, r) : T.shrub_species;
	const float yaw = r.get() * TAU_F, flat = TREE_DEEP + r.get() * TREE_DEEP_SPAN;
	const DrawnBody &m = at.drawn(species);
	const float reach = std::min(p.w, p.d) * HALF * SHRUB_FILL, wide = round_width(m, reach, flat);
	const float tall = m.tall_within(wide, std::min(p.h, 2 * reach * SHRUB_TALL));
	at.add({ at.foot(p), tall, wide, yaw, 0, 0, species, foliage_tint(r), wide * flat });
}

// The pieces of a run (a hedge, a flower bed) in a room `run` long and `thick` across: each the model in its own plan proportions,
// as deep as the room is thick (in a room shorter than such a piece, one piece as long as the room); the first and the last drawn
// up to the room's ends, the others between them at equal steps, their faces lapping.
struct Pieces {
	int n = 0;
	float sx = 0, sz = 0; // a piece's scales along the run and across it
	float first = 0, step = 0; // the first piece's middle along the run, from the run's middle, and the step to the next
};
Pieces run_pieces(const DrawnBody &m, float run, float thick) {
	Pieces q;
	q.sx = std::min(thick * HALF / m.half_z(1.f, 1.f), run / (m.back_x(1.f) + m.fore_x(1.f)));
	q.sz = m.deep_within(q.sx, thick * HALF);
	const float back = m.back_x(q.sx), fore = m.fore_x(q.sx);
	const float span = std::max(0.f, run - back - fore); // from the first piece's middle to the last's
	const float lap = 2 * (m.faces_x() > 0 ? m.faces_x() : m.half_x(1.f)) * q.sx * PIECE_JOIN;
	q.n = 1 + int(std::ceil(span / lap - PIECE_ROUND));
	q.first = back - run * HALF;
	q.step = q.n > 1 ? span / float(q.n - 1) : 0;
	return q;
}

// A run's pieces stood along it: `piece(k)` gives the k-th one's height.
template <typename F>
void stand_run(const Stand &at, const b40::LotElement &p, const Pieces &q, int species, const Color &tint, float lift, F &&piece) {
	const bool along_x = p.w >= p.d;
	const V2 way = along_x ? at.f.e0 : at.f.e1;
	const float yaw = std::atan2(-way.y, way.x); // the instance's x axis (cos, 0, -sin) along the run
	for (int k = 0; k < q.n; k++) {
		const float t = q.first + float(k) * q.step;
		Vector3 foot = at.ground(p.x + (along_x ? t : 0), p.y + (along_x ? 0 : t));
		foot.y += lift + p.z;
		at.add({ foot, piece(k), q.sx, yaw, 0, 0, species, tint, q.sz });
	}
}

// Clipped pieces along the hedge's run, each on the ground under it: the hedge as thick and as tall as its element.
void plant_hedge(const Stand &at, const TreeLook &T, const b40::LotElement &p, Draws &r) {
	const DrawnBody &m = at.drawn(T.hedge_species);
	const Pieces q = run_pieces(m, std::max(p.w, p.d), std::min(p.w, p.d));
	const float tall = m.tall_within(q.sx, p.h);
	stand_run(at, p, q, T.hedge_species, foliage_tint(r), 0, [tall](int) { return tall; });
}

// A flower bed in pieces of clumps, each one instance of a bed's model on the bed's soil: the patch covered from edge to edge, each
// clump its own height under the bed's. (A scatter of single clumps, 24 models a bed, threw 3 million triangles of shadow on the town
// and tripled the frame's GPU time, 2026-11-05: a bed's pieces throw none.)
void plant_bed(const Stand &at, const TreeLook &T, const b40::LotElement &p, Draws &r) {
	const int species = T.bed_species[size_t(r.get() * float(FLOWERING_KINDS)) % FLOWERING_KINDS];
	const DrawnBody &m = at.drawn(species);
	const Pieces q = run_pieces(m, std::max(p.w, p.d), std::min(p.w, p.d));
	// A bed on the ground stands on its own soil, over the ground; one in a planter on the planter's (its element's z), its foot just under it.
	const float soil = p.z > 0 ? 0.f : BED_LIFT_M, lift = p.z > 0 ? 0.f : PLANT_FOOT_BELOW + BED_LIFT_M;
	stand_run(at, p, q, species, Color(1, 1, 1, 1), lift,
			[&](int) { return std::max(q.sx * BED_FLAT, m.tall_within(q.sx, p.h * (BED_TALL + r.get() * BED_TALL_SPAN) - soil)); });
}
} // namespace

bool planted_kind(b40::Element kind) { return b40::is_plant(kind); }

PlantRoom plant_room(const b40::LotFrame &f, const b40::LotElement &p) {
	if (p.angle == 0) return { f, p };
	const float c = std::cos(p.angle), s = std::sin(p.angle);
	b40::LotElement local = p;
	local.x = local.y = local.angle = 0;
	return { b40::LotFrame{ f.at(p.x, p.y), f.e0 * c + f.e1 * s, f.e1 * c - f.e0 * s }, local };
}

void place_plant(const CityView &s, const TreeLook &T, const TreeKit &kit, const b40::LotFrame &f, const b40::LotElement &element, uint32_t draws,
		GardenView view, std::vector<TreeInstance> &out) {
	const PlantRoom room = plant_room(f, element);
	const b40::LotElement &p = room.p;
	const Stand at{ s, kit, room.f, out };
	Draws r{ draws };
	switch (p.kind) {
		case b40::EL_TREE: plant_tree(at, T, p, r); break;
		case b40::EL_SHRUB: plant_shrub(at, T, p, r); break;
		case b40::EL_HEDGE: plant_hedge(at, T, p, r); break;
		case b40::EL_FLOWERS:
			if (view == GardenView::NEAR) plant_bed(at, T, p, r);
			break;
		default: break;
	}
}

void each_garden_plant(const CityView &s, const Building &rec, const std::function<void(const b40::LotElement &, uint32_t)> &fn) {
	const std::shared_ptr<const std::vector<b40::LotElement>> elements = b40::record_elements(s.designs, rec);
	uint32_t k = 0; // the plant's index among the body's plants: its draws
	for (const b40::LotElement &p : *elements)
		if (planted_kind(p.kind)) fn(p, hash_combine(rec.seed, k++ + GARDEN_SALT));
}

void place_garden(const CityView &s, const TreeLook &T, const TreeKit &kit, const Building &rec, GardenView view, std::vector<TreeInstance> &out) {
	const b40::LotFrame f = b40::lot_frame(rec);
	each_garden_plant(s, rec, [&](const b40::LotElement &p, uint32_t draws) { place_plant(s, T, kit, f, p, draws, view, out); });
}

} // namespace city
