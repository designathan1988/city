// Terrain3D 1.1 scripting API (get_data, add_region_blank, get_region, set_height_map, update_maps).
// Control map: FORMAT_RF memory holding uint32 bit fields; bit 0 = autoshader (Terrain3D control map docs).
#include "render/terrain_bridge.h"
#include "core/shader_contracts/land.h"

#include "core/grid.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2i.hpp>

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
	for (int bz = 0; bz < hf.regions; bz++)
		for (int bx = 0; bx < hf.regions; bx++) write_region(hf, data, bx, bz, true);
	data->call("update_maps", MAP_TYPE_ALL, true, false);
	data->call("calc_height_range", true);
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
	for (int64_t k : touched) write_region(hf, data, key_x(k), key_y(k), false);
	data->call("update_maps", MAP_TYPE_HEIGHT, false, false);
	return int64_t(touched.size()) * hf.region_size * hf.region_size * BYTES_PER_SAMPLE;
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
	for (int64_t k : touched) write_control(hf, paint, data, key_x(k), key_y(k));
	data->call("update_maps", MAP_TYPE_CONTROL, false, false);
	return int64_t(touched.size()) * hf.region_size * hf.region_size * BYTES_PER_SAMPLE;
}

void TerrainBridge::push_all_paint(const Heightfield &hf, const GroundPaint &paint, Object *terrain3d) {
	Object *data = terrain_data(terrain3d);
	if (!data || paint.all().empty()) return;
	for (int bz = 0; bz < hf.regions; bz++)
		for (int bx = 0; bx < hf.regions; bx++) write_control(hf, paint, data, bx, bz);
	data->call("update_maps", MAP_TYPE_CONTROL, true, false);
}

} // namespace city
