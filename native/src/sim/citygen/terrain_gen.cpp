// Port of Roadcraft src/jobs/terrainGen.ts (same constants, same order of operations, double precision).
#include "core/math.h"
#include "sim/citygen/terrain_gen.h"

#include "core/seeded.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double RIVER_RADIUS = 80, RIVER_STRENGTH = 24, CANAL_FROM_EDGE = 300;
constexpr double RIVER_CARVE = 1.45, RIVER_BED_DEPTH = 0.42, RIVER_BED_FLOOR = 0.72, ROUGH_REACH_MIN = 0.78;
constexpr double CANAL_REACH = RIVER_RADIUS * (1 + ROUGH_REACH_MIN) / 2;
constexpr double BED_CARVE = RIVER_STRENGTH * RIVER_CARVE * RIVER_BED_DEPTH;
constexpr double WATER_FILL = 0.45, WATER_MARGIN = 0.16, RIVER_MIN_DEPTH = 0.5, TERRAIN_WATER_HEIGHT = 0.048;
constexpr int MAX_FLOOD = 1600; // round(40 000 × 1.6² / 8²)
constexpr double VALLEY_FLOOR = 12, VALLEY_WIDTH = 380, MIN_FALL = 0.0003;
constexpr double LAKE_GRID = 2200, BASIN_RADIUS = 170, BASIN_DEPTH = 9, BASIN_REACH = 1.4, BASIN_WOBBLE = 0.38, BASIN_NOISE = 95;
constexpr double LAKE_EDGE_MAX = 0.74, LAKE_RIVER_GAP = 760, LAKE_CHANCE = 0.6;
constexpr double HILLS_BASE = 18, HILLS_AMP = 30, HILLS_SCALE = 2600, BUMPS_AMP = 6, BUMPS_SCALE = 520;
constexpr double EDGE_START = 0.82, EDGE_RAMP = 0.18, RANGE_BASE = 90, RANGE_AMP = 70, RANGE_SCALE = 700;
constexpr int BANK_PROBES = 8, SMOOTH_PASSES = 4, DESCEND_STEPS = 200, LAKE_KEY = 512, LAKE_ROW = 1024;
constexpr int64_t SAMPLE_ROW = 65536;
constexpr int N4[4][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
constexpr int N8[8][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }, { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 } };

// 4.0's value-noise hash (different from core/seeded.h hash2): result in [-1, 1).
double vhash(int32_t x, int32_t y) {
	uint32_t h = uint32_t(x) * 374761393U ^ uint32_t(y) * 668265263U;
	h = (h ^ (h >> 13)) * 1274126177U;
	return (h ^ (h >> 16)) / 2147483648.0 - 1;
}
double value_noise(double x, double y) {
	double x0 = std::floor(x), y0 = std::floor(y), fx = x - x0, fy = y - y0;
	double sx = fx * fx * (3 - 2 * fx), sy = fy * fy * (3 - 2 * fy);
	int32_t ix = js_int(x0), iy = js_int(y0);
	double a = vhash(ix, iy), b = vhash(ix + 1, iy), c = vhash(ix, iy + 1), d = vhash(ix + 1, iy + 1);
	return (a * (1 - sx) + b * sx) * (1 - sy) + (c * (1 - sx) + d * sx) * sy;
}
double influence(double u) { return u * u * (3 - 2 * u); }
double bed_profile(double d, double reach) { return influence(std::min(1.0, (1 - d / reach) / RIVER_BED_FLOOR)); }
} // namespace

TerrainGen::TerrainGen(uint32_t seed, double map_half) : seed_(seed), half_(map_half) {
	if (!flat()) plan_river();
}

double TerrainGen::course_z() const { return half_ - CANAL_FROM_EDGE; }

bool TerrainGen::basin_of(int i, int j, double &x, double &z) const {
	auto r = [&](int k) { return 0.5 + 0.5 * vhash(int32_t(uint32_t(i * 7 + k) + seed_ * 13U), j * 11 - k); };
	if (r(3) > LAKE_CHANCE) return false;
	x = (i + 0.25 + 0.5 * r(1)) * LAKE_GRID;
	z = (j + 0.25 + 0.5 * r(2)) * LAKE_GRID;
	return !(std::max(std::fabs(x), std::fabs(z)) / half_ > LAKE_EDGE_MAX || std::fabs(z - course_z()) < LAKE_RIVER_GAP);
}

