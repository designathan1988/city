// Terrain3D 1.1 scripting API (get_data, add_region_blank, remove_regionl, get_region, set_height_map, update_maps: adding or
// removing a region makes the texture arrays again from the regions that stand, Terrain3DData docs).
// Control map: FORMAT_RF memory holding uint32 bit fields; bit 0 = autoshader (Terrain3D control map docs).
#include "render/terrain_bridge.h"
#include "core/shader_contracts/land.h"

#include "core/grid.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2i.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t CONTROL_AUTOSHADER = 1u;
constexpr int MAP_TYPE_HEIGHT = 0, MAP_TYPE_CONTROL = 1, MAP_TYPE_ALL = 3; // Terrain3DRegion.MapType
// Control map bits (Terrain3D docs, Control Map Format; PaintBits, core/shader_contracts/land.h, which ground_paint.gdshaderinc reads):
// base id, overlay id (the biome), blend, uv angle and scale (the biome's amount here), reserved (the gullies: how much, then a wipe).
// A PaintCell value is a byte: each field takes its top bits.
constexpr int CELL_BITS = 8;
constexpr int BIOME_DROP = CELL_BITS - PAINT_BIOME_BITS, GULLY_DROP = CELL_BITS - PAINT_GULLY_BITS, WIPE_DROP = CELL_BITS - 1;
constexpr uint32_t WIPE_BIT = 1u << PAINT_GULLY_BITS;

uint32_t control_of(const PaintCell &c) {
	uint32_t wipe = (c.gully_wipe >> WIPE_DROP) ? WIPE_BIT : 0u;
	return (uint32_t(c.surface) << PAINT_BASE_SHIFT) | (uint32_t(c.biome) << PAINT_OVERLAY_SHIFT) | (uint32_t(c.amount) << PAINT_BLEND_SHIFT) |
			(uint32_t(c.biome_amount >> BIOME_DROP) << PAINT_BIOME_SHIFT) | ((uint32_t(c.gully >> GULLY_DROP) | wipe) << PAINT_GULLY_SHIFT) | CONTROL_AUTOSHADER;
}
constexpr int BYTES_PER_SAMPLE = 4;
constexpr int MAPS_A_REGION = 3; // height, control and colour: what Terrain3D keeps and sends for each
const char *const FAR_HEIGHTS = "far_heights", *const FAR_CONTROLS = "far_controls", *const FAR_CELLS = "far_cells"; // terrain_common.gdshaderinc

Object *terrain_data(Object *terrain3d) {
	if (!terrain3d) return nullptr;
	return Object::cast_to<Object>(terrain3d->call("get_data"));
}
} // namespace

void TerrainBridge::write_region(const Heightfield &hf, Object *data, int bx, int bz, bool create) {
	const int R = hf.region_size;
	const Vector2i loc(bx - hf.regions / 2, bz - hf.regions / 2);
	Object *region = Object::cast_to<Object>(create ? data->call("add_region_blank", loc, false) : data->call("get_region", loc));
	if (!region) return;
	PackedByteArray bytes;
	bytes.resize(int64_t(R) * R * BYTES_PER_SAMPLE);
	float *dst = reinterpret_cast<float *>(bytes.ptrw());
	for (int pz = 0; pz < R; pz++)
		std::memcpy(dst + size_t(pz) * R, &hf.final_h[size_t(bz * R + pz) * hf.N + size_t(bx) * R], size_t(R) * BYTES_PER_SAMPLE);
	region->call("set_height_map", Image::create_from_data(R, R, false, Image::FORMAT_RF, bytes));
	if (create) {
		PackedByteArray cb;
		cb.resize(int64_t(R) * R * BYTES_PER_SAMPLE);
		uint32_t *c = reinterpret_cast<uint32_t *>(cb.ptrw());
		for (int i = 0; i < R * R; i++) c[i] = CONTROL_AUTOSHADER;
		region->call("set_control_map", Image::create_from_data(R, R, false, Image::FORMAT_RF, cb));
	}
	region->call("calc_height_range");
	region->call("set_edited", true);
	region->call("set_modified", true);
}

void TerrainBridge::push_all(const Heightfield &hf, Object *terrain3d) {
	Object *data = terrain_data(terrain3d);
	if (!data) {
		UtilityFunctions::push_error("Terrain3D: get_data() indisponivel");
		return;
	}
	terrain3d->set("vertex_spacing", hf.spacing);
	for (const int64_t k : live_) data->call("remove_regionl", Vector2i(key_x(k) - hf.regions / 2, key_y(k) - hf.regions / 2), false);
	if (!live_.empty()) data->call("update_maps", MAP_TYPE_ALL, true, false);
	live_.clear();
	far_cells_ = 0; // the far maps are of the land before: made again at the next follow
}

