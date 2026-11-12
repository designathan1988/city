// The terrain's bilinear sampler and an immutable final-height version with clamped window semantics.
// Heightfield::patch captures its radix root/metadata under the map read lock; sampling needs no copy or source lock.
#pragma once

#include "core/math.h"
#include "core/paged_grid.h"

#include <cmath>
#include <cstddef>
#include <vector>

namespace city {

// Bilinear sample of an N x N per-vertex grid at world (x, z) (world x = (ix - N/2) * spacing, clamped to the grid);
// value(i) gives the stored sample i (index iz * N + ix) in metres.
template <typename F>
float grid_sample_nodes(float x, float z, int N, float spacing, F &&value) {
	float fx = x / spacing + N * 0.5f, fz = z / spacing + N * 0.5f;
	int ix = clampi(int(std::floor(fx)), 0, N - 2), iz = clampi(int(std::floor(fz)), 0, N - 2);
	float tx = clampf(fx - ix, 0, 1), tz = clampf(fz - iz, 0, 1);
	return lerpf(lerpf(value(ix, iz), value(ix + 1, iz), tx), lerpf(value(ix, iz + 1), value(ix + 1, iz + 1), tx), tz);
}

template <typename F>
float grid_sample(float x, float z, int N, float spacing, F &&value) {
	return grid_sample_nodes(x, z, N, spacing, [&](int ix, int iz) { return value(size_t(iz) * size_t(N) + size_t(ix)); });
}

struct HeightPatch {
	int N = 0; // the whole grid's side and spacing (the patch is a window of it)
	float spacing = 1;
	int x0 = 0, z0 = 0, w = 0, h = 0; // grid nodes [x0, x0 + w) x [z0, z0 + h)
	PagedGrid<float>::Snapshot final_h; // retained source version, no copy of the window or mutable reader cache

	HeightPatch regional() const {
		HeightPatch result = *this;
		if (!final_h.empty()) result.final_h = final_h.window(x0, z0, w, h);
		return result;
	}

	// The same value as Heightfield::height_at for every point whose four grid nodes lie in the window.
	float height_at(float x, float z) const {
		if (final_h.empty()) return 0.f;
		auto values = final_h.reader();
		return grid_sample_nodes(x, z, N, spacing, [&](int ix, int iz) {
			const int gx = clampi(ix - x0, 0, w - 1), gz = clampi(iz - z0, 0, h - 1);
			return values.at(x0 + gx, z0 + gz);
		});
	}
};

} // namespace city
