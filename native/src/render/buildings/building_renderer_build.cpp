// CPU assembly of the building levels on a job's thread: gather copies what the build reads from the map (the
// records of its chunks and the ground under their lots) while the job holds the map lock shared for a moment; the
// near/mid shells per 128 m sub-chunk (with the facade parts as instanced unit boxes grouped in five thickness bands)
// and the per-superchunk far/HLOD shell are then built from that copy without the lock. A later job bakes them into
// meshes when the frame's upload budget admits it (Built::bake, building_renderer_commit.cpp; chunk_jobs.h).
#include "render/buildings/building_renderer.h"
#include "render/buildings/cast_hull.h"

#include "sim/buildings/b40/civic.h"
#include "render/buildings/facade.h"
#include "render/buildings/flag_cloth.h"

#include <algorithm>
#include <cfloat>
#include <cmath>

using namespace godot;

namespace city {

namespace {
// The far mesh's COLOR.a: surface kind in the low bits (SurfaceKind < 8), the chunk in the superchunk above them;
// the layout building_far.gdshader decodes (KIND_BITS, KIND_MASK).
constexpr int FAR_KIND_MASK = (1 << SURFACE_KIND_BITS) - 1;
constexpr float BYTE_MAX = 255.f;
static_assert(CHUNKS_PER_SUPER * CHUNKS_PER_SUPER << SURFACE_KIND_BITS <= FAR_OFFICE_BIT, "chunk index must fit COLOR.a under the office bit");

constexpr float AABB_PAD = 1.f;
constexpr int LINE_FLOATS = 4, LINE_ORIGIN = 3; // a plain MultiMesh buffer: three lines of a basis and an origin column

// The box of a chunk with nothing to bound yet.
AABB chunk_box(int64_t key) {
	return AABB(Vector3(float(key_x(key)) * CHUNK, AABB_MIN_Y, float(key_y(key)) * CHUNK), Vector3(CHUNK, AABB_H, CHUNK));
}

// What the instances of a plain MultiMesh buffer cover: each one's origin and its three axes, a unit model at most each way. A band's and
// a mid level's visibility range is measured to the centre of this box, so it has to stand where they stand: the chunk's own box has
// its centre hundreds of metres over the street.
AABB rows_box(const PackedFloat32Array &buf, int64_t key) {
	const int64_t rows = buf.size() / MM_PLAIN_STRIDE;
	if (rows == 0) return chunk_box(key);
	const float *r = buf.ptr();
	Vector3 mn, mx;
	for (int64_t i = 0; i < rows; i++, r += MM_PLAIN_STRIDE) {
		const Vector3 o(r[LINE_ORIGIN], r[LINE_ORIGIN + LINE_FLOATS], r[LINE_ORIGIN + 2 * LINE_FLOATS]);
		const Vector3 reach(std::abs(r[0]) + std::abs(r[1]) + std::abs(r[2]), std::abs(r[LINE_FLOATS]) + std::abs(r[LINE_FLOATS + 1]) + std::abs(r[LINE_FLOATS + 2]),
				std::abs(r[2 * LINE_FLOATS]) + std::abs(r[2 * LINE_FLOATS + 1]) + std::abs(r[2 * LINE_FLOATS + 2]));
		mn = i == 0 ? o - reach : mn.min(o - reach);
		mx = i == 0 ? o + reach : mx.max(o + reach);
	}
	const Vector3 pad(AABB_PAD, AABB_PAD, AABB_PAD);
	return AABB(mn - pad, mx - mn + pad * 2);
}

int sub_index(const Building &b, int64_t key) {
	int bx = int(std::floor(b.x / SUBCHUNK)) - key_x(key) * SUBCHUNKS_PER_CHUNK;
	int bz = int(std::floor(b.z / SUBCHUNK)) - key_y(key) * SUBCHUNKS_PER_CHUNK;
	bx = std::clamp(bx, 0, SUBCHUNKS_PER_CHUNK - 1);
	bz = std::clamp(bz, 0, SUBCHUNKS_PER_CHUNK - 1);
	return bz * SUBCHUNKS_PER_CHUNK + bx;
}
} // namespace

void BuildingRenderer::set_broken(const std::unordered_set<uint32_t> &seeds) {
	auto published = std::make_shared<const std::unordered_set<uint32_t>>(seeds); // copied once per change, on the main thread
	std::lock_guard<std::mutex> lk(broken_mutex_);
	broken_ = std::move(published);
}

std::shared_ptr<RuinMesh> BuildingRenderer::ruin_of(const b40::Data &data, const Building &rec, const HeightPatch &ground, const RID &material) const {
	return city::ruin_of(bodies_, data, look_, rec, ground, material);
}

BuildingRenderer::Inputs BuildingRenderer::gather(const CityState &s, const int64_t *chunk_keys, int count) const {
	Inputs in;
	{
		std::lock_guard<std::mutex> lk(broken_mutex_);
		in.broken = broken_; // the published set itself: no job copies it
	}
	float minx = FLT_MAX, minz = FLT_MAX, maxx = -FLT_MAX, maxz = -FLT_MAX;
	for (int k = 0; k < count; k++) {
		const float x0 = float(key_x(chunk_keys[k])) * CHUNK, z0 = float(key_y(chunk_keys[k])) * CHUNK;
		s.buildings.query(x0, z0, x0 + CHUNK - b40::EPS, z0 + CHUNK - b40::EPS, [&](int id) {
			const Building &b = s.buildings.items[size_t(id)];
			if (b.floors == 0 && b.body == 0 && !b40::is_open_lot(b.fn)) return;
			in.records.push_back(b);
			in.ids.push_back(id);
			const float r = BuildingBodies::ground_reach(b);
			minx = std::min(minx, b.x - r);
			minz = std::min(minz, b.z - r);
			maxx = std::max(maxx, b.x + r);
			maxz = std::max(maxz, b.z + r);
		});
	}
	if (!in.records.empty()) in.ground = s.terrain.patch(minx, minz, maxx, maxz);
	in.view = s.interior;
	in.now_min = s.clock.minutes.load();
	in.wear_span_min = double(s.catalog.growth.decay_span_min);
	return in;
}


// The near level's parts into their bands, sub-chunk by sub-chunk.
void BuildingRenderer::file_parts(Built &out, std::array<b40::Shell, SUB_COUNT> &parts_shells) const {
	auto band_of = [this](float thick) {
		int band = 0;
		while (band + 1 < BAND_COUNT && thick > look_.part_bands[size_t(std::min(band, int(look_.part_bands.size()) - 1))].thick_max)
			band++;
		return std::min(band, BAND_COUNT - 1);
	};
	for (size_t i = 0; i < SUB_COUNT; i++) {
		b40::Shell &p = parts_shells[i];
		std::array<std::vector<b40::BoxInstance>, BAND_COUNT> boxes;
		std::array<std::vector<b40::PropInstance>, b40::BAY_PROPS> props;
		for (const b40::BoxInstance &inst : p.boxes) boxes[size_t(band_of(inst.thick))].push_back(inst);
		for (const b40::PropInstance &inst : p.props) props[inst.kind].push_back(inst);
		p.boxes = {}; // in the bands now
		p.props = {};
		for (int b = 0; b < BAND_COUNT; b++) out.bands[i][size_t(b)] = bake_band(boxes[size_t(b)]);
		for (int k = 0; k < b40::BAY_PROPS; k++) out.bands[i][size_t(BAND_COUNT + k)] = bake_props(props[size_t(k)]);
	}
	for (int b = 0; b < BAND_COUNT; b++) out.band_mesh[size_t(b)] = unit_box_.rid;
	for (int k = 0; k < b40::BAY_PROPS; k++) out.band_mesh[size_t(BAND_COUNT + k)] = prop_mesh_[size_t(k)].rid;
}

// Each shell of a chunk told where what it does not hold goes.
void BuildingRenderer::link_shells(Built &out, std::array<b40::Shell, SUB_COUNT> &mid_parts) {
	for (size_t i = 0; i < SUB_COUNT; i++) {
		out.near_glass[i].rooms = out.mid_glass[i].rooms = true;
		out.near_shell[i].lawns = &out.near_lawns[i];
		out.near_shell[i].washes = &out.near_washes[i];
		out.mid_shell[i].lawns = &out.mid_lawns[i];
		out.near_shell[i].box_sink = &out.parts_shell[i];
		out.mid_shell[i].box_sink = &mid_parts[i];
		out.near_shell[i].glass = &out.near_glass[i];
		out.near_shell[i].sheer = &out.near_sheer[i];
		out.near_shell[i].cards = &out.near_cards[i];
		out.mid_shell[i].cards = &out.mid_cards[i];
		out.mid_shell[i].veiled = true;
		out.mid_shell[i].glass = &out.mid_glass[i];
	}
	for (b40::Shell &p : out.parts_shell) p.instanced = true;
	for (size_t i = 0; i < SUB_COUNT; i++) out.parts_shell[i].cards = &out.near_cards[i];
	for (b40::Shell &p : mid_parts) p.instanced = p.veiled = true;
	for (size_t i = 0; i < SUB_COUNT; i++) mid_parts[i].cards = &out.mid_cards[i];
	for (size_t i = 0; i < SUB_COUNT; i++) {
		out.flat_glass[i].rooms = true;
		out.flat_shell[i].glass = &out.flat_glass[i];
	}
}

BuildingRenderer::Built BuildingRenderer::build_cpu(const Inputs &in, int64_t key, bool near, bool mid) const {
	BuildingBodies &bodies = bodies_;
	Built out;
	out.key = key;
	out.mats = bake_mats_;
	if (!near && !mid) return out;
	std::array<b40::Shell, SUB_COUNT> &near_shells = out.near_shell, &parts_shells = out.parts_shell, &mid_shells = out.mid_shell;
	std::array<b40::Shell, SUB_COUNT> mid_parts;
	link_shells(out, mid_parts);
	for (size_t r = 0; r < in.records.size(); r++) {
		const Building &rec = in.records[r];
		// The near level draws a building the Interior layer opens cut; the coarse levels draw it whole (the camera is close).
		const bool cut = near && in.view.cuts(in.ids[r]);
		b40::Placed pl;
		bodies.place(designs(), look_, in.ground, rec, pl, cut ? &in.view.cut : nullptr);
		int sub = sub_index(rec, key);
		pl.wear = wear_.share(rec.built, in.now_min, in.wear_span_min, out.wear_due);
		const float wear_tone = wear_.tone(pl.wear);
		for (b40::Shell *s : { &near_shells[size_t(sub)], &parts_shells[size_t(sub)], &mid_shells[size_t(sub)], &mid_parts[size_t(sub)], &out.flat_shell[size_t(sub)] }) {
			s->wear = pl.wear;
			s->wear_tone = wear_tone;
		}
		const bool own_design = pl.b->signature != 0 && !cut; // cut open it is the shell that shows its floors
		const bool broken = in.broken->count(rec.seed) > 0; // what stands of it is in pieces: its lot is still drawn here
		if (own_design) {
			const b40::Basis &l = pl.lot;
			const float s = pl.b->signature_scale; // the drawing's unit in metres: a grown design stands at its true size
			out.signatures.push_back({ int(pl.b->signature), Transform3D(Basis(l.e0 * s, l.e2 * s, -l.e1 * s), l.at(-pl.b->signature_x, -pl.b->signature_y, 0)) });
		}
		if (near) {
			if (!own_design && !broken) {
				b40::emit_facades(near_shells[size_t(sub)], parts_shells[size_t(sub)], pl);
				b40::emit_roofs(near_shells[size_t(sub)], parts_shells[size_t(sub)], pl);
			}
			if (cut) b40::emit_interior(near_shells[size_t(sub)], parts_shells[size_t(sub)], pl);
			if (lots_drawn_.load()) b40::emit_lot(near_shells[size_t(sub)], parts_shells[size_t(sub)], pl);
			if (!broken) out.walls.add(pl, state_->catalog.tune.render_occluder_min_storeys);
			out.cut = out.cut || cut;
			if (!mid) continue; // both asked (first build close to the camera): the coarse mid level follows
			if (cut) bodies.place(designs(), look_, in.ground, rec, pl);
		}
		pl.coarse = true;
		mid_shells[size_t(sub)].flat = &out.flat_shell[size_t(sub)];
		if (!own_design && !broken) {
			b40::emit_far(mid_shells[size_t(sub)], pl, true);
			b40::emit_relief(mid_shells[size_t(sub)], mid_parts[size_t(sub)], pl);
			b40::emit_roofs(mid_shells[size_t(sub)], mid_parts[size_t(sub)], pl);
		}
		if (lots_drawn_.load()) b40::emit_lot(mid_shells[size_t(sub)], mid_parts[size_t(sub)], pl);
	}
	for (size_t i = 0; i < SUB_COUNT; i++) { // `out` is about to move
		near_shells[i].glass = mid_shells[i].glass = out.flat_shell[i].glass = nullptr;
		near_shells[i].lawns = mid_shells[i].lawns = nullptr;
		near_shells[i].washes = nullptr;
		near_shells[i].box_sink = mid_shells[i].box_sink = nullptr;
		near_shells[i].sheer = nullptr;
		near_shells[i].cards = mid_shells[i].cards = mid_parts[i].cards = parts_shells[i].cards = nullptr;
	}
	if (near) {
		file_parts(out, parts_shells);
		std::vector<b40::FlagInstance> flags;
		for (b40::Shell &s : near_shells) { // the masts' flags, one cloth MultiMesh for the chunk
			flags.insert(flags.end(), s.flags.begin(), s.flags.end());
			s.flags.clear();
		}
		out.flags = b40::bake_flags(flags);
		out.unit_flag = unit_flag_.rid;
		out.has_near = true;
	}
	out.has_mid = mid;
	out.hull_error_m = look_.hull_error_m;
	if (mid) {
		for (size_t i = 0; i < SUB_COUNT; i++) {
			out.mid_buf[i][MID_BAYS] = bake_bays(mid_shells[i].bays);
			std::vector<b40::BoxInstance> thin, solid;
			for (const b40::BoxInstance &inst : mid_parts[i].boxes) (inst.thick > look_.part_bands[size_t(MID_SOLID_BAND - 1)].thick_max ? solid : thin).push_back(inst);
			out.mid_buf[i][MID_BOXES] = bake_band(thin);
			out.mid_buf[i][MID_SOLIDS] = bake_band(solid);
			mid_shells[i].bays = {};
			mid_shells[i].flat = nullptr;
		}
		out.mid_mesh[MID_BAYS] = unit_bay_.rid;
		out.mid_mesh[MID_BOXES] = out.mid_mesh[MID_SOLIDS] = unit_box_.rid;
	}
	return out;
}

BuildingRenderer::SuperBuilt BuildingRenderer::build_super_cpu(const Inputs &in, int64_t super_key) const {
	BuildingBodies &bodies = bodies_;
	SuperBuilt out;
	out.key = super_key;
	out.mats = bake_mats_;
	b40::Shell &far = out.far_shell;
	far.far_tones = &look_.finish_far;
	far.rooms = true; // each side's windows, as the other levels code them
	far.coarse = true;
	far.custom_used = 1;
	const int sx = key_x(super_key) * CHUNKS_PER_SUPER, sz = key_y(super_key) * CHUNKS_PER_SUPER;
	for (const Building &rec : in.records) {
		b40::Placed pl;
		bodies.place(designs(), look_, in.ground, rec, pl);
		pl.lean = true;
		far.wear = pl.wear = wear_.share(rec.built, in.now_min, in.wear_span_min, out.wear_due);
		far.wear_tone = wear_.tone(pl.wear);
		const size_t first = far.col.size();
		b40::emit_far(far, pl, true);
		// the building's chunk in the superchunk above the kind bits of COLOR.a (building_far.gdshader chunk_mask)
		const int64_t ck = chunk_of(rec.x, rec.z, CHUNK);
		const int chunk = (key_y(ck) - sz) * CHUNKS_PER_SUPER + (key_x(ck) - sx);
		for (size_t v = first; v < far.col.size(); v++) {
			const int kind = int(std::lround(far.col[v].a * BYTE_MAX)) & FAR_KIND_MASK;
			far.col[v].a = float(kind | (chunk << SURFACE_KIND_BITS) | (pl.b->use == b40::USE_COMMERCIAL ? FAR_OFFICE_BIT : 0)) / BYTE_MAX;
		}
	}
	return out;
}

void BuildingRenderer::Built::bake() {
	for (int i = 0; i < SUB_COUNT; i++) {
		if (has_near) {
			held[HELD_NEAR_WALLS].add(near_shell[size_t(i)], true);
			held[HELD_NEAR_PANES].add(near_glass[size_t(i)], true);
			held[HELD_NEAR_GUARDS].add(near_sheer[size_t(i)], true);
			held[HELD_NEAR_CARDS].add(near_cards[size_t(i)], true);
			held[HELD_NEAR_PIECES].add(parts_shell[size_t(i)], true);
			near[size_t(i)] = BuildingRenderer::bake(mats, mats.walls, mats.near_glass, near_shell[size_t(i)], true, &near_glass[size_t(i)], &near_sheer[size_t(i)],
					&near_cards[size_t(i)], &near_lawns[size_t(i)], &near_washes[size_t(i)]);
			parts[size_t(i)] = BuildingRenderer::bake(mats, mats.walls, mats.walls, parts_shell[size_t(i)], true);
		}
		if (has_mid) {
			held[HELD_MID_WALLS].add(mid_shell[size_t(i)], true);
			held[HELD_MID_PANES].add(mid_glass[size_t(i)], true);
			held[HELD_MID_CARDS].add(mid_cards[size_t(i)], true);
			mid[size_t(i)] = BuildingRenderer::bake(mats, mats.walls, mats.walls, mid_shell[size_t(i)], true, &mid_glass[size_t(i)], nullptr, &mid_cards[size_t(i)],
					&mid_lawns[size_t(i)]);
			mid_hull[size_t(i)] = cast_hull(mid_shell[size_t(i)], mid_glass[size_t(i)], hull_error_m, mats.cast);
			held[HELD_MID_PANES].add(flat_glass[size_t(i)], true);
			flat[size_t(i)] = BuildingRenderer::bake(mats, mats.walls, mats.walls, flat_shell[size_t(i)], true, &flat_glass[size_t(i)]);
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
		flat_shell[size_t(i)] = b40::Shell();
		flat_glass[size_t(i)] = b40::Shell();
		near_lawns[size_t(i)] = b40::Shell();
		near_washes[size_t(i)] = b40::Shell();
		mid_lawns[size_t(i)] = b40::Shell();
	}
	for (size_t i = 0; i < SUB_COUNT; i++)
		for (size_t b = 0; b < BAND_DRAWS; b++) {
			band_count[i][b] = int(bands[i][b].size() / MM_PLAIN_STRIDE);
			if (band_count[i][b] > 0) {
				band_box[i][b] = rows_box(bands[i][b], key);
				band_mm[i][b] = bake_multimesh(band_mesh[b], bands[i][b], band_box[i][b]);
			}
			bands[i][b] = PackedFloat32Array();
		}
	for (size_t i = 0; i < SUB_COUNT; i++)
		for (size_t k = 0; k < MID_DRAWS; k++) {
			mid_count[i][k] = int(mid_buf[i][k].size() / MM_PLAIN_STRIDE);
			if (mid_count[i][k] > 0) {
				mid_box[i][k] = rows_box(mid_buf[i][k], key);
				mid_mm[i][k] = bake_multimesh(mid_mesh[k], mid_buf[i][k], mid_box[i][k]);
			}
			mid_buf[i][k] = PackedFloat32Array();
		}
	flag_count = int(flags.size() / MM_PLAIN_STRIDE);
	flag_box = rows_box(flags, key);
	flag_mm = bake_multimesh(unit_flag, flags, flag_box);
	flags = PackedFloat32Array();
	baked = true;
}

} // namespace city
