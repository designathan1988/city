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
constexpr int HOLD_SHARE = 8; // the view goes this share of the window from its middle before it follows
constexpr int CHANNELS = 4;
constexpr float SLAB_HEIGHTS = 3.f; // thin to under a twentieth three heights up
constexpr float BUCKET_M = 160.f;
constexpr float EMPTY_SLAB = 1e30f;

int64_t bucket_of(int i, int j) { return cell_key(i, j); }
} // namespace

void FogField::reset() {
	std::lock_guard<std::mutex> lk(mutex_);
	has_done_ = false;
	seen_ = ~uint64_t(0);
	any_ = false;
}

void FogField::work(const CityState &s, float ox, float oz, uint64_t revision) {
	std::shared_lock<std::shared_mutex> map(s.map_mutex);
	const FogDef &def = s.catalog.nature.fog;
	Raster r;
	r.side = def.texel_m * FOG_RES;
	r.ox = ox;
	r.oz = oz;
	r.low = EMPTY_SLAB;
	r.high = -EMPTY_SLAB;
	const std::vector<FogDab> &dabs = s.nature.fog;
	if (!dabs.empty()) {
		std::unordered_map<int64_t, std::vector<const FogDab *>> buckets;
		for (const FogDab &d : dabs)
			for (int j = int(std::floor((d.z - d.radius) / BUCKET_M)); j <= int(std::floor((d.z + d.radius) / BUCKET_M)); j++)
				for (int i = int(std::floor((d.x - d.radius) / BUCKET_M)); i <= int(std::floor((d.x + d.radius) / BUCKET_M)); i++) buckets[bucket_of(i, j)].push_back(&d);
		r.texels.resize(int64_t(FOG_RES) * FOG_RES * CHANNELS * int64_t(sizeof(uint16_t)));
		uint16_t *t = reinterpret_cast<uint16_t *>(r.texels.ptrw());
		FogSample f;
		static const std::vector<const FogDab *> NONE;
		for (int j = 0; j < FOG_RES; j++)
			for (int i = 0; i < FOG_RES; i++) {
				float x = ox + (i + 0.5f) * def.texel_m, z = oz + (j + 0.5f) * def.texel_m;
				auto it = buckets.find(bucket_of(int(std::floor(x / BUCKET_M)), int(std::floor(z / BUCKET_M))));
				f = FogSample{};
				if (it != buckets.end()) fog_at(it->second, x, z, def, f);
				float ground = s.terrain.in_bounds(x, z) ? s.terrain.height_at(x, z) : s.terrain.water_level;
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
	std::lock_guard<std::mutex> lk(mutex_);
	done_ = std::move(r);
	has_done_ = true;
	seen_ = revision;
}

void FogField::sync(const CityState &s, float cam_x, float cam_z) {
	{
		std::lock_guard<std::mutex> lk(mutex_);
		if (has_done_) {
			has_done_ = false;
			busy_ = false;
			any_ = done_.any;
			ox_ = done_.ox; oz_ = done_.oz; side_ = done_.side; low_ = done_.low; high_ = done_.high;
			if (any_) {
				Ref<Image> img = Image::create_from_data(FOG_RES, FOG_RES, false, Image::FORMAT_RGBAH, done_.texels);
				if (texture_.is_valid()) texture_->update(img);
				else texture_ = ImageTexture::create_from_image(img);
			}
		}
	}
	if (busy_ || !s.jobs) return;
	const FogDef &def = s.catalog.nature.fog;
	float side = def.texel_m * FOG_RES, hold = side / HOLD_SHARE;
	bool moved = any_ && (std::fabs(cam_x - (ox_ + side * 0.5f)) > hold || std::fabs(cam_z - (oz_ + side * 0.5f)) > hold);
	uint64_t rev = s.nature.fog_revision;
	if (rev == seen_ && !moved) return;
	busy_ = true;
	float ox = std::floor((cam_x - side * 0.5f) / def.texel_m) * def.texel_m, oz = std::floor((cam_z - side * 0.5f) / def.texel_m) * def.texel_m;
	s.jobs->submit(int64_t(-1), 0.f, [this, &s, ox, oz, rev]() { work(s, ox, oz, rev); });
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
