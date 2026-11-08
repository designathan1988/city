// A building read off a 3D model (body_survey.h).
#include "sim/commands/body_survey.h"
#include "core/plane_geometry.h"

#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/catalog/catalog.h"
#include "sim/commands/model_ops.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <unordered_map>

namespace city {

using namespace b40;

namespace {
constexpr float LEAST_AREA = 2e-4f; // a triangle smaller than this (m2) is noise of the model
// The hull is rounded to centimetres (HULL_STEP a metre) with HULL_EPS of slack; a colour's luma weighs its channels by
// ITU-R BT.601; a packed 0xRRGGBB's shifts and a channel's top; a box's sides.
constexpr float HULL_STEP = 100.f, HULL_EPS = 1e-6f, LUMA_R = .3f, LUMA_G = .59f, LUMA_B = .11f;
constexpr int RED_SHIFT = 16, GREEN_SHIFT = 8, SIDES = 4;
constexpr size_t BOX_SIDES = 4;
constexpr int RGB_MAX = 255;
// The colours a survey falls back on when the model shows none (a wall, a roof, a trim), and a flag's dark field (its white
// and red are the default flag's, b40/types.h).
constexpr uint32_t WALL_RGB = 0xd2cec4, ROOF_RGB = 0x9a9890, TRIM_RGB = 0xc9c3b4, FLAG_DARK = 0x111418;

using Hull = std::vector<std::array<float, 2>>;

struct Top {
	float x0, x1, y0, y1, z;
	Hull pts;
};

struct Block {
	float x0 = 0, x1 = 0, y0 = 0, y1 = 0, z0 = 0, z1 = 0;
	Hull hull;
	int base = 0, top = 0;
	bool has_look = false;
	SurveyColour look{};
};

bool meets(const SurveyShape &K, const Top &a, const Top &b) {
	return std::fabs(a.z - b.z) < K.touch_m && b.x0 <= a.x1 + K.touch_m && b.x1 >= a.x0 - K.touch_m && b.y0 <= a.y1 + K.touch_m && b.y1 >= a.y0 - K.touch_m;
}

void absorb(Top &a, const Top &b) {
	a.x0 = std::min(a.x0, b.x0);
	a.x1 = std::max(a.x1, b.x1);
	a.y0 = std::min(a.y0, b.y0);
	a.y1 = std::max(a.y1, b.y1);
	a.pts.insert(a.pts.end(), b.pts.begin(), b.pts.end());
}

Hull convex_hull(Hull p) {
	for (auto &q : p) q = { std::round(q[0] * HULL_STEP) / HULL_STEP, std::round(q[1] * HULL_STEP) / HULL_STEP };
	std::sort(p.begin(), p.end());
	p.erase(std::unique(p.begin(), p.end()), p.end());
	auto cross = [](const std::array<float, 2> &o, const std::array<float, 2> &a, const std::array<float, 2> &b) { return (a[0] - o[0]) * (b[1] - o[1]) - (a[1] - o[1]) * (b[0] - o[0]); };
	return monotone_hull(p, cross, HULL_EPS);
}

// The tops of the model filed by a uniform grid over its plan: each top in every cell its box, grown by TOUCH_M, covers, so
// what can meet a box is what the cells under it hold. A top's box only grows (it absorbs others), so it is filed again in the
// cells it reaches anew; one absorbed stays in its cells, dead. The grid's cell is a share of the model's extent.
class TopGrid {
public:
	TopGrid(const SurveyShape &k, float x0, float y0, float x1, float y1) : K(k), x0_(x0), y0_(y0) {
		constexpr float GRID_CELLS = 256.f; // the model's extent in this many cells, each at least least_cell_m
		cell_ = std::max({ (x1 - x0) / GRID_CELLS, (y1 - y0) / GRID_CELLS, K.least_cell_m });
	}
	// Files top `id`, whose box was `was` (nothing when new), in the cells its box `now` reaches anew.
	void file(int id, const Top *was, const Top &now) {
		int i0, j0, i1, j1, oi0 = 0, oj0 = 0, oi1 = -1, oj1 = -1;
		span(now.x0 - K.touch_m, now.y0 - K.touch_m, now.x1 + K.touch_m, now.y1 + K.touch_m, i0, j0, i1, j1);
		if (was) span(was->x0 - K.touch_m, was->y0 - K.touch_m, was->x1 + K.touch_m, was->y1 + K.touch_m, oi0, oj0, oi1, oj1);
		for (int j = j0; j <= j1; j++)
			for (int i = i0; i <= i1; i++)
				if (i < oi0 || i > oi1 || j < oj0 || j > oj1) cells_[key(i, j)].push_back(id);
	}
	// fn(id) once for every top filed in a cell the box covers (alive or not: the caller tells).
	template <typename F>
	void near(float x0, float y0, float x1, float y1, F &&fn) {
		int i0, j0, i1, j1;
		span(x0, y0, x1, y1, i0, j0, i1, j1);
		stamp_++;
		for (int j = j0; j <= j1; j++)
			for (int i = i0; i <= i1; i++) {
				const auto it = cells_.find(key(i, j));
				if (it == cells_.end()) continue;
				for (int id : it->second) {
					if (size_t(id) >= seen_.size()) seen_.resize(size_t(id) + 1, 0);
					if (seen_[size_t(id)] == stamp_) continue;
					seen_[size_t(id)] = stamp_;
					fn(id);
				}
			}
	}

private:
	const SurveyShape &K; // the survey's sizes (buildings.json shapes.survey)
	static constexpr int ROW_BITS = 32; // a cell's key: its column over the row's 32 bits
	static int64_t key(int i, int j) { return (int64_t(i) << ROW_BITS) ^ int64_t(uint32_t(j)); }
	void span(float x0, float y0, float x1, float y1, int &i0, int &j0, int &i1, int &j1) const {
		i0 = int(std::floor((x0 - x0_) / cell_));
		j0 = int(std::floor((y0 - y0_) / cell_));
		i1 = int(std::floor((x1 - x0_) / cell_));
		j1 = int(std::floor((y1 - y0_) / cell_));
	}
	float x0_, y0_, cell_ = 1;
	std::unordered_map<int64_t, std::vector<int>> cells_;
	std::vector<uint32_t> seen_;
	uint32_t stamp_ = 0;
};

// The model's blocks, from the faces that look up. Each face joins the first-made top it meets (a top keeps the height of the
// face that made it), then tops that meet merge, always the first pair in the order they were made, the earlier taking the
// later, until none meet. The grid finds what a box can meet; the order of the merges is the one the pairs give.
std::vector<Block> blocks_of(const SurveyShape &K, const std::vector<SurveyTriangle> &tris) {
	std::vector<Top> tops;
	float mx0 = tris.empty() ? 0 : tris[0][0].x, my0 = tris.empty() ? 0 : tris[0][0].y, mx1 = mx0, my1 = my0;
	for (const SurveyTriangle &t : tris)
		for (const SurveyPoint &p : t) {
			mx0 = std::min(mx0, p.x);
			my0 = std::min(my0, p.y);
			mx1 = std::max(mx1, p.x);
			my1 = std::max(my1, p.y);
		}
	TopGrid grid(K, mx0, my0, mx1, my1);
	std::vector<char> alive;
	for (const SurveyTriangle &t : tris) {
		const SurveyPoint &a = t[0], &b = t[1], &c = t[2];
		const float ux = b.x - a.x, uy = b.y - a.y, uz = b.z - a.z, vx = c.x - a.x, vy = c.y - a.y, vz = c.z - a.z;
		const float nx = uy * vz - uz * vy, ny = uz * vx - ux * vz, nz = ux * vy - uy * vx, length = std::sqrt(nx * nx + ny * ny + nz * nz);
		if (length < LEAST_AREA || std::fabs(nz) / length < K.flat_normal) continue; // either winding: models are not always consistent
		Top top{ std::min({ a.x, b.x, c.x }), std::max({ a.x, b.x, c.x }), std::min({ a.y, b.y, c.y }), std::max({ a.y, b.y, c.y }), (a.z + b.z + c.z) / 3, { { a.x, a.y }, { b.x, b.y }, { c.x, c.y } } };
		int first = -1;
		grid.near(top.x0, top.y0, top.x1, top.y1, [&](int id) {
			if ((first < 0 || id < first) && meets(K, tops[size_t(id)], top)) first = id;
		});
		if (first >= 0) {
			const Top was = { tops[size_t(first)].x0, tops[size_t(first)].x1, tops[size_t(first)].y0, tops[size_t(first)].y1, 0, {} };
			absorb(tops[size_t(first)], top);
			grid.file(first, &was, tops[size_t(first)]);
		} else {
			tops.push_back(std::move(top));
			alive.push_back(1);
			grid.file(int(tops.size()) - 1, nullptr, tops.back());
		}
	}
	// a top grown by later triangles can now touch another: the first meeting pair merges, the earlier top taking the later
	const auto first_meeting = [&](int c, bool below) {
		int found = -1;
		const Top &t = tops[size_t(c)];
		grid.near(t.x0, t.y0, t.x1, t.y1, [&](int id) {
			if (!alive[size_t(id)] || id == c || (below ? id > c : id < c) || (found >= 0 && id > found)) return;
			if (meets(K, tops[size_t(c)], tops[size_t(id)])) found = id;
		});
		return found;
	};
	const auto merge = [&](int into, int from) {
		const Top was = { tops[size_t(into)].x0, tops[size_t(into)].x1, tops[size_t(into)].y0, tops[size_t(into)].y1, 0, {} };
		absorb(tops[size_t(into)], tops[size_t(from)]);
		alive[size_t(from)] = 0;
		Hull().swap(tops[size_t(from)].pts);
		grid.file(into, &was, tops[size_t(into)]);
	};
	for (int c = 0; c < int(tops.size());) {
		if (!alive[size_t(c)]) {
			c++;
			continue;
		}
		const int lower = first_meeting(c, true); // no pair of earlier tops meets: only one with `c`, which grew, may
		if (lower >= 0) {
			merge(lower, c);
			c = lower;
			continue;
		}
		const int higher = first_meeting(c, false);
		if (higher >= 0) merge(c, higher);
		else c++;
	}
	std::vector<Block> out;
	for (size_t ti = 0; ti < tops.size(); ti++) {
		if (!alive[ti]) continue;
		const Top &t = tops[ti];
		if (t.z <= K.least_top_z || t.x1 - t.x0 <= K.least_top_side || t.y1 - t.y0 <= K.least_top_side) continue;
		const float cx = (t.x0 + t.x1) / 2, cy = (t.y0 + t.y1) / 2;
		float foot = 0;
		grid.near(cx, cy, cx, cy, [&](int id) {
			const Top &q = tops[size_t(id)];
			if (alive[size_t(id)] && q.z < t.z - K.touch_m && cx >= q.x0 && cx <= q.x1 && cy >= q.y0 && cy <= q.y1) foot = std::max(foot, q.z);
		});
		Block b;
		b.x0 = t.x0;
		b.x1 = t.x1;
		b.y0 = t.y0;
		b.y1 = t.y1;
		b.z0 = foot;
		b.z1 = t.z;
		b.hull = convex_hull(t.pts);
		out.push_back(std::move(b));
	}
	std::sort(out.begin(), out.end(), [](const Block &a, const Block &b) { return a.z1 < b.z1; });
	return out;
}

float luminance(const SurveyColour &c) { return LUMA_R * c[0] + LUMA_G * c[1] + LUMA_B * c[2]; }
uint32_t rgb_of(const SurveyColour &c) {
	auto byte = [](float x) { return uint32_t(std::lround(x * float(RGB_MAX))); };
	return (byte(c[0]) << RED_SHIFT) | (byte(c[1]) << GREEN_SHIFT) | byte(c[2]);
}
float apart(const SurveyColour &a, const SurveyColour &b) { return std::sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2])); }

