// Rasterises the foliage texture (see leaf_atlas.h): every leaf and leaflet is a lens (an ellipse in its own frame),
// drawn in grey with a lit half and a darker rim; later leaves cover earlier ones.
#include "render/trees/leaf_atlas.h"

#include "render/trees/linear_mips.h"

#include "core/math.h"
#include "core/random.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int RGBA = 4, ALPHA = 3;
constexpr float BYTE = 255.f;
// A frond's outline up its length: where the sine starts, how much of it the frond runs, and how full its shoulders stand.
constexpr float FROND_FOOT = 0.06f, FROND_RUN = 0.95f, FROND_ARCH = 0.7f;
constexpr int REGIONS = 3; // the far tuft, the near tuft, the frond
constexpr float HALF_F = 0.5f;
constexpr int TUFT_WIDE = 0, TUFT_CLOSE = 1, FROND_CELL = 2; // the regions, left to right

struct Look {
	int px = 0, leaves = 0, leaflets = 0;
	float near_leaves = 1;
	float len0 = 0, len1 = 0, wid0 = 0, wid1 = 0, spread = 0, squash = 1, heart = 1, shade_min = 0, lit_lift = 0, rim = 1, rim_share = 1;
	float leaflet_w = 0, leaflet_reach = 0, leaflet_vary = 1, leaflet_rise = 0, leaflet_tilt = 0, rachis_w = 0, rachis_tone = 0, frond_shade_min = 0;
	uint32_t seed = 0;
};

struct Canvas {
	int w = 0, h = 0;
	PackedByteArray px;
	// One lens of length `len` and width `wid` centred at (cx, cy), turned by `ang`; `tone` 0..1 before its lit half
	// and rim.
	void lens(const Look &L, float cx, float cy, float len, float wid, float ang, float tone) {
		float a = len * HALF_F, b = wid * HALF_F, c = std::cos(ang), s = std::sin(ang), reach = std::max(a, b);
		int x0 = std::max(0, int(cx - reach)), x1 = std::min(w - 1, int(cx + reach) + 1);
		int y0 = std::max(0, int(cy - reach)), y1 = std::min(h - 1, int(cy + reach) + 1);
		uint8_t *d = px.ptrw();
		for (int y = y0; y <= y1; y++)
			for (int x = x0; x <= x1; x++) {
				float dx = x + HALF_F - cx, dy = y + HALF_F - cy;
				float u = dx * c + dy * s, v = -dx * s + dy * c;
				float e = (u * u) / (a * a) + (v * v) / (b * b);
				if (e > 1.f) continue;
				float t = tone * (v < 0 ? 1.f + L.lit_lift : 1.f) * (e > L.rim_share * L.rim_share ? L.rim : 1.f);
				uint8_t g = uint8_t(std::clamp(t, 0.f, 1.f) * BYTE);
				uint8_t *p = d + size_t((y * w + x) * RGBA);
				p[0] = p[1] = p[2] = g;
				p[ALPHA] = uint8_t(BYTE);
			}
	}
};

float range2(const Array &a, int i) { return a.size() > i ? float(a[i]) : 0.f; }

bool load(const Dictionary &d, Look &L, PackedStringArray &errors) {
	const char *keys[] = { "px", "leaves", "near_leaves", "leaf_len_px", "leaf_wid_px", "spread", "squash", "heart_bias", "leaf_tone_min", "lit_lift", "rim",
		"rim_share", "leaflets", "leaflet_px", "leaflet_reach", "leaflet_vary", "leaflet_rise", "leaflet_tilt", "rachis_px", "rachis_tone", "frond_tone_min",
		"seed" };
	for (const char *k : keys)
		if (!d.has(k)) errors.push_back(String("streets.json trees.atlas: falta ") + k);
	if (!errors.is_empty()) return false;
	Array len = d["leaf_len_px"], wid = d["leaf_wid_px"];
	L.px = d["px"]; L.leaves = d["leaves"]; L.near_leaves = d["near_leaves"]; L.leaflets = d["leaflets"];
	L.len0 = range2(len, 0); L.len1 = range2(len, 1); L.wid0 = range2(wid, 0); L.wid1 = range2(wid, 1);
	L.spread = d["spread"]; L.squash = d["squash"]; L.heart = d["heart_bias"]; L.shade_min = d["leaf_tone_min"]; L.lit_lift = d["lit_lift"];
	L.rim = d["rim"]; L.rim_share = d["rim_share"]; L.leaflet_w = d["leaflet_px"]; L.leaflet_reach = d["leaflet_reach"];
	L.leaflet_vary = d["leaflet_vary"];
	L.leaflet_rise = d["leaflet_rise"]; L.leaflet_tilt = d["leaflet_tilt"]; L.rachis_w = d["rachis_px"]; L.rachis_tone = d["rachis_tone"];
	L.frond_shade_min = d["frond_tone_min"]; L.seed = uint32_t(int(d["seed"]));
	return L.px > 0;
}

// A leaf cluster in region `at`, denser at its heart (radius drawn as rand^heart_bias of the spread): `leaves` of them,
// each `grow` of the look's size.
void cluster(Canvas &cv, const Look &L, Rng &r, int at, int leaves, float grow) {
	float S = float(L.px), cx = S * (at + HALF_F), cy = S * HALF_F;
	for (int i = 0; i < leaves; i++) {
		float ang = r.f() * TAU_F, rad = std::pow(r.f(), L.heart) * L.spread * S;
		float x = cx + std::cos(ang) * rad, y = cy + std::sin(ang) * rad * L.squash;
		cv.lens(L, x, y, r.range(L.len0, L.len1) * grow, r.range(L.wid0, L.wid1) * grow, r.f() * TAU_F, r.range(L.shade_min, 1.f));
	}
}

// Right region: a frond with its base at the bottom row and its tip at the top, leaflets longest a third of the way up.
void frond(Canvas &cv, const Look &L, Rng &r) {
	float S = float(L.px), mid = S * (FROND_CELL + HALF_F);
	for (int side : { -1, 1 })
		for (int j = 0; j < L.leaflets; j++) {
			float t = (j + HALF_F) / L.leaflets, y = S * (1.f - t);
			float reach = L.leaflet_reach * S * std::pow(std::sin(PI_F * std::min(1.f, FROND_FOOT + t * FROND_RUN)), FROND_ARCH) * r.range(L.leaflet_vary, 1.f);
			float ang = side < 0 ? PI_F + L.leaflet_tilt : -L.leaflet_tilt;
			cv.lens(L, mid + side * reach * HALF_F, y - reach * L.leaflet_rise, reach, L.leaflet_w, ang, r.range(L.frond_shade_min, 1.f));
		}
	cv.lens(L, mid, S * HALF_F, S, L.rachis_w, PI_F * HALF_F, L.rachis_tone);
}
} // namespace

bool LeafAtlas::build(const Dictionary &atlas, PackedStringArray &errors) {
	Look L;
	if (!load(atlas, L, errors)) return false;
	Canvas cv;
	cv.w = L.px * REGIONS;
	cv.h = L.px;
	cv.px.resize(int64_t(cv.w) * cv.h * RGBA);
	cv.px.fill(0);
	Rng r(L.seed);
	cluster(cv, L, r, TUFT_WIDE, L.leaves, 1.f);
	cluster(cv, L, r, TUFT_CLOSE, int(std::lround(L.leaves * L.near_leaves)), 1.f / std::sqrt(L.near_leaves));
	frond(cv, L, r);
	texture_ = ImageTexture::create_from_image(image_with_linear_mips(cv.w, cv.h, cv.px));
	return texture_.is_valid();
}

} // namespace city
