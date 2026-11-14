// RenderingServer side of the building renderer. On a worker: bake turns a shell into a mesh (the arrays are converted
// there, rs_util.h OwnedMesh) in the bake job the frame's upload budget admits (Built::bake, chunk_jobs.h), and
// bake_band packs the instanced parts. On the main thread: the meshes are handed to the chunk's and superchunk's
// instances, the chunk is shown or left to the far mesh, and the far mesh gets its mask. No CPU mesh assembly here
// (building_renderer_build.cpp).
#include "render/buildings/building_renderer.h"

#include "render/surface_data.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

#include <algorithm>

using namespace godot;

namespace city {

namespace {
constexpr float AABB_PAD = 1.f, BYTE = 255.f;
constexpr uint32_t BYTE_MASK = 0xff;
constexpr float BAND_CODE_STEP = 16.f; // building_parts.gdshader BAND_STEP: over every finish layer + 1
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
	return Color(float((c >> RGBA_R_SHIFT) & BYTE_MASK) / BYTE, float((c >> RGBA_G_SHIFT) & BYTE_MASK) / BYTE, float((c >> RGBA_B_SHIFT) & BYTE_MASK) / BYTE,
			float(c & BYTE_MASK) / BYTE);
}
} // namespace

// The job's surface to the draw: the instance shows the new mesh (or is emptied when the shell was empty), drawn when
// `shown`.
void BuildingRenderer::put(MeshDraw &d, Surface &sf, const RID &scenario, bool shadows, bool shown, RenderLayer layer) {
	const bool any = sf.mesh.valid();
	d.adopt(scenario, shadows, sf.mesh.release(), layer);
	if (any) RenderingServer::get_singleton()->instance_set_custom_aabb(d.inst, sf.box);
	d.visible(any && shown);
}

// A level's mesh made whole (surface_data.h): its walls, panes or far walls, glass guards, railings, lawns and lantern light each a surface
// with its material, so the engine makes their pipelines with the mesh and no drawn instance carries an override.
BuildingRenderer::Surface BuildingRenderer::bake(const b40::Shell &shell, bool with_uv2, const LevelMaterials &with, const b40::Shell *glass, const b40::Shell *sheer, const b40::Shell *cards, const b40::Shell *lawns, const b40::Shell *washes) {
	Surface sf;
	struct Part {
		const b40::Shell *shell;
		RID material;
		bool blends; // drawn in its triangles' order
	};
	const Part parts[] = { { &shell, with.walls, false }, { glass, with.glass, false }, { sheer, with.sheer, true }, { cards, with.card, false }, { lawns, with.lawn, false },
		{ washes, with.wash, true } };
	TypedArray<Dictionary> surfaces;
	Vector3 mn, mx;
	for (const Part &p : parts) {
		if (!p.shell || p.shell->triangles() == 0) continue;
		Vector3 lo, hi;
		p.shell->bounds(lo, hi);
		mn = surfaces.is_empty() ? lo : mn.min(lo);
		mx = surfaces.is_empty() ? hi : mx.max(hi);
		surfaces.push_back(p.shell->surface(p.material, with_uv2, p.blends));
	}
	if (surfaces.is_empty()) return sf;
	sf.mesh = OwnedMesh::of(create_mesh(surfaces));
	const Vector3 pad(AABB_PAD, AABB_PAD, AABB_PAD);
	sf.box = AABB(mn - pad, mx - mn + pad * 2);
	return sf;
}

