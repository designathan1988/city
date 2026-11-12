// Spatial presentation units. Every supplied row remains a caster; the engine rejects each populated sector
// separately for the camera and each light cascade. Stable row ownership is local to a sector.
#pragma once

#include "core/grid.h"
#include "render/row_keeper.h"
#include "render/static_instance_draw.h"

#include <cmath>
#include <limits>
#include <unordered_map>

namespace city {

inline godot::AABB instance_bounds(const float *row, const godot::AABB &local, float world_pad = 0) {
	const godot::Vector3 x(row[0], row[4], row[8]), y(row[1], row[5], row[9]), z(row[2], row[6], row[10]);
	const godot::Vector3 origin(row[3], row[7], row[11]), middle = local.get_center(), half = local.size * .5f;
	const godot::Vector3 center = origin + x * middle.x + y * middle.y + z * middle.z;
	const godot::Vector3 extent = x.abs() * half.x + y.abs() * half.y + z.abs() * half.z;
	const godot::Vector3 error = (origin.abs() + x.abs() * (std::fabs(middle.x) + half.x) +
			y.abs() * (std::fabs(middle.y) + half.y) + z.abs() * (std::fabs(middle.z) + half.z) + godot::Vector3(1, 1, 1)) *
			(32 * std::numeric_limits<float>::epsilon()) + godot::Vector3(world_pad, world_pad, world_pad);
	return godot::AABB(center - extent - error, (extent + error) * 2);
}

class SpatialInstanceDraw {
public:
	SpatialInstanceDraw() = default;
	SpatialInstanceDraw(const SpatialInstanceDraw &) = delete;
	SpatialInstanceDraw &operator=(const SpatialInstanceDraw &) = delete;
	~SpatialInstanceDraw() { reset(); }
	void configure(const godot::RID &scenario, const godot::RID &mesh, const godot::RID &material, bool shadows,
			RenderLayer layer, float sector_m, bool shadow_only = false, int stride = MM_STRIDE) {
		reset();
		scenario_ = scenario;
		mesh_ = mesh;
		material_ = material;
		shadows_ = shadows;
		shadow_only_ = shadow_only;
		layer_ = layer;
		sector_m_ = std::isfinite(sector_m) ? std::max(1.f, sector_m) : 1.f;
		stride_ = stride;
		visible_ = true;
	}
	void begin() {
		for (auto &entry : sectors_) {
			entry.second.keeper.begin();
			entry.second.rows.reopen();
			entry.second.has_bounds = false;
		}
	}
	void put(uint64_t id, const float *row, const godot::AABB &bounds) {
		auto cell = [&](float coordinate) {
			if (!std::isfinite(coordinate)) return 0;
			return int(std::max(double(std::numeric_limits<int>::min()), std::min(double(std::numeric_limits<int>::max()),
					std::floor(double(coordinate) / sector_m_))));
		};
		const int64_t key = cell_key(cell(row[3]), cell(row[11]));
		auto inserted = sectors_.try_emplace(key, stride_);
		Sector &s = inserted.first->second;
		if (inserted.second) s.keeper.begin();
		std::memcpy(s.keeper.place(id, 0, s.rows), row, size_t(stride_) * sizeof(float));
		s.bounds = s.has_bounds ? s.bounds.merge(bounds) : bounds;
		s.has_bounds = true;
	}
	void finish() {
		count_ = 0;
		for (auto it = sectors_.begin(); it != sectors_.end();) {
			Sector &s = it->second;
			s.keeper.sweep([&](int) -> InstanceRows & { return s.rows; });
			if (!s.rows.count()) {
				s.draw.free();
				it = sectors_.erase(it);
				continue;
			}
			s.draw.draw.shadow_only = shadow_only_;
			s.draw.set(scenario_, mesh_, material_, s.rows, s.bounds, shadows_, layer_);
			s.draw.draw.visible(visible_);
			count_ += s.rows.count();
			++it;
		}
	}
	void settle() {
		for (auto &entry : sectors_) {
			Sector &s = entry.second;
			if (s.draw.needs_settle()) s.draw.set(scenario_, mesh_, material_, s.rows, s.bounds, shadows_, layer_);
		}
	}
	void visible(bool on) {
		if (visible_ == on) return;
		visible_ = on;
		for (auto &entry : sectors_) entry.second.draw.draw.visible(on);
	}
	void reset() {
		for (auto &entry : sectors_) entry.second.draw.free();
		sectors_.clear();
		count_ = 0;
	}
	int count() const { return count_; }

private:
	struct Sector {
		explicit Sector(int stride) : rows(stride) { keeper.draws(1); }
		RowKeeper keeper;
		InstanceRows rows;
		StaticInstanceDraw draw;
		godot::AABB bounds;
		bool has_bounds = false;
	};
	std::unordered_map<int64_t, Sector> sectors_;
	godot::RID scenario_, mesh_, material_;
	RenderLayer layer_ = LAYER_VEHICLES;
	float sector_m_ = 1;
	int stride_ = MM_STRIDE, count_ = 0;
	bool shadows_ = false, shadow_only_ = false, visible_ = true;
};

} // namespace city
