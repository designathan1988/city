// Chunks streamed around the camera (roads, furniture, buildings).
#pragma once

#include "core/grid.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <vector>

namespace city {

// One per-frame count shared by every streamer and level builder: the tuning value now bounds the whole frame, not
// each system separately.
struct BuildBudget {
	int left = 0;
	explicit BuildBudget(int n) : left(n) {}
	bool take() {
		if (left <= 0) return false;
		--left;
		return true;
	}
};

// Chunks that have content (`known`), meshed only within `reach` of the camera (nearest first, `budget` per frame),
// rebuilt when marked dirty and freed beyond reach x drop_k, so memory follows the view, not the map size. A frame
// scans the cells within reach and the built set, never every chunk of the map.
struct ChunkStream {
	std::unordered_set<int64_t> known, built, dirty;
	int pending = 0;

	void clear() {
		known.clear();
		built.clear();
		dirty.clear();
		pending = 0;
	}
	void mark(int64_t k) {
		known.insert(k);
		dirty.insert(k);
	}
	// Horizontal distance: the camera's height must not push a chunk out of its reach (it hid the whole city when
	// the eye rose above render.road_end_m on a zoomed-out view).
	static float distance(int64_t k, float cx, float cz) {
		constexpr float MID = 0.5f;
		float dx = (float(key_x(k)) + MID) * CHUNK - cx, dz = (float(key_y(k)) + MID) * CHUNK - cz;
		return std::sqrt(dx * dx + dz * dz);
	}
	// build(key) meshes a chunk, drop(key) frees it.
	template <typename B, typename D>
	void update(float cx, float cz, float reach, float drop_k, BuildBudget &budget, B &&build, D &&drop) {
		for (auto it = built.begin(); it != built.end();) {
			if (distance(*it, cx, cz) <= reach * drop_k) {
				++it;
				continue;
			}
			drop(*it);
			dirty.erase(*it);
			it = built.erase(it);
		}
		std::vector<std::pair<float, int64_t>> want;
		for (auto it = dirty.begin(); it != dirty.end();) {
			if (built.count(*it)) want.push_back({ distance(*it, cx, cz), *it });
			it = built.count(*it) ? std::next(it) : dirty.erase(it); // an unbuilt chunk is met by the scan below
		}
		int r = int(std::ceil(reach / CHUNK)), ix = int(std::floor(cx / CHUNK)), iz = int(std::floor(cz / CHUNK));
		for (int j = iz - r; j <= iz + r; j++)
			for (int i = ix - r; i <= ix + r; i++) {
				int64_t k = cell_key(i, j);
				if (!known.count(k) || built.count(k)) continue;
				float d = distance(k, cx, cz);
				if (d <= reach) want.push_back({ d, k });
			}
		std::sort(want.begin(), want.end());
		int n = 0;
		for (const auto &w : want) {
			if (!budget.take()) break;
			build(w.second);
			built.insert(w.second);
			dirty.erase(w.second);
			++n;
		}
		pending = int(want.size()) - n;
	}
};

} // namespace city
