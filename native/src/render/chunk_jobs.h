// The per-chunk job queues. A job builds a chunk's surfaces on a worker (while it holds the map shared; the
// one-surface queue of roads and furniture is render/mesh_queue.h). Converting them into meshes (bake) runs on a worker too, but the RenderingServer runs a worker's upload at
// the main thread's next call into it (Godot server_wrap_mt_common.h: flush_if_pending), outside any budget of ours, and
// bakes that started in different frames can finish together: so commit_baked admits a bake only while the bytes still
// baking leave room in the upload window (UploadBudget), one bake per chunk at a time, then hands baked results to
// their instances within the frame's time budget. commit_done is the time budget alone (results with nothing to
// upload). A result whose chunk was rebuilt meanwhile (newer seq) or dropped is discarded at every step.
#pragma once

#include "core/job_pool.h"
#include "render/mesh_builder.h"
#include "render/chunk_results.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace city {

// The bakes of a queue still on a worker (main thread only): per chunk, the result's seq and bytes.
struct Baking {
	uint64_t seq = 0;
	int64_t bytes = 0;
};
class BakingSet {
public:
	bool contains(int64_t key) const { return entries_.find(key) != entries_.end(); }
	void set(int64_t key, Baking value) {
		const auto found = entries_.find(key);
		if (found != entries_.end()) bytes_ -= found->second.bytes;
		entries_[key] = value; bytes_ += value.bytes;
	}
	void erase(int64_t key, uint64_t seq) {
		const auto found = entries_.find(key);
		if (found != entries_.end() && found->second.seq == seq) { bytes_ -= found->second.bytes; entries_.erase(found); }
	}
	int64_t bytes() const { return bytes_; }
	void clear() { entries_.clear(); bytes_ = 0; }
private:
	std::unordered_map<int64_t, Baking> entries_;
	int64_t bytes_ = 0;
};
inline int64_t baking_bytes(const BakingSet &set) { return set.bytes(); }

// This frame's room in the upload window: render.upload_budget_mb less the bytes still baking (each queue's BakingSet,
// maintained by its owner). A result larger than the whole window goes alone, when nothing else is baking.
struct UploadBudget {
	int64_t left = 0, used = 0;
	bool idle = false, enabled = false;
	UploadBudget(int64_t window, int64_t baking) : left(window - baking), idle(baking == 0), enabled(window > 0) {}
	bool take(int64_t bytes) {
		if (!enabled || bytes < 0) return false; // zero/cadence-skipped frames admit no bake
		if (bytes > left && !(idle && used == 0)) return false;
		left -= bytes;
		used += bytes;
		return true;
	}
};

// The result is still the newest one asked for its chunk.
inline bool fresh(std::mutex &mutex, const std::unordered_map<int64_t, uint64_t> &requested, int64_t key, uint64_t seq) {
	std::lock_guard<std::mutex> lk(mutex);
	auto it = requested.find(key);
	return it != requested.end() && it->second == seq;
}

template <typename Built, typename Commit>
void commit_done(std::mutex &mutex, CompletedResults<Built> &done, std::unordered_map<int64_t, uint64_t> &requested,
		const std::chrono::steady_clock::time_point &until, Commit &&commit_one) {
	using clock = std::chrono::steady_clock;
	while (clock::now() < until) {
		typename CompletedResults<Built>::Entry entry;
		{
			std::lock_guard<std::mutex> lk(mutex);
			if (!done.take(entry)) break;
		}
		if (clock::now() >= until) {
			std::lock_guard<std::mutex> lk(mutex); done.restore(std::move(entry)); break;
		}
		bool current;
		{
			std::lock_guard<std::mutex> lk(mutex);
			const auto found = requested.find(entry.value.key);
			current = found != requested.end() && found->second == entry.value.seq;
			if (current) requested.erase(found);
		}
		if (current) commit_one(entry.value);
		// Stale/committed values retire individually, outside the queue lock.
	}
}

