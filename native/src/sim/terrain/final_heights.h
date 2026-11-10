// The land's final heights kept by tile (FinalHeights): the natural ground but where a road, a pad or the ground shaped to a road
// changed it, and the 16-bit form a height is stored in. The Heightfield owns one and hands its base heights in.
#pragma once

#include "core/math.h"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

namespace city {

constexpr float HEIGHT_Q_OFFSET = 100.f, HEIGHT_Q_SCALE = 100.f; // 1 cm steps from -100 m
constexpr float QUANT_LIMIT = 65535.f; // a 16-bit height's top

// A 16-bit height in metres, and back.
inline uint16_t height_quant(float h) { return uint16_t(clampf((h + HEIGHT_Q_OFFSET) * HEIGHT_Q_SCALE, 0.f, QUANT_LIMIT)); }
inline float height_dequant(uint16_t q) { return q / HEIGHT_Q_SCALE - HEIGHT_Q_OFFSET; }

// The land's final heights: the natural ground (`base`, the Heightfield's own, handed in) but where a road, a pad or the ground
// shaped to a road changed it. Kept by tile, a tile made the first time something is written in it; anywhere else a height is the
// base's. A second copy of the whole map, as it was, held 105 MiB for a 20 km land that differs from its base under the streets
// alone. A sample is named by its column and row: its tile and its place in it are a shift and a mask (a tile's side is a power of
// two), never a division. Tiles are made by whoever writes first, on any thread (rectangles that share no sample are composed at
// once, compose.cpp): the slot is taken by compare and swap, the loser's tile freed. Reads and writes of one sample are never
// concurrent (the map's lock). It moves with its land (a city is taken in whole by swapping).
class FinalHeights {
public:
	using Base = std::vector<uint16_t>;
	static constexpr int SHIFT = 6; // a tile is 2^6 = 64 samples a side: 16 KiB, a block of streets at the 4 m grid
	static constexpr int TILE = 1 << SHIFT, MASK = TILE - 1;
	FinalHeights() = default;
	FinalHeights(FinalHeights &&o) noexcept { take(o); }
	FinalHeights &operator=(FinalHeights &&o) noexcept {
		if (this != &o) {
			clear();
			take(o);
		}
		return *this;
	}
	~FinalHeights() { clear(); }
	// A land of n x n samples: no tile yet, every height the base's.
	void reset(int n) {
		clear();
		n_ = n;
		side_ = (n + MASK) >> SHIFT;
		tiles_.reset(new std::atomic<float *>[size_t(side_) * side_]);
		for (size_t i = 0; i < size_t(side_) * side_; i++) tiles_[i].store(nullptr, std::memory_order_relaxed);
	}
	bool empty() const { return n_ == 0; }
	float at(int x, int z, const Base &base) const {
		const float *t = slot(x, z).load(std::memory_order_acquire);
		return t ? t[within(x, z)] : height_dequant(base[size_t(z) * size_t(n_) + size_t(x)]);
	}
	// The sample's height, its tile made (of the base's heights) when it has none.
	void put(int x, int z, float h, const Base &base) { tile_of(x >> SHIFT, z >> SHIFT, base)[within(x, z)] = h; }
	// `count` heights of row z from column x0, a tile's run at a time.
	void row(int z, int x0, int count, float *out, const Base &base) const {
		for (int x = x0, end = x0 + count; x < end;) {
			const int run = std::min(end, ((x >> SHIFT) + 1) << SHIFT) - x;
			const float *t = slot(x, z).load(std::memory_order_acquire);
			if (t) {
				std::memcpy(out, t + within(x, z), size_t(run) * sizeof(float));
			} else {
				const uint16_t *b = base.data() + size_t(z) * size_t(n_) + size_t(x);
				for (int k = 0; k < run; k++) out[k] = height_dequant(b[k]);
			}
			out += run;
			x += run;
		}
	}
	// The rectangle (inclusive) back to the base's heights: the tiles that stand, nothing made.
	void to_base(int x0, int z0, int x1, int z1, const Base &base) {
		for (int z = z0; z <= z1; z++)
			for (int x = x0; x <= x1;) {
				const int run = std::min(x1 + 1, ((x >> SHIFT) + 1) << SHIFT) - x;
				float *t = slot(x, z).load(std::memory_order_acquire);
				if (t) {
					float *d = t + within(x, z);
					const uint16_t *b = base.data() + size_t(z) * size_t(n_) + size_t(x);
					for (int k = 0; k < run; k++) d[k] = height_dequant(b[k]);
				}
				x += run;
			}
	}
	size_t bytes() const {
		size_t made = 0;
		for (size_t i = 0; tiles_ && i < size_t(side_) * side_; i++) made += tiles_[i].load(std::memory_order_relaxed) ? 1 : 0;
		return made * size_t(TILE) * TILE * sizeof(float);
	}

private:
	static size_t within(int x, int z) { return (size_t(z & MASK) << SHIFT) | size_t(x & MASK); }
	std::atomic<float *> &slot(int x, int z) const { return tiles_[size_t(z >> SHIFT) * size_t(side_) + size_t(x >> SHIFT)]; }
	float *tile_of(int tx, int tz, const Base &base) {
		std::atomic<float *> &at = tiles_[size_t(tz) * size_t(side_) + size_t(tx)];
		float *t = at.load(std::memory_order_acquire);
		if (t) return t;
		float *made = new float[size_t(TILE) * TILE];
		for (int z = 0; z < TILE; z++)
			for (int x = 0; x < TILE; x++) {
				const int gx = (tx << SHIFT) + x, gz = (tz << SHIFT) + z;
				made[(size_t(z) << SHIFT) | size_t(x)] = gx < n_ && gz < n_ ? height_dequant(base[size_t(gz) * size_t(n_) + size_t(gx)]) : 0.f;
			}
		if (at.compare_exchange_strong(t, made, std::memory_order_acq_rel)) return made;
		delete[] made; // another thread made it first
		return t;
	}
	void take(FinalHeights &o) {
		n_ = o.n_;
		side_ = o.side_;
		tiles_ = std::move(o.tiles_);
		o.n_ = o.side_ = 0;
	}
	void clear() {
		for (size_t i = 0; tiles_ && i < size_t(side_) * side_; i++) delete[] tiles_[i].load(std::memory_order_relaxed);
		tiles_.reset();
		n_ = side_ = 0;
	}
	int n_ = 0, side_ = 0;
	std::unique_ptr<std::atomic<float *>[]> tiles_;
};

} // namespace city
