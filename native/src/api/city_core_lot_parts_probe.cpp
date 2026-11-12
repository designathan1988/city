// CityProbes::lot_parts: the lots' layout held on bodies made on the spot, with no city: `count` bodies of a zone on a lot `w` by
// `d`, from `seed` on, each as the generator makes a record's (sim/buildings/b40/gen.h make_body), and what their lots hold: every
// planter with what grows in it (none bare, each plant standing on its planter's soil), and every part stood on the ground against
// what the building holds over it (sim/buildings/b40/body_air.h: none as tall as an eave, a balcony or an upper block over it).
#include "api/city_probes.h"

#include "sim/buildings/b40/bench.h"
#include "sim/buildings/b40/body_air.h"
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/record.h"
#include "sim/state/city_state.h"

#include <algorithm>
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

// The benches of a body: how many, and those laid with their long side along the way they look (types.h bench_across).
struct Benches {
	int count = 0, askew = 0, away = 0, on_cover = 0;
	Array first_askew, first_away, first_on;
	void take(const b40::Body &b, int seed) {
		for (const b40::LotElement &p : b.elements) {
			if (p.kind != b40::EL_BENCH) continue;
			count++;
			if (b40::bench_across(p)) continue;
			askew++;
			if (first_askew.is_empty()) first_askew = Array::make(seed, p.x, p.y, p.w, p.d, p.facing);
		}
	}
	// An open lot's bench looks at the cover it stands by: the nearest cover that is not the lot's own ground (a path, a fountain's water).
	void take_sight(const b40::Body &b, int seed) {
		for (const b40::LotElement &p : b.elements) {
			if (p.kind != b40::EL_BENCH) continue;
			const b40::LotElement *by = nullptr;
			float near = b40::FAR_AWAY, to_x = 0, to_y = 0;
			bool ground = true; // the first cover is the lot's ground
			for (const b40::LotElement &c : b.elements) {
				if (c.kind != b40::EL_SURFACE) continue;
				if (ground) {
					ground = false;
					continue;
				}
				// from the bench's middle to the cover's nearest point
				const float dx = std::clamp(p.x, c.x - c.w * HALF, c.x + c.w * HALF) - p.x, dy = std::clamp(p.y, c.y - c.d * HALF, c.y + c.d * HALF) - p.y;
				const float away = std::max(std::fabs(dx), std::fabs(dy));
				if (away >= near) continue;
				near = away;
				by = &c;
				to_x = dx;
				to_y = dy;
			}
			if (!by) continue;
			if (near <= 0) { // its middle on the cover: a bench stands beside a path, never on one
				on_cover++;
				if (first_on.is_empty()) first_on = Array::make(seed, p.x, p.y, by->x, by->y, by->w, by->d);
				continue;
			}
			const b40::Point2 look = b40::bench_look(p);
			if (look.x * to_x + look.y * to_y > 0) continue;
			away++;
			if (first_away.is_empty()) first_away = Array::make(seed, p.x, p.y, p.facing, by->x, by->y);
		}
	}
	void answer(Dictionary &r) const {
		r["benches"] = count;
		r["bench_askew"] = askew; // benches laid with their long side along the way they look
		r["first_askew"] = first_askew; // seed, the bench's middle, its width and depth, its facing
		r["bench_away"] = away; // open lots: benches that do not look at the cover they stand by
		r["first_away"] = first_away; // seed, the bench's middle, its facing, the cover's middle
		r["bench_on_cover"] = on_cover; // open lots: benches standing on a path or another cover laid over the lot's ground
		r["first_on"] = first_on; // seed, the bench's middle, the cover's middle, width and depth
	}
};
} // namespace

// Bodies of an open lot (a park, a square, a sports court: BuildingFn) `w` by `d` made on the spot, from `seed` on: what stands on them.
Dictionary CityProbes::open_lot_parts(int fn, float w, float d, int seed, int count) const {
	Benches benches;
	for (int k = 0; k < count; k++) {
		b40::Body b;
		Building rec;
		rec.fn = uint8_t(fn);
		rec.w = w;
		rec.d = d;
		rec.seed = uint32_t(seed + k);
		b40::make_seed_body(state_->buildings.designs, rec, b);
		benches.take(b, seed + k);
		benches.take_sight(b, seed + k);
	}
	Dictionary r;
	r["bodies"] = count;
	benches.answer(r);
	return r;
}

Dictionary CityProbes::lot_parts(int zone, int cap, float w, float d, int seed, int count) const {
	int planters = 0, bare = 0, off_soil = 0, standing = 0, under_body = 0, trees = 0, posts = 0;
	Array first_bare, first_off, first_under;
	Benches benches;
	for (int k = 0; k < count; k++) {
		b40::Body b;
		b40::make_body(state_->buildings.designs, b, zone, cap, w, d, uint32_t(seed + k), b40::LotEdges());
		const std::vector<b40::Overhang> air = b40::body_overhangs(b);
		benches.take(b, seed + k);
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
	benches.answer(r);
	return r;
}

} // namespace city
