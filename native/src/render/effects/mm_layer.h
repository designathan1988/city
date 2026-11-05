// One MultiMesh of a fixed capacity drawn by one instance, written whole when its CPU buffer changed (the effects write
// slots rarely: only the frames something is born or moves). Layout of rs_util.h (12 transform, 4 colour, 4 custom).
#pragma once

#include "render/rs_util.h"

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
			const godot::AABB &bounds) {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		mesh = shape;
		capacity = count;
		mm = rs->multimesh_create();
		rs->multimesh_allocate_data(mm, count, godot::RenderingServer::MULTIMESH_TRANSFORM_3D, true, true);
		rs->multimesh_set_mesh(mm, mesh->get_rid());
		rs->multimesh_set_custom_aabb(mm, bounds);
		inst = rs->instance_create2(mm, scenario);
		rs->instance_geometry_set_cast_shadows_setting(
				inst, shadows ? godot::RenderingServer::SHADOW_CASTING_SETTING_ON : godot::RenderingServer::SHADOW_CASTING_SETTING_OFF);
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
	}
	void count(int n) const { godot::RenderingServer::get_singleton()->multimesh_set_visible_instances(mm, n); }
	void upload() const { godot::RenderingServer::get_singleton()->multimesh_set_buffer(mm, buf); }
	void shown(bool on) const { godot::RenderingServer::get_singleton()->instance_set_visible(inst, on); }
	void release() {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		if (inst.is_valid()) rs->free_rid(inst);
		if (mm.is_valid()) rs->free_rid(mm);
		inst = godot::RID();
		mm = godot::RID();
		mesh.unref();
	}
};

} // namespace city
