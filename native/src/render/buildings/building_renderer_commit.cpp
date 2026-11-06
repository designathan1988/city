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
constexpr float AABB_PAD = 1.f, BYTE = 255.f;
constexpr int RGBA_R_SHIFT = 24, RGBA_G_SHIFT = 16, RGBA_B_SHIFT = 8, BASIS = 3;
const char *const FAR_MASK_PARAM = "chunk_mask"; // building_far.gdshader's instance uniform

// An instance whose basis is three columns in a row.
void push_row(PackedFloat32Array &buf, const float *bs, const float *origin, float c0, float c1, float c2, float c3) {
	const Vector3 x(bs[0], bs[1], bs[2]), y(bs[BASIS], bs[BASIS + 1], bs[BASIS + 2]), z(bs[2 * BASIS], bs[2 * BASIS + 1], bs[2 * BASIS + 2]);
	mm_push_plain(buf, x, y, z, Vector3(origin[0], origin[1], origin[2]), c0, c1, c2, c3);
}

void grow(AABB &box, const AABB &more) {
	if (more.size == Vector3()) return;
	box = box.size == Vector3() ? more : box.merge(more);
}

Color unpack(uint32_t c) {
	return Color(float((c >> RGBA_R_SHIFT) & 0xff) / BYTE, float((c >> RGBA_G_SHIFT) & 0xff) / BYTE, float((c >> RGBA_B_SHIFT) & 0xff) / BYTE, float(c & 0xff) / BYTE);
}
} // namespace

// The job's surface to the draw: the instance shows the new mesh (or is emptied when the shell was empty), drawn when
// `shown`.
void BuildingRenderer::put(MeshDraw &d, Surface &sf, const RID &scenario, const RID &material, bool shadows, bool shown, RenderLayer layer) {
	const bool any = sf.mesh.valid();
	d.adopt(scenario, material, shadows, sf.mesh.release(), layer);
	if (any) RenderingServer::get_singleton()->instance_set_custom_aabb(d.inst, sf.box);
	d.visible(any && shown);
}

// A chunk's level: its walls, panes or far walls, glass guards and railings each with their material, on the mesh's surfaces, so no
// override on the instance.
void BuildingRenderer::put_level(MeshDraw &d, Surface &sf, const RID &glass, bool shown, RenderLayer layer, bool shadows) const {
	if (sf.mesh.valid()) {
		RenderingServer *rs = RenderingServer::get_singleton();
		if (sf.glass_at != 0 && sf.sheer_at != 0 && sf.card_at != 0 && sf.lawn_at != 0 && sf.wash_at != 0) rs->mesh_surface_set_material(sf.mesh.rid, 0, material_);
		if (sf.glass_at >= 0) rs->mesh_surface_set_material(sf.mesh.rid, sf.glass_at, glass);
		if (sf.sheer_at >= 0) rs->mesh_surface_set_material(sf.mesh.rid, sf.sheer_at, material_sheer_);
		if (sf.card_at >= 0) rs->mesh_surface_set_material(sf.mesh.rid, sf.card_at, material_card_);
		if (sf.lawn_at >= 0) rs->mesh_surface_set_material(sf.mesh.rid, sf.lawn_at, material_lawn_);
		if (sf.wash_at >= 0) rs->mesh_surface_set_material(sf.mesh.rid, sf.wash_at, material_wash_);
	}
	put(d, sf, scenario_, RID(), shadows, shown, layer);
}

BuildingRenderer::Surface BuildingRenderer::bake(const b40::Shell &shell, bool with_uv2, const b40::Shell *glass, const b40::Shell *sheer, const b40::Shell *cards, const b40::Shell *lawns, const b40::Shell *washes) {
	Surface sf;
	const bool any_glass = glass && glass->triangles() > 0, any_sheer = sheer && sheer->triangles() > 0, any_cards = cards && cards->triangles() > 0;
	const bool any_lawns = lawns && lawns->triangles() > 0, any_washes = washes && washes->triangles() > 0;
	if (shell.triangles() == 0 && !any_glass && !any_sheer && !any_cards && !any_lawns && !any_washes) return sf;
	sf.mesh.rid = RenderingServer::get_singleton()->mesh_create();
	shell.commit_to(sf.mesh.rid, with_uv2);
	if (any_glass) {
		sf.glass_at = shell.triangles() > 0 ? 1 : 0;
		glass->commit_to(sf.mesh.rid, with_uv2);
	}
	if (any_sheer) {
		sf.sheer_at = (shell.triangles() > 0 ? 1 : 0) + (any_glass ? 1 : 0);
		sheer->commit_to(sf.mesh.rid, with_uv2, true);
	}
	if (any_cards) {
		sf.card_at = (shell.triangles() > 0 ? 1 : 0) + (any_glass ? 1 : 0) + (any_sheer ? 1 : 0);
		cards->commit_to(sf.mesh.rid, with_uv2);
	}
	if (any_lawns) {
		sf.lawn_at = (shell.triangles() > 0 ? 1 : 0) + (any_glass ? 1 : 0) + (any_sheer ? 1 : 0) + (any_cards ? 1 : 0);
		lawns->commit_to(sf.mesh.rid, with_uv2);
	}
	if (any_washes) {
		sf.wash_at = (shell.triangles() > 0 ? 1 : 0) + (any_glass ? 1 : 0) + (any_sheer ? 1 : 0) + (any_cards ? 1 : 0) + (any_lawns ? 1 : 0);
		washes->commit_to(sf.mesh.rid, with_uv2, true);
	}
	Vector3 mn, mx, pad(AABB_PAD, AABB_PAD, AABB_PAD);
	shell.bounds(mn, mx);
	for (const b40::Shell *more : { any_glass ? glass : nullptr, any_sheer ? sheer : nullptr, any_cards ? cards : nullptr, any_lawns ? lawns : nullptr, any_washes ? washes : nullptr }) {
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
		const Color c = unpack(inst.color);
		push_row(buf, inst.basis, inst.origin, c.r, c.g, c.b, float(inst.fin));
	}
	return buf;
}

