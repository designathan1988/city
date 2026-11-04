// Seeded generators that give Roadcraft's numbers bit for bit (its src/core/rng.ts): the same seed makes the same
// city here and in the TypeScript game. All arithmetic is 32-bit unsigned like JavaScript's Math.imul and `>>> 0`;
// noise is computed in double like JavaScript numbers.
#pragma once

#include <cmath>
#include <cstdint>

namespace city {

// lowbias32
inline uint32_t hash32(uint32_t x) {
	x ^= x >> 16; x *= 0x7feb352dU;
	x ^= x >> 15; x *= 0x846ca68bU;
	x ^= x >> 16;
	return x;
}

// `hash2(a, b, seed)`: a and b are taken as int32 (JavaScript `| 0`).
inline uint32_t hash2(int32_t a, int32_t b, uint32_t seed = 0) {
	return hash32(hash32(uint32_t(a) ^ seed) + uint32_t(b) * 0x9e3779b1U);
}

// JavaScript `x | 0` for a double already integral and in range (rounded coordinates, indices).
inline int32_t js_int(double v) { return int32_t(int64_t(v)); }

// mulberry32 (Roadcraft `Rng`): next() in [0, 1); int(a, b) EXCLUDES b.
struct Mulberry32 {
	uint32_t state;
	explicit Mulberry32(uint32_t seed) : state(seed) {}
	uint32_t next_u32() {
		uint32_t t = (state += 0x6d2b79f5U);
		t = (t ^ (t >> 15)) * (t | 1U);
		t ^= t + (t ^ (t >> 7)) * (t | 61U);
		return t ^ (t >> 14);
	}
	double next() { return next_u32() / 4294967296.0; }
	double range(double a, double b) { return a + (b - a) * next(); }
	int irange_excl(int a, int b) { return a + int(std::floor(next() * (b - a))); }
};

// xoshiro128** seeded through splitmix32 (Roadcraft `Xoshiro128`, the 4.0 building stream): int(lo, hi) INCLUDES hi.
struct Xoshiro128 {
	uint32_t a = 0, b = 0, c = 0, d = 0;
	explicit Xoshiro128(uint32_t seed) {
		uint32_t x = seed;
		auto word = [&x]() {
			x += 0x9e3779b9U;
			uint32_t z = (x ^ (x >> 16)) * 0x21f0aaadU;
			z = (z ^ (z >> 15)) * 0x735a2d97U;
			return z ^ (z >> 15);
		};
		a = word(); b = word(); c = word(); d = word();
		if ((a | b | c | d) == 0) a = 1;
	}
	uint32_t next_u32() {
		uint32_t s1x5 = b * 5U;
		uint32_t out = ((s1x5 << 7) | (s1x5 >> 25)) * 9U;
		uint32_t t = b << 9;
		c ^= a; d ^= b; b ^= c; a ^= d; c ^= t;
		d = (d << 11) | (d >> 21);
		return out;
	}
	double f() { return next_u32() / 4294967296.0; }
	double range(double lo, double hi) { return lo + f() * (hi - lo); }
	int irange(int lo, int hi) { return lo + int(std::floor(f() * (hi - lo + 1))); }
	bool chance(double p) { return f() < p; }
};

// 2-D gradient noise, quintic fade, 8 gradients, roughly in [-1, 1] (Roadcraft `noise2`).
inline double noise2(double x, double y, uint32_t seed) {
	static constexpr double GRAD[16] = { 1, 0, -1, 0, 0, 1, 0, -1, 0.7071, 0.7071, -0.7071, 0.7071, 0.7071, -0.7071, -0.7071, -0.7071 };
	double x0 = std::floor(x), y0 = std::floor(y);
	double fx = x - x0, fy = y - y0;
	double u = fx * fx * fx * (fx * (fx * 6 - 15) + 10);
	double v = fy * fy * fy * (fy * (fy * 6 - 15) + 10);
	int32_t ix = js_int(x0), iy = js_int(y0);
	// Float32 gradients (Roadcraft stores GRAD in a Float32Array)
	auto g = [](int i) { return double(float(GRAD[i])); };
	int h = int(hash2(ix, iy, seed) & 7) * 2;
	double a = g(h) * fx + g(h + 1) * fy;
	h = int(hash2(ix + 1, iy, seed) & 7) * 2;
	double b = g(h) * (fx - 1) + g(h + 1) * fy;
	h = int(hash2(ix, iy + 1, seed) & 7) * 2;
	double c = g(h) * fx + g(h + 1) * (fy - 1);
	h = int(hash2(ix + 1, iy + 1, seed) & 7) * 2;
	double d = g(h) * (fx - 1) + g(h + 1) * (fy - 1);
	double ab = a + (b - a) * u;
	return 1.4 * (ab + ((c + (d - c) * u) - ab) * v);
}

inline double fbm2(double x, double y, uint32_t seed, int octaves, double lacunarity = 2, double gain = 0.5) {
	double sum = 0, amp = 1, freq = 1, norm = 0;
	for (int i = 0; i < octaves; i++) {
		sum += amp * noise2(x * freq, y * freq, seed + uint32_t(i) * 1013U);
		norm += amp;
		amp *= gain;
		freq *= lacunarity;
	}
	return sum / norm;
}

// The woods' lattice value in [0, 1) (Roadcraft `natureNoise.ts`): `a` and `b` as int32, one stream by `salt`.
inline double woods_hash(int32_t a, int32_t b, uint32_t salt) {
	uint32_t v = uint32_t(a) * 374761393U ^ uint32_t(b) * 668265263U ^ salt * 2246822519U;
	v = (v ^ (v >> 13)) * 1274126177U;
	return double(v ^ (v >> 16)) / 4294967296.0;
}

// One octave of the woods' smooth value noise, `scale` metres a cell.
inline double woods_noise(double x, double y, double scale, uint32_t salt) {
	double gx = x / scale, gy = y / scale, fx0 = std::floor(gx), fy0 = std::floor(gy);
	int32_t x0 = js_int(fx0), y0 = js_int(fy0);
	double fx = gx - fx0, fy = gy - fy0, sx = fx * fx * (3 - 2 * fx), sy = fy * fy * (3 - 2 * fy);
	double h00 = woods_hash(x0, y0, salt), h10 = woods_hash(x0 + 1, y0, salt), h01 = woods_hash(x0, y0 + 1, salt), h11 = woods_hash(x0 + 1, y0 + 1, salt);
	double top = h00 + (h10 - h00) * sx, bottom = h01 + (h11 - h01) * sx;
	return top + (bottom - top) * sy;
}

} // namespace city
