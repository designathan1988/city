// The levels of the shore (see shore_field.h).
#include "render/water/shore_field.h"

#include "sim/terrain/terrain_gen.h"

#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <algorithm>
#include <cmath>
#include <shared_mutex>
#include <vector>

using namespace godot;

namespace city {

namespace {
// One step out from the wet samples, for the samples of `frontier`'s neighbours alone: a dry one takes the highest water next
// to it, if its ground stands over that water (a bank), so a shore never climbs over a ridge into the next valley's level. A
// dry sample none of whose neighbours changed in the last step would read what it read then and stay dry: only the
// neighbours of the samples the last step set (the wet ones before the first) are looked at, each once. The step reads the
// levels as they stood before it (`set` is applied after), as a pass over a copy of the grid did.
void spread(std::vector<float> &level, const std::vector<float> &ground, int n, const std::vector<int> &frontier, std::vector<int> &next,
		std::vector<std::pair<int, float>> &set, std::vector<uint32_t> &seen, uint32_t stamp) {
	set.clear();
	next.clear();
	auto look = [&](int k) {
		if (seen[size_t(k)] == stamp) return;
		seen[size_t(k)] = stamp;
		if (level[size_t(k)] > TerrainGen::NO_WATER) return;
		const int x = k % n, z = k / n;
		float best = TerrainGen::NO_WATER;
		if (x > 0) best = std::max(best, level[size_t(k - 1)]);
		if (x + 1 < n) best = std::max(best, level[size_t(k + 1)]);
		if (z > 0) best = std::max(best, level[size_t(k - n)]);
		if (z + 1 < n) best = std::max(best, level[size_t(k + n)]);
		if (best > TerrainGen::NO_WATER && ground[size_t(k)] >= best) set.push_back({ k, best });
	};
	for (int k : frontier) {
		const int x = k % n, z = k / n;
		if (x > 0) look(k - 1);
		if (x + 1 < n) look(k + 1);
		if (z > 0) look(k - n);
		if (z + 1 < n) look(k + n);
	}
	for (const auto &[k, v] : set) {
		level[size_t(k)] = v;
		next.push_back(k);
	}
}
} // namespace

void ShoreField::reset() {
	slot_.reset(); // a job still running publishes nothing
	seen_ = asked_ = ~uint64_t(0);
}

void ShoreField::work(const CityState &s, uint64_t generation) {
	std::unique_ptr<Scratch> k = slot_.scratch(generation);
	Raster r;
	{
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		const Heightfield &t = s.terrain;
		r.side = t.water_n;
		r.step = t.water_spacing_m;
		r.half = t.half_size();
		k->level.assign(size_t(r.side) * r.side, TerrainGen::NO_WATER);
		k->ground.resize(k->level.size());
		k->frontier.clear();
		for (int z = 0; z < r.side; z++)
			for (int x = 0; x < r.side; x++) {
				size_t i = size_t(z) * r.side + x;
				float wx = x * r.step - r.half, wz = z * r.step - r.half;
				k->ground[i] = t.in_bounds(wx, wz) ? t.height_at(wx, wz) : t.water_level;
				float top = std::max(t.water[i], t.water_level);
				if (k->ground[i] < top) { // only where the water really stands over the ground
					k->level[i] = top;
					k->frontier.push_back(int(i));
				}
			}
	}
	const int passes = int(std::lround(s.catalog.tune.terrain_shore_reach_m / r.step));
	if (k->seen.size() != k->level.size()) {
		k->seen.assign(k->level.size(), 0);
		k->stamp = 0;
	}
	for (int pass = 0; pass < passes && !k->frontier.empty(); pass++) {
		if (!slot_.current(generation)) return; // a reset came: nothing of these levels is kept
		if (++k->stamp == 0) {
			std::fill(k->seen.begin(), k->seen.end(), 0);
			k->stamp = 1;
		}
		spread(k->level, k->ground, r.side, k->frontier, k->next, k->set, k->seen, k->stamp);
		k->frontier.swap(k->next);
	}
	r.texels.resize(int64_t(k->level.size()) * int64_t(sizeof(uint16_t)));
	uint16_t *out = reinterpret_cast<uint16_t *>(r.texels.ptrw());
	for (size_t i = 0; i < k->level.size(); i++) out[i] = Math::make_half_float(k->level[i]);
	slot_.publish(generation, std::move(r), std::move(k));
}

void ShoreField::sync(const CityState &s, FrameAdmission &f) {
	Raster done; // the texture's bytes are the frame ledger's: a result waits for a frame with room
	if (slot_.take(done, [&](const Raster &r) { return f.upload.take(r.texels.size()); })) {
		seen_ = asked_;
		Ref<Image> img = Image::create_from_data(done.side, done.side, false, Image::FORMAT_RH, done.texels);
		if (texture_.is_valid() && done.side == side_) texture_->update(img);
		else texture_ = ImageTexture::create_from_image(img);
		side_ = done.side;
		half_ = done.half;
		step_ = done.step;
		uploads_++;
	}
	if (slot_.busy() || !s.jobs || s.terrain.water_n == 0) return;
	uint64_t rev = revision_;
	if (rev == seen_) return;
	asked_ = rev;
	const uint64_t generation = slot_.begin();
	s.jobs->submit(JobKey{ JobLane::SHORE, 0 }, 0.f, [this, &s, generation]() { work(s, generation); });
}

Dictionary ShoreField::view() const {
	Dictionary d;
	d["revision"] = uploads_;
	if (uploads_ > 0) {
		d["texture"] = texture_;
		d["grid"] = Vector3(half_, step_, float(side_));
	}
	return d;
}

} // namespace city
