// Rasterises the foliage texture (see leaf_atlas.h): every leaf and leaflet is a lens (an ellipse in its own frame),
// drawn in grey with a lit half and a darker rim; later leaves cover earlier ones.
#include "render/trees/leaf_atlas.h"

#include "render/trees/linear_mips.h"

#include "core/math.h"
#include "core/random.h"
#include "render/data_reader.h"

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
		float a = len * HALF_F, b = wid * HALF_F, c = std::cos(ang), s = std::sin(ang), reach = std::max(a, b) + 1.f;
		int x0 = std::max(0, int(cx - reach)), x1 = std::min(w - 1, int(cx + reach) + 1);
		int y0 = std::max(0, int(cy - reach)), y1 = std::min(h - 1, int(cy + reach) + 1);
		uint8_t *d = px.ptrw();
		for (int y = y0; y <= y1; y++)
			for (int x = x0; x <= x1; x++) {
				float dx = x + HALF_F - cx, dy = y + HALF_F - cy;
				float u = dx * c + dy * s, v = -dx * s + dy * c;
				float e = (u * u) / (a * a) + (v * v) / (b * b);
				// Every line of the leaf in pixels from it, so a leaf seen close runs along a smooth line and not along the atlas's steps.
				float r = std::sqrt(e), side = std::min(a, b);
				float cover = std::clamp((1.f - r) * side + HALF_F, 0.f, 1.f);
				if (cover <= 0.f) continue;
				float inside = std::clamp((L.rim_share - r) * side + HALF_F, 0.f, 1.f), lit = std::clamp(HALF_F - v, 0.f, 1.f);
				float t = std::clamp(tone - L.rim + (tone * (1.f + L.lit_lift * lit) - tone + L.rim) * inside, 0.f, 1.f);
				// Over the leaves already there, so its edge is as smooth against them as against the air.
				uint8_t *p = d + size_t((y * w + x) * RGBA);
				float under = p[ALPHA] / BYTE * (1.f - cover), all = cover + under;
				p[0] = p[1] = p[2] = uint8_t((t * cover + p[0] / BYTE * under) / all * BYTE);
				p[ALPHA] = uint8_t(all * BYTE);
			}
	}
};

constexpr int PAIR = 2; // [low, high]

// trees.atlas through the strict reader (render/data_reader.h).
bool load(const Dictionary &d, Look &L, PackedStringArray &errors) {
	const int64_t before = errors.size();
	DataReader r{ errors, "streets.json.trees.atlas" };
	auto num = [&](const char *key) { return float(r.num(d, key)); };
	const Array len = r.list(d, "leaf_len_px", PAIR), wid = r.list(d, "leaf_wid_px", PAIR);
	L.px = r.integer(d, "px"); L.leaves = r.integer(d, "leaves"); L.near_leaves = num("near_leaves"); L.leaflets = r.integer(d, "leaflets");
	L.len0 = float(r.in("leaf_len_px").num_at(len, 0)); L.len1 = float(r.in("leaf_len_px").num_at(len, 1));
	L.wid0 = float(r.in("leaf_wid_px").num_at(wid, 0)); L.wid1 = float(r.in("leaf_wid_px").num_at(wid, 1));
	L.spread = num("spread"); L.squash = num("squash"); L.heart = num("heart_bias"); L.shade_min = num("leaf_tone_min"); L.lit_lift = num("lit_lift");
	L.rim = num("rim"); L.rim_share = num("rim_share"); L.leaflet_w = num("leaflet_px"); L.leaflet_reach = num("leaflet_reach");
	L.leaflet_vary = num("leaflet_vary");
	L.leaflet_rise = num("leaflet_rise"); L.leaflet_tilt = num("leaflet_tilt"); L.rachis_w = num("rachis_px"); L.rachis_tone = num("rachis_tone");
	L.frond_shade_min = num("frond_tone_min"); L.seed = uint32_t(r.integer(d, "seed"));
	if (d.has("px") && L.px <= 0) r.fail("px: not above 0");
	return errors.size() == before;
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
	image_ = image_with_linear_mips(cv.w, cv.h, cv.px);
	texture_ = ImageTexture::create_from_image(image_);
	return texture_.is_valid();
}

} // namespace city