// The far maps' samples of the land's rectangle (sample indices, inclusive): each the land's sample at its own place.
void TerrainBridge::far_heights_in(const Heightfield &hf, int x0, int z0, int x1, int z1) {
	float *h = reinterpret_cast<float *>(far_h_.ptrw());
	for (int j = std::max(0, (z0 + far_cells_ - 1) / far_cells_); j < far_side_ && j * far_cells_ <= z1; j++)
		for (int i = std::max(0, (x0 + far_cells_ - 1) / far_cells_); i < far_side_ && i * far_cells_ <= x1; i++)
			h[size_t(j) * far_side_ + i] = hf.final_h[size_t(j * far_cells_) * hf.N + size_t(i * far_cells_)];
}

void TerrainBridge::far_controls_in(const GroundPaint &paint, int x0, int z0, int x1, int z1) {
	uint32_t *c = reinterpret_cast<uint32_t *>(far_c_.ptrw());
	const bool painted = !paint.all().empty();
	for (int j = std::max(0, (z0 + far_cells_ - 1) / far_cells_); j < far_side_ && j * far_cells_ <= z1; j++)
		for (int i = std::max(0, (x0 + far_cells_ - 1) / far_cells_); i < far_side_ && i * far_cells_ <= x1; i++)
			c[size_t(j) * far_side_ + i] = painted ? control_of(paint.cell(i * far_cells_, j * far_cells_)) : CONTROL_AUTOSHADER;
}

// A far map's samples to its texture: made the first time, written over after (the same size).
int64_t TerrainBridge::send_far(Ref<ImageTexture> &to, const PackedByteArray &bytes) {
	const Ref<Image> image = Image::create_from_data(far_side_, far_side_, false, Image::FORMAT_RF, bytes);
	if (to.is_valid() && to->get_width() == far_side_) to->update(image);
	else to = ImageTexture::create_from_image(image);
	return bytes.size();
}

void TerrainBridge::make_far(const Heightfield &hf, const GroundPaint &paint, Object *terrain3d, int far_cells) {
	far_cells_ = std::max(1, far_cells);
	far_side_ = hf.N / far_cells_;
	far_h_.resize(int64_t(far_side_) * far_side_ * BYTES_PER_SAMPLE);
	far_c_.resize(int64_t(far_side_) * far_side_ * BYTES_PER_SAMPLE);
	far_heights_in(hf, 0, 0, hf.N - 1, hf.N - 1);
	far_controls_in(paint, 0, 0, hf.N - 1, hf.N - 1);
	send_far(far_heights_, far_h_);
	send_far(far_controls_, far_c_);
	// Terrain3D bounds its meshes by the heights of the regions that stand; the land past them rises and falls by its own: the
	// meshes' boxes take the whole land's range more (Terrain3D cull_margin: for a shader that moves vertices past the range).
	const float *h = reinterpret_cast<const float *>(far_h_.ptr());
	const auto range = std::minmax_element(h, h + size_t(far_side_) * far_side_);
	terrain3d->set("cull_margin", *range.second - *range.first);
	Object *material = Object::cast_to<Object>(terrain3d->get("material"));
	if (!material) return;
	material->call("set_shader_param", FAR_HEIGHTS, far_heights_);
	material->call("set_shader_param", FAR_CONTROLS, far_controls_);
	material->call("set_shader_param", FAR_CELLS, float(far_cells_));
}

int64_t TerrainBridge::follow(const Heightfield &hf, const GroundPaint &paint, Object *terrain3d, float x, float z, float reach_m, int far_cells) {
	Object *data = terrain_data(terrain3d);
	if (!data) return 0;
	int64_t bytes = 0;
	if (far_cells_ == 0) {
		make_far(hf, paint, terrain3d, far_cells);
		bytes += far_h_.size() + far_c_.size();
	}
	const float side = float(hf.region_size) * hf.spacing, half = float(hf.regions) * side / 2;
	// how far (x, z) stands from a region's box
	const auto off = [&](int bx, int bz) {
		const float x0 = float(bx) * side - half, z0 = float(bz) * side - half;
		const float dx = std::max({ x0 - x, 0.f, x - (x0 + side) }), dz = std::max({ z0 - z, 0.f, z - (z0 + side) });
		return std::hypot(dx, dz);
	};
	bool changed = false;
	for (auto it = live_.begin(); it != live_.end();) {
		if (off(key_x(*it), key_y(*it)) <= reach_m + side) {
			++it;
			continue;
		}
		data->call("remove_regionl", Vector2i(key_x(*it) - hf.regions / 2, key_y(*it) - hf.regions / 2), false);
		it = live_.erase(it);
		changed = true;
	}
	const int last = hf.regions - 1;
	const int bx0 = clampi(int(std::floor((x - reach_m + half) / side)), 0, last), bx1 = clampi(int(std::floor((x + reach_m + half) / side)), 0, last);
	const int bz0 = clampi(int(std::floor((z - reach_m + half) / side)), 0, last), bz1 = clampi(int(std::floor((z + reach_m + half) / side)), 0, last);
	const bool painted = !paint.all().empty();
	for (int bz = bz0; bz <= bz1; bz++)
		for (int bx = bx0; bx <= bx1; bx++) {
			if (off(bx, bz) > reach_m || !live_.insert(cell_key(bx, bz)).second) continue;
			write_region(hf, data, bx, bz, true);
			if (painted) write_control(hf, paint, data, bx, bz);
			changed = true;
		}
	if (!changed) return bytes;
	data->call("update_maps", MAP_TYPE_ALL, true, false);
	data->call("calc_height_range", true);
	return bytes + int64_t(live_.size()) * hf.region_size * hf.region_size * BYTES_PER_SAMPLE * MAPS_A_REGION;
}

