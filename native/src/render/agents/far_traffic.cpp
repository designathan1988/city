// See far_traffic.h.
#include "render/agents/far_traffic.h"

#include "core/grid.h"
#include "render/agents/fleet_shadow.h"
#include "render/render_layers.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>
#include <mutex>

using namespace godot;

namespace city {

namespace {
constexpr float SECTOR_PAD_M = 12.f; // a car's body past the end of its street
constexpr double CLOCK_WRAP_S = 4096; // the shader's clock wraps here: a float keeps a car's place to centimetres
// A car's seed from its street and its place in it (Knuth's multiplicative hash): its streets apart, its high bits the paint's.
constexpr uint32_t SEED_STEP = 2654435761u, SEED_STREETS = 31u, SEED_PAINT_SHIFT = 8;
constexpr float CURVE_QUARTER = 0.25f; // a curve's middle: a quarter of its start and of its end, half of its control point

// The middle of a street's curve (start a, control c, end b): where it is filed.
Vector3 middle_of(const FlowMark &m) { return Vector3(m.ax + 2 * m.cx + m.bx, m.ay + 2 * m.cy + m.by, m.az + 2 * m.cz + m.bz) * CURVE_QUARTER; }
} // namespace

void FarTraffic::init(const RID &scenario, const RID &material, const MeshBuilder &hull, float sector_m, float slot_m, int car_limit, float reach_m,
		const std::vector<Color> &paints) {
	free();
	scenario_ = scenario;
	material_ = material;
	sector_m_ = sector_m;
	slot_m_ = slot_m;
	car_limit_ = car_limit;
	reach_m_ = reach_m;
	paints_ = paints;
	hull_ = hull.bake(material_).shared();
	RenderingServer::get_singleton()->material_set_param(material_, "hull_half", mesh_outline(hull).size.z / 2);
}

void FarTraffic::detailed_within(float published_m, float drawn_m) {
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "seen_reach", published_m);
	rs->material_set_param(material_, "drawn_reach", drawn_m);
}

void FarTraffic::unfile(int ds, Street &st) {
	if (!st.filed) return;
	st.filed = false;
	auto it = sectors_.find(st.sector);
	if (it == sectors_.end()) return;
	std::vector<int> &list = it->second.streets;
	list.erase(std::remove(list.begin(), list.end(), ds), list.end());
	it->second.dirty = true;
}

void FarTraffic::file(int ds, Street &st) {
	const Vector3 mid = middle_of(st.told);
	const int64_t key = cell_key(int(std::floor(mid.x / sector_m_)), int(std::floor(mid.z / sector_m_)));
	if (st.filed && st.sector == key) {
		sectors_[key].dirty = true;
		return;
	}
	unfile(ds, st);
	Sector &sector = sectors_[key];
	sector.streets.push_back(ds);
	sector.reach = std::max(sector.reach, st.told.len * 0.5f);
	sector.dirty = true;
	st.filed = true;
	st.sector = key;
}

void FarTraffic::collect(ChangeLog &log) {
	{
		std::lock_guard<std::mutex> lk(log.mutex);
		taken_.swap(log.flow_marks);
	}
	if (hull_.valid())
		for (const FlowMark &m : taken_) take(m);
	taken_.clear();
}

void FarTraffic::take(const FlowMark &m) {
	if (m.ds < 0) return;
	if (size_t(m.ds) >= streets_.size()) streets_.resize(size_t(m.ds) + 1);
	Street &st = streets_[size_t(m.ds)];
	if (m.flags & FLOW_LINE) {
		st.told = m; // its line with its numbers
	} else { // its numbers alone: the line stays as it was told
		st.told.flags = uint8_t((st.told.flags & FLOW_LINE) | m.flags);
		st.told.count = m.count;
		st.told.queued = m.queued;
		st.told.speed = m.speed;
	}
	const bool drawn = (st.told.flags & FLOW_LIVE) && (st.told.flags & FLOW_LINE) && st.told.len > 0;
	if (drawn) file(m.ds, st);
	else unfile(m.ds, st);
}

