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
	// The lane graph's incremental work: drained by the traffic (not by the render). `net_nodes` are the nodes whose
	// connectors and junction must be rebuilt; the segment lists are the ids added, removed or reprofiled.
	std::vector<int> net_nodes, net_segs_removed, net_segs_changed;

	static void dedup(std::vector<int64_t> &v) {
		std::sort(v.begin(), v.end());
		v.erase(std::unique(v.begin(), v.end()), v.end());
	}
	static void dedup(std::vector<int> &v) {
		std::sort(v.begin(), v.end());
		v.erase(std::unique(v.begin(), v.end()), v.end());
	}
	void net_node(int n) { net_nodes.push_back(n); }
	void net_seg(int seg) { net_segs_changed.push_back(seg); }
	void net_seg_removed(int seg) { net_segs_removed.push_back(seg); }
	void clear_net() {
		net_nodes.clear();
		net_segs_removed.clear();
		net_segs_changed.clear();
	}
	void clear_render() {
		road_chunks.clear();
		building_chunks.clear();
		terrain_rects.clear();
	}
};

} // namespace city
