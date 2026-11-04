// RenderingServer side of the building renderer. On a worker: bake turns a shell into a mesh (the arrays are converted
// there, rs_util.h OwnedMesh) in the bake job the frame's upload budget admits (Built::bake, chunk_jobs.h), and
// bake_band packs the instanced parts. On the main thread: the meshes are handed to the chunk's and superchunk's
// instances, the chunk is shown or left to the far mesh, and the far mesh gets its mask. No CPU mesh assembly here
// (building_renderer_build.cpp).
#include "render/buildings/building_renderer.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

#include <algorithm>

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

// A chunk's level: its walls, panes or far walls, glass guards and railings each with their material, on the mesh's surfaces, so no
// override on the instance.
void BuildingRenderer::put_level(MeshDraw &d, Surface &sf, const RID &glass, bool shown) const {
	if (sf.mesh.valid()) {
		RenderingServer *rs = RenderingServer::get_singleton();
		if (sf.glass_at != 0 && sf.sheer_at != 0 && sf.card_at != 0) rs->mesh_surface_set_material(sf.mesh.rid, 0, material_);
		if (sf.glass_at >= 0) rs->mesh_surface_set_material(sf.mesh.rid, sf.glass_at, glass);
		if (sf.sheer_at >= 0) rs->mesh_surface_set_material(sf.mesh.rid, sf.sheer_at, material_sheer_);
		if (sf.card_at >= 0) rs->mesh_surface_set_material(sf.mesh.rid, sf.card_at, material_card_);
	}
	put(d, sf, scenario_, RID(), true, shown);
}

BuildingRenderer::Surface BuildingRenderer::bake(const b40::Shell &shell, bool with_uv2, const b40::Shell *glass, const b40::Shell *sheer, const b40::Shell *cards) {
	Surface sf;
	const bool any_glass = glass && glass->triangles() > 0, any_sheer = sheer && sheer->triangles() > 0, any_cards = cards && cards->triangles() > 0;
	if (shell.triangles() == 0 && !any_glass && !any_sheer && !any_cards) return sf;
	sf.mesh.rid = RenderingServer::get_singleton()->mesh_create();
	shell.commit_to(sf.mesh.rid, with_uv2);
	if (any_glass) {
		sf.glass_at = shell.triangles() > 0 ? 1 : 0;
		glass->commit_to(sf.mesh.rid, with_uv2);
	}
	if (any_sheer) {
		sf.sheer_at = (shell.triangles() > 0 ? 1 : 0) + (any_glass ? 1 : 0);
		sheer->commit_to(sf.mesh.rid, with_uv2);
	}
	if (any_cards) {
		sf.card_at = (shell.triangles() > 0 ? 1 : 0) + (any_glass ? 1 : 0) + (any_sheer ? 1 : 0);
		cards->commit_to(sf.mesh.rid, with_uv2);
	}
	Vector3 mn, mx, pad(AABB_PAD, AABB_PAD, AABB_PAD);
	shell.bounds(mn, mx);
	for (const b40::Shell *more : { any_glass ? glass : nullptr, any_sheer ? sheer : nullptr, any_cards ? cards : nullptr }) {
		if (!more) continue;
		Vector3 gn, gx;
		more->bounds(gn, gx);
		mn = mn.min(gn);
		mx = mx.max(gx);
	}
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
		mm_push_plain(buf, x, y, z, o, c.r, c.g, c.b, float(inst.fin));
	}
	return buf;
}

int64_t BuildingRenderer::Built::bytes() const {
	int64_t n = 0;
	for (int i = 0; i < SUB_COUNT; i++)
		n += near_shell[size_t(i)].bytes(true) + parts_shell[size_t(i)].bytes(true) + mid_shell[size_t(i)].bytes(true) +
				near_glass[size_t(i)].bytes(true) + mid_glass[size_t(i)].bytes(true) + near_sheer[size_t(i)].bytes(true) +
				near_cards[size_t(i)].bytes(true) + mid_cards[size_t(i)].bytes(true);
	for (const PackedFloat32Array &buf : bands) n += buf.size() * int64_t(sizeof(float));
	return n;
}

