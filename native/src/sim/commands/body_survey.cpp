// A building read off a 3D model (body_survey.h).
#include "sim/commands/body_survey.h"

#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/catalog/catalog.h"
#include "sim/commands/model_ops.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace city {

using namespace b40;

namespace {
constexpr float TOUCH_M = .05f, FLAT_NORMAL = .98f, LEAST_AREA = 2e-4f, LEAST_TOP_Z = .2f, LEAST_TOP_SIDE = .3f, LEAST_BLOCK = 2.f, LEAST_BLOCK_H = 2.5f;
constexpr float FIRST_STOREY_M = 4.5f, STOREY_M = 3.3f, ON_FIRST_STOREY_M = 1.f, FACE_OUT_M = .05f, LIGHT_WALL = .42f, BASE_DIFFERS = .12f, LOW_SHARE_END = .8f;
constexpr float PIER_W_M = .55f, PIER_D_M = .45f, CROWN_LEAST_M = 3.f, CROWN_SHARE = .6f, ABOVE_ROOF_M = .5f, WALL_SHARE = .12f, DOOR_SHARE = .45f, WIDE_SHARE = .6f, HULL_GAP_M = .05f;
constexpr float HULL_STEP = 100.f, HULL_EPS = 1e-6f, LUMA_R = .3f, LUMA_G = .59f, LUMA_B = .11f, SAMPLE_FROM = .1f, WALL_UP_FROM = .2f, WALL_UP_STEP = .3f, BAY_UP_FROM = .15f, BAY_UP_SPAN = .7f;
constexpr int RED_SHIFT = 16, GREEN_SHIFT = 8, SIDES = 4;
constexpr size_t BOX_SIDES = 4;
constexpr int RGB_MAX = 255, WALL_ALONG = 9, WALL_UP = 3, BAY_ALONG = 7, BAY_UP = 5, LEAST_SAMPLES = 8, LEAST_LIGHT = 4, TALL_STOREYS = 3, TOP_STOREYS = 3, LEAST_BASED_TOP = 4;
constexpr uint32_t WALL_RGB = 0xd2cec4, ROOF_RGB = 0x9a9890, TRIM_RGB = 0xc9c3b4, FLAG_WHITE = 0xffffff, FLAG_DARK = 0x111418, FLAG_RED = 0xc8102e;

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

bool meets(const Top &a, const Top &b) {
	return std::fabs(a.z - b.z) < TOUCH_M && b.x0 <= a.x1 + TOUCH_M && b.x1 >= a.x0 - TOUCH_M && b.y0 <= a.y1 + TOUCH_M && b.y1 >= a.y0 - TOUCH_M;
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
	if (p.size() < 3) return p;
	auto cross = [](const std::array<float, 2> &o, const std::array<float, 2> &a, const std::array<float, 2> &b) { return (a[0] - o[0]) * (b[1] - o[1]) - (a[1] - o[1]) * (b[0] - o[0]); };
	auto chain = [&](const Hull &list) {
		Hull out;
		for (const auto &q : list) {
			while (out.size() >= 2 && cross(out[out.size() - 2], out.back(), q) <= HULL_EPS) out.pop_back();
			out.push_back(q);
		}
		out.pop_back();
		return out;
	};
	Hull lower = chain(p);
	std::reverse(p.begin(), p.end());
	const Hull upper = chain(p);
	lower.insert(lower.end(), upper.begin(), upper.end());
	return lower;
}

// The model's blocks, from the faces that look up.
std::vector<Block> blocks_of(const std::vector<SurveyTriangle> &tris) {
	std::vector<Top> tops;
	for (const SurveyTriangle &t : tris) {
		const SurveyPoint &a = t[0], &b = t[1], &c = t[2];
		const float ux = b.x - a.x, uy = b.y - a.y, uz = b.z - a.z, vx = c.x - a.x, vy = c.y - a.y, vz = c.z - a.z;
		const float nx = uy * vz - uz * vy, ny = uz * vx - ux * vz, nz = ux * vy - uy * vx, length = std::sqrt(nx * nx + ny * ny + nz * nz);
		if (length < LEAST_AREA || std::fabs(nz) / length < FLAT_NORMAL) continue; // either winding: models are not always consistent
		Top top{ std::min({ a.x, b.x, c.x }), std::max({ a.x, b.x, c.x }), std::min({ a.y, b.y, c.y }), std::max({ a.y, b.y, c.y }), (a.z + b.z + c.z) / 3, { { a.x, a.y }, { b.x, b.y }, { c.x, c.y } } };
		auto near = std::find_if(tops.begin(), tops.end(), [&](const Top &q) { return meets(q, top); });
		if (near != tops.end()) absorb(*near, top);
		else tops.push_back(std::move(top));
	}
	for (bool changed = true; changed;) { // a top grown by later triangles can now touch another
		changed = false;
		for (size_t i = 0; i < tops.size() && !changed; i++)
			for (size_t k = i + 1; k < tops.size() && !changed; k++)
				if (meets(tops[i], tops[k])) {
					absorb(tops[i], tops[k]);
					tops.erase(tops.begin() + std::ptrdiff_t(k));
					changed = true;
				}
	}
	std::vector<Block> out;
	for (const Top &t : tops) {
		if (t.z <= LEAST_TOP_Z || t.x1 - t.x0 <= LEAST_TOP_SIDE || t.y1 - t.y0 <= LEAST_TOP_SIDE) continue;
		const float cx = (t.x0 + t.x1) / 2, cy = (t.y0 + t.y1) / 2;
		float foot = 0;
		for (const Top &q : tops)
			if (q.z < t.z - TOUCH_M && cx >= q.x0 && cx <= q.x1 && cy >= q.y0 && cy <= q.y1) foot = std::max(foot, q.z);
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

int level_of(float z) { return z < ON_FIRST_STOREY_M ? 0 : std::max(1, int(std::round((z - FIRST_STOREY_M) / STOREY_M)) + 1); }
float height_of(int level) { return level == 0 ? 0 : FIRST_STOREY_M + float(level - 1) * STOREY_M; }

// What the walls of a block show between two heights: the median of the light samples of its four sides, the wall and not its windows or shadows.
bool wall_shade(const std::vector<SurveyView> &views, const Block &b, float z0, float z1, SurveyColour &out) {
	struct WallLine {
		float ax, ay, bx, by, nx, ny;
	};
	const WallLine sides[SIDES] = { { b.x0, b.y0 - FACE_OUT_M, b.x1, b.y0 - FACE_OUT_M, 0, -1 }, { b.x1 + FACE_OUT_M, b.y0, b.x1 + FACE_OUT_M, b.y1, 1, 0 },
		{ b.x0, b.y1 + FACE_OUT_M, b.x1, b.y1 + FACE_OUT_M, 0, 1 }, { b.x0 - FACE_OUT_M, b.y0, b.x0 - FACE_OUT_M, b.y1, -1, 0 } };
	std::vector<SurveyColour> light;
	for (const auto &s : sides)
		for (int k = 0; k < WALL_ALONG; k++) {
			const float f = SAMPLE_FROM + LOW_SHARE_END * float(k) / float(WALL_ALONG - 1);
			for (int c = 0; c < WALL_UP; c++) {
				SurveyColour col;
				if (survey_sample(views, { s.ax + (s.bx - s.ax) * f, s.ay + (s.by - s.ay) * f, z0 + (z1 - z0) * (WALL_UP_FROM + WALL_UP_STEP * float(c)) }, s.nx, s.ny, col) && luminance(col) >= LIGHT_WALL) light.push_back(col);
			}
		}
	if (int(light.size()) < LEAST_LIGHT) return false;
	for (size_t i = 0; i < 3; i++) {
		std::nth_element(light.begin(), light.begin() + std::ptrdiff_t(light.size() / 2), light.end(), [&](const SurveyColour &p, const SurveyColour &q) { return p[i] < q[i]; });
		out[i] = light[light.size() / 2][i];
	}
	return true;
}

std::vector<Point2> shape_of(const Block &b, float w, float d) {
	std::vector<Point2> out;
	for (const auto &q : b.hull) {
		const Point2 p{ std::clamp((q[0] - b.x0) / w, 0.f, 1.f), std::clamp((q[1] - b.y0) / d, 0.f, 1.f) };
		if (out.empty() || std::hypot((p.x - out.back().x) * w, (p.y - out.back().y) * d) > HULL_GAP_M) out.push_back(p);
	}
	while (out.size() > 1 && std::hypot((out.front().x - out.back().x) * w, (out.front().y - out.back().y) * d) <= HULL_GAP_M) out.pop_back();
	if (out.size() <= 4 || int(out.size()) > MAX_FACES) out.clear();
	return out;
}

// Each bay as the model shows it: the darker it reads, the more of it is opening.
void read_bays(const std::vector<SurveyView> &views, Volume &v, float dx, float dy) {
	const Ring ring = ring_of(v);
	for (int side = 0; side < v.sides(); side++) {
		const Point2 a = ring[size_t(side)], c = ring[(size_t(side) + 1) % ring.size()];
		const float length = std::hypot(c.x - a.x, c.y - a.y), tx = (c.x - a.x) / length, ty = (c.y - a.y) / length, nx = ty, ny = -tx;
		const int count = bays_of(v, side);
		const float bw = length / float(count);
		for (int st = 0; st < v.storeys; st++) {
			const int level = v.base + st;
			const float z0 = height_of(level), z1 = height_of(level + 1);
			for (int i = 0; i < count; i++) {
				int dark = 0, seen = 0;
				for (int k = 0; k < BAY_ALONG; k++) {
					const float along = (float(i) + SAMPLE_FROM + LOW_SHARE_END * float(k) / float(BAY_ALONG - 1)) * bw;
					for (int u = 0; u < BAY_UP; u++) {
						SurveyColour col;
						const SurveyPoint p{ a.x + tx * along + nx * FACE_OUT_M + dx, a.y + ty * along + ny * FACE_OUT_M + dy, z0 + (z1 - z0) * (BAY_UP_FROM + BAY_UP_SPAN * float(u) / float(BAY_UP - 1)) };
						if (!survey_sample(views, p, nx, ny, col)) continue;
						seen++;
						dark += luminance(col) < LIGHT_WALL ? 1 : 0;
					}
				}
				if (seen < LEAST_SAMPLES) continue;
				const float share = float(dark) / float(seen);
				v.bays[bay_key(st, side, i)] = share <= WALL_SHARE ? BAY_WALL : level == 0 && share > DOOR_SHARE ? BAY_DOUBLE_DOOR : share > WIDE_SHARE ? BAY_WIDE : BAY_SASH;
			}
		}
	}
}

// What rises above the highest block goes on its roof as a lantern.
void read_crown(const std::vector<SurveyTriangle> &tris, Body &body, float dx, float dy) {
	Volume *core = &body.volumes.front();
	for (Volume &v : body.volumes)
		if (v.top() > core->top()) core = &v;
	const float roof = height_of(core->top()), cx = core->x + core->w / 2, cy = core->y + core->d / 2;
	float crown = roof;
	for (const SurveyTriangle &t : tris)
		for (const SurveyPoint &p : t)
			if (p.z > crown && std::fabs(p.x - dx - cx) < core->w / 2 && std::fabs(p.y - dy - cy) < core->d / 2) crown = p.z;
	if (crown - roof <= CROWN_LEAST_M) return;
	const float size = std::min(core->w, core->d) * CROWN_SHARE;
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
} // namespace

bool survey_body(const Data &data, const Tuning &, const std::vector<SurveyTriangle> &tris, std::vector<SurveyView> &views, Body &b) {
	std::vector<Block> kept;
	for (Block &block : blocks_of(tris))
		if (block.x1 - block.x0 >= LEAST_BLOCK && block.y1 - block.y0 >= LEAST_BLOCK && block.z1 - block.z0 >= LEAST_BLOCK_H) kept.push_back(std::move(block));
	if (kept.empty() || views.empty()) return false;
	for (SurveyView &v : views) survey_depth(v, tris);
	for (Block &block : kept) {
		const float cx = (block.x0 + block.x1) / 2, cy = (block.y0 + block.y1) / 2;
		float under = -1;
		for (const Block &o : kept)
			if (&o != &block && o.z1 <= block.z0 + 1 && cx >= o.x0 && cx <= o.x1 && cy >= o.y0 && cy <= o.y1) under = std::max(under, o.z1);
		block.base = under >= 0 ? level_of(under) : 0;
		block.top = std::max(block.base + 1, level_of(block.z1));
	}
	// A base of another colour: the highest storey from the ground that still differs from the top.
	int plinth = 0;
	bool has_plinth = false;
	SurveyColour plinth_shade{};
	for (Block &block : kept) {
		block.has_look = wall_shade(views, block, height_of(std::max(block.base, block.top - TOP_STOREYS)), height_of(block.top), block.look);
		if (block.base != 0 || block.top < LEAST_BASED_TOP || !block.has_look) continue;
		int reach = 0;
		bool has = false;
		SurveyColour first{};
		for (int level = 0; level < block.top - 2; level++) {
			SurveyColour c;
			if (wall_shade(views, block, height_of(level), height_of(level + 1), c) && apart(c, block.look) > BASE_DIFFERS) {
				reach = level + 1;
				if (!has) first = c;
				has = true;
			} else if (level > reach + 1) break;
		}
		if (reach > plinth) {
			plinth = reach;
			plinth_shade = first;
			has_plinth = has;
		}
	}
	std::stable_sort(kept.begin(), kept.end(), [](const Block &p, const Block &q) { return p.base < q.base; });
	ModelSpec m;
	m.id = "reference";
	m.kind = KIND_OFFICE;
	m.use = USE_COMMERCIAL;
	m.ground_h = FIRST_STOREY_M;
	m.storey_h = STOREY_M;
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
		if (block.hull.size() > BOX_SIDES) mb.outline = shape_of(block, w, d);
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
		read_bays(views, v, dx, dy);
		if (v.storeys >= TALL_STOREYS)
			for (int side = 0; side < v.sides(); side++) {
				v.faces[side].pier_w = PIER_W_M;
				v.faces[side].pier_d = PIER_D_M;
				v.faces[side].pier_every = 1;
			}
		SurveyColour roof;
		if (survey_sample(views, { v.x + v.w / 2 + dx, v.y + v.d / 2 + dy, height_of(v.top()) + ABOVE_ROOF_M }, 0, 0, roof)) v.roof_paint = b.add_paint({ FIN_TILE, rgb_of(roof) });
	}
	read_crown(tris, b, dx, dy);
	return true;
}

} // namespace city