int level_of(const SurveyShape &K, float z) { return z < K.on_first_storey_m ? 0 : std::max(1, int(std::round((z - K.first_storey_m) / K.storey_m)) + 1); }
float height_of(const SurveyShape &K, int level) { return level == 0 ? 0 : K.first_storey_m + float(level - 1) * K.storey_m; }

// What the walls of a block show between two heights: the median of the light samples of its four sides, the wall and not its windows or shadows.
bool wall_shade(const SurveyShape &K, const std::vector<SurveyView> &views, const Block &b, float z0, float z1, SurveyColour &out) {
	struct WallLine {
		float ax, ay, bx, by, nx, ny;
	};
	const WallLine sides[SIDES] = { { b.x0, b.y0 - K.face_out_m, b.x1, b.y0 - K.face_out_m, 0, -1 }, { b.x1 + K.face_out_m, b.y0, b.x1 + K.face_out_m, b.y1, 1, 0 },
		{ b.x0, b.y1 + K.face_out_m, b.x1, b.y1 + K.face_out_m, 0, 1 }, { b.x0 - K.face_out_m, b.y0, b.x0 - K.face_out_m, b.y1, -1, 0 } };
	std::vector<SurveyColour> light;
	for (const auto &s : sides)
		for (int k = 0; k < K.wall_along; k++) {
			const float f = K.sample_from + K.low_share_end * float(k) / float(K.wall_along - 1);
			for (int c = 0; c < K.wall_up; c++) {
				SurveyColour col;
				if (survey_sample(K, views, { s.ax + (s.bx - s.ax) * f, s.ay + (s.by - s.ay) * f, z0 + (z1 - z0) * (K.wall_up_from + K.wall_up_step * float(c)) }, s.nx, s.ny, col) && luminance(col) >= K.light_wall) light.push_back(col);
			}
		}
	if (int(light.size()) < K.least_light) return false;
	for (size_t i = 0; i < 3; i++) {
		std::nth_element(light.begin(), light.begin() + std::ptrdiff_t(light.size() / 2), light.end(), [&](const SurveyColour &p, const SurveyColour &q) { return p[i] < q[i]; });
		out[i] = light[light.size() / 2][i];
	}
	return true;
}

