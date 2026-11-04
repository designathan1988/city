// CPU assembly of the building levels on a job's thread: gather copies what the build reads from the map (the
// records of its chunks and the ground under their lots) while the job holds the map lock shared for a moment; the
// near/mid shells per 128 m sub-chunk (with the facade parts as instanced unit boxes grouped in five thickness bands)
// and the per-superchunk far/HLOD shell are then built from that copy without the lock. A later job bakes them into
// meshes when the frame's upload budget admits it (Built::bake, building_renderer_commit.cpp; chunk_jobs.h).
#include "render/buildings/building_renderer.h"

#include "sim/buildings/b40/civic.h"
#include "render/buildings/facade.h"

#include <algorithm>
#include <cfloat>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int CHUNK_SUBDIVISIONS = 2; // CHUNK / SUBCHUNK
constexpr float LOT_GROUND_PAD_M = 4.f; // ground copied past a lot's corners (plinths, kerbs and fences reach out)
constexpr int CHUNKS_PER_SUPER_AXIS = int(SUPERCHUNK / CHUNK);
// The far mesh's COLOR.a: surface kind in the low bits (SurfaceKind < 8), the chunk in the superchunk above them;
// the layout building_far.gdshader decodes (KIND_BITS, KIND_MASK).
constexpr int FAR_KIND_BITS = 3, FAR_KIND_MASK = (1 << FAR_KIND_BITS) - 1, FAR_OFFICE_BIT = 128;
constexpr float BYTE_MAX = 255.f;
static_assert(b40::SK_COUNT <= (1 << FAR_KIND_BITS), "surface kinds must fit the far mesh's kind bits");
static_assert(CHUNKS_PER_SUPER_AXIS * CHUNKS_PER_SUPER_AXIS << FAR_KIND_BITS <= FAR_OFFICE_BIT, "chunk index must fit COLOR.a under the office bit");

int sub_index(const Building &b, int64_t key) {
	int bx = int(std::floor(b.x / SUBCHUNK)) - key_x(key) * CHUNK_SUBDIVISIONS;
	int bz = int(std::floor(b.z / SUBCHUNK)) - key_y(key) * CHUNK_SUBDIVISIONS;
	bx = std::clamp(bx, 0, CHUNK_SUBDIVISIONS - 1);
	bz = std::clamp(bz, 0, CHUNK_SUBDIVISIONS - 1);
	return bz * CHUNK_SUBDIVISIONS + bx;
}
} // namespace

BuildingRenderer::Inputs BuildingRenderer::gather(const CityState &s, const int64_t *chunk_keys, int count) const {
	Inputs in;
	float minx = FLT_MAX, minz = FLT_MAX, maxx = -FLT_MAX, maxz = -FLT_MAX;
	for (int k = 0; k < count; k++) {
		const float x0 = float(key_x(chunk_keys[k])) * CHUNK, z0 = float(key_y(chunk_keys[k])) * CHUNK;
		s.buildings.query(x0, z0, x0 + CHUNK - b40::EPS, z0 + CHUNK - b40::EPS, [&](int id) {
			const Building &b = s.buildings.items[size_t(id)];
			if (b.floors == 0 && b.body == 0 && !b40::is_open_lot(b.fn)) return;
			in.records.push_back(b);
			const float r = 0.5f * std::hypot(b.w, b.d) + LOT_GROUND_PAD_M; // the lot, whatever its turn
			minx = std::min(minx, b.x - r);
			minz = std::min(minz, b.z - r);
			maxx = std::max(maxx, b.x + r);
			maxz = std::max(maxz, b.z + r);
		});
	}
	if (!in.records.empty()) in.ground = s.terrain.patch(minx, minz, maxx, maxz);
	return in;
}