// A box's row: its tone and, in one number, its finish and the band it leaves by (building_parts.gdshader: the finish + BAND_CODE_STEP
// x (band + 1); NO_BAND: the finish alone, a box drawn as far as its instance is).
PackedFloat32Array BuildingRenderer::bake_band(const std::vector<b40::BoxInstance> &boxes, int band) {
	PackedFloat32Array buf;
	for (const b40::BoxInstance &inst : boxes) {
		const Color c = unpack(inst.color);
		push_row(buf, inst.basis, inst.origin, c.r, c.g, c.b, float(inst.fin) + BAND_CODE_STEP * float(band + 1));
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
	far = BuildingRenderer::bake(far_shell, false, LevelMaterials::of_walls(far_material));
	casts.bake();
	far_shell = b40::Shell();
	baked = true;
}

void BuildingRenderer::commit_one(Built &b) {
	auto found = chunks_.find(b.key);
	if (found == chunks_.end()) return; // dropped by the stream meanwhile (drop_chunk also forgets the request)
	Chunk &ch = found->second;
	const bool current = b.version == ch.version; // else the chunk changed after the job read it: stays stale
	ch.key = b.key;
	signatures_.set_chunk(b.key, b.signatures);
	signatures_.visible(b.key, ch.shown);
	if (b.has_near) {
		for (int i = 0; i < SUB_COUNT; i++) {
			put(ch.near[size_t(i)], b.near[size_t(i)], scenario_, true, ch.shown, LAYER_BUILDING_SHELLS);
			put(ch.parts[size_t(i)], b.parts[size_t(i)], scenario_, false, ch.shown, LAYER_BUILDING_PARTS);
		}
		for (size_t i = 0; i < SUB_COUNT; i++)
			for (size_t band = 0; band < BAND_DRAWS; band++) { // each band's unit mesh carries its material: the parts' or the balcony pieces'
				ch.bands[i][band].adopt(scenario_, false, b.band_mm[i][band].release(), b.band_count[i][band], b.band_box[i][band], LAYER_BUILDING_DETAILS);
				// the shadow in the shadow material: the parts' own has a vertex() every shadow pass would run for the same silhouette
				if (const int cast = cast_of(int(band)); cast >= 0) ch.prop_casts[i][size_t(cast)].bind(scenario_, ch.bands[i][band].mm, material_cast_, b.band_box[i][band], LAYER_BUILDING_DETAILS);
			}
		// a cloth throws a shadow; its own layer, as the 4.0's flag layer has its own switch (effects panel bFlags)
		ch.flags.adopt(scenario_, true, b.flag_mm.release(), b.flag_count, b.flag_box, LAYER_BUILDING_FLAGS);
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
			put(ch.mid[size_t(i)], b.mid[size_t(i)], scenario_, false, ch.shown, LAYER_BUILDING_FACADES); // its hull throws the shadow
			OwnedMesh hull = std::move(b.mid_hull[size_t(i)]);
			ch.mid_hulls[size_t(i)].bind(scenario_, hull.rid, RID(), b.mid[size_t(i)].box, LAYER_BUILDING_FACADES); // made in the shadow material (cast_hull.h)
			ch.mid_hull_meshes[size_t(i)] = std::move(hull); // the old one goes after the instance left it
			put(ch.flat[size_t(i)], b.flat[size_t(i)], scenario_, false, ch.shown, LAYER_BUILDING_FACADES); // the plain side behind throws the shadow
		}
		for (size_t i = 0; i < SUB_COUNT; i++)
			for (size_t k = MID_BOXES; k < MID_DRAWS; k++) // no shadow of their own: the plain side behind the bays throws it
				ch.mid_draws[i][k].adopt(scenario_, false, b.mid_mm[i][k].release(), b.mid_count[i][k], b.mid_box[i][k], LAYER_BUILDING_FACADES);
		for (size_t i = 0; i < SUB_COUNT; i++) ch.mid_casts[i].bind(scenario_, ch.mid_draws[i][MID_SOLIDS].mm, material_cast_, b.mid_box[i][MID_SOLIDS], LAYER_BUILDING_FACADES);
		std::copy(b.held.begin() + HELD_MID_WALLS, b.held.end(), ch.held.begin() + HELD_MID_WALLS);
		ch.built[LEVEL_MID] = true;
		ch.stale[LEVEL_MID] = !current;
	}
	if (b.has_bays) { // the mid level's openings, in the walls' own material (the bay mesh's)
		for (size_t i = 0; i < SUB_COUNT; i++)
			ch.mid_draws[i][MID_BAYS].adopt(scenario_, false, b.mid_mm[i][MID_BAYS].release(), b.mid_count[i][MID_BAYS], b.mid_box[i][MID_BAYS], LAYER_BUILDING_FACADES);
		ch.built[LEVEL_BAYS] = true;
		ch.stale[LEVEL_BAYS] = !current;
	}
	ch.wear_due = b.ageing ? bodies_.now_min() + double(look_.wear_redraw_min) : 0; // drawn again then, its buildings older
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
		}
		if (b.has_mid || b.has_bays)
			for (const AABB &drawn : b.mid_box[i]) grow(box, drawn); // of the draws this result made: the others' boxes are empty
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
		if (ch.mid_hulls[i].inst.is_valid()) rs->instance_set_custom_aabb(ch.mid_hulls[i].inst, box);
	}
}

void BuildingRenderer::commit_super(SuperBuilt &b) {
	Super &sp = supers_[b.key];
	sp.has_content = b.far.mesh.valid();
	sp.held = b.held;
	put(sp.far, b.far, scenario_, false, true, LAYER_BUILDING_MASSES); // it throws nothing itself: its chunks' casters do (far_casts.h)
	sp.casts.commit(b.casts, scenario_, LAYER_BUILDING_MASSES);
	sp.built = true;
	sp.mask_sent = -1; // the mask update_view computed this frame goes to the (maybe new) instance at once
	send_mask(sp);
}

// A chunk whose near level is drawn again with the same buildings (see building_renderer.h).
// Buildings with or without their lots: the streamed chunks' near and mid levels and the superchunks' far meshes, the three that draw
// lots, remeshed in reach as a restyled chunk's (the old meshes stay up till the new land); the bays have none, and nothing else is touched.
void BuildingRenderer::set_lots_drawn(bool on) {
	if (lots_drawn_.exchange(on) == on) return;
	for (auto &[key, ch] : chunks_) {
		ch.version++; // a job that read the old switch lands stale and is asked again
		ch.stale[LEVEL_NEAR] = ch.built[LEVEL_NEAR];
		ch.stale[LEVEL_MID] = ch.built[LEVEL_MID];
	}
	for (auto &[key, sp] : supers_) {
		if (!sp.dirty) sp.due = building_stream::now_s(); // the switch is the player's hand: no waiting for further changes
		sp.dirty = true;
	}
}

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
	for (const ShadowTwin &d : ch.mid_hulls) d.visible(on);
	ch.flags.visible(on);
	signatures_.visible(ch.key, on);
}

void BuildingRenderer::send_mask(Super &sp) {
	if (!sp.far.inst.is_valid() || sp.mask == sp.mask_sent) return;
	RenderingServer::get_singleton()->instance_geometry_set_shader_parameter(sp.far.inst, FAR_MASK_PARAM, sp.mask);
	sp.casts.show(sp.mask); // a chunk drawn in its own detail throws its own hulls
	sp.mask_sent = sp.mask;
}

} // namespace city
