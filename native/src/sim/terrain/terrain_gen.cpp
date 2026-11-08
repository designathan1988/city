// Port of Roadcraft src/jobs/terrainGen.ts (same order of operations, double precision); its shape is tuning's "land".
#include "core/math.h"
#include "sim/terrain/terrain_gen.h"

#include "core/job_pool.h"
#include "core/seeded.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr int MAX_FLOOD = 1600; // round(40 000 × 1.6² / 8²)
// A sample's key in a lake: its row times SAMPLE_ROW plus its column; a lake square's key: (row + LAKE_KEY) * LAKE_ROW
// plus its column + LAKE_KEY, so the squares of any map's grid map to distinct keys.
constexpr int LAKE_KEY = 512, LAKE_ROW = 1024;
constexpr int64_t SAMPLE_ROW = 65536;
// The basin lattice's hash strides and the noises' salts: distinct primes, so no two fields share a stream.
constexpr int BASIN_LATTICE = 7, BASIN_ROW = 11;
constexpr uint32_t BASIN_SALT = 13U, BUMPS_SALT = 7U, RANGE_SALT = 99U;
constexpr double RIVER_END_EPS = 1.001; // the last river sample read stops this short of the end, so its next exists
// The four and the eight neighbours of a grid sample.
constexpr int N4[4][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
constexpr int N8[8][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }, { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 } };

double influence(double u) { return u * u * (3 - 2 * u); }
double bed_profile(double d, double reach, double floor_share) { return influence(std::min(1.0, (1 - d / reach) / floor_share)); }
} // namespace

TerrainGen::TerrainGen(uint32_t seed, double map_half, const Tuning &tune, const ScenicLand *authored) :
		seed_(seed), half_(map_half), water_spacing_(tune.world_water_spacing_m), tn_(tune) {
	canal_reach_ = tn_.land_river_radius_m * (1 + tn_.terrain_rough_reach_min) / 2;
	bed_carve_ = tn_.land_river_strength_m * tn_.land_river_carve * tn_.land_river_bed_depth;
	if (authored) authored_ = std::make_unique<ScenicLandField>(*authored, seed & ~FLAT_BIT);
	else if (!flat()) plan_river();
}

double TerrainGen::course_z() const { return half_ - tn_.land_canal_from_edge_m; }

bool TerrainGen::basin_of(int i, int j, double &x, double &z) const {
	auto r = [&](int k) { return 0.5 + 0.5 * value_hash(int32_t(uint32_t(i * BASIN_LATTICE + k) + seed_ * BASIN_SALT), j * BASIN_ROW - k); };
	if (r(3) > tn_.land_lake_chance) return false;
	x = (i + 0.25 + 0.5 * r(1)) * tn_.land_lake_grid_m;
	z = (j + 0.25 + 0.5 * r(2)) * tn_.land_lake_grid_m;
	return !(std::max(std::fabs(x), std::fabs(z)) / half_ > tn_.land_lake_edge_max || std::fabs(z - course_z()) < tn_.land_lake_river_gap_m);
}

double TerrainGen::land(double x, double z) const {
	double h = tn_.land_hills_base_m + tn_.land_hills_amp_m * fbm2(x / tn_.land_hills_scale_m, z / tn_.land_hills_scale_m, seed_, tn_.land_hills_octaves);
	h += tn_.land_bumps_amp_m * fbm2(x / tn_.land_bumps_scale_m, z / tn_.land_bumps_scale_m, seed_ + BUMPS_SALT, tn_.land_bumps_octaves);
	double valley = std::min(1.0, std::fabs(z - course_z()) / tn_.land_valley_width_m);
	double open = valley * valley;
	h = h * ((1 - tn_.land_open_share) + tn_.land_open_share * open);
	if (h < tn_.land_valley_floor_m) h += (tn_.land_valley_floor_m - h) * (1 - open);
	int i0 = int(std::floor(x / tn_.land_lake_grid_m)), j0 = int(std::floor(z / tn_.land_lake_grid_m));
	for (int j = j0 - 1; j <= j0 + 1; j++)
		for (int i = i0 - 1; i <= i0 + 1; i++) {
			double bx, bz;
			if (!basin_of(i, j, bx, bz)) continue;
			double d = std::hypot(x - bx, z - bz);
			if (d >= tn_.land_basin_radius_m * tn_.land_basin_reach) continue;
			double r = tn_.land_basin_radius_m * (1 + tn_.land_basin_wobble * value_noise(x / tn_.land_basin_noise_m + bx * tn_.land_basin_noise_step, z / tn_.land_basin_noise_m - bz * tn_.land_basin_noise_step));
			double t = std::min(1.0, d / r);
			h -= tn_.land_basin_depth_m * (1 - t * t) * (1 - t * t);
		}
	double edge = std::max(std::fabs(x), std::fabs(z)) / half_;
	if (edge > tn_.land_edge_start) {
		double t = (edge - tn_.land_edge_start) / tn_.land_edge_ramp;
		h += t * t * (tn_.land_range_base_m + tn_.land_range_amp_m * fbm2(x / tn_.land_range_scale_m, z / tn_.land_range_scale_m, seed_ + RANGE_SALT, tn_.land_range_octaves)) * open;
	}
	return h;
}

