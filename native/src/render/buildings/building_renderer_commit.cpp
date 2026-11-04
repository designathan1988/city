// RenderingServer side of the building renderer. On a worker: bake turns a shell into a mesh (the arrays are converted
// there, rs_util.h OwnedMesh) in the bake job the frame's upload budget admits (Built::bake, chunk_jobs.h), and
// bake_band packs the instanced parts. On the main thread: the meshes are handed to the chunk's and superchunk's
// instances, the chunk is shown or left to the far mesh, and the far mesh gets its mask. No CPU mesh assembly here
// (building_renderer_build.cpp).
#include "render/buildings/building_renderer.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

using namespace godot;

namespace city {

namespace {
constexpr float AABB_PAD = 1.f, AABB_MIN_Y = -100.f, AABB_H = 700.f, BYTE = 255.f;
constexpr int RGBA_R_SHIFT = 24, RGBA_G_SHIFT = 16, RGBA_B_SHIFT = 8, BASIS = 3;
const char *const FAR_MASK_PARAM = "chunk_mask"; // building_far.gdshader's instance uniform

// A chunk's culling box (every level and band of it).
AABB chunk_box(int64_t key) {
	return AABB(Vector3(float(key_x(key)) * CHUNK, AABB_MIN_Y, float(key_y(key)) * CHUNK), Vector3(CHUNK, AABB_H, CHUNK));
}

Color unpack(uint32_t c) {
	return Color(float((c >> RGBA_R_SHIFT) & 0xff) / BYTE, float((c >> RGBA_G_SHIFT) & 0xff) / BYTE, float((c >> RGBA_B_SHIFT) & 0xff) / BYTE, float(c & 0xff) / BYTE);
}
} // namespace

// The job's surface to the draw: the instance shows the new mesh (or is emptied when the shell was empty), drawn when
// `shown`.
void BuildingRenderer::put(MeshDraw &d, Surface &sf, const RID &scenario, const RID &material, bool shadows, bool shown) {
	const bool any = sf.mesh.valid();
	d.adopt(scenario, material, shadows, sf.mesh.release());
	if (any) RenderingServer::get_singleton()->instance_set_custom_aabb(d.inst, sf.box);
	d.visible(any && shown);
}

BuildingRenderer::Surface BuildingRenderer::bake(const b40::Shell &shell, bool with_uv2) {
	Surface sf;
	if (shell.triangles() == 0) return sf;
	sf.mesh.rid = RenderingServer::get_singleton()->mesh_create();
	shell.commit_to(sf.mesh.rid, with_uv2);
	Vector3 mn, mx, pad(AABB_PAD, AABB_PAD, AABB_PAD);
	shell.bounds(mn, mx);
	sf.box = AABB(mn - pad, mx - mn + pad * 2);
	return sf;
}

PackedFloat32Array BuildingRenderer::bake_band(const std::vector<b40::BoxInstance> &boxes) {
	PackedFloat32Array buf;
	for (const b40::BoxInstance &inst : boxes) {
		Color c = unpack(inst.color);
		const float *bs = inst.basis;
		Vector3 x(bs[0], bs[1], bs[2]), y(bs[BASIS], bs[BASIS + 1], bs[BASIS + 2]);
		Vector3 z(bs[2 * BASIS], bs[2 * BASIS + 1], bs[2 * BASIS + 2]), o(inst.origin[0], inst.origin[1], inst.origin[2]);
		mm_push(buf, x, y, z, o, Color(1, 1, 1, 1), c.r, c.g, c.b, float(inst.fin));
	}
	return buf;
}

int64_t BuildingRenderer::Built::bytes() const {
	int64_t n = 0;
	for (int i = 0; i < SUB_COUNT; i++)
		n += near_shell[size_t(i)].bytes(true) + parts_shell[size_t(i)].bytes(true) + mid_shell[size_t(i)].bytes(true);
	for (const PackedFloat32Array &buf : bands) n += buf.size() * int64_t(sizeof(float));
	return n;
}

void BuildingRenderer::Built::bake() {
	for (int i = 0; i < SUB_COUNT; i++) {
		if (has_near) {
			near[size_t(i)] = BuildingRenderer::bake(near_shell[size_t(i)], true);
			parts[size_t(i)] = BuildingRenderer::bake(parts_shell[size_t(i)], true);
		}
		if (has_mid) mid[size_t(i)] = BuildingRenderer::bake(mid_shell[size_t(i)], true);
		// each moved from a new shell: a copy of an empty one keeps the arrays' memory, then freed on the main thread
		near_shell[size_t(i)] = b40::Shell();
		parts_shell[size_t(i)] = b40::Shell();
		mid_shell[size_t(i)] = b40::Shell();
	}
	for (int b = 0; b < BAND_COUNT; b++) {
		band_count[size_t(b)] = int(bands[size_t(b)].size() / MM_STRIDE);
		band_mm[size_t(b)] = bake_multimesh(unit_box, bands[size_t(b)], chunk_box(key));
		bands[size_t(b)] = PackedFloat32Array();
	}
	baked = true;
}

void BuildingRenderer::SuperBuilt::bake() {
	far = BuildingRenderer::bake(far_shell, false);
	far_shell = b40::Shell();
	baked = true;
}

void BuildingRenderer::commit_one(Built &b) {
	auto found = chunks_.find(b.key);
	if (found == chunks_.end()) return; // dropped by the stream meanwhile (drop_chunk also forgets the request)
	Chunk &ch = found->second;
	const bool current = b.version == ch.version; // else the chunk changed after the job read it: stays stale
	if (b.has_near) {
		for (int i = 0; i < SUB_COUNT; i++) {
			put(ch.near[size_t(i)], b.near[size_t(i)], scenario_, material_, true, ch.shown);
			put(ch.parts[size_t(i)], b.parts[size_t(i)], scenario_, material_, false, ch.shown);
		}
		for (int band = 0; band < BAND_COUNT; band++)
			ch.bands[size_t(band)].adopt(scenario_, material_parts_, false, b.band_mm[size_t(band)].release(), b.band_count[size_t(band)],
					chunk_box(b.key));
		ch.built[LEVEL_NEAR] = true;
		ch.stale[LEVEL_NEAR] = !current;
	}
	if (b.has_mid) {
		for (int i = 0; i < SUB_COUNT; i++) put(ch.mid[size_t(i)], b.mid[size_t(i)], scenario_, material_, true, ch.shown);
		ch.built[LEVEL_MID] = true;
		ch.stale[LEVEL_MID] = !current;
	}
	ranges(ch);
	show(ch, ch.shown); // the bands' MultiMeshes come up visible: the chunk's state decides
}

void BuildingRenderer::commit_super(SuperBuilt &b) {
	Super &sp = supers_[b.key];
	sp.has_content = b.far.mesh.valid();
	put(sp.far, b.far, scenario_, material_far_, true, true); // casts: past the mid level it is the only thing that can
	sp.built = true;
	sp.mask_sent = -1; // the mask update_view computed this frame goes to the (maybe new) instance at once
	send_mask(sp);
}

// Every instance of the chunk on or off (one with no geometry stays off).
void BuildingRenderer::show(Chunk &ch, bool on) {
	ch.shown = on;
	for (int i = 0; i < SUB_COUNT; i++) {
		for (MeshDraw *d : { &ch.near[size_t(i)], &ch.parts[size_t(i)], &ch.mid[size_t(i)] }) d->visible(on && d->mesh.is_valid());
	}
	for (const MultiMeshDraw &d : ch.bands) d.visible(on);
}

void BuildingRenderer::send_mask(Super &sp) {
	if (!sp.far.inst.is_valid() || sp.mask == sp.mask_sent) return;
	RenderingServer::get_singleton()->instance_geometry_set_shader_parameter(sp.far.inst, FAR_MASK_PARAM, sp.mask);
	sp.mask_sent = sp.mask;
}

} // namespace city
