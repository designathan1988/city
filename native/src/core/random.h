// Deterministic hashing, RNG and value noise. Everything procedural is derived from seeds so it can be
// regenerated instead of saved.
#pragma once

#include "core/math.h"

#include <cstdint>

namespace city {

inline uint32_t hash_u32(uint32_t x) {
	x ^= x >> 16; x *= 0x7feb352dU;
	x ^= x >> 15; x *= 0x846ca68bU;
	x ^= x >> 16;
	return x;
}
inline uint32_t hash_combine(uint32_t a, uint32_t b) { return hash_u32(a ^ (b + 0x9e3779b9U + (a << 6) + (a >> 2))); }
inline float hash_float(uint32_t h) { return (hash_u32(h) & 0xFFFFFF) / float(0x1000000); }

struct Rng {
	uint64_t s;
	explicit Rng(uint64_t seed = 0x1234567ULL) : s(seed ? seed : 0x9E3779B97F4A7C15ULL) {}
	uint32_t next() {
		s ^= s << 13; s ^= s >> 7; s ^= s << 17;
		return uint32_t(s >> 11);
	}
	float f() { return (next() & 0xFFFFFF) / float(0x1000000); }
	float range(float a, float b) { return a + (b - a) * f(); }
	int irange(int a, int b) { return b <= a ? a : a + int(next() % uint32_t(b - a + 1)); }
	bool chance(float p) { return f() < p; }
};

inline float vnoise_lattice(int x, int y, uint32_t seed) {
	return hash_float(hash_combine(hash_combine(uint32_t(x) * 73856093U, uint32_t(y) * 19349663U), seed)) * 2.f - 1.f;
}
inline float vnoise(float x, float y, uint32_t seed) {
	int xi = int(std::floor(x)), yi = int(std::floor(y));
	float fx = x - xi, fy = y - yi;
	float u = fx * fx * (3 - 2 * fx), v = fy * fy * (3 - 2 * fy);
	float a = vnoise_lattice(xi, yi, seed), b = vnoise_lattice(xi + 1, yi, seed);
	float c = vnoise_lattice(xi, yi + 1, seed), d = vnoise_lattice(xi + 1, yi + 1, seed);
	return lerpf(lerpf(a, b, u), lerpf(c, d, u), v);
}
inline float fbm(float x, float y, uint32_t seed, int octaves, float lacunarity = 2.f, float gain = 0.5f) {
	float sum = 0, amp = 1, norm = 0;
	for (int i = 0; i < octaves; i++) {
		sum += vnoise(x, y, seed + uint32_t(i) * 1013U) * amp;
		norm += amp;
		amp *= gain;
		x *= lacunarity;
		y *= lacunarity;
	}
	return sum / norm;
}

} // namespace city