// Built: key, job, priority, seq, baked, bytes() and bake() (BuiltMesh, BuildingRenderer::Built / SuperBuilt). The
// queue stores ordered completion pointers and cached CPU byte totals; a frame processes one result per deadline check.
// `waiting` reads queued CPU bytes without a scan. Bakes still call RenderingServer on workers until the S04 GPU migration.
// Commit-only time buckets. scan inspects individual queued results, freed releases a stale result outside its lock,
// admit/hand perform one stage; sort stays zero because ordering happens at publication, and tail reads cached totals.
// The pool releases queued/skipped/executed closures outside its lock. This receipt publishes one terminal
// bake acknowledgement even when work never runs. Only never-started inputs can safely return to the CPU queue.
// Current bake methods run to completion once entered; future sliced bakes must carry an explicit continuation.
template<class Built> struct BakeReceipt {
	std::mutex &mutex;
	Results<Built> &done;
	std::shared_ptr<Built> job;
	bool started = false;
	BakeReceipt(std::mutex &lock, Results<Built> &output, std::shared_ptr<Built> value) : mutex(lock), done(output), job(std::move(value)) {}
	BakeReceipt(const BakeReceipt &) = delete;
	BakeReceipt &operator=(const BakeReceipt &) = delete;
	~BakeReceipt() {
		std::lock_guard<std::mutex> lock(mutex);
		done.push_bake_result(std::move(job), !started);
	}
};

struct CommitTimes {
	double scan = 0, freed = 0, sort = 0, admit = 0, hand = 0, tail = 0;
};

template <typename Built, typename Commit>
void commit_baked(JobPool &pool, std::mutex &mutex, Results<Built> &done, std::unordered_map<int64_t, uint64_t> &requested,
		BakingSet &baking, UploadBudget &upload, const std::chrono::steady_clock::time_point &until, int64_t &waiting,
		Commit &&commit_one, CommitTimes *times = nullptr) {
	using clock = std::chrono::steady_clock;
	auto mark = clock::now();
	auto lap = [&](double CommitTimes::*part) {
		const auto now = clock::now();
		if (times) times->*part += std::chrono::duration<double, std::milli>(now - mark).count();
		mark = now;
	};
	{ std::lock_guard<std::mutex> lk(mutex); done.begin_cycle(); }
	while (clock::now() < until) {
		typename Results<Built>::Entry entry;
		bool fresh_result = false;
		{
			std::lock_guard<std::mutex> lk(mutex);
			if (!done.take(entry)) break;
			const auto found = requested.find(entry.value->key);
			fresh_result = found != requested.end() && found->second == entry.value->seq;
		}
		if (clock::now() >= until) {
			std::lock_guard<std::mutex> lk(mutex); done.defer(std::move(entry)); break;
		}
		std::shared_ptr<Built> &b = entry.value;
		if (entry.baked) baking.erase(b->key, b->seq); // acknowledge even a stale completed bake
		lap(&CommitTimes::scan);
		if (!fresh_result) {
			b.reset(); // resource destruction remains outside metadata locks, one result at a time
			lap(&CommitTimes::freed); continue;
		}
		if (entry.retryable) {
			entry.baked = entry.retryable = false;
			entry.bytes = b->bytes(); // the bake never touched these CPU inputs
			{ std::lock_guard<std::mutex> lk(mutex); done.defer(std::move(entry)); }
			lap(&CommitTimes::admit); continue;
		}
		if (!entry.baked) {
			if (baking.contains(b->key) || !upload.take(entry.bytes)) {
				std::lock_guard<std::mutex> lk(mutex); done.defer(std::move(entry));
			} else {
				baking.set(b->key, Baking{b->seq, entry.bytes});
				const JobKey job_key = b->job.bake(); const float priority = entry.priority;
				auto receipt = std::make_shared<BakeReceipt<Built>>(mutex, done, std::move(b));
				pool.submit(job_key, priority, [receipt]() {
					if (JobPool::cancelled()) return;
					receipt->started = true;
					receipt->job->bake();
				});
			}
			lap(&CommitTimes::admit);
		} else {
			{
				std::lock_guard<std::mutex> lk(mutex); requested.erase(b->key);
			}
			commit_one(*b); b.reset();
			lap(&CommitTimes::hand);
		}
	}
	{ std::lock_guard<std::mutex> lk(mutex); waiting = done.waiting(); }
	lap(&CommitTimes::tail);
}

} // namespace city
