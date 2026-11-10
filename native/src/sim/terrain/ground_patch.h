// The ground as a reader off the map lock takes it: GroundPatch, a copy of a window of the land (the final heights, and when asked
// the natural ground and the water samples) made by Heightfield::ground_patch while the map is held for a moment, sampled exactly
// as the Heightfield samples itself; and GroundRead, the one handle a job-path function reads the ground through, the live
// Heightfield (its caller holds the map) or a patch (a job's copy). A read outside a patch's window stops the tests' DLL
// (core/checks.h): the copy was cut too small for what the job reads.
#pragma once

#include "core/checks.h"
#include "sim/terrain/heightfield.h"
#include "sim/terrain/terrain_gen.h"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace city {

struct GroundPatch {
	int N = 0; // the whole grid's side and spacing (the patch is a window of it)
	float spacing = 1;
	float water_level = 0;
	int x0 = 0, z0 = 0, w = 0, h = 0; // grid nodes [x0, x0 + w) x [z0, z0 + h)
	std::vector<float> final_h; // row-major, w a row
	std::vector<uint16_t> base_q; // the natural ground of the same window, quantized as the Heightfield keeps it; empty unless asked
	int water_n = 0; // the whole water grid's side and spacing
	float water_spacing = 0;
	int wx0 = 0, wz0 = 0, ww = 0, wh = 0; // water samples [wx0, wx0 + ww) x [wz0, wz0 + wh); none unless asked
	std::vector<float> water;

	bool made() const { return !final_h.empty(); }
	float half_size() const { return float(N) * spacing * 0.5f; }
	float final_at(int ix, int iz) const { return final_h[at(ix, iz)]; }
	float height_at(float x, float z) const {
		if (final_h.empty()) return 0.f;
		return grid_sample(x, z, N, spacing, [&](int ix, int iz) { return final_h[at(ix, iz)]; });
	}
	float base_height_at(float x, float z) const {
		if (CHECKS && base_q.empty()) city_stop("the natural ground read from a patch cut without it");
		if (base_q.empty()) return height_at(x, z);
		return grid_sample(x, z, N, spacing, [&](int ix, int iz) { return height_dequant(base_q[at(ix, iz)]); });
	}
	// The water sample (gx, gz) of the whole grid; NO_WATER off the grid, as Heightfield::water_at gives.
	float water_sample(int gx, int gz) const {
		if (gx < 0 || gz < 0 || gx >= water_n || gz >= water_n) return TerrainGen::NO_WATER;
		const bool in = gx >= wx0 && gz >= wz0 && gx < wx0 + ww && gz < wz0 + wh;
		if (CHECKS && !in) city_stop("a water sample outside the window of the ground a job copied");
		return in ? water[size_t(gz - wz0) * size_t(ww) + size_t(gx - wx0)] : TerrainGen::NO_WATER;
	}
	float water_at(float x, float z) const {
		if (water_n == 0) return TerrainGen::NO_WATER;
		return water_sample(int(std::lround((x + half_size()) / water_spacing)), int(std::lround((z + half_size()) / water_spacing)));
	}

private:
	size_t at(int ix, int iz) const {
		const bool in = ix >= x0 && iz >= z0 && ix < x0 + w && iz < z0 + h;
		if (CHECKS && !in) city_stop("a ground sample outside the window of the ground a job copied");
		return size_t(clampi(iz - z0, 0, h - 1)) * size_t(w) + size_t(clampi(ix - x0, 0, w - 1));
	}
};

// What Heightfield::ground_patch copies besides the final heights.
enum GroundParts : unsigned { PATCH_FINAL = 0, PATCH_NATURAL = 1, PATCH_WATER = 2 };

class GroundRead {
public:
	GroundRead(const Heightfield &live) : live_(&live) {} // read in place: the caller holds the map
	GroundRead(const GroundPatch &patch) : patch_(&patch) {}
	int n() const { return patch_ ? patch_->N : live_->N; }
	float spacing() const { return patch_ ? patch_->spacing : live_->spacing; }
	bool made() const { return patch_ ? patch_->made() : live_->N >= 2 && !live_->finals.empty(); } // a land stands
	float half_size() const { return patch_ ? patch_->half_size() : live_->half_size(); }
	bool in_bounds(float x, float z, float margin = 0.f) const {
		const float half = half_size() - margin;
		return x > -half && x < half && z > -half && z < half;
	}
	float x_of(int ix) const { return (float(ix) - float(n()) * 0.5f) * spacing(); }
	int ix_of(float x) const { return int(std::floor(x / spacing() + float(n()) * 0.5f)); } // as Heightfield::ix_of
	float height_at(float x, float z) const { return patch_ ? patch_->height_at(x, z) : live_->height_at(x, z); }
	float final_at(int ix, int iz) const { return patch_ ? patch_->final_at(ix, iz) : live_->final_at(ix, iz); }
	float base_height_at(float x, float z) const { return patch_ ? patch_->base_height_at(x, z) : live_->base_height_at(x, z); }
	float water_level() const { return patch_ ? patch_->water_level : live_->water_level; }
	float water_at(float x, float z) const { return patch_ ? patch_->water_at(x, z) : live_->water_at(x, z); }
	bool is_water(float x, float z, float clearance) const {
		const float ground = base_height_at(x, z) - clearance;
		return water_at(x, z) > ground || water_level() > ground;
	}
	// The water grid as the water surfaces read it.
	int water_n() const { return patch_ ? patch_->water_n : live_->water_n; }
	float water_spacing() const { return patch_ ? patch_->water_spacing : live_->water_spacing_m; }
	float water_sample(int gx, int gz) const {
		if (patch_) return patch_->water_sample(gx, gz);
		if (gx < 0 || gz < 0 || gx >= live_->water_n || gz >= live_->water_n) return TerrainGen::NO_WATER;
		return live_->water[size_t(gz) * size_t(live_->water_n) + size_t(gx)];
	}
	const Heightfield *live() const { return live_; } // the live land, for a reader that holds the map; null for a patch

private:
	const Heightfield *live_ = nullptr;
	const GroundPatch *patch_ = nullptr;
};

} // namespace city
