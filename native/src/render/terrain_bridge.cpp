// The land's maps on the GPU: Texture2DArray stacks (ImageTextureLayered: create_from_images makes one, update_layer replaces a
// layer by an image of the same size and format, get_layer_data reads one back, for the tests' probe only). Terrain3D is told its
// vertex spacing, how high its meshes' boxes must reach (cull_margin: no region of its own stands, so its height range is empty,
// and the margin is added to the boxes' vertical extent: Terrain3D docs) and the shader's values (Terrain3DMaterial.set_shader_param).
// A painted sample is a control word, 32 bit fields in a float's bytes (FORMAT_RF), laid out as Terrain3D's control map
// (ground_paint.gdshaderinc unpacks it).
#include "render/terrain_bridge.h"

#include "core/grid.h"
#include "core/math.h"
#include "sim/terrain/final_heights.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t CONTROL_AUTOSHADER = 1u;
// Control word bits (Terrain3D docs, Control Map Format): base id 27, overlay id 22, blend 14, uv angle and scale 7 (the
// biome's amount here), reserved 3 (the gullies: 3 bits of how much, the top bit a wipe).
constexpr int BASE_SHIFT = 27, OVERLAY_SHIFT = 22, BLEND_SHIFT = 14, BIOME_SHIFT = 7, GULLY_SHIFT = 3;
constexpr int BIOME_DROP = 1, GULLY_DROP = 5, WIPE_DROP = 7;
constexpr uint32_t WIPE_BIT = 8u;

uint32_t control_of(const PaintCell &c) {
	uint32_t wipe = (c.gully_wipe >> WIPE_DROP) ? WIPE_BIT : 0u;
	return (uint32_t(c.surface) << BASE_SHIFT) | (uint32_t(c.biome) << OVERLAY_SHIFT) | (uint32_t(c.amount) << BLEND_SHIFT) |
			(uint32_t(c.biome_amount >> BIOME_DROP) << BIOME_SHIFT) | ((uint32_t(c.gully >> GULLY_DROP) | wipe) << GULLY_SHIFT) | CONTROL_AUTOSHADER;
}
constexpr int BYTES_PER_SAMPLE = 4;
constexpr int NO_REGION = -1;
constexpr int FINE_SIDE_MOST = 8; // the fine window's side at most: its square is terrain.gdshader's FINE_LAYERS
constexpr int FINE_LAYERS = FINE_SIDE_MOST * FINE_SIDE_MOST;
// Regions a side past what the reach touches at most: the window moves a region late, never back and forth over one edge.
constexpr int WINDOW_SLACK = 2;
constexpr int FAR_TILE = 128; // a far map's tile, samples a side: what an edit of the land sends again of it
// How far from zero a height can stand (final_heights.h: the 16-bit form's range): what the terrain's meshes' boxes must hold.
constexpr float HEIGHT_MOST_M = QUANT_LIMIT / HEIGHT_Q_SCALE - HEIGHT_Q_OFFSET;
// terrain.gdshader's uniforms of the land
const char *const FINE_HEIGHTS = "fine_heights", *const FINE_CONTROLS = "fine_controls", *const FINE_HELD = "fine_held", *const FINE_SIDE = "fine_side";
const char *const FAR_HEIGHTS = "far_heights", *const FAR_CONTROLS = "far_controls", *const FAR_CELLS = "far_cells", *const FAR_TILES = "far_tiles";
const char *const FAR_TILE_SIDE = "far_tile", *const LAND_SIDE = "land_side", *const LAND_REGION = "land_region";

PackedByteArray samples(int count) {
	PackedByteArray bytes;
	bytes.resize(int64_t(count) * BYTES_PER_SAMPLE);
	return bytes;
}

// The final heights of the samples (x0 + i x step, z0 + j x step), i and j under `count`, as a map's layer; a sample past the
// land is level.
PackedByteArray heights_of(const Heightfield &hf, int x0, int z0, int step, int count) {
	PackedByteArray bytes = samples(count * count);
	float *out = reinterpret_cast<float *>(bytes.ptrw());
	if (step == 1 && x0 + count <= hf.N && z0 + count <= hf.N) {
		for (int j = 0; j < count; j++) hf.final_row(x0, z0 + j, count, out + size_t(j) * count);
		return bytes;
	}
	for (int j = 0; j < count; j++)
		for (int i = 0; i < count; i++) {
			const int x = x0 + i * step, z = z0 + j * step;
			out[size_t(j) * count + i] = x < hf.N && z < hf.N ? hf.final_at(x, z) : 0.f;
		}
	return bytes;
}

