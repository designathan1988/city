// RenderingServer side of the building renderer: uploads the near/mid shells, the parts geometry and the five instanced
// part bands of a chunk, and the far/HLOD mesh of a superchunk. No CPU mesh assembly here (building_renderer_build.cpp).
#include "render/buildings/building_renderer.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

using namespace godot;

namespace city {

namespace {
constexpr float AABB_PAD = 1.f, AABB_MIN_Y = -100.f, AABB_H = 700.f, BYTE = 255.f;

void upload(MeshDraw &d, const b40::Shell &s, const RID &scenario, const RID &material, bool shadows, bool with_uv2, const AABB &fallback) {
	d.reset(scenario, material, shadows);
	RenderingServer *rs = RenderingServer::get_singleton();
	if (s.triangles() == 0) {
		rs->instance_set_custom_aabb(d.inst, fallback);
		d.visible(false);
		return;
	}
	s.commit_to(d.mesh, with_uv2);
	Vector3 mn, mx, pad(AABB_PAD, AABB_PAD, AABB_PAD);
	s.bounds(mn, mx);
	rs->instance_set_custom_aabb(d.inst, AABB(mn - pad, mx - mn + pad * 2));
	d.visible(true);
}

constexpr int RGBA_R_SHIFT = 24, RGBA_G_SHIFT = 16, RGBA_B_SHIFT = 8, BASIS = 3;

Color unpack(uint32_t c) {
	return Color(float((c >> RGBA_R_SHIFT) & 0xff) / BYTE, float((c >> RGBA_G_SHIFT) & 0xff) / BYTE, float((c >> RGBA_B_SHIFT) & 0xff) / BYTE, float(c & 0xff) / BYTE);
}
} // namespace

void BuildingRenderer::commit_one(Built &b) {
	Chunk &ch = chunks_[b.key];
	float x0 = float(key_x(b.key)) * CHUNK, z0 = float(key_y(b.key)) * CHUNK;
	AABB fallback(Vector3(x0, AABB_MIN_Y, z0), Vector3(CHUNK, AABB_H, CHUNK));
	if (b.has_near) {
		for (int i = 0; i < SUB_COUNT; i++) {
			upload(ch.near[size_t(i)], b.near[size_t(i)], scenario_, material_, true, true, fallback);
			upload(ch.parts[size_t(i)], b.parts[size_t(i)], scenario_, material_, false, true, fallback);
		}
		ch.built[LEVEL_NEAR] = true;
	}
	if (b.has_mid) {
		for (int i = 0; i < SUB_COUNT; i++) upload(ch.mid[size_t(i)], b.mid[size_t(i)], scenario_, material_, true, true, fallback);
		ch.built[LEVEL_MID] = true;
	}
	if (b.has_near) {
		for (int band = 0; band < BAND_COUNT; band++) {
			PackedFloat32Array buf;
			for (const b40::BoxInstance &inst : b.boxes[size_t(band)]) {
				Color c = unpack(inst.color);
				const float *bs = inst.basis;
				Vector3 x(bs[0], bs[1], bs[2]), y(bs[BASIS], bs[BASIS + 1], bs[BASIS + 2]);
				Vector3 z(bs[2 * BASIS], bs[2 * BASIS + 1], bs[2 * BASIS + 2]), o(inst.origin[0], inst.origin[1], inst.origin[2]);
				mm_push(buf, x, y, z, o, Color(1, 1, 1, 1), c.r, c.g, c.b, float(inst.fin));
			}
			ch.bands[size_t(band)].set(scenario_, unit_box_, material_parts_, buf, fallback, false);
		}
	}
	ranges(ch);
}

void BuildingRenderer::commit_super(SuperBuilt &b) {
	Super &sp = supers_[b.key];
	AABB fallback(Vector3(0, AABB_MIN_Y, 0), Vector3(SUPERCHUNK, AABB_H, SUPERCHUNK));
	upload(sp.far, b.far, scenario_, material_far_, false, false, fallback);
	ranges_super(sp);
}

} // namespace city