int64_t TerrainBridge::push_rects(const Heightfield &hf, Object *terrain3d, const std::vector<int> &rects) {
	Object *data = terrain_data(terrain3d);
	if (!data || rects.empty()) return 0;
	std::vector<int64_t> touched;
	const int last = hf.regions - 1;
	for (size_t i = 0; i + 3 < rects.size(); i += 4) {
		int bx0 = clampi(rects[i] / hf.region_size, 0, last), bz0 = clampi(rects[i + 1] / hf.region_size, 0, last);
		int bx1 = clampi(rects[i + 2] / hf.region_size, 0, last), bz1 = clampi(rects[i + 3] / hf.region_size, 0, last);
		for (int z = bz0; z <= bz1; z++)
			for (int x = bx0; x <= bx1; x++) touched.push_back(cell_key(x, z));
	}
	std::sort(touched.begin(), touched.end());
	touched.erase(std::unique(touched.begin(), touched.end()), touched.end());
	int64_t bytes = 0;
	for (int64_t k : touched) {
		if (!live_.count(k)) continue; // no region stands there: the far map alone shows it
		write_region(hf, data, key_x(k), key_y(k), false);
		bytes += int64_t(hf.region_size) * hf.region_size * BYTES_PER_SAMPLE;
	}
	if (bytes > 0) data->call("update_maps", MAP_TYPE_HEIGHT, false, false);
	if (far_cells_ > 0) {
		for (size_t i = 0; i + 3 < rects.size(); i += 4) far_heights_in(hf, rects[i], rects[i + 1], rects[i + 2], rects[i + 3]);
		bytes += send_far(far_heights_, far_h_);
	}
	return bytes;
}

void TerrainBridge::write_control(const Heightfield &hf, const GroundPaint &paint, Object *data, int bx, int bz) {
	const int R = hf.region_size;
	Object *region = Object::cast_to<Object>(data->call("get_region", Vector2i(bx - hf.regions / 2, bz - hf.regions / 2)));
	if (!region) return;
	PackedByteArray cb;
	cb.resize(int64_t(R) * R * BYTES_PER_SAMPLE);
	uint32_t *c = reinterpret_cast<uint32_t *>(cb.ptrw());
	for (int z = 0; z < R; z++)
		for (int x = 0; x < R; x++) c[size_t(z) * R + x] = control_of(paint.cell(bx * R + x, bz * R + z));
	region->call("set_control_map", Image::create_from_data(R, R, false, Image::FORMAT_RF, cb));
	region->call("set_edited", true);
	region->call("set_modified", true);
}

int64_t TerrainBridge::push_paint(const Heightfield &hf, const GroundPaint &paint, Object *terrain3d, const std::vector<int> &rects) {
	Object *data = terrain_data(terrain3d);
	if (!data || rects.empty()) return 0;
	std::vector<int64_t> touched;
	const int last = hf.regions - 1;
	for (size_t i = 0; i + 3 < rects.size(); i += 4)
		for (int z = clampi(rects[i + 1] / hf.region_size, 0, last); z <= clampi(rects[i + 3] / hf.region_size, 0, last); z++)
			for (int x = clampi(rects[i] / hf.region_size, 0, last); x <= clampi(rects[i + 2] / hf.region_size, 0, last); x++) touched.push_back(cell_key(x, z));
	std::sort(touched.begin(), touched.end());
	touched.erase(std::unique(touched.begin(), touched.end()), touched.end());
	int64_t bytes = 0;
	for (int64_t k : touched) {
		if (!live_.count(k)) continue;
		write_control(hf, paint, data, key_x(k), key_y(k));
		bytes += int64_t(hf.region_size) * hf.region_size * BYTES_PER_SAMPLE;
	}
	if (bytes > 0) data->call("update_maps", MAP_TYPE_CONTROL, false, false);
	if (far_cells_ > 0) {
		for (size_t i = 0; i + 3 < rects.size(); i += 4) far_controls_in(paint, rects[i], rects[i + 1], rects[i + 2], rects[i + 3]);
		bytes += send_far(far_controls_, far_c_);
	}
	return bytes;
}

void TerrainBridge::push_all_paint(const Heightfield &hf, const GroundPaint &paint, Object *terrain3d) {
	Object *data = terrain_data(terrain3d);
	if (!data || paint.all().empty()) return;
	for (const int64_t k : live_) write_control(hf, paint, data, key_x(k), key_y(k));
	if (!live_.empty()) data->call("update_maps", MAP_TYPE_CONTROL, true, false);
	far_cells_ = 0; // the far map's painted ground with it, at the next follow
}

} // namespace city
