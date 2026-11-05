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
constexpr int WATER_CHUNKS_A_FRAME = 4;
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
	dirty_.clear();
}

void WaterRenderer::mark_all(const Heightfield &hf) {
	float h = hf.half_size();
	for_chunks(-h, -h, h, h, [&](int64_t k) { dirty_.push_back(k); });
}

void WaterRenderer::build(const Heightfield &hf, int64_t key) {
	MeshBuilder mb;
	const float step = hf.water_spacing_m, shift = hf.water_n * SHIFT_HALF;
	int x0 = clampi(int(std::floor(key_x(key) * CHUNK / step + shift)), 0, hf.water_n), x1 = clampi(int(std::floor((key_x(key) + 1) * CHUNK / step + shift)), 0, hf.water_n);
	int z0 = clampi(int(std::floor(key_y(key) * CHUNK / step + shift)), 0, hf.water_n), z1 = clampi(int(std::floor((key_y(key) + 1) * CHUNK / step + shift)), 0, hf.water_n);
	const Vector3 up(0, 1, 0);
	const Color white(1, 1, 1, 1);
	for (int gz = z0; gz < z1; gz++)
		for (int gx = x0; gx < x1; gx++) {
			float cell = cell_level(hf, gx, gz);
			if (cell == TerrainGen::NO_WATER) continue;
			auto level = [&](int cx, int cz) {
				float l = corner_level(hf, cx, cz);
				return l == TerrainGen::NO_WATER ? cell : l; // a corner with no water round it stays on the cell's surface
			};
			float ax = (gx - shift) * step, az = (gz - shift) * step;
			mb.quad(Vector3(ax, level(gx, gz), az), Vector3(ax + step, level(gx + 1, gz), az), Vector3(ax + step, level(gx + 1, gz + 1), az + step),
					Vector3(ax, level(gx, gz + 1), az + step), white, up);
		}
	Draw &d = chunks_[key];
	OwnedMesh next = mb.empty() ? OwnedMesh() : mb.bake();
	swap_base(d.inst, d.mesh, next.release(), scenario_, material_, false);
}

void WaterRenderer::sync(const Heightfield &hf) {
	if (dirty_.empty() || hf.water.empty()) return;
	std::sort(dirty_.begin(), dirty_.end());
	dirty_.erase(std::unique(dirty_.begin(), dirty_.end()), dirty_.end());
	for (int n = 0; n < WATER_CHUNKS_A_FRAME && !dirty_.empty(); n++) {
		build(hf, dirty_.back());
		dirty_.pop_back();
	}
}

} // namespace city
