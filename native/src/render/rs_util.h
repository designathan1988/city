// RenderingServer helpers: instances drawn without scene nodes (lighter for thousands of chunks).
#pragma once

#include "render/render_layers.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <algorithm>
#include <cstring>

namespace city {

// The vertical span every chunk's and every camera-wide box is given: the map's lowest ground to past its tallest building.
constexpr float AABB_MIN_Y = -100.f, AABB_H = 700.f, AABB_HALF_H = 500.f;

// Floats per instance: 12 transform + 4 colour + 4 custom. Props/agents/trees keep colour white and put
// their tint in custom.rgb (the instance colour would also multiply fixed-colour parts).
constexpr int MM_STRIDE = 20;
// Without the colour: 12 transform + 4 custom. The renderer keeps three copies of every instance's floats on the CPU, whether
// it interpolates them or not (Godot servers/rendering/storage/mesh_storage.cpp multimesh_allocate_data), so a thing that
// repeats by the thousand and has no colour of its own goes without.
constexpr int MM_PLAIN_STRIDE = 16;

// Appends one instance in the documented MultiMesh layout (row-major 3x4 transform, colour, custom).
inline void mm_push(godot::PackedFloat32Array &buf, const godot::Vector3 &x, const godot::Vector3 &y, const godot::Vector3 &z,
		const godot::Vector3 &o, const godot::Color &col, float c0, float c1, float c2, float c3) {
	const float v[MM_STRIDE] = { x.x, y.x, z.x, o.x, x.y, y.y, z.y, o.y, x.z, y.z, z.z, o.z, col.r, col.g, col.b, col.a, c0, c1, c2, c3 };
	int64_t at = buf.size();
	buf.resize(at + MM_STRIDE);
	float *w = buf.ptrw() + at;
	for (int i = 0; i < MM_STRIDE; i++) w[i] = v[i];
}

// The same into a row of InstanceRows.
inline void mm_row(float *w, const godot::Vector3 &x, const godot::Vector3 &y, const godot::Vector3 &z, const godot::Vector3 &o, const godot::Color &col,
		float c0, float c1, float c2, float c3) {
	const float v[MM_STRIDE] = { x.x, y.x, z.x, o.x, x.y, y.y, z.y, o.y, x.z, y.z, z.z, o.z, col.r, col.g, col.b, col.a, c0, c1, c2, c3 };
	std::memcpy(w, v, sizeof(v));
}
inline void mm_row_plain(float *w, const godot::Vector3 &x, const godot::Vector3 &y, const godot::Vector3 &z, const godot::Vector3 &o, float c0, float c1,
		float c2, float c3) {
	const float v[MM_PLAIN_STRIDE] = { x.x, y.x, z.x, o.x, x.y, y.y, z.y, o.y, x.z, y.z, z.z, o.z, c0, c1, c2, c3 };
	std::memcpy(w, v, sizeof(v));
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

// The rows of a MultiMesh refilled every frame. The array lives from frame to frame at the size of the draw's room, so a frame costs
// one ptrw, a cursor per row and no allocation, and what goes to the RenderingServer is the array itself, the size it asks.
class InstanceRows {
public:
	explicit InstanceRows(int stride = MM_STRIDE) : stride_(stride) {}
	void restart() {
		count_ = 0;
		base_ = rows_.is_empty() ? nullptr : rows_.ptrw();
	}
	float *add() {
		if (int64_t(count_ + 1) * stride_ > rows_.size()) grow(count_ + 1);
		return base_ + int64_t(count_++) * stride_;
	}
	void append(const InstanceRows &more) {
		if (more.count_ == 0) return;
		if (int64_t(count_ + more.count_) * stride_ > rows_.size()) grow(count_ + more.count_);
		std::memcpy(base_ + int64_t(count_) * stride_, more.base_, sizeof(float) * size_t(more.count_) * size_t(stride_));
		count_ += more.count_;
	}
	int count() const { return count_; }
	int stride() const { return stride_; }
	// RowKeeper's: the rows stay from the last frame, each one written or dropped.
	void reopen() { base_ = rows_.is_empty() ? nullptr : rows_.ptrw(); }
	float *row(int i) { return base_ + int64_t(i) * stride_; }
	void drop_last() { count_--; }
	const godot::PackedFloat32Array &fitted(int room) {
		if (rows_.size() != int64_t(room) * stride_) {
			rows_.resize(int64_t(room) * stride_);
			base_ = rows_.ptrw();
		}
		return rows_;
	}

private:
	void grow(int wanted) {
		rows_.resize(int64_t(std::max(wanted, count_ * 2)) * stride_);
		base_ = rows_.ptrw();
	}
	godot::PackedFloat32Array rows_;
	float *base_ = nullptr;
	int count_ = 0, stride_;
};

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

// A second instance of what a draw shows, in the shadow maps alone and only out to `end`: what stops being wider than a texel of the
// sun stops being drawn four times over, and the picture keeps it. The draw itself casts nothing.
struct ShadowTwin {
	godot::RID inst;
	void bind(const godot::RID &scenario, const godot::RID &base, const godot::RID &material, const godot::AABB &bounds, RenderLayer layer) {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		if (!base.is_valid()) {
			free();
			return;
		}
		if (!inst.is_valid()) {
			inst = rs->instance_create2(base, scenario);
			rs->instance_set_layer_mask(inst, layer);
			rs->instance_geometry_set_cast_shadows_setting(inst, godot::RenderingServer::SHADOW_CASTING_SETTING_SHADOWS_ONLY);
			rs->instance_geometry_set_material_override(inst, material);
		} else {
			rs->instance_set_base(inst, base);
		}
		rs->instance_set_custom_aabb(inst, bounds);
	}
	void range(float end) const {
		if (inst.is_valid())
			godot::RenderingServer::get_singleton()->instance_geometry_set_visibility_range(inst, 0, end, 0, 0,
					godot::RenderingServer::VISIBILITY_RANGE_FADE_DISABLED);
	}
	void visible(bool on) const {
		if (inst.is_valid()) godot::RenderingServer::get_singleton()->instance_set_visible(inst, on);
	}
	void free() {
		if (inst.is_valid()) godot::RenderingServer::get_singleton()->free_rid(inst);
		inst = godot::RID();
	}
};

// A refilled MultiMesh's room grows past what it holds in these steps and shrinks only two steps below: a count that changes
// every frame (vehicles, people, trains, lit lenses) never reallocates the GPU buffer every frame (multimesh_allocate_data
// frees it and makes another, its uniform sets and commands again: Godot renderer_rd/storage_rd/mesh_storage.cpp); only the
// visible count moves (multimesh_set_visible_instances).
constexpr int MM_ROOM_STEP = 32;

// One MultiMesh drawn by one instance. Created lazily, its room kept by the rule above (its only owner). `bounds` is both the
// instance's and the MultiMesh's own box: without one, every multimesh_set_buffer walks all the instances on the main thread
// to rebuild the MultiMesh AABB (Godot renderer_rd/storage_rd/mesh_storage.cpp _multimesh_set_buffer).
struct MultiMeshDraw {
	godot::RID mm, inst;
	int capacity = -1; // instances allocated
	int count = 0; // instances drawn
	bool coloured_ = true; // the layout allocated
	bool shadow_only = false; // drawn into the shadow maps alone: another draw shows what it stands for

	void set(const godot::RID &scenario, const godot::RID &mesh, const godot::RID &material, const godot::PackedFloat32Array &buf,
			const godot::AABB &bounds, bool shadows, RenderLayer layer, bool coloured = true) {
		const int stride = coloured ? MM_STRIDE : MM_PLAIN_STRIDE;
		const int n = int(buf.size() / stride);
		if (!make_room(scenario, mesh, material, n, shadows, layer, coloured)) return;
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		if (n > 0 && n == capacity) {
			rs->multimesh_set_buffer(mm, buf);
		} else if (n > 0) { // the buffer fills the whole room (the size the RenderingServer asks); the rest is not drawn
			godot::PackedFloat32Array room = buf;
			room.resize(int64_t(capacity) * stride);
			rs->multimesh_set_buffer(mm, room);
		}
		show(n, bounds);
	}
	// The same from rows kept between frames: the room is the array's own size, nothing is copied on this side.
	void set(const godot::RID &scenario, const godot::RID &mesh, const godot::RID &material, InstanceRows &rows, const godot::AABB &bounds,
			bool shadows, RenderLayer layer) {
		const int n = rows.count();
		if (!make_room(scenario, mesh, material, n, shadows, layer, rows.stride() == MM_STRIDE)) return;
		if (n > 0) godot::RenderingServer::get_singleton()->multimesh_set_buffer(mm, rows.fitted(capacity));
		show(n, bounds);
	}
	// Shows `next` (a MultiMesh of `count` instances a job filled, bake_multimesh, now owned here): swap_base.
	void adopt(const godot::RID &scenario, const godot::RID &material, bool shadows, const godot::RID &next, int count_in,
			const godot::AABB &bounds, RenderLayer layer) {
		swap_base(inst, mm, next, scenario, material, shadows, layer);
		capacity = count = mm.is_valid() ? count_in : 0;
		coloured_ = false; // bake_multimesh's layout
		if (inst.is_valid()) godot::RenderingServer::get_singleton()->instance_set_custom_aabb(inst, bounds);
	}
	void visible(bool on) const {
		if (inst.is_valid()) godot::RenderingServer::get_singleton()->instance_set_visible(inst, on && count > 0);
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
		inst = mm = material_ = godot::RID();
		capacity = -1;
		count = 0;
	}

private:
	godot::RID material_; // the override the instance has
	// False when there is nothing to draw and nothing drawn yet.
	bool make_room(const godot::RID &scenario, const godot::RID &mesh, const godot::RID &material, int n, bool shadows, RenderLayer layer, bool coloured) {
		if (n == 0 && !mm.is_valid()) return false;
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		if (!mm.is_valid()) {
			mm = rs->multimesh_create();
			inst = rs->instance_create2(mm, scenario);
			rs->instance_set_layer_mask(inst, layer);
			rs->instance_geometry_set_cast_shadows_setting(inst, shadow_only ? godot::RenderingServer::SHADOW_CASTING_SETTING_SHADOWS_ONLY :
					(shadows ? godot::RenderingServer::SHADOW_CASTING_SETTING_ON : godot::RenderingServer::SHADOW_CASTING_SETTING_OFF));
		}
		if (material != material_) {
			material_ = material;
			if (material.is_valid()) rs->instance_geometry_set_material_override(inst, material);
		}
		if (n > capacity || n + 2 * MM_ROOM_STEP < capacity || coloured != coloured_) {
			capacity = (n + MM_ROOM_STEP - 1) / MM_ROOM_STEP * MM_ROOM_STEP;
			coloured_ = coloured;
			rs->multimesh_allocate_data(mm, capacity, godot::RenderingServer::MULTIMESH_TRANSFORM_3D, coloured, true);
			rs->multimesh_set_mesh(mm, mesh);
		}
		return true;
	}
	void show(int n, const godot::AABB &bounds) {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		rs->multimesh_set_custom_aabb(mm, bounds);
		rs->multimesh_set_visible_instances(mm, n);
		count = n;
		rs->instance_set_custom_aabb(inst, bounds);
		rs->instance_set_visible(inst, n > 0);
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
