// The ecosystem's raster for the ground (see ecology_raster.h).
#include "render/cover/ecology_raster.h"

#include "core/grid.h"
#include "core/math.h"
#include "sim/nature/ecology.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
constexpr int CHANNELS = 4; // RGBA8: dry herbs, wet ground, bare soil, unused
constexpr int DRY = 0, WET = 1, BARE = 2;
constexpr float BYTE_TOP = 255.f; // an 8-bit channel's full value
constexpr int NEIGHBOURS = 1; // chunks round a touched one whose corners read it: the window (mean_cells + 1 corners) is under a chunk

uint8_t byte_of(float v) { return uint8_t(std::lround(clampf(v, 0.f, 1.f) * BYTE_TOP)); }

// One corner's ground, as Roadcraft's plateTexels.ts ecologyCornersInto packs it: the herbs' dry share where they grow, the
// waterlogged ground, and the soil left bare by the herbs and the trees.
void pack(uint8_t *o, const Community &c) {
	o[DRY] = byte_of(c.grass * c.dry);
	o[WET] = byte_of(c.wet);
	o[BARE] = byte_of((1.f - c.grass) * (1.f - std::min(1.f, c.canopy + c.trees)));
}
} // namespace

void EcologyRaster::reset() {
	slot_.reset(); // a job still running publishes nothing, and its raster goes with the old map
	waiting_.clear();
	whole_ = true;
}

void EcologyRaster::touch(const std::vector<int64_t> &chunks) {
	for (int64_t k : chunks)
		for (int dz = -NEIGHBOURS; dz <= NEIGHBOURS; dz++)
			for (int dx = -NEIGHBOURS; dx <= NEIGHBOURS; dx++) waiting_.push_back(cell_key(key_x(k) + dx, key_y(k) + dz));
}

void EcologyRaster::work(const CityState &s, uint64_t generation, std::vector<int64_t> chunks, bool whole) {
	std::unique_ptr<Scratch> k = slot_.scratch(generation);
	Raster r;
	const EcologyDef &d = s.catalog.nature.ecology;
	{
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		r.half = s.terrain.half_size();
	}
	r.step = d.grid_m;
	r.side = int(std::lround(2.f * r.half / r.step)) + 1;
	{
		if (k->side != r.side || k->half != r.half || k->step != r.step) { // another map's raster: all of this one's corners again
			k->texels.assign(size_t(r.side) * size_t(r.side) * CHANNELS, 0);
			k->side = r.side;
			k->half = r.half;
			k->step = r.step;
			whole = true;
		}
		if (whole) {
			chunks.clear();
			const int c0 = int(std::floor(-r.half / CHUNK)), c1 = int(std::ceil(r.half / CHUNK)) - 1;
			for (int cz = c0; cz <= c1; cz++)
				for (int cx = c0; cx <= c1; cx++) chunks.push_back(cell_key(cx, cz));
		}
		for (int64_t key : chunks) {
			if (!slot_.current(generation)) return; // a reset came: nothing of this raster is kept
			const float x0 = float(key_x(key)) * CHUNK, z0 = float(key_y(key)) * CHUNK;
			const int i0 = int(std::lround((x0 + r.half) / r.step)), j0 = int(std::lround((z0 + r.half) / r.step));
			// The map is read shared a chunk at a time, so an edit waits for one chunk and not the whole map: what it changes
			// marks its chunks again (touch), and the next job redoes them.
			EcologyField f;
			bool grows = false;
			{
				std::shared_lock<std::shared_mutex> map(s.map_mutex);
				grows = s.nature.nature.region >= 0 || s.nature.ground.any_cover();
				if (grows) f = compute_ecology(s.terrain, s.nature.ground, s.nature.nature, d, x0, z0, CHUNK);
			}
			if (grows) {
				for (int j = 0; j < f.side; j++) {
					const int z = j0 + j;
					if (z < 0 || z >= r.side) continue;
					for (int i = 0; i < f.side; i++) {
						const int x = i0 + i;
						if (x < 0 || x >= r.side) continue;
						pack(&k->texels[(size_t(z) * size_t(r.side) + size_t(x)) * CHANNELS], f.at[size_t(j) * size_t(f.side) + size_t(i)]);
					}
				}
			} else { // no ecosystem on the map: nothing of it on the ground
				const int n = int(std::ceil(CHUNK / r.step)) + 1;
				for (int z = std::max(0, j0); z < std::min(r.side, j0 + n); z++) {
					const int xa = std::max(0, i0), xb = std::min(r.side, i0 + n);
					if (xb > xa) std::memset(&k->texels[(size_t(z) * size_t(r.side) + size_t(xa)) * CHANNELS], 0, size_t(xb - xa) * CHANNELS);
				}
			}
		}
	}
	r.texels.resize(int64_t(k->texels.size()));
	std::memcpy(r.texels.ptrw(), k->texels.data(), k->texels.size());
	slot_.publish(generation, std::move(r), std::move(k));
}

void EcologyRaster::sync(const CityState &s, FrameAdmission &f) {
	Raster done; // the texture's bytes are the frame ledger's: a result waits for a frame with room
	if (slot_.take(done, [&](const Raster &r) { return f.upload.take(r.texels.size()); })) {
		Ref<Image> img = Image::create_from_data(done.side, done.side, false, Image::FORMAT_RGBA8, done.texels);
		if (texture_.is_valid() && done.side == side_) texture_->update(img);
		else texture_ = ImageTexture::create_from_image(img);
		side_ = done.side;
		half_ = done.half;
		step_ = done.step;
		uploads_++;
	}
	if (slot_.busy() || !s.jobs || s.terrain.N == 0 || (!whole_ && waiting_.empty())) return;
	ChangeLog::dedup(waiting_);
	std::vector<int64_t> chunks;
	chunks.swap(waiting_);
	const bool whole = whole_;
	whole_ = false;
	const uint64_t generation = slot_.begin();
	s.jobs->submit(JobKey{ JobLane::ECOLOGY, 0 }, 0.f, [this, &s, generation, chunks = std::move(chunks), whole]() mutable {
		work(s, generation, std::move(chunks), whole);
	});
}

Dictionary EcologyRaster::view() const {
	Dictionary d;
	d["revision"] = int64_t(uploads_);
	if (uploads_ > 0) {
		d["texture"] = texture_;
		d["grid"] = Vector3(half_, step_, float(side_));
	}
	return d;
}

} // namespace city
