// The town's window (see canyon_field.h).
#include "render/atmosphere/canyon_field.h"

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
constexpr float CELL_M = 4.f;
constexpr float FIELD_SPAN = RES * CELL_M;
constexpr int HOLD_SHARE = 8; // the view goes this share of the window from its middle before it follows
constexpr int REACH_CELLS = 12; // as far as a pixel looks for what stands over it (the shader's longest step)
constexpr float NOTHING_Y = -9000.f; // no solid here: under every ground
constexpr float EDGE_REACH_M = 64.f; // a lot's centre this far past the window may still reach into it
constexpr int CHANNELS = 2;

struct StandingLot {
	Building rec;
	float ground = 0;
};

// Each cell's height into `out`, the highest of the cells within `reach` along the row, and then along the column.
void reach_max(const std::vector<float> &in, std::vector<float> &out) {
	std::vector<float> rows(in.size());
	for (int y = 0; y < RES; y++)
		for (int x = 0; x < RES; x++) {
			float m = NOTHING_Y;
			for (int k = std::max(0, x - REACH_CELLS); k <= std::min(RES - 1, x + REACH_CELLS); k++) m = std::max(m, in[size_t(y) * RES + k]);
			rows[size_t(y) * RES + x] = m;
		}
	for (int y = 0; y < RES; y++)
		for (int x = 0; x < RES; x++) {
			float m = NOTHING_Y;
			for (int k = std::max(0, y - REACH_CELLS); k <= std::min(RES - 1, y + REACH_CELLS); k++) m = std::max(m, rows[size_t(k) * RES + x]);
			out[size_t(y) * RES + x] = m;
		}
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
	int i0 = std::max(0, int(std::floor((minx - ox) / CELL_M))), i1 = std::min(RES - 1, int(std::floor((maxx - ox) / CELL_M)));
	int j0 = std::max(0, int(std::floor((minz - oz) / CELL_M))), j1 = std::min(RES - 1, int(std::floor((maxz - oz) / CELL_M)));
	for (int j = j0; j <= j1; j++)
		for (int i = i0; i <= i1; i++) {
			float dx = ox + (i + 0.5f) * CELL_M - f.o.x, dz = oz + (j + 0.5f) * CELL_M - f.o.y;
			float lx = dx * f.e0.x + dz * f.e0.y, ly = dx * f.e1.x + dz * f.e1.y;
			if (lx < v.x || lx > v.x + v.w || ly < v.y || ly > v.y + v.d) continue;
			float &c = cells[size_t(j) * RES + i];
			c = std::max(c, top);
		}
}
} // namespace

void CanyonField::reset() {
	std::lock_guard<std::mutex> lk(mutex_);
	has_done_ = false;
	seen_ = ~uint64_t(0);
	any_ = false;
}

void CanyonField::work(const CityState &s, float ox, float oz, uint64_t revision) {
	std::vector<StandingLot> lots;
	{
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		s.buildings.query(ox - EDGE_REACH_M, oz - EDGE_REACH_M, ox + FIELD_SPAN + EDGE_REACH_M, oz + FIELD_SPAN + EDGE_REACH_M, [&](int id) {
			const Building &b = s.buildings.items[size_t(id)];
			if (b.alive && b.floors > 0) lots.push_back({ b, s.terrain.height_at(b.x, b.z) });
		});
	}
	std::vector<float> cells(size_t(RES) * RES, NOTHING_Y), near(cells.size());
	for (const StandingLot &l : lots) {
		const b40::Body &b = bodies_.body(s.buildings.designs, l.rec);
		const b40::LotFrame f = b40::lot_frame(l.rec);
		for (const b40::Volume &v : b.volumes) {
			float top = l.ground;
			for (int level = 0; level < v.top(); level++) top += b.level_h(level);
			lay(cells, f, v, top, ox, oz);
		}
	}
	reach_max(cells, near);
	Raster r;
	r.ox = ox;
	r.oz = oz;
	r.any = !lots.empty();
	r.texels.resize(int64_t(RES) * RES * CHANNELS * int64_t(sizeof(uint16_t)));
	uint16_t *out = reinterpret_cast<uint16_t *>(r.texels.ptrw());
	for (size_t k = 0; k < cells.size(); k++) {
		out[k * CHANNELS] = Math::make_half_float(cells[k]);
		out[k * CHANNELS + 1] = Math::make_half_float(near[k]);
	}
	std::lock_guard<std::mutex> lk(mutex_);
	done_ = std::move(r);
	has_done_ = true;
	seen_ = revision;
}

void CanyonField::sync(const CityState &s, float cam_x, float cam_z) {
	{
		std::lock_guard<std::mutex> lk(mutex_);
		if (has_done_) {
			has_done_ = false;
			busy_ = false;
			any_ = done_.any;
			ox_ = done_.ox;
			oz_ = done_.oz;
			if (any_) {
				Ref<Image> img = Image::create_from_data(RES, RES, false, Image::FORMAT_RGH, done_.texels);
				if (texture_.is_valid()) texture_->update(img);
				else texture_ = ImageTexture::create_from_image(img);
			}
		}
	}
	if (busy_ || !s.jobs) return;
	float hold = FIELD_SPAN / HOLD_SHARE;
	bool moved = std::fabs(cam_x - (ox_ + FIELD_SPAN * 0.5f)) > hold || std::fabs(cam_z - (oz_ + FIELD_SPAN * 0.5f)) > hold;
	uint64_t rev = revision_;
	if (rev == seen_ && !moved) return;
	busy_ = true;
	float ox = std::floor((cam_x - FIELD_SPAN * 0.5f) / CELL_M) * CELL_M, oz = std::floor((cam_z - FIELD_SPAN * 0.5f) / CELL_M) * CELL_M;
	s.jobs->submit(JobKey{ JobLane::CANYON, 0 }, 0.f, [this, &s, ox, oz, rev]() { work(s, ox, oz, rev); });
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