std::vector<V2> shape_of(const SurveyShape &K, const Block &b, float w, float d) {
	std::vector<V2> out;
	for (const auto &q : b.hull) {
		const V2 p{ std::clamp((q[0] - b.x0) / w, 0.f, 1.f), std::clamp((q[1] - b.y0) / d, 0.f, 1.f) };
		if (out.empty() || std::hypot((p.x - out.back().x) * w, (p.y - out.back().y) * d) > K.hull_gap_m) out.push_back(p);
	}
	while (out.size() > 1 && std::hypot((out.front().x - out.back().x) * w, (out.front().y - out.back().y) * d) <= K.hull_gap_m) out.pop_back();
	if (out.size() <= 4 || int(out.size()) > MAX_FACES) out.clear();
	return out;
}

// Each bay as the model shows it: the darker it reads, the more of it is opening.
void read_bays(const SurveyShape &K, const std::vector<SurveyView> &views, Volume &v, float dx, float dy) {
	const Ring ring = ring_of(v);
	for (int side = 0; side < v.sides(); side++) {
		const V2 a = ring[size_t(side)], c = ring[(size_t(side) + 1) % ring.size()];
		const float length = std::hypot(c.x - a.x, c.y - a.y), tx = (c.x - a.x) / length, ty = (c.y - a.y) / length, nx = ty, ny = -tx;
		const int count = bays_of(v, side);
		const float bw = length / float(count);
		for (int st = 0; st < v.storeys; st++) {
			const int level = v.base + st;
			const float z0 = height_of(K, level), z1 = height_of(K, level + 1);
			for (int i = 0; i < count; i++) {
				int dark = 0, seen = 0;
				for (int k = 0; k < K.bay_along; k++) {
					const float along = (float(i) + K.sample_from + K.low_share_end * float(k) / float(K.bay_along - 1)) * bw;
					for (int u = 0; u < K.bay_up; u++) {
						SurveyColour col;
						const SurveyPoint p{ a.x + tx * along + nx * K.face_out_m + dx, a.y + ty * along + ny * K.face_out_m + dy, z0 + (z1 - z0) * (K.bay_up_from + K.bay_up_span * float(u) / float(K.bay_up - 1)) };
						if (!survey_sample(K, views, p, nx, ny, col)) continue;
						seen++;
						dark += luminance(col) < K.light_wall ? 1 : 0;
					}
				}
				if (seen < K.least_samples) continue;
				const float share = float(dark) / float(seen);
				v.bays[bay_key(st, side, i)] = share <= K.wall_share ? BAY_WALL : level == 0 && share > K.door_share ? BAY_DOUBLE_DOOR : share > K.wide_share ? BAY_WIDE : BAY_SASH;
			}
		}
	}
}

