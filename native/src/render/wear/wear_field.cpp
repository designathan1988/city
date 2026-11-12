// The wear field: what agents leave on the ground and which tiles go to the GPU (wear_field.h).
#include "render/wear/wear_field.h"

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
constexpr float BYTE_MAX = 255.f, U32_SCALE = 4294967296.f;
constexpr int BYTE_TOP = 255, XORSHIFT_A = 13, XORSHIFT_B = 17, XORSHIFT_C = 5, HOLD_SHARE = 8;
constexpr uint32_t RNG_START = 0x9e3779b9u;
const char *const ORIGIN_PARAM = "wear_origin"; // road.gdshader
const char *const FIELD_PARAM = "wear_field";
const char *const CELL_PARAM = "wear_cell_m";

int wrap(int v, int size) { return v & (size - 1); } // size a power of two: right for negative cells too

Ref<Image> tile_image(const PackedByteArray &bytes, int size) {
	return Image::create_from_data(size, size, false, Image::FORMAT_RG8, bytes);
}
} // namespace

bool WearField::init(const RenderContext &ctx, PackedStringArray &errors) {
	const Dictionary t = Dictionary(ctx.table("streets", errors).get("wear", Dictionary()));
	cell_ = float(t["field_cell_m"]);
	upload_every_ = float(t["upload_every_s"]);
	wheels_ = float(t["wheels_per_s"]);
	feet_ = float(t["feet_per_s"]);
	track_ = float(t["track_m"]);
	most_step_ = float(t["most_step_s"]);
	per_frame_ = int(t["tiles_per_frame"]);
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
	state_ = RNG_START;
	ox_ = oy_ = -RES / 2;
	pending_.clear();
	for (size_t i = 0; i < tiles_.size(); i++) {
		tiles_[i].fill(0);
		dirty_[i] = queued_[i] = false;
		clears_[i] = ClearMask();
		touch(int(i));
	}
	send_origin();
}

void WearField::send_origin() const {
	RenderingServer::get_singleton()->material_set_param(material_, ORIGIN_PARAM, Vector2(float(ox_), float(oy_)));
}

// Xorshift32 (Marsaglia, "Xorshift RNGs", J. Stat. Softw. 2003) for the stochastic rounding of `add`.
float WearField::random() {
	state_ ^= state_ << XORSHIFT_A;
	state_ ^= state_ >> XORSHIFT_B;
	state_ ^= state_ << XORSHIFT_C;
	return float(state_) / U32_SCALE;
}

void WearField::queue(int tile) {
	if (queued_[size_t(tile)]) return;
	queued_[size_t(tile)] = true;
	pending_.push_back(tile);
}

void WearField::touch(int tile) {
	dirty_[size_t(tile)] = true;
	queue(tile);
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
	realize_clear(tile);
	const size_t at = (size_t(wy % LAYER_CELLS) * LAYER_CELLS + size_t(wx % LAYER_CELLS)) * CHANNELS + size_t(channel);
	const uint8_t before = tiles_[size_t(tile)].ptr()[at];
	if (before == BYTE_TOP) return; // preserve stochastic-rounding RNG above, but do not dirty unchanged bytes
	tiles_[size_t(tile)].ptrw()[at] = uint8_t(std::min(BYTE_TOP, int(before) + step));
	touch(tile);
}