double TerrainGen::land(double x, double z) const {
	double h = HILLS_BASE + HILLS_AMP * fbm2(x / HILLS_SCALE, z / HILLS_SCALE, seed_, 4);
	h += BUMPS_AMP * fbm2(x / BUMPS_SCALE, z / BUMPS_SCALE, seed_ + 7, 3);
	double valley = std::min(1.0, std::fabs(z - course_z()) / VALLEY_WIDTH);
	double open = valley * valley;
	h = h * (0.25 + 0.75 * open);
	if (h < VALLEY_FLOOR) h += (VALLEY_FLOOR - h) * (1 - open);
	int i0 = int(std::floor(x / LAKE_GRID)), j0 = int(std::floor(z / LAKE_GRID));
	for (int j = j0 - 1; j <= j0 + 1; j++)
		for (int i = i0 - 1; i <= i0 + 1; i++) {
			double bx, bz;
			if (!basin_of(i, j, bx, bz)) continue;
			double d = std::hypot(x - bx, z - bz);
			if (d >= BASIN_RADIUS * BASIN_REACH) continue;
			double r = BASIN_RADIUS * (1 + BASIN_WOBBLE * value_noise(x / BASIN_NOISE + bx * 0.01, z / BASIN_NOISE - bz * 0.01));
			double t = std::min(1.0, d / r);
			h -= BASIN_DEPTH * (1 - t * t) * (1 - t * t);
		}
	double edge = std::max(std::fabs(x), std::fabs(z)) / half_;
	if (edge > EDGE_START) {
		double t = (edge - EDGE_START) / EDGE_RAMP;
		h += t * t * (RANGE_BASE + RANGE_AMP * fbm2(x / RANGE_SCALE, z / RANGE_SCALE, seed_ + 99, 4)) * open;
	}
	return h;
}

void TerrainGen::plan_river() {
	int n = int(std::ceil(half_ * 2 / WATER_SPACING)) + 1;
	course_z_.assign(n, 0);
	level_.assign(n, 0);
	depth_.assign(n, 0);
	for (int i = 0; i < n; i++) {
		double x = i * WATER_SPACING - half_, zc = course_z();
		double bank = land(x, zc);
		for (int k = 0; k < BANK_PROBES; k++) {
			double a = double(k) / BANK_PROBES * PI_D * 2;
			bank = std::min(bank, land(x + std::cos(a) * RIVER_RADIUS, zc + std::sin(a) * RIVER_RADIUS));
		}
		double bed = land(x, zc) - BED_CARVE;
		course_z_[i] = zc;
		level_[i] = bank - std::max(WATER_MARGIN, WATER_FILL * (bank - bed));
		depth_[i] = std::max(RIVER_MIN_DEPTH, level_[i] - bed);
	}
	for (int pass = 0; pass < SMOOTH_PASSES; pass++)
		for (int i = 1; i + 1 < n; i++) level_[i] = (level_[i - 1] + level_[i] * 2 + level_[i + 1]) / 4;
	int third = std::max(1, n / 3);
	double head = 0, tail = 0;
	for (int i = 0; i < third; i++) { head += level_[i]; tail += level_[n - 1 - i]; }
	double step = MIN_FALL * WATER_SPACING;
	auto fall = [&](int i, double prev) { level_[i] = std::max(SEA_LEVEL + 1, std::min(level_[i], prev - step)); };
	if (head >= tail)
		for (int i = 1; i < n; i++) fall(i, level_[i - 1]);
	else
		for (int i = n - 2; i >= 0; i--) fall(i, level_[i + 1]);
}

TerrainGen::River TerrainGen::river_at(double x, double z) const {
	int n = int(level_.size());
	double f = std::min(n - 1.001, std::max(0.0, (x + half_) / WATER_SPACING));
	int i = int(std::floor(f));
	double t = f - i;
	double zc = course_z_[i] + (course_z_[i + 1] - course_z_[i]) * t;
	River r;
	r.level = level_[i] + (level_[i + 1] - level_[i]) * t;
	double depth = depth_[i] + (depth_[i + 1] - depth_[i]) * t;
	double slope = (course_z_[i + 1] - course_z_[i]) / WATER_SPACING;
	r.d = std::fabs(z - zc) / std::sqrt(1 + slope * slope);
	r.reach = CANAL_REACH;
	r.floor = r.level - depth;
	r.section = r.d < r.reach ? bed_profile(r.d, r.reach) : 0;
	return r;
}

double TerrainGen::base_height(double x, double z) {
	if (flat()) return FLAT_Y;
	double h = land(x, z);
	River r = river_at(x, z);
	if (r.section > 0 && h > r.floor) h -= (h - r.floor) * r.section;
	return std::min(MAX_Y, std::max(MIN_Y, h));
}

const TerrainGen::Lake *TerrainGen::lake_of(int i, int j) {
	int64_t key = int64_t(i + LAKE_KEY) * LAKE_ROW + (j + LAKE_KEY);
	auto it = lakes_.find(key);
	if (it != lakes_.end()) return it->second.get();
	std::unique_ptr<Lake> lake;
	double bx, bz;
	if (basin_of(i, j, bx, bz)) {
		auto h_of = [&](int gx, int gz) { return base_height(gx * WATER_SPACING - half_, gz * WATER_SPACING - half_); };
		int gx = int(std::round((bx + half_) / WATER_SPACING)), gz = int(std::round((bz + half_) / WATER_SPACING));
		bool moved = true;
		for (int k = 0; moved && k < DESCEND_STEPS; k++) {
			moved = false;
			double best = h_of(gx, gz);
			int nx = gx, nz = gz;
			for (const auto &d : N8) {
				double v = h_of(gx + d[0], gz + d[1]);
				if (v < best) { best = v; nx = gx + d[0]; nz = gz + d[1]; }
			}
			if (nx != gx || nz != gz) { gx = nx; gz = nz; moved = true; }
		}
		lake = std::make_unique<Lake>();
		flood(*lake, gx, gz, h_of(gx, gz));
		if (lake->flooded.empty()) lake.reset();
	}
	const Lake *out = lake.get();
	lakes_[key] = std::move(lake);
	return out;
}