// What rises above the highest block goes on its roof as a lantern.
void read_crown(const SurveyShape &K, const std::vector<SurveyTriangle> &tris, Body &body, float dx, float dy) {
	Volume *core = &body.volumes.front();
	for (Volume &v : body.volumes)
		if (v.top() > core->top()) core = &v;
	const float roof = height_of(K, core->top()), cx = core->x + core->w / 2, cy = core->y + core->d / 2;
	float crown = roof;
	for (const SurveyTriangle &t : tris)
		for (const SurveyPoint &p : t)
			if (p.z > crown && std::fabs(p.x - dx - cx) < core->w / 2 && std::fabs(p.y - dy - cy) < core->d / 2) crown = p.z;
	if (crown - roof <= K.crown_least_m) return;
	const float size = std::min(core->w, core->d) * K.crown_share;
	RoofPart lantern;
	lantern.kind = GEAR_LANTERN;
	lantern.x = cx;
	lantern.y = cy;
	lantern.w = lantern.d = size;
	lantern.h = crown - roof;
	lantern.flag.pattern = FLAG_STRIPES;
	lantern.flag.rgb[0] = FLAG_WHITE;
	lantern.flag.rgb[1] = FLAG_DARK;
	lantern.flag.rgb[2] = FLAG_RED;
	core->gear.push_back(lantern);
}
// Each block's look, and a base of another colour: the highest storey from the ground that still differs from the top.
struct Plinth {
	int storeys = 0;
	bool has = false;
	SurveyColour shade{};
};
Plinth plinth_of(const SurveyShape &K, const std::vector<SurveyView> &views, std::vector<Block> &kept) {
	Plinth out;
	for (Block &block : kept) {
		block.has_look = wall_shade(K, views, block, height_of(K, std::max(block.base, block.top - K.top_storeys)), height_of(K, block.top), block.look);
		if (block.base != 0 || block.top < K.least_based_top || !block.has_look) continue;
		int reach = 0;
		bool has = false;
		SurveyColour first{};
		for (int level = 0; level < block.top - 2; level++) {
			SurveyColour c;
			if (wall_shade(K, views, block, height_of(K, level), height_of(K, level + 1), c) && apart(c, block.look) > K.base_differs) {
				reach = level + 1;
				if (!has) first = c;
				has = true;
			} else if (level > reach + 1) break;
		}
		if (reach > out.storeys) out = Plinth{ reach, has, first };
	}
	return out;
}

} // namespace

