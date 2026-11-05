// Paint and draw helpers of the generator: skin tint, HSL nudge, lightness, sRGB <-> linear (IEC 61966-2-1), and the
// Xoshiro choose/mix/snap draws, each a fixed number of draws so a body is the same for its seed.
#pragma once

#include "core/seeded.h"
#include "sim/buildings/b40/types.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace city::b40 {

constexpr float SNAP_GRID = 2.f; // snap() = round(2x)/2
constexpr int CH_R = 16, CH_G = 8;
constexpr uint32_t CH_MASK = 255;
constexpr float CH_MAX = 255.f;

inline float mixr(Xoshiro128 &r, float a, float b) { return a + (b - a) * float(r.f()); }
inline float snap(float x) { return std::round(x * SNAP_GRID) / SNAP_GRID; }
inline float half_down(float x) { return std::floor(x * SNAP_GRID) / SNAP_GRID; }
template <typename T>
const T &choose(Xoshiro128 &r, const std::vector<T> &list) { return list[size_t(r.irange(0, int(list.size()) - 1))]; }
template <typename T, size_t N>
T choose(Xoshiro128 &r, const T (&list)[N]) { return list[r.irange(0, int(N) - 1)]; }

inline float ch(uint32_t c, int s) { return float((c >> s) & CH_MASK); }
inline uint32_t pack(float r, float g, float b) {
	auto q = [](float v) { return uint32_t(std::clamp(std::round(v), 0.f, CH_MAX)); };
	return (q(r) << CH_R) | (q(g) << CH_G) | q(b);
}
// Every channel x k, rounded (a tint, a stone base darker than its wall).
inline uint32_t scale_rgb(uint32_t c, float k) { return pack(ch(c, CH_R) * k, ch(c, CH_G) * k, ch(c, 0) * k); }

inline float lightness(uint32_t c) {
	float r = ch(c, CH_R), g = ch(c, CH_G), b = ch(c, 0);
	return (std::max({ r, g, b }) + std::min({ r, g, b })) / (2.f * CH_MAX);
}
inline bool is_light(uint32_t c) {
	constexpr float LIGHT_SUM = 600;
	return ch(c, CH_R) + ch(c, CH_G) + ch(c, 0) > LIGHT_SUM;
}

// The skin's lightness tint: one float.
inline uint32_t tint(uint32_t c, Xoshiro128 &r) {
	constexpr double LO = 0.95, SPAN = 0.1;
	return scale_rgb(c, float(LO + r.f() * SPAN));
}

uint32_t nudge(uint32_t c, Xoshiro128 &r, float amount = 1.f); // three floats

// Linear <-> sRGB per channel (vertex colours are stored sRGB; shading is done on linear values).
struct Lin {
	float r = 0, g = 0, b = 0;
	Lin operator*(float k) const { return { r * k, g * k, b * k }; }
};
constexpr float SRGB_KNEE = 0.04045f, SRGB_LINEAR = 12.92f, SRGB_OFF = 0.055f, SRGB_GAMMA = 2.4f;
inline float to_lin(float c) { return c <= SRGB_KNEE ? c / SRGB_LINEAR : std::pow((c + SRGB_OFF) / (1 + SRGB_OFF), SRGB_GAMMA); }
inline Lin lin_of(uint32_t c) { return { to_lin(ch(c, CH_R) / CH_MAX), to_lin(ch(c, CH_G) / CH_MAX), to_lin(ch(c, 0) / CH_MAX) }; }

} // namespace city::b40
