// The facade grammar: style by kind/era/street character, wall
// and trim paints, bay counts on the style module, storey fills with the symmetric rhythm, and the dress (stone base,
// entrance canopy or recess, projecting reliefs, corner piers).
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/paint.h"

#include <cmath>

namespace city::b40 {

namespace {
constexpr double GOLDEN = 2654435761.0, U32 = 4294967296.0; // Knuth's multiplicative hash (2^32 over the golden ratio), and 2^32
constexpr float HALF = 0.5f, STEPS_ROUND = 1e6f; // a share of a step is rounded to millionths: a length is a whole count of steps
// The facade grammar's counts, which every body's saved bays and the commands' answers depend on: a face given no count is
// divided into bays of DEFAULT_BAY metres, and an even count is swapped for the nearer odd one within BEST_WITHIN of a bay.
constexpr float DEFAULT_BAY = 3.f, BEST_WITHIN = 0.75f;

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

void compose_storeys(const FacadeShape &K, const StyleDef &s, const std::vector<uint8_t> &rhythm, Volume &v, Ground ground, int tallest, const int *counts) {
	int top = v.top() - 1;
	for (int k = 0; k < v.storeys; k++) {
		int level = v.base + k;
		bool crown_floor = s.top != BAY_NONE && tallest >= K.crown_min_levels && level == tallest - 1 && level == top && level > 0;
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
	wall = Paint{ skin.finish, tint(data.shapes.paint, skin.rgb, r) };
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
	return std::max(1, int(std::round(steps_in(side_length(v, side), DEFAULT_BAY))));
}

void compose_facades(const Data &data, Body &b, Xoshiro128 &r) {
	const FacadeShape &K = data.shapes.facade;
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
			bool low = v.storeys < K.low_storeys;
			FaceGeometry &g = v.faces[side];
			g = FaceGeometry{ counts[side], ww, wh, s.sill, 0, 0, -1 };
			if (s.piers && !low) { g.pier_w = s.pier_w; g.pier_d = s.pier_d; g.pier_every = s.pier_every; }
		}
		compose_storeys(K, s, rhythm, v, b.ground, tallest, counts);
	}
}

void dress_building(const Data &data, Body &b, Xoshiro128 &r, float entrance, int front) {
	const FacadeShape &K = data.shapes.facade;
	const StyleDef &s = data.styles[b.style];
	int style = b.style;
	if (s.stone_base) {
		for (Volume &v : b.volumes) {
			if (v.keep || v.base != 0 || v.storeys < 2) continue;
			const Paint &w = b.wall_of(v);
			int ref = b.add_paint(Paint{ FIN_STONE, scale_rgb(w.rgb, K.stone_base_k) });
			for (int side = 0; side < 4; side++) v.storey_paint[side_key(0, side)] = ref;
		}
	}
	Volume &fv = b.volumes[size_t(front)];
	if (!fv.keep) {
		int across = bays_of(fv, 0);
		int bay = std::clamp(int(std::floor((entrance - fv.x) / fv.w * float(across))), 0, across - 1);
		float bw = fv.w / float(across), cx = fv.x + (float(bay) + HALF) * bw;
		if (b.ground == GROUND_LOBBY && modern(style)) {
			float span = std::min(fv.w - K.canopy_edge, bw * (across >= K.canopy_wide_bays ? 3 : 1) + K.canopy_pad);
			canopy(b, cx, fv.y - K.canopy_out, span, K.canopy_deep, std::min(b.ground_h - K.canopy_drop, K.canopy_max_z), K.canopy_thick);
		} else if (b.ground != GROUND_SHOP && style != STYLE_COLONIAL && style != STYLE_COLONIAL_HOUSE && fv.w > K.recess_min_w && r.f() < K.recess_share) {
			fv.reliefs.push_back(Relief{ 0, bay, bay, 0, 0, K.recess });
		} else if (b.ground != GROUND_SHOP) {
			canopy(b, cx, fv.y - K.small_out, std::min(K.small_canopy_max, bw + 1), K.small_deep, std::min(b.ground_h - K.small_drop, K.small_max_z), K.small_thick);
		}
	}
	if (style == STYLE_CONTEMPORARY || style == STYLE_MODERNIST || style == STYLE_TROPICAL || style == STYLE_BRICK) {
		for (Volume &v : b.volumes) {
			int across = bays_of(v, 0);
			if (v.keep || !v.outline.empty() || v.storeys < K.relief_min_storeys || across < K.relief_min_bays || r.f() < K.relief_skip) continue;
			int mid = across / 2, spread = across >= K.relief_wide_bays ? 1 : 0, lo = v.base == 0 ? 1 : 0;
			for (int side : { 0, 2 }) v.reliefs.push_back(Relief{ side, mid - spread, mid + spread, lo, v.storeys - 1, K.relief_out });
		}
	}
	if (style == STYLE_ART_DECO || style == STYLE_COLONIAL) {
		for (Volume &v : b.volumes) {
			int across = bays_of(v, 0);
			if (v.keep || v.storeys < K.pier_min_storeys || across < K.relief_min_bays) continue;
			int lo = v.base == 0 ? 1 : 0;
			v.reliefs.push_back(Relief{ 0, 0, 0, lo, v.storeys - 1, K.pier_out });
			v.reliefs.push_back(Relief{ 0, across - 1, across - 1, lo, v.storeys - 1, K.pier_out });
		}
	}
}

} // namespace city::b40
