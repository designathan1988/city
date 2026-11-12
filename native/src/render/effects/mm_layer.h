// One MultiMesh of a fixed capacity drawn by one instance, written whole when its CPU buffer changed (the effects write
// slots rarely: only the frames something is born or moves). Layout of rs_util.h (12 transform, 4 colour, 4 custom).
#pragma once

#include "render/rs_util.h"
#include "render/spatial_instance_draw.h"

#include <memory>

#include <godot_cpp/classes/mesh.hpp>

namespace city {

constexpr float WORLD_HALF_M = 200000.f, WORLD_UP_M = 20000.f; // a layer's box: the whole map, whatever it holds

struct MmLayer {
	godot::RID mm, inst;
	godot::Ref<godot::Mesh> mesh; // held: the MultiMesh draws its RID
	int capacity = 0;
	godot::PackedFloat32Array buf;

	// `bounds`: the MultiMesh's own box, so uploads never walk the instances (rs_util.h MultiMeshDraw).
	void init(const godot::RID &scenario, const godot::Ref<godot::Mesh> &shape, const godot::RID &material, int count, bool shadows,
			const godot::AABB &bounds, float sector_m = 0, bool shadow_only = false) {
		release();
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		shown_ = true;
		mesh = shape;
		capacity = count;
		active_ = count;
		local_box_ = shape->get_aabb();
		if (sector_m > 0) {
			spatial_ = std::make_unique<SpatialInstanceDraw>();
			spatial_->configure(scenario, mesh->get_rid(), material, shadows, LAYER_BASE, sector_m, shadow_only);
			row_bounds_.resize(size_t(count));
			buf.resize(int64_t(count) * MM_STRIDE);
			buf.fill(0.f);
			return;
		}
		mm = rs->multimesh_create();
		rs->multimesh_allocate_data(mm, count, godot::RenderingServer::MULTIMESH_TRANSFORM_3D, true, true);
		rs->multimesh_set_mesh(mm, mesh->get_rid());
		rs->multimesh_set_custom_aabb(mm, bounds);
		inst = rs->instance_create2(mm, scenario);
		rs->instance_geometry_set_cast_shadows_setting(
				inst, shadow_only ? godot::RenderingServer::SHADOW_CASTING_SETTING_SHADOWS_ONLY :
				(shadows ? godot::RenderingServer::SHADOW_CASTING_SETTING_ON : godot::RenderingServer::SHADOW_CASTING_SETTING_OFF));
		rs->instance_geometry_set_material_override(inst, material);
		rs->instance_set_custom_aabb(inst, bounds);
		buf.resize(int64_t(count) * MM_STRIDE);
		buf.fill(0.f);
		rs->multimesh_set_buffer(mm, buf);
	}
	float *slot(int i) { return buf.ptrw() + int64_t(i) * MM_STRIDE; }
	// Instance `i`: basis columns x, y, z, origin, custom.
	void put(int i, const godot::Vector3 &x, const godot::Vector3 &y, const godot::Vector3 &z, const godot::Vector3 &o, float c0, float c1,
			float c2, float c3) {
		float *w = slot(i);
		const float v[MM_STRIDE] = { x.x, y.x, z.x, o.x, x.y, y.y, z.y, o.y, x.z, y.z, z.z, o.z, 1, 1, 1, 1, c0, c1, c2, c3 };
		for (int k = 0; k < MM_STRIDE; k++) w[k] = v[k];
		if (spatial_) row_bounds_[size_t(i)] = instance_bounds(w, local_box_, world_pad_);
	}
	void envelope(const godot::AABB &local, float world_pad = 0) { local_box_ = local; world_pad_ = world_pad; }
	void bounds(int i, const godot::AABB &world) { if (spatial_) row_bounds_[size_t(i)] = world; }
	void shift_bounds(int first, int end) { if (spatial_) for (int i = first; i + 1 < end; i++) row_bounds_[size_t(i)] = row_bounds_[size_t(i + 1)]; }
	void count(int n) {
		active_ = n;
		if (spatial_) { if (!n) spatial_->visible(false); }
		else if (mm.is_valid()) godot::RenderingServer::get_singleton()->multimesh_set_visible_instances(mm, n);
	}
	void upload() {
		if (!spatial_) { if (mm.is_valid()) godot::RenderingServer::get_singleton()->multimesh_set_buffer(mm, buf); return; }
		spatial_->begin();
		const float *rows = buf.ptr();
		for (int i = 0; i < active_; i++) spatial_->put(uint64_t(i), rows + int64_t(i) * MM_STRIDE, row_bounds_[size_t(i)]);
		spatial_->finish();
		spatial_->visible(shown_ && active_ > 0);
	}
	void settle() { if (spatial_) spatial_->settle(); }
	void shown(bool on) {
		shown_ = on;
		if (spatial_) spatial_->visible(on && active_ > 0);
		else if (inst.is_valid()) godot::RenderingServer::get_singleton()->instance_set_visible(inst, on);
	}
	void release() {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		if (inst.is_valid()) rs->free_rid(inst);
		if (mm.is_valid()) rs->free_rid(mm);
		inst = godot::RID();
		mm = godot::RID();
		mesh.unref();
		if (spatial_) spatial_->reset();
		spatial_.reset();
		row_bounds_.clear();
	}

private:
	std::unique_ptr<SpatialInstanceDraw> spatial_;
	std::vector<godot::AABB> row_bounds_;
	godot::AABB local_box_;
	float world_pad_ = 0;
	int active_ = 0;
	bool shown_ = true;
};

} // namespace city
