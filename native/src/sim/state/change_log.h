// The one channel through which the simulation announces what changed. Render adapters consume and clear it on
// the main thread; the lot cache consumes lot_cells. Nothing else keeps its own dirty flags.
#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

namespace city {

struct ChangeLog {
	std::vector<int64_t> road_chunks; // CHUNK keys whose road meshes and street props must be rebuilt
	std::vector<int64_t> lot_cells; // CHUNK keys whose proposed lots must be recomputed (streets or zones changed)
	std::vector<int64_t> building_chunks; // CHUNK keys whose building meshes must be rebuilt
	std::vector<int> terrain_rects; // x0, z0, x1, z1 height-sample rectangles (inclusive)
	bool network_changed = false; // lanes and routes must be refreshed

	static void dedup(std::vector<int64_t> &v) {
		std::sort(v.begin(), v.end());
		v.erase(std::unique(v.begin(), v.end()), v.end());
	}
	void clear_render() {
		road_chunks.clear();
		building_chunks.clear();
		terrain_rects.clear();
	}
};

} // namespace city
