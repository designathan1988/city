// The walls given to the engine as occluders, and the occluder of one chunk (occluders.h).
#include "render/buildings/occluders.h"

#include "sim/buildings/b40/blocks.h"

#include <godot_cpp/classes/rendering_server.hpp>

using namespace godot;

namespace city {

namespace {
constexpr int CORNERS = 4;
constexpr int WALL_INDICES[] = { 0, 1, 2, 0, 2, 3 };
} // namespace

void OccluderWalls::add(const b40::Placed &pl, int min_storeys) {
	const b40::Body &b = *pl.b;
	// Only walls that surely stand: a body drawn by a design of its own is its model, not these blocks (an open canopy, an arch, a tower
	// thinner than its box), and where a block is a void, an intersection or an exclusion the blocks are cut by one another.
	if (b.signature != 0 || b40::has_modes(b)) return;
	for (const b40::Volume &v : b.volumes) {
		if (v.base != 0 || v.storeys < min_storeys || !v.outline.empty()) continue;
		const float top = b.block_z(v, v.storeys);
		const float x[CORNERS] = { v.x, v.x + v.w, v.x + v.w, v.x };
		const float y[CORNERS] = { v.y, v.y, v.y + v.d, v.y + v.d };
		for (int side = 0; side < CORNERS; side++) {
			const int next = (side + 1) % CORNERS, first = int(points.size());
			points.push_back(pl.lot.at(x[side], y[side], 0.f));
			points.push_back(pl.lot.at(x[next], y[next], 0.f));
			points.push_back(pl.lot.at(x[next], y[next], top));
			points.push_back(pl.lot.at(x[side], y[side], top));
			for (int k : WALL_INDICES) triangles.push_back(first + k);
		}
	}
}

void ChunkOccluder::set(const RID &scenario, const OccluderWalls &walls) {
	clear();
	if (walls.points.is_empty()) return;
	RenderingServer *rs = RenderingServer::get_singleton();
	occluder_ = rs->occluder_create();
	rs->occluder_set_mesh(occluder_, walls.points, walls.triangles);
	instance_ = rs->instance_create2(occluder_, scenario);
}

void ChunkOccluder::clear() {
	RenderingServer *rs = RenderingServer::get_singleton();
	if (rs == nullptr) return;
	if (instance_.is_valid()) rs->free_rid(instance_);
	if (occluder_.is_valid()) rs->free_rid(occluder_);
	instance_ = RID();
	occluder_ = RID();
}

} // namespace city