BuildingRenderer::Built BuildingRenderer::build_cpu(const Inputs &in, int64_t key, bool near, bool mid) const {
	thread_local BuildingBodies bodies;
	Built out;
	out.key = key;
	if (!near && !mid) return out;
	std::array<b40::Shell, SUB_COUNT> &near_shells = out.near_shell, &parts_shells = out.parts_shell, &mid_shells = out.mid_shell;
	std::array<b40::Shell, SUB_COUNT> mid_parts;
	for (size_t i = 0; i < SUB_COUNT; i++) {
		out.near_glass[i].rooms = out.mid_glass[i].rooms = true;
		near_shells[i].glass = &out.near_glass[i];
		near_shells[i].sheer = &out.near_sheer[i];
		near_shells[i].cards = &out.near_cards[i];
		mid_shells[i].cards = &out.mid_cards[i];
		mid_shells[i].glass = &out.mid_glass[i];
	}
	for (b40::Shell &p : parts_shells) p.instanced = true;
	for (b40::Shell &p : mid_parts) p.instanced = true;
	for (const Building &rec : in.records) {
		b40::Placed pl;
		bodies.place(designs(), look_, in.ground, rec, pl);
		int sub = sub_index(rec, key);
		if (near) {
			b40::emit_facades(near_shells[size_t(sub)], parts_shells[size_t(sub)], pl);
			b40::emit_roofs(near_shells[size_t(sub)], parts_shells[size_t(sub)], pl);
			b40::emit_lot(near_shells[size_t(sub)], parts_shells[size_t(sub)], pl);
			if (!mid) continue; // both asked (first build close to the camera): the coarse mid level follows
		}
		pl.coarse = true;
		b40::emit_far(mid_shells[size_t(sub)], pl, true);
		b40::emit_roofs(mid_shells[size_t(sub)], mid_parts[size_t(sub)], pl);
		b40::emit_lot(mid_shells[size_t(sub)], mid_parts[size_t(sub)], pl);
	}
	for (size_t i = 0; i < SUB_COUNT; i++) { // `out` is about to move
		near_shells[i].glass = mid_shells[i].glass = nullptr;
		near_shells[i].sheer = nullptr;
		near_shells[i].cards = mid_shells[i].cards = nullptr;
	}
	if (near) {
		auto band_of = [this](float thick) {
			int band = 0;
			while (band + 1 < BAND_COUNT && thick > look_.part_bands[size_t(std::min(band, int(look_.part_bands.size()) - 1))].thick_max)
				band++;
			return std::min(band, BAND_COUNT - 1);
		};
		std::array<std::vector<b40::BoxInstance>, BAND_COUNT> boxes;
		for (b40::Shell &p : parts_shells) {
			for (const b40::BoxInstance &inst : p.boxes) boxes[size_t(band_of(inst.thick))].push_back(inst);
			p.boxes = {}; // in the bands now
		}
		for (int b = 0; b < BAND_COUNT; b++) out.bands[size_t(b)] = bake_band(boxes[size_t(b)]);
		out.unit_box = unit_box_->get_rid();
		out.has_near = true;
	}
	out.has_mid = mid;
	return out;
}

BuildingRenderer::SuperBuilt BuildingRenderer::build_super_cpu(const Inputs &in, int64_t super_key) const {
	thread_local BuildingBodies bodies;
	SuperBuilt out;
	out.key = super_key;
	b40::Shell &far = out.far_shell;
	far.far_tones = &look_.finish_far;
	const int sx = key_x(super_key) * CHUNKS_PER_SUPER_AXIS, sz = key_y(super_key) * CHUNKS_PER_SUPER_AXIS;
	for (const Building &rec : in.records) {
		b40::Placed pl;
		bodies.place(designs(), look_, in.ground, rec, pl);
		pl.lean = true;
		const size_t first = far.col.size();
		b40::emit_far(far, pl, true);
		// the building's chunk in the superchunk above the kind bits of COLOR.a (building_far.gdshader chunk_mask)
		const int64_t ck = chunk_of(rec.x, rec.z, CHUNK);
		const int chunk = (key_y(ck) - sz) * CHUNKS_PER_SUPER_AXIS + (key_x(ck) - sx);
		for (size_t v = first; v < far.col.size(); v++) {
			const int kind = int(std::lround(far.col[v].a * BYTE_MAX)) & FAR_KIND_MASK;
			far.col[v].a = float(kind | (chunk << FAR_KIND_BITS) | (pl.b->use == b40::USE_COMMERCIAL ? FAR_OFFICE_BIT : 0)) / BYTE_MAX;
		}
	}
	return out;
}

} // namespace city
