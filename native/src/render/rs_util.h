// RenderingServer helpers: instances drawn without scene nodes (lighter for thousands of chunks).
#pragma once

#include "render/instance_rows.h"
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
// A range holds this share of itself either way of its edge (Godot: margins with the fade disabled are a hysteresis): a camera standing at the edge does not flicker.
constexpr float RANGE_LINGER_SHARE = 0.03f;

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
			godot::RenderingServer::get_singleton()->instance_geometry_set_visibility_range(inst, 0, end, 0, end * RANGE_LINGER_SHARE,
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
		forget_told(); // another multimesh, and maybe another instance: nothing was told to them
		if (inst.is_valid()) godot::RenderingServer::get_singleton()->instance_set_custom_aabb(inst, bounds);
	}
	void visible(bool on) const {
		if (inst.is_valid()) tell_drawn(on && count > 0);
	}
	void range(float begin, float end) const {
		if (inst.is_valid())
			godot::RenderingServer::get_singleton()->instance_geometry_set_visibility_range(inst, begin, end, begin * RANGE_LINGER_SHARE, end * RANGE_LINGER_SHARE,
					godot::RenderingServer::VISIBILITY_RANGE_FADE_DISABLED);
	}
	void free() {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		if (inst.is_valid()) rs->free_rid(inst);
		if (mm.is_valid()) rs->free_rid(mm);
		inst = mm = material_ = godot::RID();
		capacity = -1;
		count = 0;
		forget_told();
	}

private:
	godot::RID material_; // the override the instance has
	void forget_told() {
		told_ = UNTOLD;
		box_told_ = false;
		drawn_ = true;
	}
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
			told_ = UNTOLD; // a new allocation: its count and box are told again
			box_told_ = false;
		}
		return true;
	}
	// The count, the box and whether it is drawn, each told to the engine when it differs from what it was told last: a draw set
	// every frame costs its buffer alone, and one that stays empty nothing.
	void show(int n, const godot::AABB &bounds) {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		if (!box_told_ || bounds != box_) {
			rs->multimesh_set_custom_aabb(mm, bounds);
			rs->instance_set_custom_aabb(inst, bounds);
			box_ = bounds;
			box_told_ = true;
		}
		if (n != told_) {
			rs->multimesh_set_visible_instances(mm, n);
			told_ = n;
		}
		count = n;
		tell_drawn(n > 0);
	}
	void tell_drawn(bool on) const {
		if (on == drawn_) return;
		godot::RenderingServer::get_singleton()->instance_set_visible(inst, on);
		drawn_ = on;
	}
	godot::AABB box_; // as the engine was told last (show)
	bool box_told_ = false;
	int told_ = UNTOLD; // the visible instances as told last
	mutable bool drawn_ = true; // an instance is made drawn
	static constexpr int UNTOLD = -2; // no count the engine takes (-1 is all of them)
};

} // namespace city

#include "render/mesh_draw.h" // what stands on the above: a mesh a job made, and drawing one
