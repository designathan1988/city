// Water surfaces per chunk (see water_renderer.h).
#include "render/water/water_renderer.h"

#include "core/grid.h"
#include "render/mesh_builder.h"
#include "sim/terrain/terrain_gen.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int CELLS_PER_UNIT = 64; // work partition, never a cap on output cells or quality
constexpr float SHIFT_HALF = 0.5f;

// The level at a corner: the highest water of the four samples round it.
float corner_level(const Heightfield &hf, int gx, int gz) {
	float best = TerrainGen::NO_WATER;
	for (int j = gz - 1; j <= gz; j++)
		for (int i = gx - 1; i <= gx; i++)
			if (i >= 0 && j >= 0 && i < hf.water_n && j < hf.water_n) best = std::max(best, hf.water[size_t(j) * hf.water_n + i]);
	return best;
}

// The water of a cell, the highest of its four samples: any wet corner draws it, its dry corners lying on ground at or over
// the level, so the shore is where the terrain comes through the surface and not along the grid.
float cell_level(const Heightfield &hf, int gx, int gz) { return corner_level(hf, gx + 1, gz + 1); }
} // namespace

bool WaterRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("water", errors);
	return errors.is_empty();
}

void WaterRenderer::reset() {
	RenderingServer *rs = RenderingServer::get_singleton();
	for (auto &kv : chunks_) {
		if (kv.second.inst.is_valid()) rs->free_rid(kv.second.inst);
		if (kv.second.mesh.is_valid()) rs->free_rid(kv.second.mesh);
	}
	chunks_.clear();
	work_.reset();
	dirty_.clear();
}

void WaterRenderer::mark_all(const Heightfield &hf) {
	float h = hf.half_size();
	for_chunks(-h, -h, h, h, [&](int64_t k) { dirty_.insert(k); });
}

uint64_t WaterRenderer::stamp(const Heightfield &hf, int64_t key) const {
	const float halo = hf.water_spacing_m * 2;
	return hf.water_stamp(key_x(key) * CHUNK - halo, key_y(key) * CHUNK - halo,
		(key_x(key) + 1) * CHUNK + halo, (key_y(key) + 1) * CHUNK + halo);
}

void WaterRenderer::begin(const Heightfield &hf, int64_t key) {
	work_.emplace();
	Work &w = *work_;
	w.key = key; w.stamp = stamp(hf, key);
	const float step = hf.water_spacing_m, shift = hf.water_n * SHIFT_HALF;
	w.x0 = clampi(int(std::floor(key_x(key) * CHUNK / step + shift)), 0, hf.water_n);
	w.x1 = clampi(int(std::floor((key_x(key) + 1) * CHUNK / step + shift)), 0, hf.water_n);
	w.gz = clampi(int(std::floor(key_y(key) * CHUNK / step + shift)), 0, hf.water_n);
	w.z1 = clampi(int(std::floor((key_y(key) + 1) * CHUNK / step + shift)), 0, hf.water_n);
	w.gx = w.x0;
	// Empty edge chunks still publish an empty replacement in a later admitted unit.
	if (w.x0 == w.x1) w.gz = w.z1;
}

void WaterRenderer::sync(const Heightfield &hf, BuildBudget &budget) {
	if (hf.water.empty()) return;
	while ((work_ || !dirty_.empty()) && budget.take()) {
		if (!work_) { begin(hf, *dirty_.rbegin()); continue; }
		Work &w = *work_;
		if (w.stamp != stamp(hf, w.key)) { const int64_t key = w.key; begin(hf, key); continue; }
		if (w.gz == w.z1) {
			Draw &d = chunks_[w.key];
			OwnedMesh next = w.mesh.empty() ? OwnedMesh() : w.mesh.bake();
			swap_base(d.inst, d.mesh, next.release(), scenario_, material_, false, LAYER_WATER);
			// All marks received so far describe the water version just rechecked under the map read lock.
			dirty_.erase(w.key); work_.reset();
			continue;
		}
		const float step = hf.water_spacing_m, shift = hf.water_n * SHIFT_HALF;
		const Vector3 up(0, 1, 0);
		const Color white(1, 1, 1, 1);
		for (int n = 0; n < CELLS_PER_UNIT && w.gz < w.z1; ++n) {
			const int gx = w.gx, gz = w.gz;
			const float cell = cell_level(hf, gx, gz);
			if (cell != TerrainGen::NO_WATER) {
				const auto level = [&](int cx, int cz) {
					const float l = corner_level(hf, cx, cz);
					return l == TerrainGen::NO_WATER ? cell : l;
				};
				const float ax = (gx - shift) * step, az = (gz - shift) * step;
				w.mesh.quad(Vector3(ax, level(gx, gz), az), Vector3(ax + step, level(gx + 1, gz), az),
					Vector3(ax + step, level(gx + 1, gz + 1), az + step), Vector3(ax, level(gx, gz + 1), az + step), white, up);
			}
			if (++w.gx == w.x1) { w.gx = w.x0; ++w.gz; }
		}
	}
}

} // namespace city