PackedFloat32Array BuildingRenderer::bake_bays(const std::vector<b40::BayInstance> &bays) {
	PackedFloat32Array buf;
	for (const b40::BayInstance &inst : bays) push_row(buf, inst.basis, inst.origin, inst.shares, inst.wall, inst.trim, inst.code);
	return buf;
}

PackedFloat32Array BuildingRenderer::bake_props(const std::vector<b40::PropInstance> &props) {
	PackedFloat32Array buf;
	for (const b40::PropInstance &inst : props) {
		const Color c = unpack(inst.color);
		push_row(buf, inst.basis, inst.origin, c.r, c.g, c.b, 0);
	}
	return buf;
}

int64_t BuildingRenderer::Built::bytes() const {
	int64_t n = 0;
	for (int i = 0; i < SUB_COUNT; i++)
		n += near_washes[size_t(i)].bytes(true) + near_lawns[size_t(i)].bytes(true) + mid_lawns[size_t(i)].bytes(true) + flat_shell[size_t(i)].bytes(true) + flat_glass[size_t(i)].bytes(true) + near_shell[size_t(i)].bytes(true) + parts_shell[size_t(i)].bytes(true) + mid_shell[size_t(i)].bytes(true) +
				near_glass[size_t(i)].bytes(true) + mid_glass[size_t(i)].bytes(true) + near_sheer[size_t(i)].bytes(true) +
				near_cards[size_t(i)].bytes(true) + mid_cards[size_t(i)].bytes(true);
	for (const auto &sub : bands)
		for (const PackedFloat32Array &buf : sub) n += buf.size() * int64_t(sizeof(float));
	for (const auto &sub : mid_buf)
		for (const PackedFloat32Array &buf : sub) n += buf.size() * int64_t(sizeof(float));
	n += flags.size() * int64_t(sizeof(float));
	return n;
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
			put_level(ch.near[size_t(i)], b.near[size_t(i)], material_glass_, ch.shown, LAYER_BUILDING_SHELLS);
			put(ch.parts[size_t(i)], b.parts[size_t(i)], scenario_, material_, false, ch.shown, LAYER_BUILDING_PARTS);
		}
		for (size_t i = 0; i < SUB_COUNT; i++)
			for (size_t band = 0; band < BAND_DRAWS; band++) {
				const bool prop = band >= BAND_COUNT;
				ch.bands[i][band].adopt(scenario_, prop ? material_props_ : material_parts_, false, b.band_mm[i][band].release(), b.band_count[i][band], b.band_box[i][band],
						LAYER_BUILDING_DETAILS);
				if (const int cast = cast_of(int(band)); cast >= 0)
					ch.prop_casts[i][size_t(cast)].bind(scenario_, ch.bands[i][band].mm, material_cast_, b.band_box[i][band], LAYER_BUILDING_DETAILS);
			}
		// a cloth throws a shadow; its own layer, as the 4.0's flag layer has its own switch (effects panel bFlags)
		ch.flags.adopt(scenario_, material_parts_, true, b.flag_mm.release(), b.flag_count, b.flag_box, LAYER_BUILDING_FLAGS);
		ch.occluder.set(scenario_, b.walls);
		std::copy(b.held.begin(), b.held.begin() + HELD_MID_WALLS, ch.held.begin());
		ch.instances = b.flag_count;
		ch.props = 0;
		for (size_t i = 0; i < SUB_COUNT; i++)
			for (size_t band = 0; band < BAND_DRAWS; band++) (band < BAND_COUNT ? ch.instances : ch.props) += b.band_count[i][band];
		ch.built[LEVEL_NEAR] = true;
		ch.stale[LEVEL_NEAR] = !current;
		ch.cut = b.cut;
	}
	if (b.has_mid) {
		for (int i = 0; i < SUB_COUNT; i++) {
			put_level(ch.mid[size_t(i)], b.mid[size_t(i)], material_, ch.shown, LAYER_BUILDING_FACADES);
			put_level(ch.flat[size_t(i)], b.flat[size_t(i)], material_, ch.shown, LAYER_BUILDING_FACADES, false); // the plain side behind throws the shadow
		}
		for (size_t i = 0; i < SUB_COUNT; i++)
			for (size_t k = 0; k < MID_DRAWS; k++) // no shadow of their own: the plain side behind the bays throws it
				ch.mid_draws[i][k].adopt(scenario_, k == MID_BAYS ? material_ : material_parts_, false, b.mid_mm[i][k].release(), b.mid_count[i][k],
						b.mid_box[i][k], LAYER_BUILDING_FACADES);
		for (size_t i = 0; i < SUB_COUNT; i++)
			ch.mid_casts[i].bind(scenario_, ch.mid_draws[i][MID_SOLIDS].mm, material_cast_, b.mid_box[i][MID_SOLIDS], LAYER_BUILDING_FACADES);
		std::copy(b.held.begin() + HELD_MID_WALLS, b.held.end(), ch.held.begin() + HELD_MID_WALLS);
		ch.built[LEVEL_MID] = true;
		ch.stale[LEVEL_MID] = !current;
	}
	fit_reach(ch, b);
	ranges(ch);
	show(ch, ch.shown); // the bands' MultiMeshes come up visible: the chunk's state decides
}

