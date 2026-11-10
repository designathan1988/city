// Water surfaces per chunk (see water_renderer.h).
#include "render/water/water_renderer.h"

#include "core/grid.h"
#include "render/mesh_builder.h"
#include "sim/state/city_cut.h"
#include "sim/terrain/terrain_gen.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float SHIFT_HALF = 0.5f;

// The level at a corner: the highest water of the four samples round it.
float corner_level(const GroundRead &ground, int gx, int gz) {
	float best = TerrainGen::NO_WATER;
	for (int j = gz - 1; j <= gz; j++)
		for (int i = gx - 1; i <= gx; i++)
			if (i >= 0 && j >= 0 && i < ground.water_n() && j < ground.water_n()) best = std::max(best, ground.water_sample(i, j));
	return best;
}

// The water of a cell, the highest of its four samples: any wet corner draws it, its dry corners lying on ground at or over
// the level, so the shore is where the terrain comes through the surface and not along the grid.
float cell_level(const GroundRead &ground, int gx, int gz) { return corner_level(ground, gx + 1, gz + 1); }
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
	clear_queue();
}

// A cell draws when one of its four samples holds water (cell_level): a sample is under the cells either side of it on each axis,
// and those cells' chunks are the ones asked. A map with no chunk drawn yet (reset) needs no other.
void WaterRenderer::mark_wet(const Heightfield &hf) {
	const float step = hf.water_spacing_m, shift = hf.water_n * SHIFT_HALF;
	const int n = hf.water_n;
	// the first cell of chunk column (or row) k, as build() cuts them
	auto first_cell = [&](int k) { return clampi(int(std::floor(k * CHUNK / step + shift)), 0, n); };
	auto chunk_of_cell = [&](int g) {
		const int guess = int(std::floor((float(g) - shift) * step / CHUNK));
		for (int k = guess - 1; k <= guess + 1; k++)
			if (first_cell(k) <= g && g < first_cell(k + 1)) return k;
		return guess;
	};
	for (int sz = 0; sz < n; sz++)
		for (int sx = 0; sx < n; sx++) {
			if (hf.water[size_t(sz) * n + sx] == TerrainGen::NO_WATER) continue;
			for (int gz = std::max(0, sz - 1); gz <= std::min(n - 1, sz); gz++)
				for (int gx = std::max(0, sx - 1); gx <= std::min(n - 1, sx); gx++) dirty_.insert(cell_key(chunk_of_cell(gx), chunk_of_cell(gz)));
		}
}

void WaterRenderer::build(const GroundRead &ground, int64_t key, MeshBuilder &mb) {
	const int n = ground.water_n();
	const float step = ground.water_spacing(), shift = n * SHIFT_HALF;
	int x0 = clampi(int(std::floor(key_x(key) * CHUNK / step + shift)), 0, n), x1 = clampi(int(std::floor((key_x(key) + 1) * CHUNK / step + shift)), 0, n);
	int z0 = clampi(int(std::floor(key_y(key) * CHUNK / step + shift)), 0, n), z1 = clampi(int(std::floor((key_y(key) + 1) * CHUNK / step + shift)), 0, n);
	const Vector3 up(0, 1, 0);
	const Color white(1, 1, 1, 1);
	for (int gz = z0; gz < z1; gz++)
		for (int gx = x0; gx < x1; gx++) {
			float cell = cell_level(ground, gx, gz);
			if (cell == TerrainGen::NO_WATER) continue;
			auto level = [&](int cx, int cz) {
				float l = corner_level(ground, cx, cz);
				return l == TerrainGen::NO_WATER ? cell : l; // a corner with no water round it stays on the cell's surface
			};
			float ax = (gx - shift) * step, az = (gz - shift) * step;
			mb.quad(Vector3(ax, level(gx, gz), az), Vector3(ax + step, level(gx + 1, gz), az), Vector3(ax + step, level(gx + 1, gz + 1), az + step),
					Vector3(ax, level(gx, gz + 1), az + step), white, up);
		}
}

void WaterRenderer::commit_one(BuiltMesh &b) {
	Draw &d = chunks_[b.key];
	swap_base(d.inst, d.mesh, b.mesh.release(), scenario_, material_, false, LAYER_WATER);
}

void WaterRenderer::offer(const CityState &s, const Vector3 &cam, std::vector<FrameAdmission::Unit> &out) {
	if (dirty_.empty() || s.terrain.water.empty() || !s.jobs) return;
	JobPool &pool = *s.jobs;
	for (int64_t key : dirty_) {
		if (busy(key)) continue; // its result lands first; the chunk stays marked and is asked again then
		const float dx = (float(key_x(key)) + SHIFT_HALF) * CHUNK - cam.x, dz = (float(key_y(key)) + SHIFT_HALF) * CHUNK - cam.z;
		const float d = std::sqrt(dx * dx + dz * dz);
		out.push_back(FrameAdmission::Unit{ chunks_.count(key) ? 1 : 0, d, 1, [this, &s, &pool, key, d]() {
					   dirty_.erase(key);
					   // the job copies the water samples its cells and their corners read (a sample each side of the chunk), then
					   // builds from the copy with the map let go
					   submit_cut(pool, JobKey{ JobLane::WATER, key }, key, d,
							   [&s, key]() {
								   return cut_city(s, CUT_WATER, [key](const CityState &live) {
									   const float x0 = float(key_x(key)) * CHUNK, z0 = float(key_y(key)) * CHUNK;
									   return CutBox{ x0, z0, x0 + CHUNK, z0 + CHUNK }.grown(live.terrain.water_spacing_m);
								   });
							   },
							   [key](const CityCut &cut, MeshBuilder &mb) { build(GroundRead(cut.ground), key, mb); });
				   } });
	}
}

} // namespace city
