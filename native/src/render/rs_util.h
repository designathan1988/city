// RenderingServer helpers: instances drawn without scene nodes (lighter for thousands of chunks).
#pragma once

#include "render/render_layers.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace city {

// Floats per instance: 12 transform + 4 colour + 4 custom. Props/agents/trees keep colour white and put
// their tint in custom.rgb (the instance colour would also multiply fixed-colour parts).
constexpr int MM_STRIDE = 20;
constexpr int MM_PLAIN_STRIDE = 16; // no colour: the engine keeps three CPU copies of each instance, so what is not read is not stored

// Appends one instance in the documented MultiMesh layout (row-major 3x4 transform, colour, custom).
inline void mm_push(godot::PackedFloat32Array &buf, const godot::Vector3 &x, const godot::Vector3 &y, const godot::Vector3 &z,
		const godot::Vector3 &o, const godot::Color &col, float c0, float c1, float c2, float c3) {
	const float v[MM_STRIDE] = { x.x, y.x, z.x, o.x, x.y, y.y, z.y, o.y, x.z, y.z, z.z, o.z, col.r, col.g, col.b, col.a, c0, c1, c2, c3 };
	int64_t at = buf.size();
	buf.resize(at + MM_STRIDE);
	float *w = buf.ptrw() + at;
	for (int i = 0; i < MM_STRIDE; i++) w[i] = v[i];
}

// The same without the colour, for a material that reads only INSTANCE_CUSTOM.
inline void mm_push_plain(godot::PackedFloat32Array &buf, const godot::Vector3 &x, const godot::Vector3 &y, const godot::Vector3 &z,
		const godot::Vector3 &o, float c0, float c1, float c2, float c3) {
	const float v[MM_PLAIN_STRIDE] = { x.x, y.x, z.x, o.x, x.y, y.y, z.y, o.y, x.z, y.z, z.z, o.z, c0, c1, c2, c3 };
	int64_t at = buf.size();
	buf.resize(at + MM_PLAIN_STRIDE);
	float *w = buf.ptrw() + at;
	for (int i = 0; i < MM_PLAIN_STRIDE; i++) w[i] = v[i];
}

// `inst` (made on first use in `scenario`, on render `layer`, with `material` when valid, casting shadows or not) shows
// `next` in place of `current`, which is freed; an invalid `next` empties the instance but keeps it for the next base.
inline void swap_base(godot::RID &inst, godot::RID &current, const godot::RID &next, const godot::RID &scenario,
		const godot::RID &material, bool shadows, RenderLayer layer) {
	godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
	if (!inst.is_valid()) {
		if (!next.is_valid()) return;
		inst = rs->instance_create2(next, scenario);
		rs->instance_set_layer_mask(inst, layer);
		rs->instance_geometry_set_cast_shadows_setting(inst,
				shadows ? godot::RenderingServer::SHADOW_CASTING_SETTING_ON : godot::RenderingServer::SHADOW_CASTING_SETTING_OFF);
		if (material.is_valid()) rs->instance_geometry_set_material_override(inst, material);
	} else {
		rs->instance_set_base(inst, next);
	}
	if (current.is_valid()) rs->free_rid(current);
	current = next;
}

// One MultiMesh drawn by one instance. Created lazily, resized to fit the buffer. `bounds` is both the instance's and
// the MultiMesh's own box: without one, every multimesh_set_buffer walks all the instances on the main thread to
// rebuild the MultiMesh AABB (Godot renderer_rd/storage_rd/mesh_storage.cpp _multimesh_set_buffer).
struct MultiMeshDraw {
	godot::RID mm, inst;
	int capacity = -1;

