// Chunk sizes, integer cell keys and the flat spatial hash used by every system for neighbourhood queries
// (simpler and more cache/thread friendly than a pointer quadtree).
#pragma once

#include "core/math.h"

#include <cstdint>
#include <limits>
#include <unordered_set>
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

// Calls fn(key) for every CHUNK the box touches.
template <typename F>
void for_chunks(double minx, double minz, double maxx, double maxz, F &&fn) {
	for (int j = int(std::floor(minz / CHUNK)); j <= int(std::floor(maxz / CHUNK)); j++)
		for (int i = int(std::floor(minx / CHUNK)); i <= int(std::floor(maxx / CHUNK)); i++) fn(cell_key(i, j));
}

// Cells conservatively covering a line, including float-rounded boundary samples. Duplicate boundary cells are
// allowed; consumers choose their own deduplication/publication order. Work grows with length, not rectangle area.
template <typename F>
void for_segment_cells(double ax, double ay, double bx, double by, float cell, F &&fn) {
	const int steps = std::max(1, int(std::ceil(std::max(std::fabs(bx - ax), std::fabs(by - ay)) / cell)));
	const float infinity = std::numeric_limits<float>::infinity();
	for (int i = 0; i < steps; i++) {
		const double a = double(i) / steps, b = double(i + 1) / steps;
		const double x0 = i == 0 ? ax : ax + (bx - ax) * a, x1 = i + 1 == steps ? bx : ax + (bx - ax) * b;
		const double y0 = i == 0 ? ay : ay + (by - ay) * a, y1 = i + 1 == steps ? by : ay + (by - ay) * b;
		const int minx = int(std::floor(std::nextafter(float(std::min(x0, x1)), -infinity) / cell));
		const int maxx = int(std::floor(std::nextafter(float(std::max(x0, x1)), infinity) / cell));
		const int miny = int(std::floor(std::nextafter(float(std::min(y0, y1)), -infinity) / cell));
		const int maxy = int(std::floor(std::nextafter(float(std::max(y0, y1)), infinity) / cell));
		for (int y = miny; y <= maxy; y++)
			for (int x = minx; x <= maxx; x++) fn(cell_key(x, y));
	}
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
	// First encounter order is preserved; membership checks do not rescan the previously reported ids.
	template <typename F>
	void query_unique(float minx, float miny, float maxx, float maxy, F &&fn) const {
		std::unordered_set<int> seen;
		query(minx, miny, maxx, maxy, [&](int id) { if (seen.insert(id).second) fn(id); });
	}
	// A narrow corridor of cells covering a segment. Sub-boxes are at most one cell wide before conservative float
	// rounding; shared boundary cells are visited once. Cell order is deterministic (the same row-major order as query).
	template <typename F>
	void query_segment(double ax, double ay, double bx, double by, F &&fn) const {
		std::unordered_set<int64_t> seen;
		std::vector<int64_t> keys;
		for_segment_cells(ax, ay, bx, by, cell, [&](int64_t k) { if (seen.insert(k).second) keys.push_back(k); });
		std::sort(keys.begin(), keys.end(), [](int64_t a, int64_t b) { return key_y(a) != key_y(b) ? key_y(a) < key_y(b) : key_x(a) < key_x(b); });
		for (int64_t k : keys) {
			auto it = cells.find(k);
			if (it != cells.end()) for (int id : it->second) fn(id);
		}
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