bool survey_body(const Data &data, const std::vector<SurveyTriangle> &tris, std::vector<SurveyView> &views, Body &b) {
	const SurveyShape &K = data.shapes.survey;
	std::vector<Block> kept;
	for (Block &block : blocks_of(K, tris))
		if (block.x1 - block.x0 >= K.least_block && block.y1 - block.y0 >= K.least_block && block.z1 - block.z0 >= K.least_block_h) kept.push_back(std::move(block));
	if (kept.empty() || views.empty()) return false;
	for (SurveyView &v : views) survey_depth(v, tris);
	for (Block &block : kept) {
		const float cx = (block.x0 + block.x1) / 2, cy = (block.y0 + block.y1) / 2;
		float under = -1;
		for (const Block &o : kept)
			if (&o != &block && o.z1 <= block.z0 + 1 && cx >= o.x0 && cx <= o.x1 && cy >= o.y0 && cy <= o.y1) under = std::max(under, o.z1);
		block.base = under >= 0 ? level_of(K, under) : 0;
		block.top = std::max(block.base + 1, level_of(K, block.z1));
	}
	const Plinth base = plinth_of(K, views, kept);
	const int plinth = base.storeys;
	const bool has_plinth = base.has;
	const SurveyColour plinth_shade = base.shade;
	std::stable_sort(kept.begin(), kept.end(), [](const Block &p, const Block &q) { return p.base < q.base; });
	ModelSpec m;
	m.id = "reference";
	m.kind = KIND_OFFICE;
	m.use = USE_COMMERCIAL;
	m.ground_h = K.first_storey_m;
	m.storey_h = K.storey_m;
	m.wall = { FIN_STONE, WALL_RGB };
	m.roof = { FIN_CONCRETE, ROOF_RGB };
	m.trim = { FIN_STONE, TRIM_RGB };
	std::vector<int> walls; // what each block is built in: a block's own look, the base's, or -1 for the building's
	std::vector<SurveyColour> looks;
	auto add = [&](const Block &block, int base, int storeys, const SurveyColour *look) {
		const float w = block.x1 - block.x0, d = block.y1 - block.y0;
		ModelBlock mb;
		mb.x = block.x0;
		mb.y = block.y0;
		mb.w = w;
		mb.d = d;
		mb.base = base;
		mb.storeys = storeys;
		mb.fill = BAY_SASH;
		if (block.hull.size() > BOX_SIDES) mb.outline = shape_of(K, block, w, d);
		m.blocks.push_back(std::move(mb));
		walls.push_back(look != nullptr ? int(looks.size()) : -1);
		if (look != nullptr) looks.push_back(*look);
	};
	for (const Block &block : kept) {
		const SurveyColour *own = block.has_look ? &block.look : nullptr;
		if (has_plinth && block.base == 0 && block.top > plinth) {
			add(block, 0, plinth, &plinth_shade);
			add(block, plinth, block.top - plinth, own);
		} else add(block, block.base, block.top - block.base, has_plinth && block.top <= plinth ? &plinth_shade : own);
	}
	model_body(data, m, b);
	const float dx = m.blocks.front().x - b.volumes.front().x, dy = m.blocks.front().y - b.volumes.front().y; // the body stands in its lot's frame now
	for (size_t i = 0; i < b.volumes.size() && i < walls.size(); i++) {
		Volume &v = b.volumes[i];
		if (walls[i] >= 0) v.wall = b.add_paint({ FIN_STONE, rgb_of(looks[size_t(walls[i])]) });
		read_bays(K, views, v, dx, dy);
		if (v.storeys >= K.tall_storeys)
			for (int side = 0; side < v.sides(); side++) {
				v.faces[side].pier_w = K.pier_w_m;
				v.faces[side].pier_d = K.pier_d_m;
				v.faces[side].pier_every = 1;
			}
		SurveyColour roof;
		if (survey_sample(K, views, { v.x + v.w / 2 + dx, v.y + v.d / 2 + dy, height_of(K, v.top()) + K.above_roof_m }, 0, 0, roof)) v.roof_paint = b.add_paint({ FIN_TILE, rgb_of(roof) });
	}
	read_crown(K, tris, b, dx, dy);
	return true;
}

} // namespace city