void TerrainGen::plan_river() {
	int n = int(std::ceil(half_ * 2 / water_spacing_)) + 1;
	course_z_.assign(n, 0);
	level_.assign(n, 0);
	depth_.assign(n, 0);
	for (int i = 0; i < n; i++) {
		double x = i * water_spacing_ - half_, zc = course_z();
		double bank = land(x, zc);
		for (int k = 0; k < tn_.land_bank_probes; k++) {
			double a = double(k) / tn_.land_bank_probes * PI_D * 2;
			bank = std::min(bank, land(x + std::cos(a) * tn_.land_river_radius_m, zc + std::sin(a) * tn_.land_river_radius_m));
		}
		double bed = land(x, zc) - bed_carve_;
		course_z_[i] = zc;
		level_[i] = bank - std::max(tn_.land_water_margin_m, tn_.land_water_fill * (bank - bed));
		depth_[i] = std::max(tn_.land_river_min_depth_m, level_[i] - bed);
	}
	for (int pass = 0; pass < tn_.land_smooth_passes; pass++)
		for (int i = 1; i + 1 < n; i++) level_[i] = (level_[i - 1] + level_[i] * 2 + level_[i + 1]) / 4;
	int third = std::max(1, n / 3);
	double head = 0, tail = 0;
	for (int i = 0; i < third; i++) { head += level_[i]; tail += level_[n - 1 - i]; }
	double step = tn_.land_river_min_fall * water_spacing_;
	auto fall = [&](int i, double prev) { level_[i] = std::max(OCEAN_Y + 1, std::min(level_[i], prev - step)); };
	if (head >= tail)
		for (int i = 1; i < n; i++) fall(i, level_[i - 1]);
	else
		for (int i = n - 2; i >= 0; i--) fall(i, level_[i + 1]);
}

TerrainGen::River TerrainGen::river_at(double x, double z) const {
	int n = int(level_.size());
	double f = std::min(n - RIVER_END_EPS, std::max(0.0, (x + half_) / water_spacing_));
	int i = int(std::floor(f));
	double t = f - i;
	double zc = course_z_[i] + (course_z_[i + 1] - course_z_[i]) * t;
	River r;
	r.level = level_[i] + (level_[i + 1] - level_[i]) * t;
	double depth = depth_[i] + (depth_[i + 1] - depth_[i]) * t;
	double slope = (course_z_[i + 1] - course_z_[i]) / water_spacing_;
	r.d = std::fabs(z - zc) / std::sqrt(1 + slope * slope);
	r.reach = canal_reach_;
	r.floor = r.level - depth;
	r.section = r.d < r.reach ? bed_profile(r.d, r.reach, tn_.terrain_river_floor_share) : 0;
	return r;
}

double TerrainGen::base_height(double x, double z) const {
	if (authored_) return std::min(double(tn_.terrain_max_height_m), std::max(double(tn_.terrain_min_height_m), authored_->height(x, z)));
	if (flat()) return tn_.land_plain_m;
	double h = land(x, z);
	River r = river_at(x, z);
	if (r.section > 0 && h > r.floor) h -= (h - r.floor) * r.section;
	return std::min(double(tn_.terrain_max_height_m), std::max(double(tn_.terrain_min_height_m), h));
}