void BuildingRenderer::Built::bake() {
	for (int i = 0; i < SUB_COUNT; i++) {
		if (has_near) {
			held[HELD_NEAR_WALLS].add(near_shell[size_t(i)], true);
			held[HELD_NEAR_PANES].add(near_glass[size_t(i)], true);
			held[HELD_NEAR_GUARDS].add(near_sheer[size_t(i)], true);
			held[HELD_NEAR_CARDS].add(near_cards[size_t(i)], true);
			held[HELD_NEAR_PIECES].add(parts_shell[size_t(i)], true);
			near[size_t(i)] = BuildingRenderer::bake(near_shell[size_t(i)], true, &near_glass[size_t(i)], &near_sheer[size_t(i)], &near_cards[size_t(i)]);
			parts[size_t(i)] = BuildingRenderer::bake(parts_shell[size_t(i)], true);
		}
		if (has_mid) {
			held[HELD_MID_WALLS].add(mid_shell[size_t(i)], true);
			held[HELD_MID_PANES].add(mid_glass[size_t(i)], true);
			held[HELD_MID_CARDS].add(mid_cards[size_t(i)], true);
			mid[size_t(i)] = BuildingRenderer::bake(mid_shell[size_t(i)], true, &mid_glass[size_t(i)], nullptr, &mid_cards[size_t(i)]);
		}
		// each moved from a new shell: a copy of an empty one keeps the arrays' memory, then freed on the main thread
		near_shell[size_t(i)] = b40::Shell();
		parts_shell[size_t(i)] = b40::Shell();
		mid_shell[size_t(i)] = b40::Shell();
		near_glass[size_t(i)] = b40::Shell();
		near_sheer[size_t(i)] = b40::Shell();
		near_cards[size_t(i)] = b40::Shell();
		mid_cards[size_t(i)] = b40::Shell();
		mid_glass[size_t(i)] = b40::Shell();
	}
	for (int b = 0; b < BAND_COUNT; b++) {
		band_count[size_t(b)] = int(bands[size_t(b)].size() / MM_PLAIN_STRIDE);
		band_mm[size_t(b)] = bake_multimesh(unit_box, bands[size_t(b)], chunk_box(key));
		bands[size_t(b)] = PackedFloat32Array();
	}
	baked = true;
}

void BuildingRenderer::SuperBuilt::bake() {
	held.add(far_shell, false);
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
			put_level(ch.near[size_t(i)], b.near[size_t(i)], material_glass_, ch.shown);
			put(ch.parts[size_t(i)], b.parts[size_t(i)], scenario_, material_, false, ch.shown);
		}
		for (int band = 0; band < BAND_COUNT; band++)
			ch.bands[size_t(band)].adopt(scenario_, material_parts_, false, b.band_mm[size_t(band)].release(), b.band_count[size_t(band)],
					chunk_box(b.key));
		std::copy(b.held.begin(), b.held.begin() + HELD_MID_WALLS, ch.held.begin());
		ch.instances = 0;
		for (int count : b.band_count) ch.instances += count;
		ch.built[LEVEL_NEAR] = true;
		ch.stale[LEVEL_NEAR] = !current;
	}
	if (b.has_mid) {
		for (int i = 0; i < SUB_COUNT; i++) put_level(ch.mid[size_t(i)], b.mid[size_t(i)], material_, ch.shown);
		std::copy(b.held.begin() + HELD_MID_WALLS, b.held.end(), ch.held.begin() + HELD_MID_WALLS);
		ch.built[LEVEL_MID] = true;
		ch.stale[LEVEL_MID] = !current;
	}
	ranges(ch);
	show(ch, ch.shown); // the bands' MultiMeshes come up visible: the chunk's state decides
}

void BuildingRenderer::commit_super(SuperBuilt &b) {
	Super &sp = supers_[b.key];
	sp.has_content = b.far.mesh.valid();
	sp.held = b.held;
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
