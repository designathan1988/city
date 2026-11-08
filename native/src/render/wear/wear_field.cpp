// The wear field: what agents leave on the ground and which tiles go to the GPU (wear_field.h).
#include "render/wear/wear_field.h"
#include "core/random.h"
#include "render/data_reader.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/vector2.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>

using namespace godot;

namespace city {

namespace {
constexpr float BYTE_MAX = 255.f, U32_SCALE = 4294967296.f; // a byte's top; 2^32, a hash's range
// a byte's top; the window follows the eye once it has moved an eighth of its width from the middle
constexpr int BYTE_TOP = 255, HOLD_SHARE = 8;
const char *const ORIGIN_PARAM = "wear_origin"; // road.gdshader
const char *const FIELD_PARAM = "wear_field";
const char *const CELL_PARAM = "wear_cell_m";

int wrap(int v, int size) { return v & (size - 1); } // size a power of two: right for negative cells too

Ref<Image> tile_image(const PackedByteArray &bytes, int size) {
	return Image::create_from_data(size, size, false, Image::FORMAT_RG8, bytes);
}
} // namespace

bool WearField::init(const RenderContext &ctx, PackedStringArray &errors) {
	const Dictionary streets = ctx.table("streets", errors);
	if (!errors.is_empty()) return false;
	const DataValue t = DataReader{ errors, "streets.json" }.at(streets)["wear"]; // render/data_reader.h
	cell_ = t["field_cell_m"].f();
	upload_every_ = t["upload_every_s"].f();
	wheels_ = t["wheels_per_s"].f();
	feet_ = t["feet_per_s"].f();
	track_ = t["track_m"].f();
	most_step_ = t["most_step_s"].f();
	per_frame_ = t["tiles_per_frame"].whole();
	if (!errors.is_empty()) return false;
	hold_ = RES / HOLD_SHARE;
	material_ = ctx.material("road", errors);
	if (!material_.is_valid()) return false;
	TypedArray<Ref<Image>> layers;
	for (PackedByteArray &tile : tiles_) {
		tile.resize(int64_t(LAYER_CELLS) * LAYER_CELLS * CHANNELS);
		tile.fill(0);
		layers.push_back(tile_image(tile, LAYER_CELLS));
	}
	RenderingServer *rs = RenderingServer::get_singleton();
	texture_ = rs->texture_2d_layered_create(layers, RenderingServer::TEXTURE_LAYERED_2D_ARRAY);
	rs->material_set_param(material_, FIELD_PARAM, texture_);
	rs->material_set_param(material_, CELL_PARAM, cell_);
	reset();
	return true;
}

void WearField::free() {
	if (texture_.is_valid()) RenderingServer::get_singleton()->free_rid(texture_);
	texture_ = RID();
}

void WearField::reset() {
	if (!texture_.is_valid()) return;
	state_ = GOLDEN_GAMMA_32;
	ox_ = oy_ = -RES / 2;
	pending_.clear();
	for (size_t i = 0; i < tiles_.size(); i++) {
		tiles_[i].fill(0);
		dirty_[i] = false;
		touch(int(i));
	}
	send_origin();
}

void WearField::send_origin() const {
	RenderingServer::get_singleton()->material_set_param(material_, ORIGIN_PARAM, Vector2(float(ox_), float(oy_)));
}

// Xorshift32 (core/random.h) for the stochastic rounding of `add`.
float WearField::random() { return float(xorshift32(state_)) / U32_SCALE; }

void WearField::touch(int tile) {
	if (dirty_[size_t(tile)]) return;
	dirty_[size_t(tile)] = true;
	pending_.push_back(tile);
}

// A tick's wear is a fraction of a byte's step: one step more with that fraction's chance, the sum keeping its mean
// (stochastic rounding, Gupta et al., ICML 2015).
void WearField::add(int channel, float x, float z, float amount) {
	const int gx = int(std::floor(x / cell_)), gy = int(std::floor(z / cell_));
	if (gx < ox_ || gy < oy_ || gx >= ox_ + RES || gy >= oy_ + RES) return;
	const float q = amount * BYTE_MAX, whole = std::floor(q);
	const int step = int(whole) + (random() < q - whole ? 1 : 0);
	if (step <= 0) return;
	const int wx = wrap(gx, RES), wy = wrap(gy, RES), tile = (wy / LAYER_CELLS) * LAYERS_AXIS + wx / LAYER_CELLS;
	const size_t at = (size_t(wy % LAYER_CELLS) * LAYER_CELLS + size_t(wx % LAYER_CELLS)) * CHANNELS + size_t(channel);
	const uint8_t was = tiles_[size_t(tile)][int64_t(at)];
	const uint8_t now = uint8_t(std::min(BYTE_TOP, int(was) + step));
	if (now == was) return; // saturated: the tile sends nothing new
	tiles_[size_t(tile)].ptrw()[at] = now;
	touch(tile);
}

void WearField::paint(float x0, float z0, float x1, float z1, float value) {
	const uint8_t b = uint8_t(std::lround(std::clamp(value, 0.f, 1.f) * BYTE_MAX));
	for (int gy = int(std::floor(z0 / cell_)); gy <= int(std::floor(z1 / cell_)); gy++) {
		for (int gx = int(std::floor(x0 / cell_)); gx <= int(std::floor(x1 / cell_)); gx++) {
			if (gx < ox_ || gy < oy_ || gx >= ox_ + RES || gy >= oy_ + RES) continue;
			const int wx = wrap(gx, RES), wy = wrap(gy, RES), tile = (wy / LAYER_CELLS) * LAYERS_AXIS + wx / LAYER_CELLS;
			uint8_t *bytes = tiles_[size_t(tile)].ptrw() + (size_t(wy % LAYER_CELLS) * LAYER_CELLS + size_t(wx % LAYER_CELLS)) * CHANNELS;
			bytes[0] = bytes[1] = b;
			touch(tile);
		}
	}
}

void WearField::wear(const PoseFrame &f, float elapsed) {
	const float dt = std::min(elapsed, most_step_);
	if (dt <= 0) return;
	publications_++;
	const int newest = POSE_TRAIL - 1;
	for (const AgentRecord &rec : f.vehicles) {
		const Pose &p = rec.at[newest];
		const float nx = -p.fz * track_ / 2, nz = p.fx * track_ / 2, k = dt * wheels_;
		add(0, p.x + nx, p.z + nz, k);
		add(0, p.x - nx, p.z - nz, k);
	}
	for (const AgentRecord &rec : f.people) add(1, rec.at[newest].x, rec.at[newest].z, dt * feet_);
}

int64_t WearField::worn_cells() const {
	int64_t n = 0;
	for (const PackedByteArray &tile : tiles_) {
		const uint8_t *b = tile.ptr();
		const int64_t size = tile.size(); // once: each size() is a call into the engine
		for (int64_t i = 0; i < size; i += CHANNELS) n += (b[i] | b[i + 1]) != 0 ? 1 : 0;
	}
	return n;
}

// The cells of the rectangle cleared, row by row: each row's run inside one tile in one memset (a run that was clean already
// leaves its tile as it was, unsent).
void WearField::clear_cell_range(int gx0, int gx1, int gy0, int gy1) {
	for (int gy = gy0; gy <= gy1; gy++) {
		const int wy = wrap(gy, RES), row = wy % LAYER_CELLS, tile_row = (wy / LAYER_CELLS) * LAYERS_AXIS;
		for (int gx = gx0; gx <= gx1;) {
			const int wx = wrap(gx, RES), in_tile = wx % LAYER_CELLS, run = std::min(gx1 - gx + 1, LAYER_CELLS - in_tile);
			const int tile = tile_row + wx / LAYER_CELLS;
			const size_t from = (size_t(row) * LAYER_CELLS + size_t(in_tile)) * CHANNELS, bytes = size_t(run) * CHANNELS;
			const uint8_t *read = tiles_[size_t(tile)].ptr() + from;
			if (std::any_of(read, read + bytes, [](uint8_t b) { return b != 0; })) {
				std::memset(tiles_[size_t(tile)].ptrw() + from, 0, bytes);
				touch(tile);
			}
			gx += run;
		}
	}
}

void WearField::follow(float x, float z) {
	const int cx = int(std::floor(x / cell_)), cz = int(std::floor(z / cell_));
	const int dx = cx - (ox_ + RES / 2), dy = cz - (oy_ + RES / 2);
	if (std::abs(dx) <= hold_ && std::abs(dy) <= hold_) return;
	const int nx = cx - RES / 2, ny = cz - RES / 2;
	// The cells that come in hold the place that left: cleared. A jump past the whole window clears it all.
	if (nx > ox_) clear_cell_range(ox_ + RES, std::min(nx + RES - 1, ox_ + 2 * RES - 1), ny, ny + RES - 1);
	else if (nx < ox_) clear_cell_range(std::max(nx, ox_ - RES), ox_ - 1, ny, ny + RES - 1);
	if (ny > oy_) clear_cell_range(nx, nx + RES - 1, oy_ + RES, std::min(ny + RES - 1, oy_ + 2 * RES - 1));
	else if (ny < oy_) clear_cell_range(nx, nx + RES - 1, std::max(ny, oy_ - RES), oy_ - 1);
	ox_ = nx;
	oy_ = ny;
	send_origin();
}

int64_t WearField::upload(float dt) {
	for (float &s : since_) s += dt;
	int sent = 0;
	RenderingServer *rs = RenderingServer::get_singleton();
	for (size_t i = 0; i < pending_.size() && sent < per_frame_;) {
		const int tile = pending_[i];
		if (since_[size_t(tile)] < upload_every_) {
			i++;
			continue;
		}
		rs->texture_2d_update(texture_, tile_image(tiles_[size_t(tile)], LAYER_CELLS), tile);
		since_[size_t(tile)] = 0;
		dirty_[size_t(tile)] = false;
		pending_[i] = pending_.back();
		pending_.pop_back();
		sent++;
	}
	return int64_t(sent) * LAYER_CELLS * LAYER_CELLS * CHANNELS;
}

} // namespace city
