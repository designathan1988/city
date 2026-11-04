// Chunk sizes, integer cell keys and the flat spatial hash used by every system for neighbourhood queries
// (simpler and more cache/thread friendly than a pointer quadtree).
#pragma once

#include "core/math.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

// Ownership chunks (data, terrain edits, road meshes) and smaller render sub-chunks for near detail.
constexpr float CHUNK = 256.f;
constexpr float SUBCHUNK = 128.f;
// The building HLOD cell (4 x 4 chunks): one far mesh each. The densest of the 24 km city is 8 MiB of arrays
// (render_cost super_mb_max), well inside one frame's upload window (render.upload_budget_mb).
constexpr float SUPERCHUNK = 1024.f;

inline int64_t cell_key(int cx, int cy) { return (int64_t(cx) << 32) ^ int64_t(uint32_t(cy)); }
inline int key_x(int64_t k) { return int(k >> 32); }
inline int key_y(int64_t k) { return int(int32_t(uint32_t(k & 0xFFFFFFFF))); }
inline int64_t chunk_of(float x, float z, float size = CHUNK) {
	return cell_key(int(std::floor(x / size)), int(std::floor(z / size)));
}

struct SpatialHash {
	float cell = 64.f;
	std::unordered_map<int64_t, std::vector<int>> cells;

	void clear() { cells.clear(); }
	void insert_aabb(int id, float minx, float miny, float maxx, float maxy) {
		for_cells(minx, miny, maxx, maxy, [&](int64_t k) { cells[k].push_back(id); });
	}
	void remove_aabb(int id, float minx, float miny, float maxx, float maxy) {
		for_cells(minx, miny, maxx, maxy, [&](int64_t k) {
			auto it = cells.find(k);
			if (it == cells.end()) return;
			auto &v = it->second;
			v.erase(std::remove(v.begin(), v.end(), id), v.end());
			if (v.empty()) cells.erase(it);
		});
	}
	// Calls fn(id) for every id in touched cells; an id can be reported more than once.
	template <typename F>
	void query(float minx, float miny, float maxx, float maxy, F &&fn) const {
		for_cells(minx, miny, maxx, maxy, [&](int64_t k) {
			auto it = cells.find(k);
			if (it == cells.end()) return;
			for (int id : it->second) fn(id);
		});
	}

private:
	template <typename F>
	void for_cells(float minx, float miny, float maxx, float maxy, F &&fn) const {
		int x0 = int(std::floor(minx / cell)), x1 = int(std::floor(maxx / cell));
		int y0 = int(std::floor(miny / cell)), y1 = int(std::floor(maxy / cell));
		for (int y = y0; y <= y1; y++)
			for (int x = x0; x <= x1; x++) fn(cell_key(x, y));
	}
};

} // namespace city