// 4.0's flood (render/terrain.ts:2338-2401): from the basin's lowest sample the water rises, lowest ground first
// (a binary heap with the same sift order as the TypeScript), until it would spill over the rim; it stands at the rim
// less WATER_FILL of the depth and covers the ground under that level joined to the bottom, plus one ring of shore.
void TerrainGen::flood(Lake &lake, int gx, int gz, double bottom) {
	auto h_of = [&](int x, int z) { return base_height(x * WATER_SPACING - half_, z * WATER_SPACING - half_); };
	auto key_of = [](int x, int z) { return int64_t(z) * SAMPLE_ROW + x; };
	std::vector<int64_t> heap;
	std::vector<double> hh;
	std::unordered_map<int64_t, bool> seen;
	auto push = [&](int x, int z) {
		int64_t k = key_of(x, z);
		if (!seen.emplace(k, true).second) return;
		size_t n = heap.size();
		heap.push_back(k);
		hh.push_back(h_of(x, z));
		while (n > 0) {
			size_t p = (n - 1) >> 1;
			if (hh[p] <= hh[n]) break;
			std::swap(heap[p], heap[n]);
			std::swap(hh[p], hh[n]);
			n = p;
		}
	};
	auto pop = [&](int64_t &k, double &h) {
		k = heap[0];
		h = hh[0];
		int64_t lk = heap.back();
		double lh = hh.back();
		heap.pop_back();
		hh.pop_back();
		if (heap.empty()) return;
		heap[0] = lk;
		hh[0] = lh;
		for (size_t n = 0;;) {
			size_t l = n * 2 + 1, r = l + 1, m = n;
			if (l < heap.size() && hh[l] < hh[m]) m = l;
			if (r < heap.size() && hh[r] < hh[m]) m = r;
			if (m == n) break;
			std::swap(heap[m], heap[n]);
			std::swap(hh[m], hh[n]);
			n = m;
		}
	};
	double rim = bottom;
	int filled = 0;
	push(gx, gz);
	while (!heap.empty() && filled < MAX_FLOOD * 2) {
		int64_t k;
		double h;
		pop(k, h);
		if (h < rim - TERRAIN_WATER_HEIGHT) break;
		rim = std::max(rim, h);
		filled++;
		int x = int(k % SAMPLE_ROW), z = int(k / SAMPLE_ROW);
		for (const auto &d : N4) push(x + d[0], z + d[1]);
	}
	double level = rim - std::max(WATER_MARGIN, WATER_FILL * (rim - bottom));
	if (!(level - bottom > 1 && level > SEA_LEVEL + 1)) return;
	auto &W = lake.flooded;
	W[key_of(gx, gz)] = float(level);
	std::vector<int> queue{ gx, gz };
	for (size_t head = 0; head < queue.size() && int(W.size()) < MAX_FLOOD; head += 2)
		for (const auto &d : N4) {
			int nx = queue[head] + d[0], nz = queue[head + 1] + d[1];
			int64_t k = key_of(nx, nz);
			if (W.count(k) || h_of(nx, nz) >= level - TERRAIN_WATER_HEIGHT) continue;
			W[k] = float(level);
			queue.push_back(nx);
			queue.push_back(nz);
		}
	std::vector<int64_t> keys;
	for (const auto &kv : W) keys.push_back(kv.first);
	for (int64_t k : keys) {
		int x = int(k % SAMPLE_ROW), z = int(k / SAMPLE_ROW);
		for (const auto &d : N8) W.emplace(key_of(x + d[0], z + d[1]), float(level));
	}
	lake.level = level;
}

float TerrainGen::water_at_sample(int gx, int gz) {
	if (flat()) return NO_WATER;
	double x = gx * WATER_SPACING - half_, z = gz * WATER_SPACING - half_;
	River r = river_at(x, z);
	if (r.d < r.reach + WATER_SPACING) return float(r.level);
	float best = NO_WATER;
	int i0 = int(std::floor(x / LAKE_GRID)), j0 = int(std::floor(z / LAKE_GRID));
	for (int j = j0 - 1; j <= j0 + 1; j++)
		for (int i = i0 - 1; i <= i0 + 1; i++) {
			const Lake *l = lake_of(i, j);
			if (!l) continue;
			auto it = l->flooded.find(int64_t(gz) * SAMPLE_ROW + gx);
			if (it != l->flooded.end() && it->second > best) best = it->second;
		}
	return best;
}

float TerrainGen::water_level_at(double x, double z) {
	return water_at_sample(int(std::round((x + half_) / WATER_SPACING)), int(std::round((z + half_) / WATER_SPACING)));
}

} // namespace city
