// The one owner of what a frame admits (render.commit_budget_ms, render.max_chunk_rebuilds_per_frame, render.upload_budget_mb):
// one deadline for the whole render phase, one count of build jobs, one byte ledger for every upload to the RenderingServer, and
// one priority order for the units of every producer. RenderWorld::sync makes it at its top and hands it to each producer.
#pragma once

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <vector>

#include <godot_cpp/classes/rendering_server.hpp>

namespace city {

// One per-frame count of build jobs shared by every streamer and level builder: each submitted job takes one.
struct BuildBudget {
	int left = 0;
	explicit BuildBudget(int n) : left(n) {}
	// For a stream whose build only registers the chunk (no mesh work): every chunk in reach is handed over at once.
	static BuildBudget unlimited() { return BuildBudget(std::numeric_limits<int>::max()); }
	bool take() { return take(1); }
	bool take(int jobs) { // all of a unit's jobs, or none
		if (left < jobs) return false;
		left -= jobs;
		return true;
	}
};

// This frame's bytes in the upload window: render.upload_budget_mb less the bytes still baking (admitted earlier, their upload
// not yet run) and less the debt an earlier frame left. Every upload pays here: a deferrable one (a mesh to bake, a buffer a
// result hands over) only within what is left; a mandatory one (a terrain region an edit changed) always, into debt if it must.
// A deferrable unit larger than the whole window goes alone on a whole ledger (an upload frame, nothing baking, no debt, nothing
// paid yet) and its excess is the next frames' debt: the window holds as an average, never more than one unit over it.
struct UploadBudget {
	int64_t window = 0, left = 0, used = 0, debt = 0;
	bool whole = false;
	UploadBudget(int64_t window_bytes, int64_t baking, int64_t debt_bytes = 0)
			: window(window_bytes), left(window_bytes - baking - debt_bytes), debt(debt_bytes), whole(window_bytes > 0 && baking == 0 && debt_bytes == 0) {}
	bool take(int64_t bytes) {
		if (bytes > left && !(whole && used == 0)) return false;
		left -= bytes;
		used += bytes;
		return true;
	}
	void charge(int64_t bytes) {
		left -= bytes;
		used += bytes;
	}
	// What the next frame starts owing.
	int64_t debt_after() const { return std::max<int64_t>(0, debt + used - window); }
};

// A producer's units ready to act on this frame, in one order with every other producer's: rank first (0: nothing drawn in the
// unit's place yet, a hole; 1: something stands there meanwhile), then the producer's own priority (distance to the eye).
class FrameAdmission;

class AdmittedQueue {
public:
	struct Offer {
		int rank = 1;
		float priority = 0;
		AdmittedQueue *queue = nullptr;
		size_t index = 0;
		bool operator<(const Offer &o) const {
			if (rank != o.rank) return rank < o.rank;
			return priority < o.priority;
		}
	};
	virtual ~AdmittedQueue() = default;
	virtual void intake() = 0; // the results the workers finished, the stale retired (outside the queue's lock)
	virtual void offer_bakes(std::vector<Offer> &) {} // results waiting for an upload's bake
	virtual void offer_ready(std::vector<Offer> &out) = 0; // results ready to hand to their instances
	virtual void bake(size_t, FrameAdmission &) {} // admits one bake if the ledger has room
	virtual void hand(size_t index, FrameAdmission &f) = 0; // hands one result over (pays its main-thread uploads)
	virtual void settle() = 0; // the acted ones leave the queue's lists
};

class FrameAdmission {
public:
	using clock = std::chrono::steady_clock;
	FrameAdmission(clock::time_point deadline, int credits, UploadBudget bytes) : until(deadline), build(credits), upload(bytes) {}
	clock::time_point until;
	BuildBudget build;
	UploadBudget upload;
	bool due() const { return clock::now() < until; }
	// The frame's one flush of the RenderingServer's queue: a worker's bake converts its arrays on the worker and queues only the
	// upload, which the server runs at the main thread's next call into it (Godot server_wrap_mt_common.h: flush_if_pending). The
	// admitted bakes that finished since the last frame (their bytes paid when admitted) run their uploads here, at a fixed point
	// inside the deadline, and the time is the frame's uploads (RenderingServer::force_sync, the call Godot binds for it: the queue
	// synced with the render thread, or flushed without one). Returns milliseconds.
	double flush_uploads() const {
		const clock::time_point at = clock::now();
		godot::RenderingServer::get_singleton()->force_sync();
		return std::chrono::duration<double, std::milli>(clock::now() - at).count();
	}

	// A build unit offered by a producer: `jobs` credits, done by `go`.
	struct Unit {
		int rank = 1;
		float distance = 0;
		int jobs = 1;
		std::function<void()> go;
	};
	// Every producer's build units of this frame in one order, each submitted while the credits and the deadline last.
	// Returns how many were left unsubmitted.
	int submit(std::vector<Unit> &units) {
		std::stable_sort(units.begin(), units.end(), [](const Unit &a, const Unit &b) {
			if (a.rank != b.rank) return a.rank < b.rank;
			return a.distance < b.distance;
		});
		size_t n = 0;
		for (; n < units.size(); n++) {
			if (!due() || !build.take(units[n].jobs)) break;
			units[n].go();
		}
		return int(units.size() - n);
	}

	// Every queue's results in one order: the bakes the ledger admits, then the hand-overs while the deadline lasts. `ms`, when
	// given (one per queue), gets the milliseconds each queue's hand-overs took (render_cost probe).
	void commit(const std::vector<AdmittedQueue *> &queues, std::vector<double> *ms = nullptr) {
		for (AdmittedQueue *q : queues) q->intake();
		offers_.clear();
		for (AdmittedQueue *q : queues) q->offer_bakes(offers_);
		std::stable_sort(offers_.begin(), offers_.end());
		for (const AdmittedQueue::Offer &o : offers_) o.queue->bake(o.index, *this);
		offers_.clear();
		for (AdmittedQueue *q : queues) q->offer_ready(offers_);
		std::stable_sort(offers_.begin(), offers_.end());
		if (ms) ms->assign(queues.size(), 0.0);
		for (const AdmittedQueue::Offer &o : offers_) {
			const clock::time_point at = clock::now();
			if (at >= until) break;
			o.queue->hand(o.index, *this);
			if (!ms) continue;
			const size_t q = size_t(std::find(queues.begin(), queues.end(), o.queue) - queues.begin());
			(*ms)[q] += std::chrono::duration<double, std::milli>(clock::now() - at).count();
		}
		for (AdmittedQueue *q : queues) q->settle();
	}

private:
	std::vector<AdmittedQueue::Offer> offers_;
};

} // namespace city
