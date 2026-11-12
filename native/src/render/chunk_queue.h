// Chunks streamed around the camera (roads, furniture, buildings).
#pragma once

#include "core/grid.h"
#include "core/work_budget.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <unordered_set>
#include <vector>

namespace city {

// Work credits shared by participating streamers, level builders and transit continuations; RenderWorld decides their allocation.
using BuildBudget = WorkBudget;

// Chunks that have content (`known`), meshed only within `reach` of the camera (nearest first, `budget` per frame),
// rebuilt when marked dirty and freed beyond reach x drop_k, so memory follows the view, not the map size. A frame
// scans the cells within reach and the built set, never every chunk of the map.
struct ChunkStream {
	int pending = 0;
	bool knows(int64_t key) const { return known_.count(key) != 0; }
	void inherit_known(const ChunkStream &source) { known_ = source.known_; view_valid_ = false; }

	void clear() {
		known_.clear();
		far_.clear();
		built_.clear();
		dirty_.clear();
		want_.clear();
		view_valid_ = false;
		pending = 0;
	}
	void mark_far(int64_t k) {
		far_.insert(k);
		mark(k); // new reach affects this key; no camera-wide rediscovery is needed
	}
	void mark(int64_t k) {
		known_.insert(k);
		if (built_.count(k)) {
			dirty_.insert(k);
			if (view_valid_) want_.insert({distance(k, cx_, cz_), k});
		} else if (view_valid_) {
			const float d = distance(k, cx_, cz_);
			if (d <= key_reach(k)) want_.insert({d, k});
		}
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
	void update(float cx, float cz, float reach, float drop_k, BuildBudget &budget, B &&build, D &&drop, float far_reach = -1) {
		if (!view_valid_ || cx != cx_ || cz != cz_ || reach != reach_ || drop_k != drop_k_ || far_reach != far_reach_) {
			cx_ = cx; cz_ = cz; reach_ = reach; far_reach_ = far_reach; drop_k_ = drop_k; view_valid_ = true;
			for (auto it = built_.begin(); it != built_.end();) {
				if (knows(*it) && distance(*it, cx, cz) <= key_reach(*it) * drop_k) { ++it; continue; }
				drop(*it); dirty_.erase(*it); it = built_.erase(it);
			}
			want_.clear();
			for (int64_t key : dirty_) if (built_.count(key)) want_.insert({distance(key, cx, cz), key});
			const int r = int(std::ceil(std::max(reach, far_reach) / CHUNK)), ix = int(std::floor(cx / CHUNK)), iz = int(std::floor(cz / CHUNK));
			for (int j = iz - r; j <= iz + r; j++)
				for (int i = ix - r; i <= ix + r; i++) {
					const int64_t key = cell_key(i, j);
					if (!knows(key) || built_.count(key)) continue;
					const float d = distance(key, cx, cz);
					if (d <= key_reach(key)) want_.insert({d, key});
				}
		}
		while (!want_.empty() && budget.take()) {
			const auto w = *want_.begin(); want_.erase(want_.begin());
			build(w.second);
			built_.insert(w.second);
			dirty_.erase(w.second);
		}
		pending = int(want_.size());
	}
private:
	float key_reach(int64_t key) const { return far_.count(key) ? std::max(reach_, far_reach_) : reach_; }
	std::unordered_set<int64_t> known_, built_, dirty_, far_;
	std::set<std::pair<float, int64_t>> want_;
	bool view_valid_ = false;
	float cx_ = 0, cz_ = 0, reach_ = 0, drop_k_ = 0, far_reach_ = -1;
};

} // namespace city
