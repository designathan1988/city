// The one channel through which the simulation announces what changed. Render adapters consume and clear it on
// the main thread; the traffic drains the net_* lists; the lot cache consumes lot_cells. Nothing else keeps its own
// dirty flags. Locking: producers append while holding CityState::map_mutex exclusively (commands, growth, load);
// a consumer that holds the map lock only shared drains under `mutex` too (swapping the lists out), so draining never
// needs the map exclusively and never waits for the mesh jobs that read the map.
#pragma once

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <vector>

namespace city {

struct ChangeLog {
	std::mutex mutex; // consumers under the shared map lock (see above)
	std::vector<int64_t> road_chunks; // CHUNK keys whose road meshes and street props must be rebuilt
	std::vector<int64_t> lot_cells; // CHUNK keys whose proposed lots must be recomputed (streets or zones changed)
	std::vector<int64_t> building_chunks; // CHUNK keys whose building meshes must be rebuilt
	std::vector<int> terrain_rects; // x0, z0, x1, z1 height-sample rectangles (inclusive)
	std::vector<int> paint_rects; // x0, z0, x1, z1: the same samples, their painted ground changed
	std::vector<int64_t> nature_chunks; // CHUNK keys whose trees and ground cover must be placed again
	std::vector<int64_t> water_chunks; // CHUNK keys whose water surface must be built again
	bool network_changed = false; // lanes and routes must be refreshed
	// The lane graph's incremental work: drained by the traffic (not by the render). `net_nodes` are the nodes whose
	// connectors and junction must be rebuilt; the segment lists are the ids added, removed or reprofiled.
	std::vector<int> net_nodes, net_segs_removed, net_segs_changed;
	// Segments whose street furniture must be placed again though their lanes did not change (a hydrant moving to
	// another corner of its square, a zone change moving a bin): drained by the traffic with the net_* lists.
	std::vector<int> furniture_segs;
	// Buildings that came or went: their bays, and the kerbs their car gates keep bays off, are laid out again by the traffic.
	std::vector<int> lots_changed;
	// Junctions whose signal settings changed: only their plan is made again, the lanes and connectors stay.
	std::vector<int> signal_nodes;

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
		furniture_segs.clear();
		lots_changed.clear();
		signal_nodes.clear();
	}
	void clear_render() {
		road_chunks.clear();
		building_chunks.clear();
		terrain_rects.clear();
		paint_rects.clear();
		nature_chunks.clear();
		water_chunks.clear();
	}
};

} // namespace city
