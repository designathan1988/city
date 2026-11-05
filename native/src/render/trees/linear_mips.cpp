// See linear_mips.h.
#include "render/trees/linear_mips.h"

#include <algorithm>
#include <array>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int RGBA = 4, ALPHA = 3, LEVELS = 256;
constexpr float BYTE_TOP = 255.f, ROUND_UP = 0.5f;
// The sRGB transfer function (IEC 61966-2-1).
constexpr float SRGB_KNEE = 0.04045f, SRGB_LINEAR_KNEE = 0.0031308f, SRGB_SLOPE = 12.92f, SRGB_OFFSET = 0.055f, SRGB_GAMMA = 2.4f;

float decoded(float s) { return s <= SRGB_KNEE ? s / SRGB_SLOPE : std::pow((s + SRGB_OFFSET) / (1.f + SRGB_OFFSET), SRGB_GAMMA); }
float encoded(float l) { return l <= SRGB_LINEAR_KNEE ? l * SRGB_SLOPE : (1.f + SRGB_OFFSET) * std::pow(l, 1.f / SRGB_GAMMA) - SRGB_OFFSET; }
uint8_t to_byte(float v) { return uint8_t(std::clamp(v, 0.f, 1.f) * BYTE_TOP + ROUND_UP); }

// The level under one: each texel the mean of the block of `src` it covers, the block's light and its cover alike.
void halve(const uint8_t *src, int sw, int sh, uint8_t *dst, int dw, int dh, const std::array<float, LEVELS> &light) {
	for (int y = 0; y < dh; y++) {
		int y0 = y * sh / dh, y1 = std::max(y0 + 1, (y + 1) * sh / dh);
		for (int x = 0; x < dw; x++) {
			int x0 = x * sw / dw, x1 = std::max(x0 + 1, (x + 1) * sw / dw);
			float sum[RGBA] = {};
			for (int j = y0; j < y1; j++)
				for (int i = x0; i < x1; i++) {
					const uint8_t *p = src + (size_t(j) * sw + i) * RGBA;
					for (int c = 0; c < ALPHA; c++) sum[c] += light[p[c]];
					sum[ALPHA] += float(p[ALPHA]) / BYTE_TOP;
				}
			float n = float((y1 - y0) * (x1 - x0));
			uint8_t *d = dst + (size_t(y) * dw + x) * RGBA;
			for (int c = 0; c < ALPHA; c++) d[c] = to_byte(encoded(sum[c] / n));
			d[ALPHA] = to_byte(sum[ALPHA] / n);
		}
	}
}
} // namespace

Ref<Image> image_with_linear_mips(int w, int h, const PackedByteArray &base) {
	std::array<float, LEVELS> light;
	for (int i = 0; i < LEVELS; i++) light[size_t(i)] = decoded(float(i) / BYTE_TOP);
	int64_t total = 0;
	for (int lw = w, lh = h;; lw = std::max(lw / 2, 1), lh = std::max(lh / 2, 1)) {
		total += int64_t(lw) * lh * RGBA;
		if (lw == 1 && lh == 1) break;
	}
	PackedByteArray data;
	data.resize(total);
	uint8_t *d = data.ptrw();
	std::copy(base.ptr(), base.ptr() + int64_t(w) * h * RGBA, d);
	for (int lw = w, lh = h; lw > 1 || lh > 1;) {
		int nw = std::max(lw / 2, 1), nh = std::max(lh / 2, 1);
		uint8_t *next = d + int64_t(lw) * lh * RGBA;
		halve(d, lw, lh, next, nw, nh, light);
		d = next;
		lw = nw;
		lh = nh;
	}
	return Image::create_from_data(w, h, true, Image::FORMAT_RGBA8, data);
}

} // namespace city
