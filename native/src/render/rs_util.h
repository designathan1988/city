// RenderingServer helpers: instances drawn without scene nodes (lighter for thousands of chunks).
#pragma once

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace city {

// Floats per instance: 12 transform + 4 colour + 4 custom. Props/agents/trees keep colour white and put
// their tint in custom.rgb (the instance colour would also multiply fixed-colour parts).
constexpr int MM_STRIDE = 20;

// Appends one instance in the documented MultiMesh layout (row-major 3x4 transform, colour, custom).
inline void mm_push(godot::PackedFloat32Array &buf, const godot::Vector3 &x, const godot::Vector3 &y, const godot::Vector3 &z,
		const godot::Vector3 &o, const godot::Color &col, float c0, float c1, float c2, float c3) {
	const float v[MM_STRIDE] = { x.x, y.x, z.x, o.x, x.y, y.y, z.y, o.y, x.z, y.z, z.z, o.z, col.r, col.g, col.b, col.a, c0, c1, c2, c3 };
	int64_t at = buf.size();
	buf.resize(at + MM_STRIDE);
	float *w = buf.ptrw() + at;
	for (int i = 0; i < MM_STRIDE; i++) w[i] = v[i];
}

// One MultiMesh drawn by one instance. Created lazily, resized to fit the buffer.
struct MultiMeshDraw {
	godot::RID mm, inst;
	int capacity = -1;

	void set(const godot::RID &scenario, const godot::RID &mesh, const godot::RID &material, const godot::PackedFloat32Array &buf,
			const godot::AABB &bounds, bool shadows) {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		int count = int(buf.size() / MM_STRIDE);
		if (!mm.is_valid()) {
			mm = rs->multimesh_create();
			inst = rs->instance_create2(mm, scenario);
			rs->instance_geometry_set_cast_shadows_setting(inst,
					shadows ? godot::RenderingServer::SHADOW_CASTING_SETTING_ON : godot::RenderingServer::SHADOW_CASTING_SETTING_OFF);
		}
		if (material.is_valid()) rs->instance_geometry_set_material_override(inst, material);
		if (count != capacity) {
			rs->multimesh_allocate_data(mm, count, godot::RenderingServer::MULTIMESH_TRANSFORM_3D, true, true);
			rs->multimesh_set_mesh(mm, mesh);
			capacity = count;
		}
		if (count > 0) rs->multimesh_set_buffer(mm, buf);
		rs->instance_set_custom_aabb(inst, bounds);
		rs->instance_set_visible(inst, count > 0);
	}
	void visible(bool on) const {
		if (inst.is_valid()) godot::RenderingServer::get_singleton()->instance_set_visible(inst, on && capacity > 0);
	}
	void range(float begin, float end) const {
		if (inst.is_valid())
			godot::RenderingServer::get_singleton()->instance_geometry_set_visibility_range(inst, begin, end, 0, 0,
					godot::RenderingServer::VISIBILITY_RANGE_FADE_DISABLED);
	}
	void free() {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		if (inst.is_valid()) rs->free_rid(inst);
		if (mm.is_valid()) rs->free_rid(mm);
		inst = mm = godot::RID();
		capacity = -1;
	}
};

// A plain mesh drawn by one instance (road and overlay chunks).
struct MeshDraw {
	godot::RID mesh, inst;
	void reset(const godot::RID &scenario, const godot::RID &material, bool shadows) {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		if (!mesh.is_valid()) {
			mesh = rs->mesh_create();
			inst = rs->instance_create2(mesh, scenario);
			rs->instance_geometry_set_cast_shadows_setting(inst,
					shadows ? godot::RenderingServer::SHADOW_CASTING_SETTING_ON : godot::RenderingServer::SHADOW_CASTING_SETTING_OFF);
		}
		rs->mesh_clear(mesh);
		if (material.is_valid()) rs->instance_geometry_set_material_override(inst, material);
	}
	void visible(bool on) const {
		if (inst.is_valid()) godot::RenderingServer::get_singleton()->instance_set_visible(inst, on);
	}
	void free() {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		if (inst.is_valid()) rs->free_rid(inst);
		if (mesh.is_valid()) rs->free_rid(mesh);
		inst = mesh = godot::RID();
	}
};

} // namespace city
