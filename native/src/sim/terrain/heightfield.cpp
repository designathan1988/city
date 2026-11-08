// Terrain generation, height queries, raycast, sculpt brush and modifier composition.
#include "sim/terrain/heightfield.h"

#include "core/job_pool.h"
#include "sim/terrain/terrain_gen.h"

#include <algorithm>
#include <cmath>


namespace city {

namespace {
// a ray's march over the ground: its step, and its longer steps while it is high over it (from 40 m up, at most 60 m), then halvings
// round the crossing
constexpr float RAY_STEP = 0.75f, RAY_FAST_ABOVE = 40.f, RAY_FAST_MAX = 60.f;
constexpr int RAY_REFINE = 18;
} // namespace

void Heightfield::init(int tiles, float tile_m, float spacing_m, uint32_t p_seed, ChangeLog *p_log, const Tuning *p_tune, JobPool *p_jobs) {
	log = p_log;
	jobs = p_jobs;
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
	road_shapes.clear();
	shape_hash.clear();
	shape_hash.cell = SUBCHUNK;
	ground_moved.clear();
	waiting.clear(); // rectangles of the land before: a hold open around the founding stays open
	water.clear();
	water_original.clear();
	water_n = 0;
	tile_dirty.assign(size_t(regions) * regions, 0);
}

void Heightfield::fill(TerrainGen &gen, float water_spacing) {
	auto rows = [&](size_t n, const std::function<void(size_t)> &row) {
		if (jobs) jobs->parallel_for(n, row);
		else for (size_t k = 0; k < n; k++) row(k);
	};
	rows(size_t(N), [&](size_t iz) {
		for (int ix = 0; ix < N; ix++) {
			size_t i = iz * N + ix;
			base_q[i] = quant(float(gen.base_height(x_of(ix), x_of(int(iz)))));
			final_h[i] = dequant(base_q[i]);
		}
	});
	// water: one sample per water_spacing, the river and lakes of the generator (the lakes made first: then it only reads)
	water_spacing_m = water_spacing;
	water_n = int(std::lround(N * spacing / water_spacing));
	water.assign(size_t(water_n) * water_n, TerrainGen::NO_WATER);
	if (gen.flat()) return;
	gen.plan_lakes(water_n, jobs);
	rows(size_t(water_n), [&](size_t gz) {
		for (int gx = 0; gx < water_n; gx++) water[gz * water_n + gx] = gen.water_at_sample(gx, int(gz));
	});
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
	if (r.empty()) return;
	ground_moved.insert(ground_moved.end(), { x_of(r.x0), x_of(r.z0), x_of(r.x1), x_of(r.z1) }); // the roads there read it again
	recompose(r.x0 - 1, r.z0 - 1, r.x1 + 1, r.z1 + 1);
}

void Heightfield::put_road_shape(int id, const RoadShape &r) {
	if (id >= int(road_shapes.size())) road_shapes.resize(size_t(id) + 1);
	RoadShape &old = road_shapes[size_t(id)];
	float minx = r.minx, minz = r.minz, maxx = r.maxx, maxz = r.maxz;
	if (old.alive) {
		shape_hash.remove_aabb(id, old.minx, old.minz, old.maxx, old.maxz);
		minx = std::min(minx, old.minx); minz = std::min(minz, old.minz);
		maxx = std::max(maxx, old.maxx); maxz = std::max(maxz, old.maxz);
	}
	old = r;
	old.alive = true;
	shape_hash.insert_aabb(id, r.minx, r.minz, r.maxx, r.maxz);
	recompose_world_rect(minx, minz, maxx, maxz);
}

void Heightfield::drop_road_shape(int id) {
	if (id < 0 || id >= int(road_shapes.size()) || !road_shapes[size_t(id)].alive) return;
	RoadShape &r = road_shapes[size_t(id)];
	shape_hash.remove_aabb(id, r.minx, r.minz, r.maxx, r.maxz);
	r.alive = false;
	r.pts.clear();
	recompose_world_rect(r.minx, r.minz, r.maxx, r.maxz);
}

std::vector<float> Heightfield::take_ground_moved() {
	std::vector<float> out;
	out.swap(ground_moved);
	return out;
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

void Heightfield::finish_load() {
	final_h.resize(base_q.size());
	for (size_t i = 0; i < base_q.size(); i++) final_h[i] = dequant(base_q[i]);
	mods.clear();
	free_mods.clear();
	mod_hash.clear();
	mod_hash.cell = SUBCHUNK;
	road_shapes.clear(); // derived again from the roads read after (sim/commands/road_grade_sync.cpp)
	shape_hash.clear();
	shape_hash.cell = SUBCHUNK;
	ground_moved.clear();
}

} // namespace city
