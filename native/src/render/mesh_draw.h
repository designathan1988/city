// A mesh made by a job, and one drawn on its own. rs_util.h includes this at its end: it stands on swap_base.
#pragma once

#include "render/rs_util.h"

namespace city {

// A mesh made by a job, off the main thread, whole with its materials (surface_data.h: mesh_create_from_surfaces, whose pipelines the
// engine makes then, in the background): the conversion never lands on the frame and the uploads are paced by the bakes the frame
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
	static OwnedMesh of(const godot::RID &made) { // takes `made` over (surface_data.h mesh_of_arrays, create_mesh)
		OwnedMesh m;
		m.rid = made;
		return m;
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
	void adopt(const godot::RID &scenario, bool shadows, const godot::RID &next, RenderLayer layer) {
		swap_base(inst, mesh, next, scenario, shadows, layer);
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