// Every level of each sub-chunk on one box, that grows with what it holds.
void BuildingRenderer::fit_reach(Chunk &ch, const Built &b) {
	RenderingServer *rs = RenderingServer::get_singleton();
	for (size_t i = 0; i < SUB_COUNT; i++) {
		AABB &box = ch.reach_box[i];
		if (b.has_near) {
			grow(box, b.near[i].box);
			grow(box, b.parts[i].box);
			for (const AABB &drawn : b.band_box[i]) grow(box, drawn);
		}
		if (b.has_mid) {
			grow(box, b.mid[i].box);
			grow(box, b.flat[i].box);
			for (const AABB &drawn : b.mid_box[i]) grow(box, drawn);
		}
		if (box.size == Vector3()) continue;
		for (const MeshDraw *d : { &ch.near[i], &ch.parts[i], &ch.mid[i], &ch.flat[i] })
			if (d->inst.is_valid()) rs->instance_set_custom_aabb(d->inst, box);
		for (const MultiMeshDraw &d : ch.mid_draws[i])
			if (d.inst.is_valid()) rs->instance_set_custom_aabb(d.inst, box);
		for (const MultiMeshDraw &d : ch.bands[i])
			if (d.inst.is_valid()) rs->instance_set_custom_aabb(d.inst, box);
		for (const ShadowTwin &d : ch.prop_casts[i])
			if (d.inst.is_valid()) rs->instance_set_custom_aabb(d.inst, box);
		if (ch.mid_casts[i].inst.is_valid()) rs->instance_set_custom_aabb(ch.mid_casts[i].inst, box);
	}
}

void BuildingRenderer::commit_super(SuperBuilt &b) {
	Super &sp = supers_[b.key];
	sp.has_content = b.far.mesh.valid();
	sp.held = b.held;
	put(sp.far, b.far, scenario_, material_far_, true, true, LAYER_BUILDING_MASSES); // casts: past the mid level it is the only thing that can
	sp.built = true;
	sp.mask_sent = -1; // the mask update_view computed this frame goes to the (maybe new) instance at once
	send_mask(sp);
}

// A chunk whose near level is drawn again with the same buildings (see building_renderer.h).
void BuildingRenderer::restyle_chunk(int64_t key) {
	auto found = chunks_.find(key);
	if (found == chunks_.end()) return; // not streamed: nothing drawn to change
	Chunk &ch = found->second;
	ch.version++;
	ch.stale[LEVEL_NEAR] = ch.built[LEVEL_NEAR];
}

// Every instance of the chunk on or off (one with no geometry stays off).
void BuildingRenderer::show(Chunk &ch, bool on) {
	ch.shown = on;
	for (int i = 0; i < SUB_COUNT; i++) {
		for (MeshDraw *d : { &ch.near[size_t(i)], &ch.parts[size_t(i)], &ch.mid[size_t(i)], &ch.flat[size_t(i)] }) d->visible(on && d->mesh.is_valid());
	}
	for (size_t i = 0; i < SUB_COUNT; i++) {
		for (const MultiMeshDraw &d : ch.bands[i]) d.visible(on);
		for (int band = 0; band < BAND_DRAWS; band++)
			if (const int cast = cast_of(band); cast >= 0) ch.prop_casts[i][size_t(cast)].visible(on && ch.bands[i][size_t(band)].count > 0);
	}
	for (const auto &sub : ch.mid_draws)
		for (const MultiMeshDraw &d : sub) d.visible(on);
	for (size_t i = 0; i < SUB_COUNT; i++) ch.mid_casts[i].visible(on && ch.mid_draws[i][MID_SOLIDS].count > 0);
	ch.flags.visible(on);
}

void BuildingRenderer::send_mask(Super &sp) {
	if (!sp.far.inst.is_valid() || sp.mask == sp.mask_sent) return;
	RenderingServer::get_singleton()->instance_geometry_set_shader_parameter(sp.far.inst, FAR_MASK_PARAM, sp.mask);
	sp.mask_sent = sp.mask;
}

} // namespace city