// The same samples' painted ground, as control words: a painted tile is looked up once for the run of samples that fall in it.
PackedByteArray controls_of(const Heightfield &hf, const GroundPaint &paint, int x0, int z0, int step, int count) {
	PackedByteArray bytes = samples(count * count);
	uint32_t *out = reinterpret_cast<uint32_t *>(bytes.ptrw());
	const bool painted = !paint.all().empty();
	const PaintTile *tile = nullptr;
	int tile_x = -1, tile_z = -1;
	for (int j = 0; j < count; j++)
		for (int i = 0; i < count; i++) {
			const int x = x0 + i * step, z = z0 + j * step;
			uint32_t word = CONTROL_AUTOSHADER;
			if (painted && x < hf.N && z < hf.N) {
				if (x / PAINT_TILE != tile_x || z / PAINT_TILE != tile_z) {
					tile_x = x / PAINT_TILE;
					tile_z = z / PAINT_TILE;
					tile = paint.tile_at(x, z);
				}
				if (tile) word = control_of((*tile)[GroundPaint::within(x, z)]);
			}
			out[size_t(j) * count + i] = word;
		}
	return bytes;
}

// The window's first region along one axis: where it stood, moved as little as holds the regions `lo`..`hi` the reach touches;
// about them when they are more than the window holds.
int slide(int from, int lo, int hi, int window, int regions) {
	const int least = hi - window + 1; // the first region at least, to hold `hi`
	if (least > lo) return clampi((lo + least) / 2, 0, regions - window);
	return clampi(clampi(from, least, lo), 0, regions - window);
}

// The cells of `size` samples a side that hold a sample of the rectangle, each once in `out` (cell_key).
void cells_under(int x0, int z0, int x1, int z1, int size, int last, std::vector<int64_t> &out) {
	if (x0 > x1 || z0 > z1) return;
	for (int z = clampi(z0 / size, 0, last); z <= clampi(z1 / size, 0, last); z++)
		for (int x = clampi(x0 / size, 0, last); x <= clampi(x1 / size, 0, last); x++) out.push_back(cell_key(x, z));
}

void once(std::vector<int64_t> &keys) {
	std::sort(keys.begin(), keys.end());
	keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
}

Object *material_of(Object *terrain3d) {
	return terrain3d ? Object::cast_to<Object>(terrain3d->get("material")) : nullptr;
}
} // namespace

void TerrainBridge::MapStack::make(int p_side, int p_layers) {
	if (maps.is_valid() && side == p_side && layers == p_layers) return; // a new land of the same size writes over the old one's
	side = p_side;
	layers = p_layers;
	PackedByteArray blank = samples(side * side);
	blank.fill(0);
	const Ref<Image> image = Image::create_from_data(side, side, false, Image::FORMAT_RF, blank);
	TypedArray<Image> images;
	for (int i = 0; i < layers; i++) images.push_back(image);
	maps.instantiate();
	maps->create_from_images(images);
}

int64_t TerrainBridge::MapStack::write(int layer, const PackedByteArray &bytes) {
	maps->update_layer(Image::create_from_data(side, side, false, Image::FORMAT_RF, bytes), layer);
	return bytes.size();
}

float TerrainBridge::MapStack::read(int layer, int x, int y) const {
	const Ref<Image> image = maps->get_layer_data(layer);
	return image.is_valid() ? image->get_pixel(x, y).r : 0.f;
}

void TerrainBridge::new_land(const Heightfield &hf, Object *terrain3d) {
	if (!terrain3d) return;
	terrain3d->set("vertex_spacing", hf.spacing);
	terrain3d->set("cull_margin", std::max(HEIGHT_Q_OFFSET, HEIGHT_MOST_M));
	made_ = false;
	pending_ = 1; // nothing of this land is drawn yet
}

