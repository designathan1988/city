// The terrain's one bilinear sampler (grid_sample) and a copy of the final heights over a box (Heightfield::patch),
// sampled with it exactly as Heightfield::height_at: a mesh job copies the ground under its lots while it holds the
// map lock briefly and builds from the copy without the lock.
#pragma once

#include "core/math.h"

#include <cmath>
#include <cstddef>
#include <vector>

namespace city {

// Bilinear sample of an N x N per-vertex grid at world (x, z) (world x = (ix - N/2) * spacing, clamped to the grid);
// value(ix, iz) gives the stored sample of column ix, row iz, in metres (the caller has both: no index is turned back into them).
template <typename F>
float grid_sample(float x, float z, int N, float spacing, F &&value) {
	float fx = x / spacing + N * 0.5f, fz = z / spacing + N * 0.5f;
	int ix = clampi(int(std::floor(fx)), 0, N - 2), iz = clampi(int(std::floor(fz)), 0, N - 2);
	float tx = clampf(fx - ix, 0, 1), tz = clampf(fz - iz, 0, 1);
	return lerpf(lerpf(value(ix, iz), value(ix + 1, iz), tx), lerpf(value(ix, iz + 1), value(ix + 1, iz + 1), tx), tz);
}

struct HeightPatch {
	int N = 0; // the whole grid's side and spacing (the patch is a window of it)
	float spacing = 1;
	int x0 = 0, z0 = 0, w = 0, h = 0; // grid nodes [x0, x0 + w) x [z0, z0 + h)
	std::vector<float> final_h; // row-major, w per row

	// The same value as Heightfield::height_at for every point whose four grid nodes lie in the window.
	float height_at(float x, float z) const {
		if (final_h.empty()) return 0.f;
		return grid_sample(x, z, N, spacing, [&](int ix, int iz) {
			return final_h[size_t(clampi(iz - z0, 0, h - 1)) * size_t(w) + size_t(clampi(ix - x0, 0, w - 1))];
		});
	}
};

} // namespace city
