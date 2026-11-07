// Chunks streamed around the camera (roads, furniture, buildings).
#pragma once

#include "core/grid.h"
#include "render/frame_admission.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <unordered_set>
#include <utility>
#include <vector>

namespace city {

// Chunks that have content (`known`), meshed only within `reach` of the camera (nearest first, under the frame's admission),
// rebuilt when marked dirty and freed beyond reach x drop_k, so memory follows the view, not the map size. The candidates
// (known chunks not built inside the window round the camera's cell, and built chunks marked dirty) are kept as they change:
// a mark, a build, a drop, the camera entering another cell or the reach changing. A frame sorts only them, and looks for
// chunks to free only when the camera or the reach moved.
struct ChunkStream {
	std::unordered_set<int64_t> known, built, dirty;
	int pending = 0;

	void clear() {
		known.clear();
		built.clear();
		dirty.clear();
		cand_.clear();
		pending = 0;
		window_ok_ = false;
		moved_ = true;
	}
	void mark(int64_t k) {
		known.insert(k);
		if (built.count(k)) dirty.insert(k);
		if (built.count(k) || in_window(k)) cand_.insert(k);
	}
	// A built chunk drawn again (its content changed for another reason than a mark of this stream).
	void rebuild(int64_t k) {
		if (!built.count(k)) return;
		dirty.insert(k);
		cand_.insert(k);
	}
	void adopt_known(const ChunkStream &other) { // the chunks with content are another stream's
		known = other.known;
		window_ok_ = false;
	}
	// Horizontal distance: the camera's height must not push a chunk out of its reach (it hid the whole city when
	// the eye rose above render.road_end_m on a zoomed-out view).
	static float distance(int64_t k, float cx, float cz) {
		constexpr float MID = 0.5f;
		float dx = (float(key_x(k)) + MID) * CHUNK - cx, dz = (float(key_y(k)) + MID) * CHUNK - cz;
		return std::sqrt(dx * dx + dz * dz);
	}
	// The frame's build units of this stream (rank 0 a chunk not built, 1 a dirty one rebuilt; jobs(key) credits each), the chunks past reach
	// freed by drop(key). build(key, distance) meshes a chunk; busy(key): a result of it is on its way, so it waits (dirty) for that one to land.
	template <typename J, typename B, typename D, typename Busy>
	void offer(float cx, float cz, float reach, float drop_k, J &&jobs, std::vector<FrameAdmission::Unit> &out, B &&build, D &&drop, Busy &&busy) {
		if (cx != cx_ || cz != cz_ || reach != reach_) moved_ = true;
		cx_ = cx;
		cz_ = cz;
		reach_ = reach;
		if (moved_) {
			moved_ = false;
			for (auto it = built.begin(); it != built.end();) {
				if (distance(*it, cx, cz) <= reach * drop_k) {
					++it;
					continue;
				}
				const int64_t k = *it;
				drop(k);
				dirty.erase(k);
				cand_.erase(k);
				it = built.erase(it);
				if (in_window(k) && known.count(k)) cand_.insert(k);
			}
		}
		const int r = int(std::ceil(reach / CHUNK)), ix = int(std::floor(cx / CHUNK)), iz = int(std::floor(cz / CHUNK));
		if (!window_ok_ || r != r_ || ix != ix_ || iz != iz_) {
			r_ = r;
			ix_ = ix;
			iz_ = iz;
			window_ok_ = true;
			cand_.clear();
			for (int64_t k : dirty) cand_.insert(k);
			for (int j = iz - r; j <= iz + r; j++)
				for (int i = ix - r; i <= ix + r; i++) {
					const int64_t k = cell_key(i, j);
					if (known.count(k) && !built.count(k)) cand_.insert(k);
				}
		}
		want_.clear();
		for (int64_t k : cand_) {
			const float d = distance(k, cx, cz);
			if (built.count(k) || d <= reach) want_.push_back({ d, k });
		}
		std::sort(want_.begin(), want_.end());
		pending = 0;
		for (const auto &w : want_) {
			const int64_t k = w.second;
			const float d = w.first;
			pending++;
			if (busy(k)) continue;
			out.push_back(FrameAdmission::Unit{ built.count(k) ? 1 : 0, d, jobs(k), [this, k, d, build]() {
						   build(k, d);
						   built.insert(k);
						   dirty.erase(k);
						   cand_.erase(k);
						   pending--;
					   } });
		}
	}
	// The same, every unit handed over at once (a stream whose build only registers the chunk).
	template <typename B, typename D>
	void update_all(float cx, float cz, float reach, float drop_k, B &&build, D &&drop) {
		std::vector<FrameAdmission::Unit> units;
		offer(cx, cz, reach, drop_k, [](int64_t) { return 0; }, units, std::forward<B>(build), std::forward<D>(drop), [](int64_t) { return false; });
		for (FrameAdmission::Unit &u : units) u.go();
	}

private:
	bool in_window(int64_t k) const {
		return window_ok_ && std::abs(key_x(k) - ix_) <= r_ && std::abs(key_y(k) - iz_) <= r_;
	}
	std::unordered_set<int64_t> cand_;
	std::vector<std::pair<float, int64_t>> want_;
	bool window_ok_ = false, moved_ = true;
	int r_ = 0, ix_ = 0, iz_ = 0;
	float cx_ = 0, cz_ = 0, reach_ = 0;
};

} // namespace city
