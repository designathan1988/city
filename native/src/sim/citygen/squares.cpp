// The squares of a generated city (squares.h): lots that touch make a block; a block of the right size on a main road that reaches
// every line of its box may be a square, one per neighbourhood, the nearest its middle. The square is a body of its own: a lawn, two
// walks crossing at a plaza, and trees on a lattice through the crossing, each an element whose rectangle is the room of its crown.
#include "sim/citygen/squares.h"

#include "core/math.h"
#include "sim/buildings/b40/civic.h"
#include "sim/citygen/lot_gen.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <unordered_map>

namespace city {

using namespace b40;

namespace {
constexpr double FILE_CELL_M = 60; // the grid lots are filed in while they are joined
constexpr float LAWN_H = 0.06f;
constexpr double SHARED_LINE_M = 1e-3; // two lots' shared side, reached from each one's own middle, within a millimetre
constexpr uint32_t LCG_MUL = 1664525U, LCG_ADD = 1013904223U, TREE_SEED_MUL = 31U, TREE_SEED_STEP = 7919U, SQUARE_SALT = 0x711eU;
constexpr double LCG_UNIT = 4294967296.0;

struct Plot {
	const Lot *lot;
	V2 c[4], centre, front;
};

Plot plot_of(const Lot &l) {
	const float sn = std::sin(float(l.rot)), cs = std::cos(float(l.rot)), hw = float(l.w) / 2, hd = float(l.d) / 2;
	const V2 centre(float(l.x), float(l.z)), along(cs, -sn), out(sn, cs);
	return Plot{ &l, { centre - along * hw + out * hd, centre + along * hw + out * hd, centre + along * hw - out * hd, centre - along * hw - out * hd }, centre, centre + out * hd };
}

float to_side(V2 p, V2 a, V2 b) {
	const V2 e = b - a;
	const float l2 = e.x * e.x + e.y * e.y;
	float t = l2 > 0 ? ((p.x - a.x) * e.x + (p.y - a.y) * e.y) / l2 : 0;
	t = std::clamp(t, 0.f, 1.f);
	return std::hypot(a.x + e.x * t - p.x, a.y + e.y * t - p.y);
}

bool touches(const Plot &a, const Plot &b, float reach) {
	for (const V2 &p : a.c)
		for (int i = 0; i < 4; i++)
			if (to_side(p, b.c[i], b.c[(i + 1) % 4]) < reach) return true;
	return false;
}

struct Joined {
	std::vector<int> parent;
	int find(int i) { return parent[size_t(i)] == i ? i : parent[size_t(i)] = find(parent[size_t(i)]); }
};

std::vector<std::vector<int>> blocks_of(const std::vector<Plot> &plots, float reach) {
	Joined j{ std::vector<int>(plots.size()) };
	std::iota(j.parent.begin(), j.parent.end(), 0);
	std::map<std::pair<int, int>, std::vector<int>> grid;
	auto cell = [](float v) { return int(std::floor(v / FILE_CELL_M)); };
	for (size_t i = 0; i < plots.size(); i++) grid[{ cell(plots[i].centre.x), cell(plots[i].centre.y) }].push_back(int(i));
	for (size_t i = 0; i < plots.size(); i++)
		for (int dx = -1; dx <= 1; dx++)
			for (int dz = -1; dz <= 1; dz++) {
				auto it = grid.find({ cell(plots[i].centre.x) + dx, cell(plots[i].centre.y) + dz });
				if (it == grid.end()) continue;
				for (int k : it->second)
					if (k > int(i) && j.find(int(i)) != j.find(k) && (touches(plots[i], plots[size_t(k)], reach) || touches(plots[size_t(k)], plots[i], reach))) j.parent[size_t(j.find(int(i)))] = j.find(k);
			}
	std::map<int, std::vector<int>> out;
	for (size_t i = 0; i < plots.size(); i++) out[j.find(int(i))].push_back(int(i));
	std::vector<std::vector<int>> list;
	for (auto &e : out) list.push_back(std::move(e.second));
	return list;
}

bool on_main_road(const CityState &s, const std::vector<int> &mains, V2 at, float reach) {
	bool hit = false;
	s.roads.for_segments_near(at.x - reach, at.y - reach, at.x + reach, at.y + reach, [&](int sg) {
		if (hit || std::find(mains.begin(), mains.end(), s.roads.segs[size_t(sg)].profile) == mains.end()) return;
		double t, d;
		closest_t(s.roads.segs[size_t(sg)].q, at.x, at.y, t, d);
		hit = d < reach;
	});
	return hit;
}

// The site a square would take of a block: the box of every lot's corners in the frame of the block's first lot (along its front,
// in from its street), as a lot (its middle in the world, its width, its depth, the way it faces); where the walks cross in that
// lot's frame (x from its middle, y from its front); and whether the block reaches every line of the box. A square is one record,
// and a record's lot is a rectangle: a block that leaves a stretch of its box's lines to a street or to another block is no square.
struct SquareSite {
	V2 centre, middle, cross; // the block's middle and the box's in the world; the crossing in the lot's frame
	float w = 0, d = 0, rot = 0;
	bool whole = false;
};

struct Chosen {
	SquareSite site;
	const Lot *first = nullptr; // what the square's record takes its zone, seed and street from
	float score = 0;
};

// The middle of a block: its plots' middles weighed by their areas, and the areas' sum.
struct Middle {
	V2 at; // in the world
	double dx = 0, dz = 0; // from the block's first lot's middle: as fine far from the map's origin as beside it
	float area = 0;
};
Middle middle_of(const std::vector<Plot> &plots, const std::vector<int> &block) {
	const Lot &first = *plots[size_t(block[0])].lot;
	double sx = 0, sz = 0, area = 0;
	for (int i : block) {
		const Lot &l = *plots[size_t(i)].lot;
		const double a = l.w * l.d;
		area += a;
		sx += (l.x - first.x) * a;
		sz += (l.z - first.z) * a;
	}
	Middle m;
	m.dx = sx / area;
	m.dz = sz / area;
	m.at = V2(float(first.x + m.dx), float(first.z + m.dz));
	m.area = float(area);
	return m;
}

// One lot of a block in the site's frame: how far it reaches along the first lot's front (a) and in from its street (b).
struct Reach {
	double a0 = FAR_AWAY, a1 = -FAR_AWAY, b0 = FAR_AWAY, b1 = -FAR_AWAY;
	void take(const Reach &o) {
		a0 = std::min(a0, o.a0);
		a1 = std::max(a1, o.a1);
		b0 = std::min(b0, o.b0);
		b1 = std::max(b1, o.b1);
	}
	double wide() const { return a1 - a0; }
	double deep() const { return b1 - b0; }
};

// The site of a block whose middle is `centre`, measured from the lots' own numbers about the first lot's middle, so a block far
// from the map's origin is measured as finely as one beside it.
SquareSite site_of(const std::vector<Plot> &plots, const std::vector<int> &block, const Middle &centre) {
	const Lot &first = *plots[size_t(block[0])].lot;
	const double sn = std::sin(first.rot), cs = std::cos(first.rot);
	// in the first lot's frame, of a point (dx, dz) from its middle: a along its front, b in from its street
	const auto a_of = [&](double dx, double dz) { return dx * cs - dz * sn; };
	const auto b_of = [&](double dx, double dz) { return -dx * sn - dz * cs; };
	std::vector<Reach> lots;
	Reach box;
	bool along = true; // every lot laid along the site's frame: its reach there is its own rectangle, no wider
	for (int i : block) {
		const Lot &l = *plots[size_t(i)].lot;
		const double s = std::sin(l.rot), c = std::cos(l.rot);
		Reach r;
		for (double u : { -0.5, 0.5 })
			for (double v : { -0.5, 0.5 }) {
				const double dx = l.x - first.x + c * l.w * u + s * l.d * v, dz = l.z - first.z - s * l.w * u + c * l.d * v;
				r.take(Reach{ a_of(dx, dz), a_of(dx, dz), b_of(dx, dz), b_of(dx, dz) });
			}
		along = along && r.wide() + r.deep() <= l.w + l.d + SHARED_LINE_M;
		lots.push_back(r);
		box.take(r);
	}
	SquareSite site;
	site.centre = centre.at;
	site.w = float(box.wide());
	site.d = float(box.deep());
	site.rot = float(first.rot);
	const double ma = (box.a0 + box.a1) / 2, mb = (box.b0 + box.b1) / 2;
	site.middle = V2(float(first.x + cs * ma - sn * mb), float(first.z - sn * ma - cs * mb));
	site.cross = V2(float(a_of(centre.dx, centre.dz) - ma), float(b_of(centre.dx, centre.dz) - box.b0));
	// The block reaches every line of its box: the lots standing on a line hold stretches of it that never lie in one another (the
	// lots of a block share sides, never ground), so they hold the whole line when their stretches add up to its length.
	double front = 0, back = 0, left = 0, right = 0;
	for (const Reach &r : lots) {
		front += r.b0 <= box.b0 + SHARED_LINE_M ? r.wide() : 0;
		back += r.b1 >= box.b1 - SHARED_LINE_M ? r.wide() : 0;
		left += r.a0 <= box.a0 + SHARED_LINE_M ? r.deep() : 0;
		right += r.a1 >= box.a1 - SHARED_LINE_M ? r.deep() : 0;
	}
	site.whole = along && std::min(front, back) >= box.wide() - SHARED_LINE_M && std::min(left, right) >= box.deep() - SHARED_LINE_M;
	return site;
}

// The body of a square on its site: a lawn from line to line, two walks crossing where the block's middle is, a round plaza over
// the crossing, and trees on a lattice through it, a step apart, so the rows stand the same either side of each walk.
Body lay(const CityPlanDef &d, const Data &data, const SquareSite &site, uint32_t &random) {
	const float W = site.w, D = site.d, cx = site.cross.x, cy = site.cross.y;
	Body body;
	body.plan.W = W;
	body.plan.D = D;
	body.use = USE_COMMERCIAL;
	const int lawn = body.add_paint(data.color("lawn")), walk = body.add_paint(data.color("concrete"));
	const float w = float(d.square_walk_half_m), plaza = float(d.square_plaza_m);
	put(body, EL_SURFACE, -W / 2, 0, W / 2, D, LAWN_H, lawn);
	put(body, EL_SURFACE, -W / 2, cy - w, W / 2, cy + w, LAWN_H * 2, walk);
	put(body, EL_SURFACE, cx - w, 0, cx + w, D, LAWN_H * 2, walk);
	put_round(body, EL_SURFACE, cx, cy, plaza * 2, plaza * 2, LAWN_H * 2, walk); // the circle the trees stand clear of, below
	const float step = float(d.square_tree_grid_m), clear = float(d.square_tree_edge_m);
	for (int i = int(std::ceil((-W / 2 - cx) / step)); cx + float(i) * step <= W / 2; i++)
		for (int j = int(std::ceil(-cy / step)); cy + float(j) * step <= D; j++) {
			const float a = float(i) * step, b = float(j) * step, x = cx + a, y = cy + b;
			if (std::fabs(a) < w || std::fabs(b) < w || std::hypot(a, b) < plaza) continue; // no trunk on a walk or on the plaza
			// How far the trunk stands from the square's nearest line: a trunk keeps its clearance from them.
			const float edge = std::min(std::min(x + W / 2, W / 2 - x), std::min(y, D - y));
			if (edge < clear) continue;
			// The tree's element is the room of its crown: the square about the trunk that reaches neither the square's lines nor past
			// half a step, where the next tree's room begins. A tree narrowed by the lines is the same tree smaller.
			const float room = 2 * std::min(step / 2, edge);
			random = random * LCG_MUL + LCG_ADD;
			const float tall = float(d.square_tree_low_m + double(random) / LCG_UNIT * (d.square_tree_high_m - d.square_tree_low_m));
			put_at(body, EL_TREE, x, y, room, room, tall * room / step, -1);
		}
	return body;
}

// The square of a chosen block, and its record.
void set_down(CityState &s, const Chosen &c, uint32_t &random) {
	Building rec;
	rec.x = c.site.middle.x;
	rec.z = c.site.middle.y;
	rec.w = c.site.w;
	rec.d = c.site.d;
	rec.rot = c.site.rot;
	rec.zone = uint8_t(c.first->zone);
	rec.fn = FN_SQUARE;
	rec.seed = c.first->seed;
	rec.profile = uint16_t(c.first->profile);
	rec.body = s.buildings.designs.edited->add(lay(s.catalog.growth.plan, s.buildings.designs, c.site, random));
	s.buildings.add(rec);
}
} // namespace

bool square_on(const CityPlanDef &plan, const Data &data, const std::vector<Lot> &lots, uint32_t seed, Body &body) {
	if (lots.empty()) return false;
	std::vector<Plot> plots;
	for (const Lot &l : lots) plots.push_back(plot_of(l));
	std::vector<int> block(plots.size());
	std::iota(block.begin(), block.end(), 0);
	const SquareSite site = site_of(plots, block, middle_of(plots, block));
	if (!site.whole) return false;
	uint32_t random = seed;
	body = lay(plan, data, site, random);
	return true;
}

int plan_squares(CityState &s, uint32_t seed) {
	const CityPlanDef &d = s.catalog.growth.plan;
	std::vector<int64_t> keys;
	for (const auto &kv : s.lots.by_cell) keys.push_back(kv.first);
	std::sort(keys.begin(), keys.end());
	std::vector<Plot> plots;
	for (int64_t key : keys)
		for (const Lot &l : s.lots.by_cell[key]) plots.push_back(plot_of(l));
	std::vector<int> mains;
	for (const std::string &name : d.square_main_roads) mains.push_back(s.catalog.profile_id(name.c_str()));
	std::map<std::pair<int, int>, Chosen> by_cell;
	const float cell = float(d.square_cell_m);
	for (std::vector<int> &block : blocks_of(plots, float(d.square_touch_m))) {
		const Middle middle = middle_of(plots, block);
		const float area = middle.area;
		const V2 centre = middle.at;
		bool fronting = false;
		for (int i : block) fronting = fronting || on_main_road(s, mains, plots[size_t(i)].front, float(d.square_front_m));
		if (area < d.square_area_low || area > d.square_area_high || !fronting) continue;
		const int kx = int(std::floor(centre.x / cell)), kz = int(std::floor(centre.y / cell));
		const float score = std::hypot(centre.x - (float(kx) + 0.5f) * cell, centre.y - (float(kz) + 0.5f) * cell);
		auto it = by_cell.find({ kx, kz });
		if (it != by_cell.end() && score >= it->second.score) continue;
		const SquareSite site = site_of(plots, block, middle);
		if (site.whole) by_cell[{ kx, kz }] = Chosen{ site, plots[size_t(block[0])].lot, score };
	}
	std::vector<const Chosen *> chosen;
	for (const auto &e : by_cell) chosen.push_back(&e.second);
	std::sort(chosen.begin(), chosen.end(), [](const Chosen *a, const Chosen *b) {
		return a->site.centre.x != b->site.centre.x ? a->site.centre.x < b->site.centre.x : a->site.centre.y < b->site.centre.y;
	});
	uint32_t index = 0;
	for (const Chosen *c : chosen) {
		uint32_t random = seed * SQUARE_SALT * TREE_SEED_MUL + index * TREE_SEED_STEP;
		set_down(s, *c, random);
		index++;
	}
	return int(chosen.size());
}

} // namespace city
