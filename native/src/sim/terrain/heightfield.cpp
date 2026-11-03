// Terrain generation, height queries, raycast, sculpt brush and modifier composition.
#include "sim/terrain/heightfield.h"

#include "sim/terrain/terrain_gen.h"

#include <algorithm>
#include <cmath>

#include <thread>

namespace city {

namespace {
constexpr float RAY_STEP = 0.75f, RAY_FAST_ABOVE = 40.f, RAY_FAST_MAX = 60.f;
constexpr int RAY_REFINE = 18;
} // namespace

void Heightfield::init(int tiles, float tile_m, float spacing_m, uint32_t p_seed, ChangeLog *p_log, const Tuning *p_tune) {
	log = p_log;
	tune = p_tune;
	seed = p_seed;
	spacing = spacing_m;
	region_size = int(std::lround(tile_m / spacing_m));
	regions = clampi(tiles, 2, TERRAIN_MAX_REGIONS);
	N = regions * region_size;
	base_q.assign(size_t(N) * N, quant(0.f));
	final_h.assign(size_t(N) * N, 0.f);
	mods.clear();
	free_mods.clear();
	mod_hash.clear();
	mod_hash.cell = SUBCHUNK;
	water.clear();
	water_n = 0;
	tile_dirty.assign(size_t(regions) * regions, 0);
}

void Heightfield::fill(TerrainGen &gen, float water_spacing) {
	auto rows = [&](int z0, int z1) {
		for (int iz = z0; iz < z1; iz++)
			for (int ix = 0; ix < N; ix++) {
				size_t i = size_t(iz) * N + ix;
				base_q[i] = quant(float(gen.base_height(x_of(ix), x_of(iz))));
				final_h[i] = dequant(base_q[i]);
			}
	};
	unsigned hw = std::max(1u, std::thread::hardware_concurrency());
	std::vector<std::thread> threads;
	int per = N / int(hw) + 1;
	for (unsigned t = 0; t < hw; t++) {
		int z0 = int(t) * per, z1 = std::min(N, z0 + per);
		if (z0 < z1) threads.emplace_back(rows, z0, z1);
	}
	for (auto &t : threads) t.join();
	// water: one sample per water_spacing, the river and lakes of the generator (its lake cache is not shared)
	water_spacing_m = water_spacing;
	water_n = int(std::lround(N * spacing / water_spacing));
	water.assign(size_t(water_n) * water_n, TerrainGen::NO_WATER);
	if (gen.flat()) return;
	for (int gz = 0; gz < water_n; gz++)
		for (int gx = 0; gx < water_n; gx++) water[size_t(gz) * water_n + gx] = gen.water_at_sample(gx, gz);
}

float Heightfield::water_at(float x, float z) const {
	if (water_n == 0) return TerrainGen::NO_WATER;
	int gx = int(std::lround((x + half_size()) / water_spacing_m)), gz = int(std::lround((z + half_size()) / water_spacing_m));
	if (gx < 0 || gz < 0 || gx >= water_n || gz >= water_n) return TerrainGen::NO_WATER;
	return water[size_t(gz) * water_n + gx];
}

float Heightfield::height_at(float x, float z) const {
	return grid_sample(x, z, N, spacing, [&](size_t i) { return final_h[i]; });
}

float Heightfield::base_height_at(float x, float z) const {
	return grid_sample(x, z, N, spacing, [&](size_t i) { return dequant(base_q[i]); });
}

HeightPatch Heightfield::patch(float minx, float minz, float maxx, float maxz) const {
	HeightPatch p;
	p.N = N;
	p.spacing = spacing;
	if (N < 2 || final_h.empty()) return p;
	// grid_sample reads nodes floor(f) and floor(f) + 1: one node of margin below, two above
	p.x0 = clampi(ix_of(minx) - 1, 0, N - 1);
	p.z0 = clampi(ix_of(minz) - 1, 0, N - 1);
	const int x1 = clampi(ix_of(maxx) + 2, 0, N - 1), z1 = clampi(ix_of(maxz) + 2, 0, N - 1);
	p.w = x1 - p.x0 + 1;
	p.h = z1 - p.z0 + 1;
	p.final_h.resize(size_t(p.w) * size_t(p.h));
	for (int z = 0; z < p.h; z++)
		std::copy_n(final_h.begin() + long(size_t(p.z0 + z) * size_t(N) + size_t(p.x0)), p.w, p.final_h.begin() + long(size_t(z) * size_t(p.w)));
	return p;
}

bool Heightfield::raycast(const V2 &o2, float oy, const V2 &d2, float dy, float max_dist, float &hx, float &hy, float &hz) const {
	auto above = [&](float t) { return oy + dy * t - height_at(o2.x + d2.x * t, o2.y + d2.y * t); };
	if (above(0.f) < 0) return false;
	float step = spacing * RAY_STEP, prev_t = 0.f;
	for (float t = step; t < max_dist; t += step) {
		float a = above(t);
		if (a <= 0.f) {
			float lo = prev_t, hi = t;
			for (int i = 0; i < RAY_REFINE; i++) {
				float m = (lo + hi) * 0.5f;
				if (above(m) > 0) lo = m; else hi = m;
			}
			hx = o2.x + d2.x * hi;
			hz = o2.y + d2.y * hi;
			hy = height_at(hx, hz);
			return in_bounds(hx, hz);
		}
		prev_t = t;
		if (a > RAY_FAST_ABOVE) t += std::min(a * 0.5f, RAY_FAST_MAX); // skip fast while high above ground
	}
	return false;
}

int Heightfield::add_modifier(const std::vector<float> &pts, float flat, float max_run) {
	int id = free_mods.empty() ? int(mods.size()) : free_mods.back();
	TerrainModifier m;
	m.pts = pts;
	m.flat = flat;
	m.max_run = max_run;
	m.minx = m.minz = BIG;
	m.maxx = m.maxz = -BIG;
	for (size_t i = 0; i + 2 < pts.size(); i += 3) {
		m.minx = std::min(m.minx, pts[i]);
		m.maxx = std::max(m.maxx, pts[i]);
		m.minz = std::min(m.minz, pts[i + 2]);
		m.maxz = std::max(m.maxz, pts[i + 2]);
	}
	float e = flat + max_run;
	m.minx -= e; m.minz -= e; m.maxx += e; m.maxz += e;
	put_modifier(id, m);
	return id;
}

void Heightfield::put_modifier(int id, const TerrainModifier &rec) {
	if (id >= int(mods.size())) {
		for (int k = int(mods.size()); k < id; k++) free_mods.push_back(k);
		mods.resize(size_t(id) + 1);
	}
	free_mods.erase(std::remove(free_mods.begin(), free_mods.end(), id), free_mods.end());
	TerrainModifier &m = mods[id];
	m = rec;
	m.alive = true;
	mod_hash.insert_aabb(id, m.minx, m.minz, m.maxx, m.maxz);
	recompose_world_rect(m.minx, m.minz, m.maxx, m.maxz);
}

void Heightfield::write_samples(const std::vector<uint32_t> &idx, const std::vector<uint16_t> &values) {
	SampleRect r{ N, N, -1, -1 };
	for (size_t k = 0; k < idx.size() && k < values.size(); k++) {
		uint32_t i = idx[k];
		if (i >= base_q.size()) continue;
		base_q[i] = values[k];
		mark_tile(i);
		int x = int(i % N), z = int(i / N);
		r.x0 = std::min(r.x0, x); r.x1 = std::max(r.x1, x);
		r.z0 = std::min(r.z0, z); r.z1 = std::max(r.z1, z);
	}
	if (!r.empty()) recompose(r.x0 - 1, r.z0 - 1, r.x1 + 1, r.z1 + 1);
}

void Heightfield::remove_modifier(int id) {
	if (id < 0 || id >= int(mods.size()) || !mods[id].alive) return;
	TerrainModifier &m = mods[id];
	mod_hash.remove_aabb(id, m.minx, m.minz, m.maxx, m.maxz);
	m.alive = false;
	m.pts.clear();
	free_mods.push_back(id);
	recompose_world_rect(m.minx, m.minz, m.maxx, m.maxz);
}

void Heightfield::recompose_world_rect(float minx, float minz, float maxx, float maxz) {
	recompose(ix_of(minx) - 1, ix_of(minz) - 1, ix_of(maxx) + 1, ix_of(maxz) + 1);
}

void Heightfield::apply_modifier(const TerrainModifier &m, int x0, int z0, int x1, int z1, std::vector<float> &best_w) {
	const int W = x1 - x0 + 1;
	int ax0 = std::max(x0, ix_of(m.minx)), ax1 = std::min(x1, ix_of(m.maxx) + 1);
	int az0 = std::max(z0, ix_of(m.minz)), az1 = std::min(z1, ix_of(m.maxz) + 1);
	const size_t np = m.pts.size() / 3;
	for (int z = az0; z <= az1; z++)
		for (int x = ax0; x <= ax1; x++) {
			V2 p(x_of(x), x_of(z));
			float best_d = BIG, th = 0.f;
			if (np == 1) {
				best_d = (p - V2(m.pts[0], m.pts[2])).len();
				th = m.pts[1];
			}
			for (size_t i = 0; i + 1 < np; i++) {
				const float *a = &m.pts[i * 3], *b = a + 3;
				float t;
				float d = dist_point_segment(p, V2(a[0], a[2]), V2(b[0], b[2]), &t);
				if (d < best_d) {
					best_d = d;
					th = lerpf(a[1], b[1], t);
				}
			}
			size_t gi = size_t(z) * N + x;
			float base = dequant(base_q[gi]);
			float run = clampf(std::fabs(base - th) * tune->terrain_embankment_ratio, tune->terrain_min_run_m, m.max_run);
			float w = best_d <= m.flat ? 1.f : 1.f - (best_d - m.flat) / run;
			if (w <= 0.f) continue;
			w = smooth01(w);
			size_t li = size_t(z - z0) * W + (x - x0);
			if (w > best_w[li]) {
				best_w[li] = w;
				final_h[gi] = lerpf(base, th - tune->terrain_road_sink_m, w); // terrain sits just under the road surface
			}
		}
}

void Heightfield::recompose(int x0, int z0, int x1, int z1) {
	x0 = clampi(x0, 0, N - 1); x1 = clampi(x1, 0, N - 1);
	z0 = clampi(z0, 0, N - 1); z1 = clampi(z1, 0, N - 1);
	if (x0 > x1 || z0 > z1) return;
	for (int z = z0; z <= z1; z++)
		for (int x = x0; x <= x1; x++) final_h[size_t(z) * N + x] = dequant(base_q[size_t(z) * N + x]);
	std::vector<float> best_w(size_t(x1 - x0 + 1) * (z1 - z0 + 1), 0.f);
	float minx = x_of(x0), maxx = x_of(x1), minz = x_of(z0), maxz = x_of(z1);
	std::vector<int> ids;
	mod_hash.query(minx, minz, maxx, maxz, [&](int id) { ids.push_back(id); });
	std::sort(ids.begin(), ids.end());
	ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
	for (int id : ids)
		if (mods[id].alive) apply_modifier(mods[id], x0, z0, x1, z1, best_w);
	if (log) log->terrain_rects.insert(log->terrain_rects.end(), { x0, z0, x1, z1 });
}

void Heightfield::finish_load() {
	final_h.resize(base_q.size());
	for (size_t i = 0; i < base_q.size(); i++) final_h[i] = dequant(base_q[i]);
	mods.clear();
	free_mods.clear();
	mod_hash.clear();
	mod_hash.cell = SUBCHUNK;
}

} // namespace city
