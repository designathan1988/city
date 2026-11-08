// The town's window (see canyon_field.h).
#include "render/atmosphere/canyon_field.h"
#include "core/shader_contracts/land.h"

#include "render/atmosphere/fog_field.h"

#include "sim/buildings/b40/record.h"

#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <shared_mutex>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr int RES = 512; // texels across the window
constexpr float FIELD_SPAN = RES * CANYON_CELL_M;
constexpr int REACH_CELLS = 12; // as far as a pixel looks for what stands over it (the shader's longest step)
constexpr float NOTHING_Y = -9000.f; // no solid here: under every ground
constexpr float EDGE_REACH_M = 64.f; // a lot's centre this far past the window may still reach into it
constexpr int CHANNELS = 2;
constexpr int64_t TEXTURE_BYTES = int64_t(RES) * RES * CHANNELS * int64_t(sizeof(uint16_t));

struct StandingLot {
	Building rec;
	float ground = 0;
};

// The highest value of every window of 2 x `reach` + 1 cells along a line of `n` (the window cut at the line's ends), van
// Herk / Gil-Werman: the line padded with NOTHING_Y at both ends, cut in blocks of the window's width, each block's running
// maximum from its start (`ahead`) and from its end (`behind`); a window meets at most two blocks, so its maximum is the
// higher of the two. A tie keeps the earlier cell, as the scan from the window's start did (std::max keeps its first argument).
void sliding_max(const float *in, float *out, int n, int reach, std::vector<float> &pad, std::vector<float> &ahead, std::vector<float> &behind) {
	const int w = 2 * reach + 1, len = n + 2 * reach, size = (len + w - 1) / w * w;
	pad.assign(size_t(size), NOTHING_Y);
	std::copy(in, in + n, pad.begin() + reach);
	ahead.resize(size_t(size));
	behind.resize(size_t(size));
	for (int b = 0; b < size; b += w) {
		ahead[size_t(b)] = pad[size_t(b)];
		for (int i = b + 1; i < b + w; i++) ahead[size_t(i)] = std::max(ahead[size_t(i - 1)], pad[size_t(i)]);
		behind[size_t(b + w - 1)] = pad[size_t(b + w - 1)];
		for (int i = b + w - 2; i >= b; i--) behind[size_t(i)] = std::max(pad[size_t(i)], behind[size_t(i + 1)]);
	}
	for (int x = 0; x < n; x++) out[x] = x % w == 0 ? ahead[size_t(x + w - 1)] : std::max(behind[size_t(x)], ahead[size_t(x + w - 1)]);
}

// One volume laid into the cells whose centre it holds, the highest kept.
void lay(std::vector<float> &cells, const b40::LotFrame &f, const b40::Volume &v, float top, float ox, float oz) {
	const V2 corner[] = { f.at(v.x, v.y), f.at(v.x + v.w, v.y), f.at(v.x + v.w, v.y + v.d), f.at(v.x, v.y + v.d) };
	float minx = corner[0].x, maxx = minx, minz = corner[0].y, maxz = minz;
	for (const V2 &c : corner) {
		minx = std::min(minx, c.x);
		maxx = std::max(maxx, c.x);
		minz = std::min(minz, c.y);
		maxz = std::max(maxz, c.y);
	}
	int i0 = std::max(0, int(std::floor((minx - ox) / CANYON_CELL_M))), i1 = std::min(RES - 1, int(std::floor((maxx - ox) / CANYON_CELL_M)));
	int j0 = std::max(0, int(std::floor((minz - oz) / CANYON_CELL_M))), j1 = std::min(RES - 1, int(std::floor((maxz - oz) / CANYON_CELL_M)));
	for (int j = j0; j <= j1; j++)
		for (int i = i0; i <= i1; i++) {
			float dx = ox + (i + 0.5f) * CANYON_CELL_M - f.o.x, dz = oz + (j + 0.5f) * CANYON_CELL_M - f.o.y;
			float lx = dx * f.e0.x + dz * f.e0.y, ly = dx * f.e1.x + dz * f.e1.y;
			if (lx < v.x || lx > v.x + v.w || ly < v.y || ly > v.y + v.d) continue;
			float &c = cells[size_t(j) * RES + i];
			c = std::max(c, top);
		}
}
} // namespace

void CanyonField::reset() {
	slot_.reset(); // a job still running publishes nothing; its bodies go with its scratch
	seen_ = asked_ = ~uint64_t(0);
	any_ = asking_ = drawn_ = false;
	view_revision_++;
}

// The lots a window reads have their centre within EDGE_REACH_M of it; a building chunk holds its buildings' centres.
bool CanyonField::reads(int64_t chunk, float ox, float oz) const {
	const float x0 = float(key_x(chunk)) * CHUNK, z0 = float(key_y(chunk)) * CHUNK;
	return x0 <= ox + FIELD_SPAN + EDGE_REACH_M && x0 + CHUNK >= ox - EDGE_REACH_M && z0 <= oz + FIELD_SPAN + EDGE_REACH_M && z0 + CHUNK >= oz - EDGE_REACH_M;
}

