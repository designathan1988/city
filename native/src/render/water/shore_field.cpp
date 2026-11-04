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
// One step out from the wet samples: a dry one takes the highest water next to it, if its ground stands over that water
// (a bank), so a shore never climbs over a ridge into the next valley's level.
void spread(const std::vector<float> &from, std::vector<float> &to, const std::vector<float> &ground, int n) {
	for (int z = 0; z < n; z++)
		for (int x = 0; x < n; x++) {
			size_t k = size_t(z) * n + x;
			if (from[k] > TerrainGen::NO_WATER) continue;
			float best = TerrainGen::NO_WATER;
			if (x > 0) best = std::max(best, from[k - 1]);
			if (x + 1 < n) best = std::max(best, from[k + 1]);
			if (z > 0) best = std::max(best, from[k - n]);
			if (z + 1 < n) best = std::max(best, from[k + n]);
			if (best > TerrainGen::NO_WATER && ground[k] >= best) to[k] = best;
		}
}
} // namespace

void ShoreField::reset() {
	std::lock_guard<std::mutex> lk(mutex_);
	has_done_ = false;
	seen_ = ~uint64_t(0);
}

void ShoreField::work(const CityState &s, uint64_t revision) {
	Raster r;
	std::vector<float> level, ground;
	{
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		const Heightfield &t = s.terrain;
		r.side = t.water_n;
		r.step = t.water_spacing_m;
		r.half = t.half_size();
		level.assign(size_t(r.side) * r.side, TerrainGen::NO_WATER);
		ground.resize(level.size());
		for (int z = 0; z < r.side; z++)
			for (int x = 0; x < r.side; x++) {
				size_t k = size_t(z) * r.side + x;
				float wx = x * r.step - r.half, wz = z * r.step - r.half;
				ground[k] = t.in_bounds(wx, wz) ? t.height_at(wx, wz) : t.water_level;
				float top = std::max(t.water[k], t.water_level);
				if (ground[k] < top) level[k] = top; // only where the water really stands over the ground
			}
	}
	std::vector<float> from;
	const int passes = int(std::lround(s.catalog.tune.terrain_shore_reach_m / r.step));
	for (int pass = 0; pass < passes; pass++) {
		from = level;
		spread(from, level, ground, r.side);
	}
	r.texels.resize(int64_t(level.size()) * int64_t(sizeof(uint16_t)));
	uint16_t *out = reinterpret_cast<uint16_t *>(r.texels.ptrw());
	for (size_t k = 0; k < level.size(); k++) out[k] = Math::make_half_float(level[k]);
	std::lock_guard<std::mutex> lk(mutex_);
	done_ = std::move(r);
	has_done_ = true;
	seen_ = revision;
}

void ShoreField::sync(const CityState &s) {
	{
		std::lock_guard<std::mutex> lk(mutex_);
		if (has_done_) {
			has_done_ = false;
			busy_ = false;
			Ref<Image> img = Image::create_from_data(done_.side, done_.side, false, Image::FORMAT_RH, done_.texels);
			if (texture_.is_valid() && done_.side == side_) texture_->update(img);
			else texture_ = ImageTexture::create_from_image(img);
			side_ = done_.side;
			half_ = done_.half;
			step_ = done_.step;
			done_.texels = PackedByteArray();
			uploads_++;
		}
	}
	if (busy_ || !s.jobs || s.terrain.water_n == 0) return;
	uint64_t rev = revision_;
	if (rev == seen_) return;
	busy_ = true;
	s.jobs->submit(int64_t(-1), 0.f, [this, &s, rev]() { work(s, rev); });
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