// One sector's cars from its streets' numbers.
void FarTraffic::write(int64_t key, Sector &sector) {
	cars_ -= sector.cars;
	sector.rows.restart();
	const int room = std::max(0, car_limit_ - cars_);
	int n = 0;
	for (int ds : sector.streets) {
		const FlowMark &m = streets_[size_t(ds)].told;
		// a street of the window: its cars' pace is written negative, and the shader shows them only where the detailed vehicles
		// they stand in for are not drawn (far_cars.gdshader)
		const float way = (m.flags & FLOW_INSIDE) != 0 ? -1.f : 1.f;
		const int lanes = std::max(1, int(m.lanes));
		const int count = int(std::lround(m.count)), queued = std::min(count, int(std::lround(m.queued)));
		// the street as the shader reads it: the point (2u - u*u, 0, u*u) of the instance's space is its centre at the share u
		const Vector3 a(m.ax, m.ay, m.az), c(m.cx, m.cy, m.cz), b(m.bx, m.by, m.bz);
		const auto put = [&](int k, float place, float speed, float stretch) {
			const int lane = k % lanes;
			const float right = (float(lanes - 1) * 0.5f - float(lane)) * m.lane_step; // lane 0 is the rightmost
			const uint32_t seed = (uint32_t(ds) * SEED_STREETS + uint32_t(k)) * SEED_STEP;
			Color paint = paints_.empty() ? Color(1, 1, 1) : paints_[size_t(seed >> SEED_PAINT_SHIFT) % paints_.size()];
			paint.a = 1;
			mm_row(sector.rows.add(), c - a, Vector3(0, 1, 0), b - c, a, paint, place / m.len, way * speed / m.len, stretch / m.len, right);
			n++;
		};
		// the queue stands packed at the street's end, a row of its lanes every standing room back from it
		int k = 0;
		for (; k < queued && n < room; k++) {
			const float place = m.len - (std::floor(float(k) / float(lanes)) + 0.5f) * slot_m_;
			if (place < 0) break;
			put(k, place, 0, 0);
		}
		// the rest spaced over the free stretch before the queue, moving round it at the street's speed
		const int moving = count - queued;
		const float stretch = std::max(0.f, m.len - std::ceil(float(queued) / float(lanes)) * slot_m_);
		for (int i = 0; i < moving && stretch > 0 && n < room; i++) put(queued + i, (float(i) + 0.5f) / float(moving) * stretch, m.speed, stretch);
	}
	sector.cars = n;
	cars_ += n;
	sector.dirty = false;
	const float pad = sector.reach + SECTOR_PAD_M, side = sector_m_ + pad * 2;
	const AABB box(Vector3(float(key_x(key)) * sector_m_ - pad, AABB_MIN_Y, float(key_y(key)) * sector_m_ - pad), Vector3(side, AABB_H, side));
	sector.draw.set(scenario_, hull_.rid, sector.rows, box, false, LAYER_FAR_TRAFFIC); // a layer the far views keep (render_layers.h)
	sector.draw.range(reach_m_);
}

void FarTraffic::draw(double clock_s) {
	if (!hull_.valid()) return;
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "clock", float(std::fmod(clock_s, CLOCK_WRAP_S)));
	rs->material_set_param(material_, "seen_at", Vector2(look_.x, look_.z));
	for (auto it = sectors_.begin(); it != sectors_.end();) {
		Sector &sector = it->second;
		if (sector.streets.empty()) { // its last street left it: its draw goes with it
			cars_ -= sector.cars;
			sector.draw.free();
			it = sectors_.erase(it);
			continue;
		}
		if (sector.dirty) write(it->first, sector);
		++it;
	}
}

void FarTraffic::clear() {
	for (auto &kv : sectors_) kv.second.draw.free();
	sectors_.clear();
	streets_.clear();
	cars_ = 0;
}

void FarTraffic::free() {
	clear();
	hull_.reset();
}

} // namespace city
