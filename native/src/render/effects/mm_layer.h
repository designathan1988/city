// One MultiMesh of a fixed capacity drawn by one instance, written whole when its CPU buffer changed (the effects write
// slots rarely: only the frames something is born or moves). Layout of rs_util.h (12 transform, 4 colour, 4 custom).
// The instance's box, what the engine culls it by for the camera and for each light's frustum, is one of three:
//   FIXED  the box given at init (the whole map). A see-through layer keeps it: the engine sorts see-through instances back to front
//          by the distance to their box's centre (render_forward_clustered.cpp), so the box is also its place in that order;
//   SHAPED the instances' own: each put() adds its instance's box (the shape's box through its transform, grown by `reach`, how
//          far the layer's shader may move a vertex past it); an owner that writes every instance again calls clear_bounds() first;
//   OWNED  what the owner sets (set_bounds): instances its shader places from elsewhere (a texture of bones).
#pragma once

#include "render/rs_util.h"

#include <godot_cpp/classes/mesh.hpp>

#include <algorithm>
#include <cmath>

namespace city {

constexpr float WORLD_HALF_M = 200000.f, WORLD_UP_M = 20000.f; // a FIXED layer's box: the whole map, whatever it holds

inline godot::AABB world_box() {
	return godot::AABB(godot::Vector3(-WORLD_HALF_M, -WORLD_UP_M, -WORLD_HALF_M), godot::Vector3(2 * WORLD_HALF_M, 2 * WORLD_UP_M, 2 * WORLD_HALF_M));
}

// The box of `local` through the transform of basis columns x, y, z and origin o.
inline godot::AABB moved_box(const godot::AABB &local, const godot::Vector3 &x, const godot::Vector3 &y, const godot::Vector3 &z, const godot::Vector3 &o) {
	const godot::Vector3 c = local.get_center(), h = local.size * 0.5f;
	const godot::Vector3 centre = o + x * c.x + y * c.y + z * c.z;
	const godot::Vector3 half(std::fabs(x.x) * h.x + std::fabs(y.x) * h.y + std::fabs(z.x) * h.z, std::fabs(x.y) * h.x + std::fabs(y.y) * h.y + std::fabs(z.y) * h.z,
			std::fabs(x.z) * h.x + std::fabs(y.z) * h.y + std::fabs(z.z) * h.z);
	return godot::AABB(centre - half, half * 2.f);
}

enum class LayerBounds { FIXED, SHAPED, OWNED };

struct MmLayer {
	godot::RID mm, inst;
	godot::Ref<godot::Mesh> mesh; // held: the MultiMesh draws its RID
	int capacity = 0;
	godot::PackedFloat32Array buf;
	LayerBounds bounds_kind = LayerBounds::FIXED;
	godot::AABB shape_box; // SHAPED: the shape's own box, as the shader may stretch it
	float reach = 0; // SHAPED: how far past that the shader may move a vertex, in metres of the world
	godot::AABB box, sent; // SHAPED and OWNED: the instances' box, and the one the instance has
	bool boxed = false;

