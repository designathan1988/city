// The 4.0 facade grammar (jobs/b40/facades.ts + massing.ts dressBuilding): style by kind/era/street character, wall
// and trim paints, bay counts on the style module, storey fills with the symmetric rhythm, and the dress (stone base,
// entrance canopy or recess, projecting reliefs, corner piers).
#include "render/buildings/b40_gen.h"
#include "render/buildings/b40_paint.h"

#include <cmath>

namespace city::b40 {

namespace {
constexpr double GOLDEN = 2654435761.0, U32 = 4294967296.0;
constexpr float HALF = 0.5f, STEPS_ROUND = 1e6f, BEST_WITHIN = 0.75f, DEFAULT_BAY = 3.f;
constexpr int LOW_STOREYS = 3, CROWN_MIN_LEVELS = 4;
constexpr float CANOPY_EDGE = 0.6f, CANOPY_WIDE_BAYS = 5, CANOPY_PAD = 1.2f, CANOPY_DROP = 0.45f, CANOPY_MAX_Z = 3.6f;
constexpr float CANOPY_OUT = 1.2f, CANOPY_DEEP = 2.4f, CANOPY_THICK = 0.22f, RECESS = -1.2f, RECESS_MIN_W = 6.f, RECESS_SHARE = 0.45f;
constexpr float SMALL_CANOPY_MAX = 4.f, SMALL_DROP = 0.35f, SMALL_MAX_Z = 3.1f, SMALL_OUT = 0.8f, SMALL_DEEP = 1.6f, SMALL_THICK = 0.18f;
constexpr int RELIEF_MIN_STOREYS = 6, RELIEF_MIN_BAYS = 5, RELIEF_WIDE_BAYS = 9, PIER_MIN_STOREYS = 4;
constexpr float STONE_BASE_K = 0.86f;
constexpr float RELIEF_SKIP = 0.45f, RELIEF_OUT = 0.6f, PIER_OUT = 0.5f;

float steps_in(float units, float step) { return std::round(units / step * STEPS_ROUND) / STEPS_ROUND; }
bool overlaps(const Volume &a, const Volume &b) {
	return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.d && a.y + a.d > b.y;
}
bool modern(int s) {
	return s == STYLE_CONTEMPORARY || s == STYLE_MODERNIST || s == STYLE_GLASS || s == STYLE_TROPICAL || s == STYLE_MODERN_HOUSE;
}
void canopy(Body &b, float x, float y, float w, float d, float z, float h) {
	LotElement e;
	e.kind = EL_CANOPY; e.x = x; e.y = y; e.w = w; e.d = d; e.z = z; e.h = h;
	b.elements.push_back(e);
}

void compose_storeys(const StyleDef &s, const std::vector<uint8_t> &rhythm, Volume &v, Ground ground, int tallest, const int *counts) {
	int top = v.top() - 1;
	for (int k = 0; k < v.storeys; k++) {
		int level = v.base + k;
		bool crown_floor = s.top != BAY_NONE && tallest >= CROWN_MIN_LEVELS && level == tallest - 1 && level == top && level > 0;
		uint8_t fill;
		bool rhythm_bays = false;
		if (level == 0) fill = ground == GROUND_SHOP ? BAY_SHOPFRONT : ground == GROUND_WORKS ? BAY_WALL : s.ground_fill;
		else if (crown_floor) fill = s.top;
		else { fill = s.side_fill; rhythm_bays = true; }
		for (int side = 0; rhythm_bays && side < v.sides(); side++) {
			if (!v.outline.empty() ? counts[side] < 3 : side == 1 || side == 3) continue;
			for (int i = 0; i < counts[side]; i++) {
				uint8_t c = rhythm[size_t(std::min(i, counts[side] - 1 - i)) % rhythm.size()];
				if (c != fill) v.bays[bay_key(k, side, i)] = c;
			}
		}
		if (level == 0 && (fill == BAY_GOURMET || fill == BAY_LOGGIA || fill == BAY_BALCONY)) fill = BAY_WIDE;
		v.fill[size_t(k)] = fill;
	}
}
} // namespace

int style_for(const Data &data, Kind kind, Xoshiro128 &r, uint32_t character, int era) {
	const auto &list = data.weights[kind];
	float total = 0;
	for (auto &p : list) total += p.second * data.era[p.first][era];
	auto walk = [&](double at) {
		for (auto &p : list)
			if ((at -= p.second * data.era[p.first][era]) < 0) return p.first;
		return list.front().first;
	};
	if (r.f() < HALF) return walk(double(uint32_t(double(character) * GOLDEN)) / U32 * total);
	return walk(r.f() * total);
}

void paints_of(const Data &data, int style, Xoshiro128 &r, Paint &wall, Paint &trim) {
	const StyleDef &s = data.styles[size_t(style)];
	const Paint &skin = choose(r, s.skins);
	wall = Paint{ skin.finish, tint(skin.rgb, r) };
	std::vector<Paint> dark;
	for (const Paint &t : s.trims)
		if (!is_light(t.rgb)) dark.push_back(t);
	bool prefer_dark = is_light(wall.rgb) && !dark.empty() && r.f() < HALF;
	trim = choose(r, prefer_dark ? dark : s.trims);
}

int columns_for(float length, float module) {
	float exact = steps_in(length, module);
	int n = std::max(1, int(std::round(exact)));
	if (n % 2 == 1 || n < 2) return n;
	int lo = n - 1, hi = n + 1;
	int best = std::abs(exact - float(lo)) <= std::abs(exact - float(hi)) ? lo : hi;
	return std::abs(exact - float(best)) <= BEST_WITHIN ? best : n;
}

float side_length(const Volume &v, int side) {
	int n = int(v.outline.size());
	if (n > 0) {
		int p = ((side % n) + n) % n, q = (p + 1) % n;
		return std::hypot((v.outline[size_t(q)].x - v.outline[size_t(p)].x) * v.w, (v.outline[size_t(q)].y - v.outline[size_t(p)].y) * v.d);
	}
	return side == 0 || side == 2 ? v.w : v.d;
}

int bays_of(const Volume &v, int side) {
	if (v.faces[side].bays >= 0) return v.faces[side].bays;
	return std::max(1, int(std::round(steps_in(side == 0 || side == 2 ? v.w : v.d, DEFAULT_BAY))));
}

void compose_facades(const Data &data, Body &b, Xoshiro128 &r) {
	const StyleDef &s = data.styles[b.style];
	const std::vector<uint8_t> &rhythm = choose(r, s.rhythms);
	float ww = s.win_w0 + (s.win_w1 - s.win_w0) * float(r.f());
	float wh = s.win_h0 + (s.win_h1 - s.win_h0) * float(r.f());
	int tallest = 0;
	for (const Volume &v : b.volumes)
		if (!v.keep) tallest = std::max(tallest, v.top());
	for (size_t i = 0; i < b.volumes.size(); i++) {
		Volume &v = b.volumes[i];
		if (v.keep) continue;
		bool carries = false;
		for (size_t o = 0; o < b.volumes.size() && !carries; o++)
			carries = o != i && !b.volumes[o].keep && b.volumes[o].base == v.top() && overlaps(b.volumes[o], v);
		v.lines = s.lines;
		v.crown = carries ? CROWN_NONE : s.crown;
		int counts[MAX_FACES] = {};
		for (int side = 0; side < v.sides() && side < MAX_FACES; side++) {
			counts[side] = columns_for(side_length(v, side), s.module);
			bool low = v.storeys < LOW_STOREYS;
			FaceGeometry &g = v.faces[side];
			g = FaceGeometry{ counts[side], ww, wh, s.sill, 0, 0, -1 };
			if (s.piers && !low) { g.pier_w = s.pier_w; g.pier_d = s.pier_d; g.pier_every = s.pier_every; }
		}
		compose_storeys(s, rhythm, v, b.ground, tallest, counts);
	}
}

void dress_building(const Data &data, Body &b, Xoshiro128 &r, float entrance, int front) {
	const StyleDef &s = data.styles[b.style];
	int style = b.style;
	if (s.stone_base) {
		for (Volume &v : b.volumes) {
			if (v.keep || v.base != 0 || v.storeys < 2) continue;
			const Paint &w = b.wall_of(v);
			int ref = b.add_paint(Paint{ FIN_STONE, scale_rgb(w.rgb, STONE_BASE_K) });
			for (int side = 0; side < 4; side++) v.storey_paint[side_key(0, side)] = ref;
		}
	}
	Volume &fv = b.volumes[size_t(front)];
	if (!fv.keep) {
		int across = bays_of(fv, 0);
		int bay = std::clamp(int(std::floor((entrance - fv.x) / fv.w * float(across))), 0, across - 1);
		float bw = fv.w / float(across), cx = fv.x + (float(bay) + HALF) * bw;
		if (b.ground == GROUND_LOBBY && modern(style)) {
			float span = std::min(fv.w - CANOPY_EDGE, bw * (across >= CANOPY_WIDE_BAYS ? 3 : 1) + CANOPY_PAD);
			canopy(b, cx, fv.y - CANOPY_OUT, span, CANOPY_DEEP, std::min(b.ground_h - CANOPY_DROP, CANOPY_MAX_Z), CANOPY_THICK);
		} else if (b.ground != GROUND_SHOP && style != STYLE_COLONIAL && style != STYLE_COLONIAL_HOUSE && fv.w > RECESS_MIN_W && r.f() < RECESS_SHARE) {
			fv.reliefs.push_back(Relief{ 0, bay, bay, 0, 0, RECESS });
		} else if (b.ground != GROUND_SHOP) {
			canopy(b, cx, fv.y - SMALL_OUT, std::min(SMALL_CANOPY_MAX, bw + 1), SMALL_DEEP, std::min(b.ground_h - SMALL_DROP, SMALL_MAX_Z), SMALL_THICK);
		}
	}
	if (style == STYLE_CONTEMPORARY || style == STYLE_MODERNIST || style == STYLE_TROPICAL || style == STYLE_BRICK) {
		for (Volume &v : b.volumes) {
			int across = bays_of(v, 0);
			if (v.keep || !v.outline.empty() || v.storeys < RELIEF_MIN_STOREYS || across < RELIEF_MIN_BAYS || r.f() < RELIEF_SKIP) continue;
			int mid = across / 2, spread = across >= RELIEF_WIDE_BAYS ? 1 : 0, lo = v.base == 0 ? 1 : 0;
			for (int side : { 0, 2 }) v.reliefs.push_back(Relief{ side, mid - spread, mid + spread, lo, v.storeys - 1, RELIEF_OUT });
		}
	}
	if (style == STYLE_ART_DECO || style == STYLE_COLONIAL) {
		for (Volume &v : b.volumes) {
			int across = bays_of(v, 0);
			if (v.keep || v.storeys < PIER_MIN_STOREYS || across < RELIEF_MIN_BAYS) continue;
			int lo = v.base == 0 ? 1 : 0;
			v.reliefs.push_back(Relief{ 0, 0, 0, lo, v.storeys - 1, PIER_OUT });
			v.reliefs.push_back(Relief{ 0, across - 1, across - 1, lo, v.storeys - 1, PIER_OUT });
		}
	}
}

} // namespace city::b40
