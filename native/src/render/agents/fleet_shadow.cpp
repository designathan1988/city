// See fleet_shadow.h.
#include "render/agents/fleet_shadow.h"

#include "core/grid.h"
#include "render/render_layers.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

using namespace godot;

namespace city {

void FleetShadow::init(const RID &scenario, const RID &material, const MeshBuilder &hull, float sector_m, float inset, int models) {
	free();
	scenario_ = scenario;
	material_ = material;
	sector_m_ = sector_m;
	inset_ = inset;
	hull_box_ = mesh_outline(hull);
	fit_.assign(size_t(models), Fit());
	hull_ = std::make_shared<OwnedMesh>();
	hull_->rid = RenderingServer::get_singleton()->mesh_create();
	hull.commit_to(hull_->rid);
}

void FleetShadow::share_geometry(const FleetShadow &source) {
	free();
	scenario_ = source.scenario_;
	material_ = source.material_;
	hull_ = source.hull_;
	hull_box_ = source.hull_box_;
	sector_m_ = source.sector_m_;
	inset_ = source.inset_;
	fit_ = source.fit_;
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
	Sector &s = sectors_[cell_key(i, j)];
	const Vector3 x = side * k.x, y(0, k.y, 0), z = along * k.z;
	const Vector3 origin = Vector3(p.x, p.y, p.z) + side * f.side + along * f.along;
	const Vector3 middle = hull_box_.get_center(), half = hull_box_.size * .5f;
	const Vector3 center = origin + x * middle.x + y * middle.y + z * middle.z;
	// The actual hulls in this sector, including their heights and headings. No map-wide Y box or guessed vehicle length.
	const Vector3 extent = x.abs() * half.x + y.abs() * half.y + z.abs() * half.z;
	const Vector3 roundoff = (origin.abs() + x.abs() * (std::fabs(middle.x) + half.x) +
			y.abs() * (std::fabs(middle.y) + half.y) + z.abs() * (std::fabs(middle.z) + half.z) + Vector3(1, 1, 1)) * (32 * std::numeric_limits<float>::epsilon());
	const AABB box(center - extent - roundoff, (extent + roundoff) * 2);
	s.bounds = s.buf.count() == 0 ? box : s.bounds.merge(box);
	mm_row_plain(s.buf.add(), x, y, z, origin, 0, 0, 0, 0);
}

void FleetShadow::draw() {
	for (auto it = sectors_.begin(); it != sectors_.end();) {
		Sector &s = it->second;
		if (s.buf.count() == 0) {
			s.draw.free();
			it = sectors_.erase(it);
			continue;
		}
		s.draw.draw.shadow_only = true;
		s.draw.set(scenario_, hull_->rid, material_, s.buf, s.bounds, true, LAYER_VEHICLES);
		s.buf.restart();
		++it;
	}
}

void FleetShadow::reset() {
	for (auto &kv : sectors_) kv.second.draw.free();
	sectors_.clear();
}

void FleetShadow::free() {
	reset();
	hull_.reset();
	fit_.clear();
}

} // namespace city