	// A FIXED layer: `bounds` is the box of the MultiMesh (so uploads never walk the instances, rs_util.h MultiMeshDraw) and of the instance.
	void init(const godot::RID &scenario, const godot::Ref<godot::Mesh> &shape, const godot::RID &material, int count, bool shadows,
			const godot::AABB &bounds) {
		create(scenario, shape, material, count, shadows, bounds);
		bounds_kind = LayerBounds::FIXED;
	}
	// A SHAPED layer: its box follows what put() writes (`local`: the shape's box as the shader may stretch it; `margin`: what it may move past it).
	void init_shaped(const godot::RID &scenario, const godot::Ref<godot::Mesh> &shape, const godot::RID &material, int count, bool shadows,
			const godot::AABB &local, float margin) {
		create(scenario, shape, material, count, shadows, world_box());
		bounds_kind = LayerBounds::SHAPED;
		shape_box = local;
		reach = margin;
	}
	// An OWNED layer: its box is what the owner sets.
	void init_owned(const godot::RID &scenario, const godot::Ref<godot::Mesh> &shape, const godot::RID &material, int count, bool shadows) {
		create(scenario, shape, material, count, shadows, world_box());
		bounds_kind = LayerBounds::OWNED;
	}
	// Room for `count` instances: a layer holds what its owner draws, so it grows (doubling) when that passes it. The owner writes
	// every instance it draws again after (the server's copy is allocated anew).
	void reserve(int count) {
		if (count <= capacity) return;
		int grown = std::max(capacity, 1);
		while (grown < count) grown *= 2;
		capacity = grown;
		godot::RenderingServer::get_singleton()->multimesh_allocate_data(mm, grown, godot::RenderingServer::MULTIMESH_TRANSFORM_3D, true, true);
		buf.resize(int64_t(grown) * MM_STRIDE);
	}
	float *slot(int i) { return buf.ptrw() + int64_t(i) * MM_STRIDE; }
	// Instance `i`: basis columns x, y, z, origin, custom.
	void put(int i, const godot::Vector3 &x, const godot::Vector3 &y, const godot::Vector3 &z, const godot::Vector3 &o, float c0, float c1,
			float c2, float c3) {
		float *w = slot(i);
		const float v[MM_STRIDE] = { x.x, y.x, z.x, o.x, x.y, y.y, z.y, o.y, x.z, y.z, z.z, o.z, 1, 1, 1, 1, c0, c1, c2, c3 };
		for (int k = 0; k < MM_STRIDE; k++) w[k] = v[k];
		if (bounds_kind == LayerBounds::SHAPED) grow(moved_box(shape_box, x, y, z, o).grow(reach));
	}
	// Instance `i` laid from `a` to `b` (a unit shape along its y, `thick` across it on x and z): a gut, a bone, a wire's link.
	void put_link(int i, const godot::Vector3 &a, const godot::Vector3 &b, float thick, float c0, float c1, float c2, float c3) {
		constexpr float NOTHING_M = 1e-6f; // a length this short has no direction
		constexpr float NEAR_UPRIGHT = 0.9f; // a link this close to vertical (cosine) takes x, not up, to find its side: any axis well off it
		godot::Vector3 along = b - a;
		const float len = std::max(along.length(), NOTHING_M);
		along /= len;
		const godot::Vector3 helper = std::fabs(along.y) < NEAR_UPRIGHT ? godot::Vector3(0, 1, 0) : godot::Vector3(1, 0, 0);
		const godot::Vector3 side = helper.cross(along).normalized(), front = along.cross(side);
		put(i, side * thick, along * len, front * thick, (a + b) * 0.5f, c0, c1, c2, c3);
	}
	void clear_bounds() { boxed = false; }
	void grow(const godot::AABB &b) {
		box = boxed ? box.merge(b) : b;
		boxed = true;
	}
	void set_bounds(const godot::AABB &b) {
		box = b;
		boxed = true;
	}
	void count(int n) const { godot::RenderingServer::get_singleton()->multimesh_set_visible_instances(mm, n); }
	// The buffer to the server, and the instance's box when it changed (a FIXED layer's never does).
	void upload() {
		godot::RenderingServer::get_singleton()->multimesh_set_buffer(mm, buf);
		send_bounds();
	}
	void send_bounds() {
		if (bounds_kind == LayerBounds::FIXED || !boxed || box == sent) return;
		godot::RenderingServer::get_singleton()->instance_set_custom_aabb(inst, box);
		sent = box;
	}
	void shown(bool on) const { godot::RenderingServer::get_singleton()->instance_set_visible(inst, on); }
	void release() {
		godot::RenderingServer *rs = godot::RenderingServer::get_singleton();
		if (inst.is_valid()) rs->free_rid(inst);
		if (mm.is_valid()) rs->free_rid(mm);
		inst = godot::RID();
		mm = godot::RID();
		mesh.unref();
	}

private:
	void create(const godot::RID &scenario, const godot::Ref<godot::Mesh> &shape, const godot::RID &material, int count, bool shadows, const godot::AABB &bounds) {
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
		sent = bounds;
		boxed = false;
		buf.resize(int64_t(count) * MM_STRIDE);
		buf.fill(0.f);
		rs->multimesh_set_buffer(mm, buf);
	}
};

} // namespace city