void CanyonField::touch(const std::vector<int64_t> &chunks) {
	for (int64_t k : chunks)
		if ((drawn_ && reads(k, ox_, oz_)) || (asking_ && reads(k, asked_ox_, asked_oz_))) {
			revision_++;
			return;
		}
}

void CanyonField::work(const CityState &s, float ox, float oz, uint64_t generation) {
	std::unique_ptr<Scratch> k = slot_.scratch(generation);
	std::vector<StandingLot> lots;
	{
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		s.buildings.query(ox - EDGE_REACH_M, oz - EDGE_REACH_M, ox + FIELD_SPAN + EDGE_REACH_M, oz + FIELD_SPAN + EDGE_REACH_M, [&](int id) {
			const Building &b = s.buildings.items[size_t(id)];
			if (b.alive && b.floors > 0) lots.push_back({ b, s.terrain.height_at(b.x, b.z) });
		});
	}
	// The generator's tables are read-only after boot and an edited body is never changed once added (BodyTable): the bodies of
	// the copied records read the same after the lock as under it.
	if (!slot_.current(generation)) return; // a reset came: nothing of this window is kept
	k->cells.assign(size_t(RES) * RES, NOTHING_Y);
	for (const StandingLot &l : lots) {
		const std::shared_ptr<const b40::Body> held = b40::record_mass(s.buildings.designs, l.rec);
		const b40::Body &b = *held;
		const b40::LotFrame f = b40::lot_frame(l.rec);
		for (const b40::Volume &v : b.volumes) {
			float top = l.ground;
			for (int level = 0; level < v.top(); level++) top += b.level_h(level);
			lay(k->cells, f, v, top, ox, oz);
		}
	}
	if (!slot_.current(generation)) return;
	k->rows.resize(k->cells.size());
	k->near.resize(k->cells.size());
	for (int y = 0; y < RES; y++) sliding_max(&k->cells[size_t(y) * RES], &k->rows[size_t(y) * RES], RES, REACH_CELLS, k->pad, k->ahead, k->behind);
	k->line.resize(size_t(RES) * 2);
	float *column = k->line.data(), *reached = k->line.data() + RES;
	for (int x = 0; x < RES; x++) {
		for (int y = 0; y < RES; y++) column[y] = k->rows[size_t(y) * RES + x];
		sliding_max(column, reached, RES, REACH_CELLS, k->pad, k->ahead, k->behind);
		for (int y = 0; y < RES; y++) k->near[size_t(y) * RES + x] = reached[y];
	}
	Raster r;
	r.ox = ox;
	r.oz = oz;
	r.any = !lots.empty();
	r.texels.resize(TEXTURE_BYTES);
	uint16_t *out = reinterpret_cast<uint16_t *>(r.texels.ptrw());
	for (size_t i = 0; i < k->cells.size(); i++) {
		out[i * CHANNELS] = Math::make_half_float(k->cells[i]);
		out[i * CHANNELS + 1] = Math::make_half_float(k->near[i]);
	}
	slot_.publish(generation, std::move(r), std::move(k));
}

void CanyonField::sync(const CityState &s, float cam_x, float cam_z, FrameAdmission &f) {
	Raster done; // the texture's bytes are the frame ledger's: a result waits for a frame with room
	if (slot_.take(done, [&](const Raster &r) { return !r.any || f.upload.take(TEXTURE_BYTES); })) {
		asking_ = false;
		drawn_ = true;
		seen_ = asked_;
		view_revision_++;
		any_ = done.any;
		ox_ = done.ox;
		oz_ = done.oz;
		if (any_) {
			Ref<Image> img = Image::create_from_data(RES, RES, false, Image::FORMAT_RGH, done.texels);
			if (texture_.is_valid()) texture_->update(img);
			else texture_ = ImageTexture::create_from_image(img);
		}
	}
	if (slot_.busy() || !s.jobs) return;
	float hold = FIELD_SPAN / LINGER_SHARE;
	bool moved = std::fabs(cam_x - (ox_ + FIELD_SPAN * 0.5f)) > hold || std::fabs(cam_z - (oz_ + FIELD_SPAN * 0.5f)) > hold;
	uint64_t rev = revision_;
	if (rev == seen_ && !moved) return;
	float ox = std::floor((cam_x - FIELD_SPAN * 0.5f) / CANYON_CELL_M) * CANYON_CELL_M, oz = std::floor((cam_z - FIELD_SPAN * 0.5f) / CANYON_CELL_M) * CANYON_CELL_M;
	asked_ = rev;
	asked_ox_ = ox;
	asked_oz_ = oz;
	asking_ = true;
	const uint64_t generation = slot_.begin();
	s.jobs->submit(JobKey{ JobLane::CANYON, 0 }, 0.f, [this, &s, ox, oz, generation]() { work(s, ox, oz, generation); });
}

Dictionary CanyonField::view() const {
	Dictionary d;
	bool on = any_ && texture_.is_valid();
	d["any"] = on;
	if (on) {
		d["texture"] = texture_;
		d["window"] = Vector3(ox_, oz_, FIELD_SPAN);
	}
	return d;
}

} // namespace city