void TerrainBridge::make(const Heightfield &hf, Object *terrain3d, float reach_m, int far_cells) {
	const float side = float(hf.region_size) * hf.spacing;
	window_ = std::min({ hf.regions, FINE_SIDE_MOST, int(std::ceil(2.f * reach_m / side)) + WINDOW_SLACK });
	held_.assign(size_t(window_) * window_, NO_REGION);
	fine_heights_.make(hf.region_size, window_ * window_);
	fine_controls_.make(hf.region_size, window_ * window_);
	// a far sample stands on the clipmap's own lattice (the levels' vertices, counted from the map's middle): a divisor of a region's side
	far_cells_ = clampi(far_cells, 1, hf.region_size);
	if (hf.region_size % far_cells_ != 0) {
		UtilityFunctions::push_error("tuning render.far_land_cells must divide a region's ", hf.region_size, " samples: ", far_cells);
		while (hf.region_size % far_cells_ != 0) far_cells_--;
	}
	far_tiles_ = (hf.N / far_cells_ + FAR_TILE - 1) / FAR_TILE;
	far_heights_.make(FAR_TILE, far_tiles_ * far_tiles_);
	far_controls_.make(FAR_TILE, far_tiles_ * far_tiles_);
	made_ = true;
	Object *material = material_of(terrain3d);
	if (!material) return;
	material->call("set_shader_param", FINE_HEIGHTS, fine_heights_.maps);
	material->call("set_shader_param", FINE_CONTROLS, fine_controls_.maps);
	material->call("set_shader_param", FAR_HEIGHTS, far_heights_.maps);
	material->call("set_shader_param", FAR_CONTROLS, far_controls_.maps);
	material->call("set_shader_param", FINE_SIDE, window_);
	material->call("set_shader_param", LAND_SIDE, hf.N);
	material->call("set_shader_param", LAND_REGION, hf.region_size);
	material->call("set_shader_param", FAR_CELLS, far_cells_);
	material->call("set_shader_param", FAR_TILES, far_tiles_);
	material->call("set_shader_param", FAR_TILE_SIDE, FAR_TILE);
}

void TerrainBridge::tell_held(Object *terrain3d) const {
	Object *material = material_of(terrain3d);
	if (!material) return;
	PackedInt32Array held;
	held.resize(FINE_LAYERS);
	held.fill(0);
	for (size_t i = 0; i < held_.size(); i++) held[int64_t(i)] = held_[i] + 1;
	material->call("set_shader_param", FINE_HELD, held);
}

int64_t TerrainBridge::follow(const Heightfield &hf, const GroundPaint &paint, Object *terrain3d, float x, float z, float reach_m, int far_cells, int64_t room) {
	if (!terrain3d || hf.regions <= 0) return 0;
	int64_t bytes = 0;
	const bool fresh = !made_;
	const float side = float(hf.region_size) * hf.spacing, half = hf.half_size();
	const auto region_at = [&](float v) { return clampi(int(std::floor((v + half) / side)), 0, hf.regions - 1); };
	const int lo_x = region_at(x - reach_m), hi_x = region_at(x + reach_m), lo_z = region_at(z - reach_m), hi_z = region_at(z + reach_m);
	if (fresh) {
		make(hf, terrain3d, reach_m, far_cells);
		const int reach = FAR_TILE * far_cells_; // a far tile's side in the land's samples
		for (int tz = 0; tz < far_tiles_; tz++)
			for (int tx = 0; tx < far_tiles_; tx++) {
				bytes += far_heights_.write(tz * far_tiles_ + tx, heights_of(hf, tx * reach, tz * reach, far_cells_, FAR_TILE));
				bytes += far_controls_.write(tz * far_tiles_ + tx, controls_of(hf, paint, tx * reach, tz * reach, far_cells_, FAR_TILE));
			}
		from_x_ = (lo_x + hi_x - window_ + 1) / 2; // the window about what the reach touches
		from_z_ = (lo_z + hi_z - window_ + 1) / 2;
	}
	const int from_x = slide(from_x_, lo_x, hi_x, window_, hf.regions), from_z = slide(from_z_, lo_z, hi_z, window_, hf.regions);
	if (!fresh && pending_ == 0 && from_x == from_x_ && from_z == from_z_) return bytes;
	from_x_ = from_x;
	from_z_ = from_z;
	// The window's regions no layer holds yet, the nearest to the eye first.
	struct Lacking {
		float off;
		int bx, bz;
	};
	std::vector<Lacking> lacking;
	for (int bz = from_z_; bz < from_z_ + window_; bz++)
		for (int bx = from_x_; bx < from_x_ + window_; bx++) {
			if (held(hf, bx, bz)) continue;
			const float x0 = float(bx) * side - half, z0 = float(bz) * side - half;
			const float dx = std::max({ x0 - x, 0.f, x - (x0 + side) }), dz = std::max({ z0 - z, 0.f, z - (z0 + side) });
			lacking.push_back({ std::hypot(dx, dz), bx, bz });
		}
	std::sort(lacking.begin(), lacking.end(), [](const Lacking &a, const Lacking &b) { return a.off < b.off; });
	const int R = hf.region_size;
	const int64_t a_region = int64_t(R) * R * BYTES_PER_SAMPLE * 2; // its heights and its painted ground
	size_t written = 0;
	for (; written < lacking.size(); written++) {
		if (!fresh && written > 0 && int64_t(written + 1) * a_region > room) break;
		const Lacking &r = lacking[written];
		const int layer = layer_of(r.bx, r.bz);
		bytes += fine_heights_.write(layer, heights_of(hf, r.bx * R, r.bz * R, 1, R));
		bytes += fine_controls_.write(layer, controls_of(hf, paint, r.bx * R, r.bz * R, 1, R));
		held_[size_t(layer)] = r.bz * hf.regions + r.bx;
	}
	pending_ = int(lacking.size() - written);
	if (written > 0) tell_held(terrain3d);
	return bytes;
}

