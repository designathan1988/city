// The fog's window (see fog_field.h).
#include "render/atmosphere/fog_field.h"

#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <algorithm>
#include <cmath>
#include <unordered_map>

using namespace godot;

namespace city {

namespace {
constexpr int FOG_RES = 512; // texels across the window
// LINGER_SHARE: fog_field.h; the view goes this share of the window from its middle before it follows
constexpr int CHANNELS = 4;
constexpr float SLAB_HEIGHTS = 3.f; // thin to under a twentieth three heights up
constexpr float BUCKET_M = FogPaint::BUCKET_M;
constexpr float EMPTY_SLAB = 1e30f;

int64_t bucket_of(int i, int j) { return cell_key(i, j); }
} // namespace

void FogField::reset() {
	result_.reset();
	seen_ = ~uint64_t(0);
	any_ = false;
	spatial_ = false;
	texture_.unref();
	view_revision_++;
}

FogField::Raster FogField::work(const CityState &s, float ox, float oz, uint64_t revision) {
	FogDef def;
	std::vector<FogDab> dabs;
	HeightPatch terrain;
	float half = 0, water = 0;
	bool spatial = false;
	{
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		if (JobPool::cancelled()) return Raster{};
		def = s.catalog.nature.fog;
		spatial = !s.nature.fog.empty();
		dabs = s.nature.fog.window(ox, oz, ox + def.texel_m * FOG_RES, oz + def.texel_m * FOG_RES);
		half = s.terrain.half_size();
		water = s.terrain.water_level;
		if (!dabs.empty()) terrain = s.terrain.patch(ox, oz, ox + def.texel_m * FOG_RES, oz + def.texel_m * FOG_RES);
	}
	terrain = terrain.regional();
	Raster r;
	r.revision = revision;
	r.side = def.texel_m * FOG_RES;
	r.ox = ox;
	r.oz = oz;
	r.low = EMPTY_SLAB;
	r.high = -EMPTY_SLAB;
	r.spatial = spatial;
	if (!dabs.empty()) {
		std::unordered_map<int64_t, std::vector<const FogDab *>> buckets;
		const int i0 = int(std::floor(ox / BUCKET_M)), i1 = int(std::floor((ox + r.side) / BUCKET_M));
		const int j0 = int(std::floor(oz / BUCKET_M)), j1 = int(std::floor((oz + r.side) / BUCKET_M));
		for (const FogDab &d : dabs) {
			if (JobPool::cancelled()) return Raster{};
			for (int j = std::max(j0, int(std::floor((d.z - d.radius) / BUCKET_M))); j <= std::min(j1, int(std::floor((d.z + d.radius) / BUCKET_M))); j++)
				for (int i = std::max(i0, int(std::floor((d.x - d.radius) / BUCKET_M))); i <= std::min(i1, int(std::floor((d.x + d.radius) / BUCKET_M))); i++) buckets[bucket_of(i, j)].push_back(&d);
		}
		r.texels.resize(int64_t(FOG_RES) * FOG_RES * CHANNELS * int64_t(sizeof(uint16_t)));
		uint16_t *t = reinterpret_cast<uint16_t *>(r.texels.ptrw());
		FogSample f;
		for (int j = 0; j < FOG_RES; j++)
			for (int i = 0; i < FOG_RES; i++) {
				if (i == 0 && JobPool::cancelled()) return Raster{};
				float x = ox + (i + 0.5f) * def.texel_m, z = oz + (j + 0.5f) * def.texel_m;
				auto it = buckets.find(bucket_of(int(std::floor(x / BUCKET_M)), int(std::floor(z / BUCKET_M))));
				f = FogSample{};
				if (it != buckets.end()) fog_at(it->second, x, z, def, f);
				float ground = x > -half && x < half && z > -half && z < half ? terrain.height_at(x, z) : water;
				// The window repeats: a world texel keeps its place modulo the side, as the shader reads it (world / side).
				int wi = int(std::floor(x / def.texel_m)) % FOG_RES, wj = int(std::floor(z / def.texel_m)) % FOG_RES;
				wi = (wi + FOG_RES) % FOG_RES;
				wj = (wj + FOG_RES) % FOG_RES;
				uint16_t *c = t + (size_t(wj) * FOG_RES + wi) * CHANNELS;
				bool on = f.density > def.min_density;
				c[0] = Math::make_half_float(on ? f.density : 0.f);
				c[1] = Math::make_half_float(ground);
				c[2] = Math::make_half_float(on ? f.height : 0.f);
				c[3] = Math::make_half_float(on ? f.speed : 0.f);
				if (on) {
					r.any = true;
					r.low = std::min(r.low, ground);
					r.high = std::max(r.high, ground + f.height * SLAB_HEIGHTS);
				}
			}
	}
	return r;
}

void FogField::sync(const CityState &s, float cam_x, float cam_z) {
	const uint64_t rev = s.nature.fog_revision;
	Raster done;
	if (result_.take(done)) {
		const float hold = done.side / LINGER_SHARE;
		const bool relevant = !done.spatial || (std::fabs(cam_x - (done.ox + done.side * 0.5f)) <= hold && std::fabs(cam_z - (done.oz + done.side * 0.5f)) <= hold);
		if (done.revision == rev && relevant) {
			seen_ = done.revision;
			view_revision_++;
			any_ = done.any;
			spatial_ = done.spatial;
			ox_ = done.ox; oz_ = done.oz; side_ = done.side; low_ = done.low; high_ = done.high;
			if (any_) {
				Ref<Image> img = Image::create_from_data(FOG_RES, FOG_RES, false, Image::FORMAT_RGBAH, done.texels);
				if (texture_.is_valid()) texture_->update(img);
				else texture_ = ImageTexture::create_from_image(img);
			}
		}
	}
	if (result_.pending() || !s.jobs) return;
	const FogDef &def = s.catalog.nature.fog;
	float side = def.texel_m * FOG_RES, hold = side / LINGER_SHARE;
	bool moved = spatial_ && (std::fabs(cam_x - (ox_ + side * 0.5f)) > hold || std::fabs(cam_z - (oz_ + side * 0.5f)) > hold);
	if (rev == seen_ && !moved) return;
	const auto ticket = result_.begin();
	float ox = std::floor((cam_x - side * 0.5f) / def.texel_m) * def.texel_m, oz = std::floor((cam_z - side * 0.5f) / def.texel_m) * def.texel_m;
	s.jobs->submit(JobKey{ JobLane::FOG, 0 }, 0.f, [ticket, &s, ox, oz, rev, completion = AsyncResult<Raster>::completion(ticket)]() {
		(void)completion; // the capture acknowledges terminal cancellation even when work is skipped
		if (AsyncResult<Raster>::retired(ticket) || JobPool::cancelled()) return;
		auto raster = work(s, ox, oz, rev);
		if (!JobPool::cancelled()) AsyncResult<Raster>::publish(ticket, std::move(raster));
	});
}

Dictionary FogField::view() const {
	Dictionary d;
	d["any"] = any_ && texture_.is_valid();
	if (any_ && texture_.is_valid()) {
		d["texture"] = texture_;
		d["slab"] = Vector2(low_, high_);
		d["window"] = Vector3(ox_, oz_, side_);
	}
	return d;
}

} // namespace city