const TerrainGen::Lake *TerrainGen::lake_of(int i, int j) {
	int64_t key = int64_t(i + LAKE_KEY) * LAKE_ROW + (j + LAKE_KEY);
	auto it = lakes_.find(key);
	if (it != lakes_.end()) return it->second.get();
	std::unique_ptr<Lake> lake = make_lake(i, j);
	const Lake *out = lake.get();
	lakes_[key] = std::move(lake);
	return out;
}

void TerrainGen::plan_lakes(int water_n, JobPool *jobs) {
	if (flat() || authored_ || water_n <= 0) return; // an authored land's lakes are its own
	// the squares water_at_sample looks at: those of the grid's first and last samples, and one more on each side
	const int lo = int(std::floor(-half_ / tn_.land_lake_grid_m)) - 1, hi = int(std::floor(((water_n - 1) * water_spacing_ - half_) / tn_.land_lake_grid_m)) + 1;
	const int side = hi - lo + 1;
	std::vector<std::unique_ptr<Lake>> made(size_t(side) * size_t(side));
	auto one = [&](size_t k) { made[k] = make_lake(lo + int(k % size_t(side)), lo + int(k / size_t(side))); };
	if (jobs) {
		jobs->parallel_for(made.size(), one);
	} else {
		for (size_t k = 0; k < made.size(); k++) one(k);
	}
	for (size_t k = 0; k < made.size(); k++)
		lakes_[int64_t(lo + int(k % size_t(side)) + LAKE_KEY) * LAKE_ROW + (lo + int(k / size_t(side)) + LAKE_KEY)] = std::move(made[k]);
}

std::unique_ptr<TerrainGen::Lake> TerrainGen::make_lake(int i, int j) const {
	std::unique_ptr<Lake> lake;
	double bx, bz;
	if (basin_of(i, j, bx, bz)) {
		auto h_of = [&](int gx, int gz) { return base_height(gx * water_spacing_ - half_, gz * water_spacing_ - half_); };
		int gx = int(std::round((bx + half_) / water_spacing_)), gz = int(std::round((bz + half_) / water_spacing_));
		bool moved = true;
		for (int k = 0; moved && k < tn_.land_descend_steps; k++) {
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
	return lake;
}

// 4.0's flood (render/terrain.ts:2338-2401): from the basin's lowest sample the water rises, lowest ground first
// (a binary heap with the same sift order as the TypeScript), until it would spill over the rim; it stands at the rim
// less tn_.land_water_fill of the depth and covers the ground under that level joined to the bottom, plus one ring of shore.
void TerrainGen::flood(Lake &lake, int gx, int gz, double bottom) const {
	auto h_of = [&](int x, int z) { return base_height(x * water_spacing_ - half_, z * water_spacing_ - half_); };
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
		if (h < rim - tn_.land_flood_slack_m) break;
		rim = std::max(rim, h);
		filled++;
		int x = int(k % SAMPLE_ROW), z = int(k / SAMPLE_ROW);
		for (const auto &d : N4) push(x + d[0], z + d[1]);
	}
	double level = rim - std::max(tn_.land_water_margin_m, tn_.land_water_fill * (rim - bottom));
	if (!(level - bottom > 1 && level > OCEAN_Y + 1)) return;
	auto &W = lake.flooded;
	W[key_of(gx, gz)] = float(level);
	std::vector<int> queue{ gx, gz };
	for (size_t head = 0; head < queue.size() && int(W.size()) < MAX_FLOOD; head += 2)
		for (const auto &d : N4) {
			int nx = queue[head] + d[0], nz = queue[head + 1] + d[1];
			int64_t k = key_of(nx, nz);
			if (W.count(k) || h_of(nx, nz) >= level - tn_.land_flood_slack_m) continue;
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
	double x = gx * water_spacing_ - half_, z = gz * water_spacing_ - half_;
	if (authored_) {
		float level = NO_WATER;
		return authored_->water(x, z, level) ? level : NO_WATER;
	}
	if (flat()) return NO_WATER;
	River r = river_at(x, z);
	if (r.d < r.reach + water_spacing_) return float(r.level);
	float best = NO_WATER;
	int i0 = int(std::floor(x / tn_.land_lake_grid_m)), j0 = int(std::floor(z / tn_.land_lake_grid_m));
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
	return water_at_sample(int(std::round((x + half_) / water_spacing_)), int(std::round((z + half_) / water_spacing_)));
}

} // namespace city