	void set(const godot::RID &scenario, const godot::RID &mesh, const godot::RID &material, const godot::PackedFloat32Array &buf,
			const godot::AABB &bounds, bool shadows, RenderLayer layer) {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		int count = int(buf.size() / MM_STRIDE);
		if (!mm.is_valid()) {
			mm = rs->multimesh_create();
			inst = rs->instance_create2(mm, scenario);
			rs->instance_set_layer_mask(inst, layer);
			rs->instance_geometry_set_cast_shadows_setting(inst,
					shadows ? godot::RenderingServer::SHADOW_CASTING_SETTING_ON : godot::RenderingServer::SHADOW_CASTING_SETTING_OFF);
		}
		if (material.is_valid()) rs->instance_geometry_set_material_override(inst, material);
		if (count != capacity) {
			rs->multimesh_allocate_data(mm, count, godot::RenderingServer::MULTIMESH_TRANSFORM_3D, true, true);
			rs->multimesh_set_mesh(mm, mesh);
			capacity = count;
		}
		rs->multimesh_set_custom_aabb(mm, bounds);
		if (count > 0) rs->multimesh_set_buffer(mm, buf);
		rs->instance_set_custom_aabb(inst, bounds);
		rs->instance_set_visible(inst, count > 0);
	}
	// Shows `next` (a MultiMesh of `count` instances a job filled, bake_multimesh, now owned here): swap_base.
	void adopt(const godot::RID &scenario, const godot::RID &material, bool shadows, const godot::RID &next, int count,
			const godot::AABB &bounds, RenderLayer layer) {
		swap_base(inst, mm, next, scenario, material, shadows, layer);
		capacity = mm.is_valid() ? count : 0;
		if (inst.is_valid()) godot::RenderingServer::get_singleton()->instance_set_custom_aabb(inst, bounds);
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

// A mesh made by a job, off the main thread: mesh_add_surface_from_arrays converts the arrays on the calling thread and
// the RenderingServer queues the surface's upload for the main thread's next call into it (Godot rendering_server.cpp /
// server_wrap_mt_common.h), so the conversion never lands on the frame and the uploads are paced by the bakes the frame
// admits (chunk_jobs.h). Freed unless handed over (a result dropped as stale frees its mesh); bake_multimesh fills a
// MultiMesh the same way.
struct OwnedMesh {
	godot::RID rid;
	OwnedMesh() = default;
	OwnedMesh(const OwnedMesh &) = delete;
	OwnedMesh &operator=(const OwnedMesh &) = delete;
	OwnedMesh(OwnedMesh &&o) noexcept : rid(o.rid) { o.rid = godot::RID(); }
	OwnedMesh &operator=(OwnedMesh &&o) noexcept {
		if (this != &o) {
			reset();
			rid = o.rid;
			o.rid = godot::RID();
		}
		return *this;
	}
	~OwnedMesh() { reset(); }
	bool valid() const { return rid.is_valid(); }
	void reset() {
		if (rid.is_valid()) godot::RenderingServer::get_singleton()->free_rid(rid);
		rid = godot::RID();
	}
	godot::RID release() {
		godot::RID r = rid;
		rid = godot::RID();
		return r;
	}
};

// A MultiMesh of `buf` (MultiMesh layout, MM_PLAIN_STRIDE floats per instance) filled on the calling thread, as a mesh bake
// (chunk_jobs.h): its own box, so the RenderingServer never walks the instances to bound it. None when `buf` is empty.
inline OwnedMesh bake_multimesh(const godot::RID &mesh, const godot::PackedFloat32Array &buf, const godot::AABB &bounds) {
	OwnedMesh m;
	const int count = int(buf.size() / MM_PLAIN_STRIDE);
	if (count == 0) return m;
	godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
	m.rid = rs->multimesh_create();
	rs->multimesh_allocate_data(m.rid, count, godot::RenderingServer::MULTIMESH_TRANSFORM_3D, false, true);
	rs->multimesh_set_mesh(m.rid, mesh);
	rs->multimesh_set_custom_aabb(m.rid, bounds);
	rs->multimesh_set_buffer(m.rid, buf);
	return m;
}

// A plain mesh drawn by one instance (road and overlay chunks).
struct MeshDraw {
	godot::RID mesh, inst;
	// Shows `next` (a mesh a job made, now owned here): swap_base.
	void adopt(const godot::RID &scenario, const godot::RID &material, bool shadows, const godot::RID &next, RenderLayer layer) {
		swap_base(inst, mesh, next, scenario, material, shadows, layer);
	}
	void reset(const godot::RID &scenario, const godot::RID &material, bool shadows, RenderLayer layer) {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		if (!mesh.is_valid()) {
			mesh = rs->mesh_create();
			inst = rs->instance_create2(mesh, scenario);
			rs->instance_set_layer_mask(inst, layer);
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
