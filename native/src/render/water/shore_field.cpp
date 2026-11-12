// Regional shore propagation and revision-coherent page publication (see shore_field.h).
#include "render/water/shore_field.h"
#include "sim/terrain/terrain_gen.h"
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <shared_mutex>

using namespace godot;
namespace city {
namespace {
constexpr int BANKS = 2, MAX_LAYERS = 256, MAX_PAGE_AXIS = 11, ROUND_SAMPLES = 2;
constexpr uint8_t BYTE_ON = 255;
static_assert(MAX_PAGE_AXIS * MAX_PAGE_AXIS * BANKS <= MAX_LAYERS);
// Every output cell is written, so a second reusable buffer replaces the old full-grid copy per pass.
template<class Stop> bool spread(const std::vector<float> &from, std::vector<float> &to, const std::vector<float> &ground, int width, int height, Stop stopped) {
	for (int z = 0; z < height; z++) {
		if (stopped()) return false;
		for (int x = 0; x < width; x++) {
			const size_t k = size_t(z) * width + x;
			to[k] = from[k];
			if (from[k] > TerrainGen::NO_WATER) continue;
			float best = TerrainGen::NO_WATER;
			if (x > 0) best = std::max(best, from[k - 1]);
			if (x + 1 < width) best = std::max(best, from[k + 1]);
			if (z > 0) best = std::max(best, from[k - width]);
			if (z + 1 < height) best = std::max(best, from[k + width]);
			if (best > TerrainGen::NO_WATER && ground[k] >= best) to[k] = best;
		}
	}
	return true;
}
} // namespace

void ShoreField::reset() {
	result_.reset(); ready_.reset();
	wanted_ = std::make_shared<std::atomic<uint64_t>>(next_generation());
	texture_.unref(); bank_map_.unref(); bank_bytes_ = {};
	decltype(published_)().swap(published_); decltype(banks_)().swap(banks_); dirty_.clear();
	layout_ = {}; visible_ = false; uploads_++;
}

void ShoreField::configure(const CityState &s) {
	std::shared_lock<std::shared_mutex> map(s.map_mutex);
	const Heightfield &t = s.terrain;
	if (t.water_n <= 0 || t.water_spacing_m <= 0) return;
	Layout next;
	next.side = t.water_n; next.step = t.water_spacing_m; next.half = t.half_size();
	next.passes = std::max(0, int(std::lround(s.catalog.tune.terrain_shore_reach_m / next.step)));
	// At most 11*11*2 = 242 layers: within the OpenGL 3 / Vulkan core minimum of 256.
	const int requested = int(std::clamp(s.catalog.tune.render_shore_page_samples, 1.0, double(next.side)));
	next.page = std::max(requested, (next.side + MAX_PAGE_AXIS - 1) / MAX_PAGE_AXIS);
	next.across = (next.side + next.page - 1) / next.page;
	if (next.side == layout_.side && next.step == layout_.step && next.half == layout_.half
		&& next.page == layout_.page && next.passes == layout_.passes) return;
	reset(); layout_ = next;
	const int count = next.across * next.across;
	published_.resize(size_t(count)); banks_.assign(size_t(count), 0);
	for (int i = 0; i < count; i++) dirty_.insert(i);
}

bool ShoreField::mark_world(float x0, float z0, float x1, float z1) {
	const Layout &l = layout_;
	if (l.side == 0) return false; // configure will request the whole first field
	const double pad = double(l.passes + ROUND_SAMPLES) * l.step;
	const double left = std::floor((double(x0) - pad + l.half) / l.step);
	const double top = std::floor((double(z0) - pad + l.half) / l.step);
	const double right = std::ceil((double(x1) + pad + l.half) / l.step);
	const double bottom = std::ceil((double(z1) + pad + l.half) / l.step);
	if (!std::isfinite(left) || !std::isfinite(top) || !std::isfinite(right) || !std::isfinite(bottom)) {
		for (int i = 0; i < l.across * l.across; i++) dirty_.insert(i);
		return true;
	}
	if (right < 0 || bottom < 0 || left >= l.side || top >= l.side) return false;
	const auto page_of = [&](double value) { return int(std::clamp(value, 0.0, double(l.side - 1))) / l.page; };
	for (int z = page_of(top); z <= page_of(bottom); z++)
		for (int x = page_of(left); x <= page_of(right); x++) dirty_.insert(z * l.across + x);
	return true;
}

void ShoreField::touch(const CityState &s, const std::vector<int64_t> &water_keys, const std::vector<int> &terrain_rects) {
	bool affected = false;
	for (int64_t key : water_keys) {
		const float x = key_x(key) * CHUNK, z = key_y(key) * CHUNK;
		affected = mark_world(x, z, x + CHUNK, z + CHUNK) || affected;
	}
	for (size_t i = 0; i + 3 < terrain_rects.size(); i += 4) {
		const Heightfield &t = s.terrain;
		// A changed final-height vertex can affect either adjacent bilinear cell.
		affected = mark_world(t.x_of(terrain_rects[i]) - t.spacing, t.x_of(terrain_rects[i + 1]) - t.spacing,
			t.x_of(terrain_rects[i + 2]) + t.spacing, t.x_of(terrain_rects[i + 3]) + t.spacing) || affected;
	}
	if (affected) wanted_->fetch_add(1);
}

ShoreField::Raster ShoreField::work(const CityState &s, Layout l, std::vector<int> requested, uint64_t revision,
		const Wanted &wanted, const Ticket &ticket) {
	Raster r; r.revision = revision; r.requested = std::move(requested);
	const auto stopped = [&] { return AsyncResult<Raster>::retired(ticket) || JobPool::cancelled() || wanted->load() != revision; };
	for (int index : r.requested) {
		if (stopped()) return r;
		const int px = (index % l.across) * l.page, pz = (index / l.across) * l.page;
		const int endx = std::min(l.side, px + l.page), endz = std::min(l.side, pz + l.page);
		const int x0 = int(std::max<int64_t>(0, int64_t(px) - l.passes)), z0 = int(std::max<int64_t>(0, int64_t(pz) - l.passes));
		const int x1 = int(std::min<int64_t>(l.side, int64_t(endx) + l.passes)), z1 = int(std::min<int64_t>(l.side, int64_t(endz) + l.passes));
		const int width = x1 - x0, height = z1 - z0;
		std::vector<float> water(size_t(width) * height);
		HeightPatch terrain; float sea;
		{
			std::shared_lock<std::shared_mutex> map(s.map_mutex);
			if (stopped() || s.terrain.water_n != l.side || s.terrain.water_spacing_m != l.step || s.terrain.half_size() != l.half) return r;
			const Heightfield &t = s.terrain; sea = t.water_level;
			for (int z = z0; z < z1; z++)
				std::copy_n(t.water.begin() + size_t(z) * l.side + x0, width, water.begin() + size_t(z - z0) * width);
			terrain = t.patch(x0 * l.step - l.half, z0 * l.step - l.half, (x1 - 1) * l.step - l.half, (z1 - 1) * l.step - l.half);
		}
		terrain = terrain.regional();
		std::vector<float> level(water.size(), TerrainGen::NO_WATER), next(water.size()), ground(water.size());
		for (int z = 0; z < height; z++) {
			if (stopped()) return r;
			for (int x = 0; x < width; x++) {
				const size_t k = size_t(z) * width + x;
				const float wx = (x + x0) * l.step - l.half, wz = (z + z0) * l.step - l.half;
				ground[k] = wx > -l.half && wx < l.half && wz > -l.half && wz < l.half ? terrain.height_at(wx, wz) : sea;
				const float top = std::max(water[k], sea);
				if (ground[k] < top) level[k] = top;
			}
		}
		for (int pass = 0; pass < l.passes; pass++) {
			if (stopped()) return r;
			if (!spread(level, next, ground, width, height, stopped)) return r;
			level.swap(next);
		}
		Page page; page.index = index; page.texels.resize(int64_t(l.page) * l.page * sizeof(uint16_t));
		uint16_t *out = reinterpret_cast<uint16_t *>(page.texels.ptrw());
		std::fill_n(out, size_t(l.page) * l.page, Math::make_half_float(TerrainGen::NO_WATER));
		for (int z = pz; z < endz; z++)
			for (int x = px; x < endx; x++) out[size_t(z - pz) * l.page + x - px] = Math::make_half_float(level[size_t(z - z0) * width + x - x0]);
		r.pages.push_back(std::move(page));
	}
	r.complete = !stopped(); return r;
}

bool ShoreField::publish(const Raster &raster) {
	const Layout &l = layout_;
	const int count = l.across * l.across;
	if (texture_.is_null()) {
		if (count * BANKS > MAX_LAYERS) return false;
		PackedByteArray blank; blank.resize(int64_t(l.page) * l.page * sizeof(uint16_t));
		std::fill_n(reinterpret_cast<uint16_t *>(blank.ptrw()), size_t(l.page) * l.page, Math::make_half_float(TerrainGen::NO_WATER));
		const Ref<Image> image = Image::create_from_data(l.page, l.page, false, Image::FORMAT_RH, blank);
		TypedArray<Image> images;
		for (int i = 0; i < count * BANKS; i++) images.push_back(image);
		Ref<Texture2DArray> texture; texture.instantiate();
		if (texture->create_from_images(images) != OK) return false;
		PackedByteArray map; map.resize(count); map.fill(0);
		Ref<ImageTexture> banks = ImageTexture::create_from_image(Image::create_from_data(l.across, l.across, false, Image::FORMAT_R8, map));
		if (banks.is_null()) return false;
		texture_ = texture; bank_map_ = banks; bank_bytes_ = map;
	}
	bool changed = false;
	for (const Page &page : raster.pages) {
		const size_t i = size_t(page.index);
		const auto &before = published_[i];
		if (before.size() == page.texels.size() && std::memcmp(before.ptr(), page.texels.ptr(), size_t(before.size())) == 0) continue;
		const uint8_t bank = banks_[i] ^ 1;
		const Ref<Image> image = Image::create_from_data(l.page, l.page, false, Image::FORMAT_RH, page.texels);
		texture_->update_layer(image, page.index * BANKS + bank);
		banks_[i] = bank; bank_bytes_.set(page.index, bank ? BYTE_ON : 0); published_[i] = page.texels;
		changed = true;
	}
	if (changed) bank_map_->update(Image::create_from_data(l.across, l.across, false, Image::FORMAT_R8, bank_bytes_));
	if (changed || !visible_) { visible_ = true; uploads_++; }
	return true;
}

void ShoreField::sync(const CityState &s) {
	configure(s);
	Raster done;
	if (result_.take(done)) ready_ = std::move(done);
	if (ready_) {
		if (!ready_->complete || ready_->revision != wanted_->load()) {
			dirty_.insert(ready_->requested.begin(), ready_->requested.end()); ready_.reset();
		} else if (publish(*ready_)) ready_.reset();
	}
	if (ready_ || result_.pending() || !s.jobs || layout_.side == 0 || dirty_.empty()) return;
	std::vector<int> requested(dirty_.begin(), dirty_.end()); dirty_.clear();
	const auto ticket = result_.begin(); const uint64_t revision = wanted_->load();
	const auto wanted = wanted_; const Layout layout = layout_;
	auto fallback = std::make_shared<Raster>();
	fallback->requested = std::move(requested); fallback->revision = revision;
	s.jobs->submit(JobKey{JobLane::SHORE, 0}, 0.f, [ticket, &s, layout, fallback, revision, wanted,
			completion = AsyncResult<Raster>::completion(ticket, fallback)]() mutable {
		(void)completion; // the capture acknowledges terminal cancellation even when work is skipped
		if (AsyncResult<Raster>::retired(ticket)) return;
		auto raster = work(s, layout, std::move(fallback->requested), revision, wanted, ticket);
		if (!AsyncResult<Raster>::retired(ticket)) AsyncResult<Raster>::publish(ticket, std::move(raster));
	});
}

Dictionary ShoreField::view() const {
	Dictionary d; d["revision"] = uploads_; d["texture"] = texture_; d["pages"] = bank_map_;
	d["grid"] = Vector3(visible_ ? layout_.half : 0, layout_.step, float(layout_.side)); return d;
}
} // namespace city
