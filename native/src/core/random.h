// Deterministic hashing, RNG and value noise. Everything procedural is derived from seeds so it can be
// regenerated instead of saved.
#pragma once

#include "core/math.h"

#include <cstdint>

namespace city {

// The 32-bit finaliser "lowbias32" (C. Wellons, hash-prospector, 2018): its shifts and multipliers as published.
constexpr int LOWBIAS_SHIFT_A = 16, LOWBIAS_SHIFT_B = 15, LOWBIAS_SHIFT_C = 16;
constexpr uint32_t LOWBIAS_MUL_A = 0x7feb352dU, LOWBIAS_MUL_B = 0x846ca68bU;
// 2^32 and 2^64 over the golden ratio (Knuth's multiplicative hashing; boost::hash_combine and SplitMix64 add them).
constexpr uint32_t GOLDEN_GAMMA_32 = 0x9e3779b9U;
constexpr uint64_t GOLDEN_GAMMA_64 = 0x9E3779B97F4A7C15ULL;
// boost::hash_combine's shifts of the running seed.
constexpr int COMBINE_SHIFT_LEFT = 6, COMBINE_SHIFT_RIGHT = 2;
// A float in [0, 1) from 24 bits, as many as a float's mantissa holds exactly.
constexpr uint32_t MANTISSA_MASK = 0xFFFFFF;
constexpr float MANTISSA_SPAN = float(0x1000000);
// Marsaglia's xorshift (2003): the 64-bit triple (13, 7, 17), whose output is the state's bits from the 11th up, and the
// 32-bit one (13, 17, 5).
constexpr int XORSHIFT64_A = 13, XORSHIFT64_B = 7, XORSHIFT64_C = 17, XORSHIFT64_OUT = 11;
constexpr int XORSHIFT32_A = 13, XORSHIFT32_B = 17, XORSHIFT32_C = 5;
constexpr uint64_t RNG_DEFAULT_SEED = 0x1234567ULL; // a fixed non-zero state for a stream given no seed of its own
// The large primes of Teschner et al. 2003 ("Optimized spatial hashing for collision detection"), one per axis.
constexpr uint32_t SPATIAL_PRIME_X = 73856093U, SPATIAL_PRIME_Y = 19349663U;
constexpr uint32_t OCTAVE_SALT = 1013U; // a prime stepping the seed from one octave to the next: no two share a lattice

inline uint32_t hash_u32(uint32_t x) {
	x ^= x >> LOWBIAS_SHIFT_A; x *= LOWBIAS_MUL_A;
	x ^= x >> LOWBIAS_SHIFT_B; x *= LOWBIAS_MUL_B;
	x ^= x >> LOWBIAS_SHIFT_C;
	return x;
}
inline uint32_t hash_combine(uint32_t a, uint32_t b) {
	return hash_u32(a ^ (b + GOLDEN_GAMMA_32 + (a << COMBINE_SHIFT_LEFT) + (a >> COMBINE_SHIFT_RIGHT)));
}
inline float hash_float(uint32_t h) { return (hash_u32(h) & MANTISSA_MASK) / MANTISSA_SPAN; }

struct Rng {
	uint64_t s;
	explicit Rng(uint64_t seed = RNG_DEFAULT_SEED) : s(seed ? seed : GOLDEN_GAMMA_64) {}
	uint32_t next() {
		s ^= s << XORSHIFT64_A; s ^= s >> XORSHIFT64_B; s ^= s << XORSHIFT64_C;
		return uint32_t(s >> XORSHIFT64_OUT);
	}
	float f() { return (next() & MANTISSA_MASK) / MANTISSA_SPAN; }
	float range(float a, float b) { return a + (b - a) * f(); }
	int irange(int a, int b) { return b <= a ? a : a + int(next() % uint32_t(b - a + 1)); }
	bool chance(float p) { return f() < p; }
};

// A 32-bit state stepped in place by xorshift32; its new value.
inline uint32_t xorshift32(uint32_t &state) {
	state ^= state << XORSHIFT32_A;
	state ^= state >> XORSHIFT32_B;
	state ^= state << XORSHIFT32_C;
	return state;
}

inline float vnoise_lattice(int x, int y, uint32_t seed) {
	return hash_float(hash_combine(hash_combine(uint32_t(x) * SPATIAL_PRIME_X, uint32_t(y) * SPATIAL_PRIME_Y), seed)) * 2.f - 1.f;
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
		sum += vnoise(x, y, seed + uint32_t(i) * OCTAVE_SALT) * amp;
		norm += amp;
		amp *= gain;
		x *= lacunarity;
		y *= lacunarity;
	}
	return sum / norm;
}

} // namespace city
