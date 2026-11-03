// CPU assembly of the building levels on a job's thread: gather copies what the build reads from the map (the
// records of its chunks and the ground under their lots) while the job holds the map lock shared for a moment; the
// near/mid shells per 128 m sub-chunk (with the facade parts as instanced unit boxes grouped in five thickness bands)
// and the per-superchunk far/HLOD shell are then built from that copy without the lock and baked into meshes
// (bake, building_renderer_commit.cpp), so the main thread only hands them to their instances.
#include "render/buildings/building_renderer.h"

#include "render/buildings/civic.h"
#include "render/buildings/facade.h"

#include <algorithm>
#include <cfloat>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int CHUNK_SUBDIVISIONS = 2; // CHUNK / SUBCHUNK
constexpr float LOT_GROUND_PAD_M = 4.f; // ground copied past a lot's corners (plinths, kerbs and fences reach out)

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
			if (b.floors == 0 && !b40::is_open_lot(b.fn)) return;
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
	std::vector<b40::Shell> near_shells(static_cast<size_t>(SUB_COUNT)), parts_shells(static_cast<size_t>(SUB_COUNT));
	std::vector<b40::Shell> mid_shells(static_cast<size_t>(SUB_COUNT)), mid_parts(static_cast<size_t>(SUB_COUNT));
	for (b40::Shell &p : parts_shells) p.instanced = true;
	for (b40::Shell &p : mid_parts) p.instanced = true;
	for (const Building &rec : in.records) {
		b40::Placed pl;
		bodies.place(data_, in.ground, rec, pl);
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
	if (near) {
		auto band_of = [this](float thick) {
			int band = 0;
			while (band + 1 < BAND_COUNT && thick > data_.part_bands[size_t(std::min(band, int(data_.part_bands.size()) - 1))].thick_max)
				band++;
			return std::min(band, BAND_COUNT - 1);
		};
		std::array<std::vector<b40::BoxInstance>, BAND_COUNT> boxes;
		for (const b40::Shell &p : parts_shells)
			for (const b40::BoxInstance &inst : p.boxes) boxes[size_t(band_of(inst.thick))].push_back(inst);
		for (int b = 0; b < BAND_COUNT; b++) out.bands[size_t(b)] = bake_band(boxes[size_t(b)]);
		for (int i = 0; i < SUB_COUNT; i++) {
			out.near[size_t(i)] = bake(near_shells[size_t(i)], true);
			out.parts[size_t(i)] = bake(parts_shells[size_t(i)], true);
		}
		out.has_near = true;
	}
	if (mid) {
		for (int i = 0; i < SUB_COUNT; i++) out.mid[size_t(i)] = bake(mid_shells[size_t(i)], true);
		out.has_mid = true;
	}
	return out;
}

BuildingRenderer::SuperBuilt BuildingRenderer::build_super_cpu(const Inputs &in, int64_t super_key) const {
	thread_local BuildingBodies bodies;
	SuperBuilt out;
	out.key = super_key;
	b40::Shell far;
	for (const Building &rec : in.records) {
		b40::Placed pl;
		bodies.place(data_, in.ground, rec, pl);
		pl.lean = true;
		b40::emit_far(far, pl, true);
	}
	out.far = bake(far, false);
	return out;
}

} // namespace city