int64_t TerrainBridge::push_rects(const Heightfield &hf, const std::vector<int> &rects) {
	if (!made_ || rects.empty()) return 0;
	std::vector<int64_t> regions, tiles;
	for (size_t i = 0; i + 3 < rects.size(); i += 4) {
		cells_under(rects[i], rects[i + 1], rects[i + 2], rects[i + 3], hf.region_size, hf.regions - 1, regions);
		// the far samples inside the rectangle: every far_cells-th of the land's
		cells_under((rects[i] + far_cells_ - 1) / far_cells_, (rects[i + 1] + far_cells_ - 1) / far_cells_, rects[i + 2] / far_cells_, rects[i + 3] / far_cells_, FAR_TILE,
				far_tiles_ - 1, tiles);
	}
	once(regions);
	once(tiles);
	int64_t bytes = 0;
	const int R = hf.region_size, reach = FAR_TILE * far_cells_;
	for (const int64_t k : regions)
		if (held(hf, key_x(k), key_y(k))) bytes += fine_heights_.write(layer_of(key_x(k), key_y(k)), heights_of(hf, key_x(k) * R, key_y(k) * R, 1, R));
	for (const int64_t k : tiles) bytes += far_heights_.write(key_y(k) * far_tiles_ + key_x(k), heights_of(hf, key_x(k) * reach, key_y(k) * reach, far_cells_, FAR_TILE));
	return bytes;
}

int64_t TerrainBridge::push_paint(const Heightfield &hf, const GroundPaint &paint, const std::vector<int> &rects) {
	if (!made_ || rects.empty()) return 0;
	std::vector<int64_t> regions, tiles;
	for (size_t i = 0; i + 3 < rects.size(); i += 4) {
		cells_under(rects[i], rects[i + 1], rects[i + 2], rects[i + 3], hf.region_size, hf.regions - 1, regions);
		cells_under((rects[i] + far_cells_ - 1) / far_cells_, (rects[i + 1] + far_cells_ - 1) / far_cells_, rects[i + 2] / far_cells_, rects[i + 3] / far_cells_, FAR_TILE,
				far_tiles_ - 1, tiles);
	}
	once(regions);
	once(tiles);
	int64_t bytes = 0;
	const int R = hf.region_size, reach = FAR_TILE * far_cells_;
	for (const int64_t k : regions)
		if (held(hf, key_x(k), key_y(k))) bytes += fine_controls_.write(layer_of(key_x(k), key_y(k)), controls_of(hf, paint, key_x(k) * R, key_y(k) * R, 1, R));
	for (const int64_t k : tiles)
		bytes += far_controls_.write(key_y(k) * far_tiles_ + key_x(k), controls_of(hf, paint, key_x(k) * reach, key_y(k) * reach, far_cells_, FAR_TILE));
	return bytes;
}

Dictionary TerrainBridge::drawn(const Heightfield &hf, float x, float z) const {
	Dictionary d;
	if (!made_) return d;
	// as terrain.gdshader's ground_height: the nearest sample, its fine region's or between the far ground's four round it
	const int ix = clampi(int(std::lround(x / hf.spacing)) + hf.N / 2, 0, hf.N - 1), iz = clampi(int(std::lround(z / hf.spacing)) + hf.N / 2, 0, hf.N - 1);
	const int R = hf.region_size, bx = ix / R, bz = iz / R;
	const bool fine = held(hf, bx, bz);
	d["fine"] = fine;
	if (fine) {
		d["height"] = fine_heights_.read(layer_of(bx, bz), ix % R, iz % R);
		return d;
	}
	const int last = hf.N / far_cells_ - 1;
	const auto far_at = [&](int fx, int fz) {
		fx = clampi(fx, 0, last);
		fz = clampi(fz, 0, last);
		return far_heights_.read((fz / FAR_TILE) * far_tiles_ + fx / FAR_TILE, fx % FAR_TILE, fz % FAR_TILE);
	};
	const int fx = ix / far_cells_, fz = iz / far_cells_;
	const float tx = float(ix % far_cells_) / float(far_cells_), tz = float(iz % far_cells_) / float(far_cells_);
	const float low = far_at(fx, fz) + (far_at(fx + 1, fz) - far_at(fx, fz)) * tx, high = far_at(fx, fz + 1) + (far_at(fx + 1, fz + 1) - far_at(fx, fz + 1)) * tx;
	d["height"] = low + (high - low) * tz;
	return d;
}

} // namespace city
