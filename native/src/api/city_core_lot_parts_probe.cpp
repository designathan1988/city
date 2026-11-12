// CityProbes::lot_parts: the lots' layout held on bodies made on the spot, with no city: `count` bodies of a zone on a lot `w` by
// `d`, from `seed` on, each as the generator makes a record's (sim/buildings/b40/gen.h make_body), and what their lots hold: every
// planter with what grows in it (none bare, each plant standing on its planter's soil), and every part stood on the ground against
// what the building holds over it (sim/buildings/b40/body_air.h: none as tall as an eave, a balcony or an upper block over it).
#include "api/city_probes.h"

#include "sim/buildings/b40/body_air.h"
#include "sim/buildings/b40/gen.h"
#include "sim/state/city_state.h"

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f;

// What grows in the planter: the plant whose middle lies over it, or null.
const b40::LotElement *grown_in(const b40::Body &b, const b40::LotElement &planter) {
	for (const b40::LotElement &q : b.elements)
		if (b40::is_plant(q.kind) && std::fabs(q.x - planter.x) < planter.w * HALF && std::fabs(q.y - planter.y) < planter.d * HALF) return &q;
	return nullptr;
}

// A part the layout stands on the lot's ground or in a planter: what its rule keeps out of the building's air (not what is hung on
// the building or stands on its wall, not the boundary, not what lies flat).
bool stands(const b40::LotElement &e) {
	switch (e.kind) {
		case b40::EL_TREE: case b40::EL_SHRUB: case b40::EL_FLOWERS: case b40::EL_PLANTER: case b40::EL_BENCH: case b40::EL_BOLLARD: case b40::EL_BIN:
		case b40::EL_LOUNGER: case b40::EL_TABLE: case b40::EL_SWING: case b40::EL_SHED: case b40::EL_LINE: case b40::EL_PALLET: case b40::EL_CAGE:
			return true;
		case b40::EL_LAMP: return e.z == 0; // on its post: one on a wall is the building's
		default: return false;
	}
}
} // namespace

Dictionary CityProbes::lot_parts(int zone, int cap, float w, float d, int seed, int count) const {
	int planters = 0, bare = 0, off_soil = 0, standing = 0, under_body = 0, trees = 0, posts = 0;
	Array first_bare, first_off, first_under;
	for (int k = 0; k < count; k++) {
		b40::Body b;
		b40::make_body(state_->buildings.designs, b, zone, cap, w, d, uint32_t(seed + k), b40::LotEdges());
		const std::vector<b40::Overhang> air = b40::body_overhangs(b);
		for (const b40::LotElement &p : b.elements) {
			if (!stands(p)) continue;
			standing++;
			trees += p.kind == b40::EL_TREE ? 1 : 0;
			posts += p.kind == b40::EL_LAMP ? 1 : 0;
			const float top = p.z + p.h;
			for (const b40::Overhang &o : air) {
				if (top <= o.under + b40::EPS || !o.over(p.x - p.w * HALF, p.y - p.d * HALF, p.x + p.w * HALF, p.y + p.d * HALF)) continue;
				under_body++;
				if (first_under.is_empty()) first_under = Array::make(seed + k, int(p.kind), p.x, p.y, p.w, top, o.under);
				break;
			}
		}
		for (const b40::LotElement &p : b.elements) {
			if (p.kind != b40::EL_PLANTER) continue;
			planters++;
			const b40::LotElement *grown = grown_in(b, p);
			if (!grown) {
				bare++;
				if (first_bare.is_empty()) first_bare = Array::make(seed + k, p.x, p.y, p.w, p.d, p.h);
			} else if (std::fabs(grown->z - (p.h - b40::PLANTER_SOIL_DROP_M)) > b40::EPS) {
				off_soil++;
				if (first_off.is_empty()) first_off = Array::make(seed + k, int(grown->kind), grown->x, grown->y, grown->z, p.h);
			}
		}
	}
	Dictionary r;
	r["bodies"] = count;
	r["planters"] = planters;
	r["bare"] = bare; // planters with nothing growing in them
	r["first_bare"] = first_bare; // seed, the planter's middle in the lot's frame, its width, depth and height
	r["off_soil"] = off_soil; // plants of a planter that do not stand on its soil
	r["first_off"] = first_off; // seed, the plant's kind, its middle, its z, the planter's height
	r["standing"] = standing; // parts stood on the lots' ground
	r["trees"] = trees;
	r["posts"] = posts; // lamps on their posts
	r["under_body"] = under_body; // of them, those as tall as something the building holds over them (body_air.h)
	r["first_under"] = first_under; // seed, the part's kind, its middle, its width, its top, the underside it passes
	return r;
}

} // namespace city
