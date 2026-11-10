// See fleet_shadow.h.
#include "render/agents/fleet_shadow.h"

#include "core/grid.h"
#include "render/render_layers.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float SECTOR_PAD_M = 12.f; // the longest vehicle past its sector's edge, by its origin
} // namespace

AABB mesh_outline(const MeshBuilder &b) {
	Vector3 lo, hi;
	for (int64_t i = 0; i < b.v.size(); i++) {
		const Vector3 &p = b.v[i];
		lo = i == 0 ? p : lo.min(p);
		hi = i == 0 ? p : hi.max(p);
	}
	return AABB(lo, hi - lo);
}

void FleetShadow::init(const RID &scenario, const RID &material, const MeshBuilder &hull, float sector_m, float inset, int models) {
	free();
	scenario_ = scenario;
	material_ = material;
	sector_m_ = sector_m;
	inset_ = inset;
	hull_box_ = mesh_outline(hull);
	fit_.assign(size_t(models), Fit());
	hull_ = hull.bake(material_).shared();
}

void FleetShadow::set_model(int model, const MeshBuilder &body) {
	const AABB box = mesh_outline(body);
	const Vector3 mid = box.get_center(), hull_mid = hull_box_.get_center();
	Fit &f = fit_[size_t(model)];
	// by the top, not the height: the hull has no wheels under it, and the roof is what must stay inside
	f.scale = Vector3(box.size.x / hull_box_.size.x, box.get_end().y / hull_box_.get_end().y, box.size.z / hull_box_.size.z) * inset_;
	f.side = mid.x - hull_mid.x * f.scale.x;
	f.along = mid.z - hull_mid.z * f.scale.z;
}

void FleetShadow::add(int model, const Pose &p) {
	const Fit &f = fit_[size_t(model)];
	const Vector3 &k = f.scale;
	const Vector3 side(p.fz, 0, -p.fx), along(p.fx, 0, p.fz);
	const int i = int(std::floor(p.x / sector_m_)), j = int(std::floor(p.z / sector_m_));
	mm_row_plain(sectors_[cell_key(i, j)].buf.add(), side * k.x, Vector3(0, k.y, 0), along * k.z, Vector3(p.x, p.y, p.z) + side * f.side + along * f.along, 0, 0, 0, 0);
}

void FleetShadow::draw() {
	for (auto it = sectors_.begin(); it != sectors_.end();) {
		Sector &s = it->second;
		if (s.buf.count() == 0) {
			s.draw.free();
			it = sectors_.erase(it);
			continue;
		}
		const float x0 = float(key_x(it->first)) * sector_m_ - SECTOR_PAD_M, z0 = float(key_y(it->first)) * sector_m_ - SECTOR_PAD_M;
		const float side = sector_m_ + SECTOR_PAD_M * 2;
		s.draw.shadow_only = true;
		s.draw.set(scenario_, hull_.rid, s.buf, AABB(Vector3(x0, AABB_MIN_Y, z0), Vector3(side, AABB_H, side)), true, LAYER_VEHICLES);
		s.buf.restart();
		++it;
	}
}

void FleetShadow::free() {
	for (auto &kv : sectors_) kv.second.draw.free();
	sectors_.clear();
	hull_.reset();
}

} // namespace city
