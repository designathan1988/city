// The town's window (see canyon_field.h).
#include "render/atmosphere/canyon_field.h"

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
constexpr float CANYON_SQUARE_M = 4.f;
constexpr float FIELD_SPAN = RES * CANYON_SQUARE_M;
constexpr int REACH_CELLS = 12; // as far as a pixel looks for what stands over it (the shader's longest step)
constexpr float NOTHING_Y = -9000.f; // no solid here: under every ground
constexpr float EDGE_REACH_M = 64.f; // a lot's centre this far past the window may still reach into it
constexpr int CHANNELS = 2;

struct StandingLot {
	Building rec;
	float ground = 0;
	b40::BodyInput body;
};

// Each cell's height into `out`, the highest of the cells within `reach` along the row, and then along the column.
void reach_max(const std::vector<float> &in, std::vector<float> &out) {
	std::vector<float> rows(in.size());
	const auto line = [](auto get, auto put) {
		int deque[RES], head = 0, tail = 0, next = 0;
		for (int at = 0; at < RES; at++) {
			const int right = std::min(RES - 1, at + REACH_CELLS);
			for (; next <= right; next++) {
				const float value = get(next);
				if (std::isnan(value)) continue; // original std::max ignores a NaN second argument
				// Strict comparison keeps the original earliest winner for equal values, including signed zero.
				while (tail > head && value > get(deque[tail - 1])) tail--;
				deque[tail++] = next;
			}
			while (head < tail && deque[head] < at - REACH_CELLS) head++;
			put(at, head < tail ? std::max(NOTHING_Y, get(deque[head])) : NOTHING_Y);
		}
	};
	for (int y = 0; y < RES; y++) {
		if (JobPool::cancelled()) return;
		line([&](int x) { return in[size_t(y) * RES + x]; }, [&](int x, float v) { rows[size_t(y) * RES + x] = v; });
	}
	for (int x = 0; x < RES; x++) {
		if (JobPool::cancelled()) return;
		line([&](int y) { return rows[size_t(y) * RES + x]; }, [&](int y, float v) { out[size_t(y) * RES + x] = v; });
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
	int i0 = std::max(0, int(std::floor((minx - ox) / CANYON_SQUARE_M))), i1 = std::min(RES - 1, int(std::floor((maxx - ox) / CANYON_SQUARE_M)));
	int j0 = std::max(0, int(std::floor((minz - oz) / CANYON_SQUARE_M))), j1 = std::min(RES - 1, int(std::floor((maxz - oz) / CANYON_SQUARE_M)));
	for (int j = j0; j <= j1; j++)
		for (int i = i0; i <= i1; i++) {
			float dx = ox + (i + 0.5f) * CANYON_SQUARE_M - f.o.x, dz = oz + (j + 0.5f) * CANYON_SQUARE_M - f.o.y;
			float lx = dx * f.e0.x + dz * f.e0.y, ly = dx * f.e1.x + dz * f.e1.y;
			if (lx < v.x || lx > v.x + v.w || ly < v.y || ly > v.y + v.d) continue;
			float &c = cells[size_t(j) * RES + i];
			c = std::max(c, top);
		}
}
} // namespace

void CanyonField::reset() {
	result_.reset();
	seen_ = ~uint64_t(0);
	any_ = false;
	texture_.unref();
	view_revision_++;
}

CanyonField::Raster CanyonField::work(const CityState &s, float ox, float oz, uint64_t revision, std::shared_ptr<BodyCache> prepared) {
	BuildingBodies bodies(std::move(prepared));
	std::vector<StandingLot> lots;
	{
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		if (JobPool::cancelled()) return Raster{};
		s.buildings.query(ox - EDGE_REACH_M, oz - EDGE_REACH_M, ox + FIELD_SPAN + EDGE_REACH_M, oz + FIELD_SPAN + EDGE_REACH_M, [&](int id) {
			const Building &b = s.buildings.items[size_t(id)];
			if (b.alive && b.floors > 0) lots.push_back({ b, s.terrain.height_at(b.x, b.z), s.buildings.designs.edited->input(b.body) });
		});
	}
	std::vector<float> cells(size_t(RES) * RES, NOTHING_Y), near(cells.size());
	for (const StandingLot &l : lots) {
		if (JobPool::cancelled()) return Raster{};
		const b40::Body &b = bodies.body(s.buildings.designs, l.rec, &l.body);
		const b40::LotFrame f = b40::lot_frame(l.rec);
		for (const b40::Volume &v : b.volumes) {
			float top = l.ground;
			for (int level = 0; level < v.top(); level++) top += b.level_h(level);
			lay(cells, f, v, top, ox, oz);
		}
	}
	reach_max(cells, near);
	if (JobPool::cancelled()) return Raster{};
	Raster r;
	r.revision = revision;
	r.ox = ox;
	r.oz = oz;
	r.any = !lots.empty();
	r.texels.resize(int64_t(RES) * RES * CHANNELS * int64_t(sizeof(uint16_t)));
	uint16_t *out = reinterpret_cast<uint16_t *>(r.texels.ptrw());
	for (size_t k = 0; k < cells.size(); k++) {
		out[k * CHANNELS] = Math::make_half_float(cells[k]);
		out[k * CHANNELS + 1] = Math::make_half_float(near[k]);
	}
	return r;
}

void CanyonField::sync(const CityState &s, float cam_x, float cam_z) {
	const uint64_t rev = revision_.load();
	Raster done;
	if (result_.take(done)) {
		const float hold = FIELD_SPAN / LINGER_SHARE;
		const bool relevant = std::fabs(cam_x - (done.ox + FIELD_SPAN * 0.5f)) <= hold && std::fabs(cam_z - (done.oz + FIELD_SPAN * 0.5f)) <= hold;
		if (done.revision == rev && relevant) {
			seen_ = done.revision;
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
	}
	if (result_.pending() || !s.jobs) return;
	float hold = FIELD_SPAN / LINGER_SHARE;
	bool moved = std::fabs(cam_x - (ox_ + FIELD_SPAN * 0.5f)) > hold || std::fabs(cam_z - (oz_ + FIELD_SPAN * 0.5f)) > hold;
	if (rev == seen_ && !moved) return;
	const auto ticket = result_.begin();
	float ox = std::floor((cam_x - FIELD_SPAN * 0.5f) / CANYON_SQUARE_M) * CANYON_SQUARE_M, oz = std::floor((cam_z - FIELD_SPAN * 0.5f) / CANYON_SQUARE_M) * CANYON_SQUARE_M;
	s.jobs->submit(JobKey{ JobLane::CANYON, 0 }, 0.f, [ticket, &s, ox, oz, rev, prepared = prepared_, completion = AsyncResult<Raster>::completion(ticket)]() {
		(void)completion; // the capture acknowledges terminal cancellation even when work is skipped
		if (AsyncResult<Raster>::retired(ticket) || JobPool::cancelled()) return;
		auto raster = work(s, ox, oz, rev, prepared);
		if (!JobPool::cancelled()) AsyncResult<Raster>::publish(ticket, std::move(raster));
	});
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
