// The squares of a generated city (squares.h): lots that touch make a block; a block of the right size on a main road may be a square,
// one per neighbourhood, the nearest its middle. The square is a body of its own: lawn, two walks crossing at a round plaza, trees on a grid.
#include "sim/citygen/squares.h"

#include "core/math.h"
#include "core/random.h"
#include "sim/buildings/b40/civic.h"
#include "sim/buildings/b40/record.h"
#include "sim/citygen/lot_gen.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <numeric>
#include <unordered_map>

namespace city {

using namespace b40;

namespace {
constexpr double FILE_CELL_M = 60; // the grid lots are filed in while they are joined
constexpr float LAWN_H = 0.06f, LATTICE_FROM_M = 200, SEAM_M = 0.35f;
constexpr uint32_t LCG_MUL = 1664525U, LCG_ADD = 1013904223U, TREE_SEED_MUL = 31U, TREE_SEED_STEP = 7919U, SQUARE_SALT = 0x711eU;
constexpr double LCG_UNIT = 4294967296.0;
constexpr float PLAZA_WIDE = 0.45f; // the round plaza's rectangles: full size, and this share across

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

bool inside(const Plot &p, V2 at) {
	const Lot &l = *p.lot;
	const float sn = std::sin(float(l.rot)), cs = std::cos(float(l.rot));
	const V2 rel = at - p.centre;
	// two lots side by side share their side: a point on the seam, and a little past it for the rounding, is on both
	return std::fabs(rel.x * cs - rel.y * sn) <= float(l.w) / 2 + SEAM_M && std::fabs(rel.x * sn + rel.y * cs) <= float(l.d) / 2 + SEAM_M;
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

struct Chosen {
	std::vector<int> block;
	V2 centre;
	float score = 0;
};

// The body of a square on the block, and its record.
void set_down(CityState &s, const std::vector<Plot> &plots, const Chosen &c, uint32_t &random) {
	const CityPlanDef &d = s.catalog.growth.plan;
	const Data &data = s.buildings.designs;
	const Lot &first = *plots[size_t(c.block[0])].lot;
	const float sn = std::sin(float(first.rot)), cs = std::cos(float(first.rot));
	const V2 along(cs, -sn), in(-sn, -cs);
	float a0 = FAR_AWAY, a1 = -FAR_AWAY, b0 = FAR_AWAY, b1 = -FAR_AWAY;
	for (int i : c.block)
		for (const V2 &p : plots[size_t(i)].c) {
			const V2 rel = p - c.centre;
			const float a = rel.x * along.x + rel.y * along.y, b = rel.x * in.x + rel.y * in.y;
			a0 = std::min(a0, a); a1 = std::max(a1, a); b0 = std::min(b0, b); b1 = std::max(b1, b);
		}
	const float W = a1 - a0, D = b1 - b0, mid = (a0 + a1) / 2, cx = -mid, cy = -b0;
	Body body;
	body.plan.W = W;
	body.plan.D = D;
	body.use = USE_COMMERCIAL;
	const int lawn = body.add_paint(data.color("lawn")), walk = body.add_paint(data.color("concrete"));
	const float w = float(d.square_walk_half_m), plaza = float(d.square_plaza_m);
	put(body, EL_SURFACE, -W / 2, 0, W / 2, D, LAWN_H, lawn);
	put(body, EL_SURFACE, -W / 2, cy - w, W / 2, cy + w, LAWN_H * 2, walk);
	put(body, EL_SURFACE, cx - w, 0, cx + w, D, LAWN_H * 2, walk);
	put_at(body, EL_SURFACE, cx, cy, plaza * 2, plaza * 2 * PLAZA_WIDE, LAWN_H * 2, walk);
	put_at(body, EL_SURFACE, cx, cy, plaza * 2 * PLAZA_WIDE, plaza * 2, LAWN_H * 2, walk);
	auto in_block = [&](float a, float b) {
		const V2 p = c.centre + along * a + in * b;
		return std::any_of(c.block.begin(), c.block.end(), [&](int i) { return inside(plots[size_t(i)], p); });
	};
	const float step = float(d.square_tree_grid_m), clear = float(d.square_tree_edge_m);
	for (float a = -LATTICE_FROM_M; a <= LATTICE_FROM_M; a += step) // squares.ts: the grid starts here, so it lies the same on any block
		for (float b = -LATTICE_FROM_M; b <= LATTICE_FROM_M; b += step) {
			if (!in_block(a, b) || std::fabs(a) < w || std::fabs(b) < w || std::hypot(a, b) < plaza) continue;
			if (!in_block(a + clear, b) || !in_block(a - clear, b) || !in_block(a, b + clear) || !in_block(a, b - clear)) continue;
			random = random * LCG_MUL + LCG_ADD;
			const float h = float(d.square_tree_low_m + double(random) / LCG_UNIT * (d.square_tree_high_m - d.square_tree_low_m));
			put_at(body, EL_TREE, cx + a, cy + b, clear, clear, h, -1);
		}
	Building rec;
	const V2 front = c.centre + along * mid + in * b0, middle = front + in * (D / 2);
	rec.x = middle.x;
	rec.z = middle.y;
	rec.w = W;
	rec.d = D;
	rec.rot = float(first.rot);
	rec.zone = uint8_t(first.zone);
	rec.fn = FN_SQUARE;
	rec.seed = first.seed;
	rec.profile = uint16_t(first.profile);
	rec.body = s.buildings.designs.edited->add(std::move(body));
	s.buildings.add(rec);
}
} // namespace

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
		float area = 0;
		V2 sum(0, 0);
		bool fronting = false;
		for (int i : block) {
			const Plot &p = plots[size_t(i)];
			const float a = float(p.lot->w * p.lot->d);
			area += a;
			sum = sum + p.centre * a;
			fronting = fronting || on_main_road(s, mains, p.front, float(d.square_front_m));
		}
		if (area < d.square_area_low || area > d.square_area_high || !fronting) continue;
		const V2 centre = sum * (1 / area);
		const int kx = int(std::floor(centre.x / cell)), kz = int(std::floor(centre.y / cell));
		const float score = std::hypot(centre.x - (float(kx) + 0.5f) * cell, centre.y - (float(kz) + 0.5f) * cell);
		auto it = by_cell.find({ kx, kz });
		if (it == by_cell.end() || score < it->second.score) by_cell[{ kx, kz }] = Chosen{ std::move(block), centre, score };
	}
	std::vector<const Chosen *> chosen;
	for (const auto &e : by_cell) chosen.push_back(&e.second);
	std::sort(chosen.begin(), chosen.end(), [](const Chosen *a, const Chosen *b) { return a->centre.x != b->centre.x ? a->centre.x < b->centre.x : a->centre.y < b->centre.y; });
	uint32_t index = 0;
	for (const Chosen *c : chosen) {
		uint32_t random = seed * SQUARE_SALT * TREE_SEED_MUL + index * TREE_SEED_STEP;
		set_down(s, plots, *c, random);
		index++;
	}
	return int(chosen.size());
}

} // namespace city
