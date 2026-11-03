// CPU assembly of the building levels: the near/mid shells per 128 m sub-chunk (with the facade parts as instanced
// unit boxes grouped in five thickness bands) and the per-superchunk far/HLOD shell. No RenderingServer calls here.
#include "render/buildings/building_renderer.h"

#include "render/buildings/civic.h"
#include "render/buildings/facade.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int CHUNK_SUBDIVISIONS = 2; // CHUNK / SUBCHUNK

template <typename F>
void each_record(const CityState &s, int64_t key, F &&fn) {
	float x0 = float(key_x(key)) * CHUNK, z0 = float(key_y(key)) * CHUNK;
	s.buildings.query(x0, z0, x0 + CHUNK - b40::EPS, z0 + CHUNK - b40::EPS, [&](int id) {
		const Building &b = s.buildings.items[size_t(id)];
		if (b.floors > 0 || b40::is_open_lot(b.fn)) fn(b);
	});
}

int sub_index(const Building &b, int64_t key) {
	int bx = int(std::floor(b.x / SUBCHUNK)) - key_x(key) * CHUNK_SUBDIVISIONS;
	int bz = int(std::floor(b.z / SUBCHUNK)) - key_y(key) * CHUNK_SUBDIVISIONS;
	bx = std::clamp(bx, 0, CHUNK_SUBDIVISIONS - 1);
	bz = std::clamp(bz, 0, CHUNK_SUBDIVISIONS - 1);
	return bz * CHUNK_SUBDIVISIONS + bx;
}
} // namespace

BuildingRenderer::Built BuildingRenderer::build_cpu(const CityState &s, int64_t key, bool near, bool mid) const {
	thread_local BuildingBodies bodies;
	Built out;
	out.key = key;
	if (!near && !mid) return out;
	std::vector<b40::Shell> near_shells(static_cast<size_t>(SUB_COUNT)), parts_shells(static_cast<size_t>(SUB_COUNT));
	std::vector<b40::Shell> mid_shells(static_cast<size_t>(SUB_COUNT)), mid_parts(static_cast<size_t>(SUB_COUNT));
	for (b40::Shell &p : parts_shells) p.instanced = true;
	for (b40::Shell &p : mid_parts) p.instanced = true;
	each_record(s, key, [&](const Building &rec) {
		b40::Placed pl;
		bodies.place(data_, s, rec, pl);
		int sub = sub_index(rec, key);
		if (near) {
			b40::emit_facades(near_shells[size_t(sub)], parts_shells[size_t(sub)], pl);
			b40::emit_roofs(near_shells[size_t(sub)], parts_shells[size_t(sub)], pl);
			b40::emit_lot(near_shells[size_t(sub)], parts_shells[size_t(sub)], pl);
			if (!mid) return; // both asked (first build close to the camera): the coarse mid level follows
		}
		pl.coarse = true;
		b40::emit_far(mid_shells[size_t(sub)], pl, true);
		b40::emit_roofs(mid_shells[size_t(sub)], mid_parts[size_t(sub)], pl);
		b40::emit_lot(mid_shells[size_t(sub)], mid_parts[size_t(sub)], pl);
	});
	if (near) {
		auto band_of = [this](float thick) {
			int band = 0;
			while (band + 1 < BAND_COUNT && thick > data_.part_bands[size_t(std::min(band, int(data_.part_bands.size()) - 1))].thick_max)
				band++;
			return std::min(band, BAND_COUNT - 1);
		};
		for (b40::Shell &p : parts_shells)
			for (const b40::BoxInstance &inst : p.boxes) out.boxes[size_t(band_of(inst.thick))].push_back(inst);
		out.near = std::move(near_shells);
		out.parts = std::move(parts_shells);
		out.has_near = true;
	}
	if (mid) {
		out.mid = std::move(mid_shells);
		out.has_mid = true;
	}
	return out;
}

BuildingRenderer::SuperBuilt BuildingRenderer::build_super_cpu(const CityState &s, int64_t super_key) const {
	thread_local BuildingBodies bodies;
	SuperBuilt out;
	out.key = super_key;
	constexpr int PER = int(SUPERCHUNK / CHUNK);
	int sx = key_x(super_key), sz = key_y(super_key);
	for (int j = 0; j < PER; j++)
		for (int i = 0; i < PER; i++) {
			int64_t ck = cell_key(sx * PER + i, sz * PER + j);
			each_record(s, ck, [&](const Building &rec) {
				b40::Placed pl;
				bodies.place(data_, s, rec, pl);
				pl.lean = true;
				b40::emit_far(out.far, pl, true);
			});
		}
	return out;
}

} // namespace city