void WearField::paint(float x0, float z0, float x1, float z1, float value) {
	const uint8_t b = uint8_t(std::lround(std::clamp(value, 0.f, 1.f) * BYTE_MAX));
	for (int gy = std::max(oy_, int(std::floor(z0 / cell_))); gy <= std::min(oy_ + RES - 1, int(std::floor(z1 / cell_))); gy++) {
		for (int gx = std::max(ox_, int(std::floor(x0 / cell_))); gx <= std::min(ox_ + RES - 1, int(std::floor(x1 / cell_))); gx++) {
			if (gx < ox_ || gy < oy_ || gx >= ox_ + RES || gy >= oy_ + RES) continue;
			const int wx = wrap(gx, RES), wy = wrap(gy, RES), tile = (wy / LAYER_CELLS) * LAYERS_AXIS + wx / LAYER_CELLS;
			realize_clear(tile);
			const size_t at = (size_t(wy % LAYER_CELLS) * LAYER_CELLS + size_t(wx % LAYER_CELLS)) * CHANNELS;
			const uint8_t *before = tiles_[size_t(tile)].ptr() + at;
			if (before[0] == b && before[1] == b) continue;
			uint8_t *bytes = tiles_[size_t(tile)].ptrw() + at;
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
	for (size_t tile = 0; tile < tiles_.size(); tile++) {
		const uint8_t *b = tiles_[tile].ptr();
		const ClearMask &mask = clears_[tile];
		for (int row = 0; row < LAYER_CELLS; row++)
			if (!mask.rows[size_t(row)]) for (int column = 0; column < LAYER_CELLS; column++) {
				if (mask.columns[size_t(column)]) continue;
				const size_t at = (size_t(row) * LAYER_CELLS + size_t(column)) * CHANNELS;
				n += (b[at] | b[at + 1]) != 0 ? 1 : 0;
			}
	}
	return n;
}

void WearField::clear_cell_range(int gx0, int gx1, int gy0, int gy1) {
	if (gx0 > gx1 || gy0 > gy1) return;
	// follow only supplies strips spanning the entire toroid in at least one axis.
	const bool rows = gx1 - gx0 + 1 >= RES;
	struct Span { int tile = 0, local = 0, count = 0; };
	std::array<Span, LAYERS_AXIS + 1> xs, ys; // a full wrapped axis can split its first physical tile in two
	const auto split = [](int begin, int end, auto &out) {
		int size = 0;
		for (int at = begin; at <= end;) {
			const int physical = wrap(at, RES), local = physical % LAYER_CELLS;
			const int count = std::min(end - at + 1, LAYER_CELLS - local);
			out[size++] = Span{physical / LAYER_CELLS, local, count};
			at += count;
		}
		return size;
	};
	const int nx = split(gx0, gx1, xs), ny = split(gy0, gy1, ys);
	// Keep the old row-major first-touch queue order, without visiting individual cells.
	for (int y = 0; y < ny; y++) for (int x = 0; x < nx; x++) {
		const int tile = ys[y].tile * LAYERS_AXIS + xs[x].tile;
		const Span &range = rows ? ys[y] : xs[x];
		auto &mask = rows ? clears_[size_t(tile)].rows : clears_[size_t(tile)].columns;
		for (int i = range.local; i < range.local + range.count; i++) mask.set(size_t(i));
		queue(tile); // preparation may find all bytes already zero: no upload in that case
	}
}

void WearField::realize_clear(int tile) {
	ClearMask &mask = clears_[size_t(tile)];
	if (mask.rows.none() && mask.columns.none()) return;
	const uint8_t *bytes = tiles_[size_t(tile)].ptr();
	uint8_t *write = nullptr;
	bool changed = false;
	const auto clear = [&](size_t from, size_t count) {
		if (std::none_of(bytes + from, bytes + from + count, [](uint8_t b) { return b != 0; })) return;
		if (!write) { write = tiles_[size_t(tile)].ptrw(); bytes = write; }
		std::memset(write + from, 0, count);
		changed = true;
	};
	for (int row = 0; row < LAYER_CELLS; row++) {
		const size_t base = size_t(row) * LAYER_CELLS * CHANNELS;
		if (mask.rows[size_t(row)]) { clear(base, LAYER_CELLS * CHANNELS); continue; }
		for (int column = 0; column < LAYER_CELLS;) {
			if (!mask.columns[size_t(column)]) { column++; continue; }
			const int from = column++;
			while (column < LAYER_CELLS && mask.columns[size_t(column)]) column++;
			clear(base + size_t(from) * CHANNELS, size_t(column - from) * CHANNELS);
		}
	}
	mask = ClearMask();
	if (changed) touch(tile);
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

void WearField::upload(float dt) {
	for (float &s : since_) s += dt;
	int prepared = 0;
	RenderingServer *rs = RenderingServer::get_singleton();
	for (size_t i = 0; i < pending_.size() && prepared < per_frame_;) {
		const int tile = pending_[i];
		if (since_[size_t(tile)] < upload_every_) {
			i++;
			continue;
		}
		realize_clear(tile);
		prepared++; // bound CPU clearing as well as uploads, including tiles found already clean
		if (dirty_[size_t(tile)]) {
			rs->texture_2d_update(texture_, tile_image(tiles_[size_t(tile)], LAYER_CELLS), tile);
			since_[size_t(tile)] = 0;
		}
		dirty_[size_t(tile)] = queued_[size_t(tile)] = false;
		pending_[i] = pending_.back();
		pending_.pop_back();

	}
}

} // namespace city
