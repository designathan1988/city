// Chunk sizes, integer cell keys and the flat spatial hash used by every system for neighbourhood queries
// (simpler and more cache/thread friendly than a pointer quadtree).
#pragma once

#include "core/checks.h"
#include "core/math.h"

#include <algorithm>
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
constexpr int CHUNKS_PER_SUPER = int(SUPERCHUNK / CHUNK); // chunks a superchunk spans, per axis
constexpr int SUBCHUNKS_PER_CHUNK = int(CHUNK / SUBCHUNK); // sub-chunks a chunk holds, per axis

// A cell key holds the column in its high 32 bits and the row in its low 32.
constexpr int KEY_HALF_BITS = 32;
constexpr int64_t KEY_LOW_MASK = 0xFFFFFFFF;
inline int64_t cell_key(int cx, int cy) { return (int64_t(cx) << KEY_HALF_BITS) ^ int64_t(uint32_t(cy)); }
inline int key_x(int64_t k) { return int(k >> KEY_HALF_BITS); }
inline int key_y(int64_t k) { return int(int32_t(uint32_t(k & KEY_LOW_MASK))); }
inline int64_t chunk_of(float x, float z, float size = CHUNK) {
	return cell_key(int(std::floor(x / size)), int(std::floor(z / size)));
}

// Calls fn(key) for every CHUNK the box touches.
template <typename F>
void for_chunks(double minx, double minz, double maxx, double maxz, F &&fn) {
	for (int j = int(std::floor(minz / CHUNK)); j <= int(std::floor(maxz / CHUNK)); j++)
		for (int i = int(std::floor(minx / CHUNK)); i <= int(std::floor(maxx / CHUNK)); i++) fn(cell_key(i, j));
}

struct SpatialHash {
	// An id filed in a cell, with the first cell of the box it was filed under (query_unique reports it there only).
	struct Entry {
		int id;
		int x0, y0;
	};
	// The cell's side, metres, for a hash given none: a few lots across. A hash that knows its reach gives its own.
	static constexpr float DEFAULT_CELL_M = 64.f;
	float cell = DEFAULT_CELL_M;
	std::unordered_map<int64_t, std::vector<Entry>> cells;
	// A window of another hash (`window`): the box it answers for. A query that leaves the box would miss what is filed outside
	// it, so it stops the tests' DLL (core/checks.h); the whole hash of the live city answers for everywhere.
	bool windowed = false;
	float win_x0 = 0, win_y0 = 0, win_x1 = 0, win_y1 = 0;

	void clear() { cells.clear(); }
	// The cells the box touches, copied as they are: a hash that answers for that box alone, each query inside it reporting the
	// same ids in the same order as this one (what a job copies of the map: sim/state/city_cut.h).
	SpatialHash window(float minx, float miny, float maxx, float maxy) const {
		SpatialHash w;
		w.cell = cell;
		w.windowed = true;
		w.win_x0 = minx;
		w.win_y0 = miny;
		w.win_x1 = maxx;
		w.win_y1 = maxy;
		for_cells(minx, miny, maxx, maxy, [&](int64_t k) {
			auto it = cells.find(k);
			if (it != cells.end()) w.cells.emplace(k, it->second);
		});
		return w;
	}
	void insert_aabb(int id, float minx, float miny, float maxx, float maxy) {
		const Entry e{ id, cell_of(minx), cell_of(miny) };
		for_cells(minx, miny, maxx, maxy, [&](int64_t k) { cells[k].push_back(e); });
	}
	void remove_aabb(int id, float minx, float miny, float maxx, float maxy) {
		for_cells(minx, miny, maxx, maxy, [&](int64_t k) {
			auto it = cells.find(k);
			if (it == cells.end()) return;
			auto &v = it->second;
			v.erase(std::remove_if(v.begin(), v.end(), [id](const Entry &e) { return e.id == id; }), v.end());
			if (v.empty()) cells.erase(it);
		});
	}
	// Calls fn(id) for every id in touched cells; an id can be reported more than once.
	template <typename F>
	void query(float minx, float miny, float maxx, float maxy, F &&fn) const {
		answers_for(minx, miny, maxx, maxy);
		for_cells(minx, miny, maxx, maxy, [&](int64_t k) {
			auto it = cells.find(k);
			if (it == cells.end()) return;
			for (const Entry &e : it->second) fn(e.id);
		});
	}
	// Calls fn(id) once for every id in touched cells, in the order of its first report by query(): the cells are walked
	// row by row, so the first touched cell of an id is the one at the larger of the query's first cell and the id's own,
	// on each axis, and the id is reported there only.
	template <typename F>
	void query_unique(float minx, float miny, float maxx, float maxy, F &&fn) const {
		answers_for(minx, miny, maxx, maxy);
		const int qx0 = cell_of(minx), qy0 = cell_of(miny);
		for_cells(minx, miny, maxx, maxy, [&](int64_t k) {
			auto it = cells.find(k);
			if (it == cells.end()) return;
			const int x = key_x(k), y = key_y(k);
			for (const Entry &e : it->second)
				if (x == std::max(qx0, e.x0) && y == std::max(qy0, e.y0)) fn(e.id);
		});
	}

private:
	void answers_for(float minx, float miny, float maxx, float maxy) const {
		if (CHECKS && windowed && (minx < win_x0 || miny < win_y0 || maxx > win_x1 || maxy > win_y1))
			city_stop("a query past the box of the map a job copied");
	}
	int cell_of(float v) const { return int(std::floor(v / cell)); }
	template <typename F>
	void for_cells(float minx, float miny, float maxx, float maxy, F &&fn) const {
		int x0 = cell_of(minx), x1 = cell_of(maxx);
		int y0 = cell_of(miny), y1 = cell_of(maxy);
		for (int y = y0; y <= y1; y++)
			for (int x = x0; x <= x1; x++) fn(cell_key(x, y));
	}
};

} // namespace city
