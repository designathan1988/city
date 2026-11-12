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
constexpr float HEDGE_NEAR_M = 2.f; // a hedge whose outer face stands this near a lot's line runs along that line
constexpr float HEDGE_SHORT_M = 2.f; // a line's hedge this much shorter than the line, at the most, runs whole (it keeps off the corners)

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
// The trees of an open lot's body: each one's element is the room of its crown (as a lot garden's tree: nothing else stands in it).
struct Grove {
	int trees = 0, bodies_bare = 0, out = 0, holds = 0, meet = 0, on_hard = 0;
	float least = b40::FAR_AWAY, slender = 0;
	Array first_out, first_holds, first_meet, first_hard;
	static bool lap(const b40::LotElement &a, const b40::LotElement &b) {
		return std::fabs(a.x - b.x) < (a.w + b.w) * HALF - b40::EPS && std::fabs(a.y - b.y) < (a.d + b.d) * HALF - b40::EPS;
	}
	void take(const b40::Body &b, int seed) {
		const float W = b.plan.W, D = b.plan.D;
		int mine = 0;
		for (size_t i = 0; i < b.elements.size(); i++) {
			const b40::LotElement &p = b.elements[i];
			if (p.kind != b40::EL_TREE) continue;
			mine++;
			least = std::min(least, std::min(p.w, p.d));
			slender = std::max(slender, p.h / std::min(p.w, p.d));
			if (p.x - p.w * HALF < -W * HALF - b40::EPS || p.x + p.w * HALF > W * HALF + b40::EPS || p.y - p.d * HALF < -b40::EPS || p.y + p.d * HALF > D + b40::EPS) {
				out++;
				if (first_out.is_empty()) first_out = Array::make(seed, p.x, p.y, p.w);
			}
			// the cover its trunk stands on: the last laid of those under it (a later cover lies over an earlier one)
			const b40::LotElement *under = nullptr;
			for (size_t k = 0; k < b.elements.size(); k++) {
				const b40::LotElement &q = b.elements[k];
				if (q.kind == b40::EL_SURFACE) {
					if (std::fabs(p.x - q.x) <= q.w * HALF && std::fabs(p.y - q.y) <= q.d * HALF) under = &q;
					continue;
				}
				if (k == i || !lap(p, q)) continue;
				if (q.kind == b40::EL_TREE) {
					if (k < i) continue; // a pair once
					meet++;
					if (first_meet.is_empty()) first_meet = Array::make(seed, p.x, p.y, p.w, q.x, q.y, q.w);
					continue;
				}
				holds++;
				if (first_holds.is_empty()) first_holds = Array::make(seed, p.x, p.y, p.w, int(q.kind), q.x, q.y);
			}
			if (under && under->paint >= 0 && b.paints[size_t(under->paint)].finish != b40::FIN_NONE) {
				on_hard++;
				if (first_hard.is_empty()) first_hard = Array::make(seed, p.x, p.y, under->x, under->y, under->w, under->d);
			}
		}
		trees += mine;
		bodies_bare += mine == 0 ? 1 : 0;
	}
	void answer(Dictionary &r) const {
		r["trees"] = trees;
		r["bare"] = bodies_bare; // bodies with no tree at all
		r["tree_out"] = out; // rooms past the lot's lines
		r["first_out"] = first_out; // seed, the tree's middle, its room's width
		r["tree_holds"] = holds; // rooms with another standing part in them (a lamp, a bench, a hedge, a fountain's rim)
		r["first_holds"] = first_holds; // seed, the tree's middle, its room, the part's kind and middle
		r["tree_meet"] = meet; // pairs of rooms in one another
		r["first_meet"] = first_meet;
		r["trunk_on_hard"] = on_hard; // trunks standing on a cover that is no lawn
		r["first_hard"] = first_hard; // seed, the trunk, the cover's middle, width and depth
		r["room_least_m"] = trees > 0 ? least : 0.f;
		r["slender"] = slender; // the most a tree's height is of its room's side
	}
};

