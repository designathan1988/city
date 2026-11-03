// RenderingServer side of the building renderer: uploads the near/mid shells, the parts geometry and the five instanced
// part bands of a chunk, and the far/HLOD mesh of a superchunk; the visibility links between them and their frees.
// No CPU mesh assembly here (building_renderer_build.cpp).
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
	auto found = chunks_.find(b.key);
	if (found == chunks_.end()) return; // dropped by the stream meanwhile (drop_chunk also forgets the request)
	Chunk &ch = found->second;
	const bool current = b.version == ch.version; // else the chunk changed after the job read it: stays stale
	float x0 = float(key_x(b.key)) * CHUNK, z0 = float(key_y(b.key)) * CHUNK;
	AABB fallback(Vector3(x0, AABB_MIN_Y, z0), Vector3(CHUNK, AABB_H, CHUNK));
	if (b.has_near) {
		for (int i = 0; i < SUB_COUNT; i++) {
			upload(ch.near[size_t(i)], b.near[size_t(i)], scenario_, material_, true, true, fallback);
			upload(ch.parts[size_t(i)], b.parts[size_t(i)], scenario_, material_, false, true, fallback);
		}
		ch.built[LEVEL_NEAR] = true;
		ch.stale[LEVEL_NEAR] = !current;
	}
	if (b.has_mid) {
		for (int i = 0; i < SUB_COUNT; i++) upload(ch.mid[size_t(i)], b.mid[size_t(i)], scenario_, material_, true, true, fallback);
		ch.built[LEVEL_MID] = true;
		ch.stale[LEVEL_MID] = !current;
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
			ch.bands[size_t(band)].set(scenario_, unit_box_->get_rid(), material_parts_, buf, fallback, false);
		}
	}
	ranges(ch);
	link(ch, far_parent(ch)); // instances made by this commit join the superchunk's far instance
}

void BuildingRenderer::commit_super(SuperBuilt &b) {
	Super &sp = supers_[b.key];
	AABB fallback(Vector3(float(key_x(b.key)) * SUPERCHUNK, AABB_MIN_Y, float(key_y(b.key)) * SUPERCHUNK),
			Vector3(SUPERCHUNK, AABB_H, SUPERCHUNK));
	upload(sp.far, b.far, scenario_, material_far_, false, false, fallback);
	sp.built = true;
	sp.has_content = b.far.triangles() > 0;
	sp.shown = sp.has_content; // what upload left
	link_super(b.key, sp.far.inst);
	show_far(sp); // the state update_view decided this frame, not the upload's
}

void BuildingRenderer::link(Chunk &ch, const RID &parent) const {
	RenderingServer *rs = RenderingServer::get_singleton();
	auto set = [&](const RID &inst) {
		if (inst.is_valid()) rs->instance_set_visibility_parent(inst, parent);
	};
	for (int i = 0; i < SUB_COUNT; i++) {
		set(ch.near[size_t(i)].inst);
		set(ch.parts[size_t(i)].inst);
		set(ch.mid[size_t(i)].inst);
	}
	for (const MultiMeshDraw &d : ch.bands) set(d.inst);
}

// Drawn while far, or while the chunks it covered are not all remeshed (never a hole); never while it holds nothing.
void BuildingRenderer::show_far(Super &sp) {
	const bool on = sp.has_content && (sp.far_state || sp.covering);
	if (on == sp.shown) return;
	sp.far.visible(on);
	sp.shown = on;
}

void BuildingRenderer::release(MeshDraw &d) {
	if (d.inst.is_valid()) RenderingServer::get_singleton()->instance_set_visibility_parent(d.inst, RID());
	d.free();
}

void BuildingRenderer::release(MultiMeshDraw &d) {
	if (d.inst.is_valid()) RenderingServer::get_singleton()->instance_set_visibility_parent(d.inst, RID());
	d.free();
}

} // namespace city