// An open lot's ways against its lines. Its front faces its street; of the other three lines only those a street runs along (the
// record's edges) lead anywhere: a hard cover laid over the lot's ground (a path) that reaches another line ends at a neighbour's
// wall, and a hedge along such a line runs whole.
struct Ways {
	enum : int { LEFT = 0, RIGHT, BACK, LINES };
	int blind = 0, gaps = 0;
	Array first_blind, first_gap;
	void take(const b40::Body &b, const b40::LotEdges &edges, int seed) {
		const float W = b.plan.W, D = b.plan.D;
		const bool street[LINES] = { edges.on_street(0), edges.on_street(1), edges.back_on_street() };
		float hedged[LINES] = {};
		bool hedge = false, ground = true; // the first cover is the lot's own ground
		for (const b40::LotElement &c : b.elements) {
			const float x0 = c.x - c.w * HALF, x1 = c.x + c.w * HALF, y1 = c.y + c.d * HALF;
			if (c.kind == b40::EL_HEDGE) {
				hedge = true;
				if (c.d > c.w && x0 < -W * HALF + HEDGE_NEAR_M) hedged[LEFT] += c.d;
				else if (c.d > c.w && x1 > W * HALF - HEDGE_NEAR_M) hedged[RIGHT] += c.d;
				else if (c.w > c.d && y1 > D - HEDGE_NEAR_M) hedged[BACK] += c.w;
				continue;
			}
			if (c.kind != b40::EL_SURFACE) continue;
			if (ground) {
				ground = false;
				continue;
			}
			if (c.paint < 0 || b.paints[size_t(c.paint)].finish == b40::FIN_NONE) continue; // a lawn leads nowhere
			const bool reaches[LINES] = { x0 <= -W * HALF + b40::EPS, x1 >= W * HALF - b40::EPS, y1 >= D - b40::EPS };
			for (int k = 0; k < LINES; k++) {
				if (!reaches[k] || street[k]) continue;
				blind++;
				if (first_blind.is_empty()) first_blind = Array::make(seed, k, c.x, c.y, c.w, c.d);
			}
		}
		if (!hedge) return;
		const float line[LINES] = { D, D, W };
		for (int k = 0; k < LINES; k++) {
			if (street[k] || hedged[k] >= line[k] - HEDGE_SHORT_M) continue;
			gaps++;
			if (first_gap.is_empty()) first_gap = Array::make(seed, k, hedged[k], line[k]);
		}
	}
	void answer(Dictionary &r) const {
		r["path_ends_blind"] = blind; // hard covers reaching a line no street runs along
		r["first_blind"] = first_blind; // seed, the line (0 left, 1 right, 2 back), the cover's middle, width and depth
		r["hedge_gaps_blind"] = gaps; // lines no street runs along whose hedge does not run whole
		r["first_gap"] = first_gap; // seed, the line, the hedge's length along it, the line's length
	}
};
} // namespace

// Bodies of an open lot (a park, a square, a sports court: BuildingFn) `w` by `d` made on the spot, from `seed` on, a street running
// along its front and along the lines `streets` names (b40::LotEdges::street): what stands on them.
Dictionary CityProbes::open_lot_parts(int fn, float w, float d, int seed, int count, int streets) const {
	Benches benches;
	Grove grove;
	Ways ways;
	for (int k = 0; k < count; k++) {
		b40::Body b;
		Building rec;
		rec.fn = uint8_t(fn);
		rec.w = w;
		rec.d = d;
		rec.seed = uint32_t(seed + k);
		rec.edges.street = uint8_t(streets);
		b40::make_seed_body(state_->buildings.designs, rec, b);
		benches.take(b, seed + k);
		benches.take_sight(b, seed + k);
		grove.take(b, seed + k);
		ways.take(b, rec.edges, seed + k);
	}
	Dictionary r;
	r["bodies"] = count;
	benches.answer(r);
	grove.answer(r);
	ways.answer(r);
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
