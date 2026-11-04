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
using BakingSet = std::unordered_map<int64_t, Baking>;

inline int64_t baking_bytes(const BakingSet &set) {
	int64_t n = 0;
	for (const auto &kv : set) n += kv.second.bytes;
	return n;
}

// This frame's room in the upload window: render.upload_budget_mb less the bytes still baking (each queue's BakingSet,
// recounted every frame). A result larger than the whole window goes alone, when nothing else is baking.
struct UploadBudget {
	int64_t left = 0, used = 0;
	bool idle = false;
	UploadBudget(int64_t window, int64_t baking) : left(window - baking), idle(baking == 0) {}
	bool take(int64_t bytes) {
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
void commit_done(std::mutex &mutex, std::vector<Built> &done, std::unordered_map<int64_t, uint64_t> &requested,
		const std::chrono::steady_clock::time_point &until, Commit &&commit_one) {
	std::vector<Built> local;
	{
		std::lock_guard<std::mutex> lk(mutex);
		local.swap(done);
	}
	std::sort(local.begin(), local.end(), [](const Built &a, const Built &b) { return a.priority < b.priority; });
	for (Built &b : local) {
		if (std::chrono::steady_clock::now() >= until) {
			std::lock_guard<std::mutex> lk(mutex);
			done.push_back(std::move(b));
			continue;
		}
		if (!fresh(mutex, requested, b.key, b.seq)) continue;
		{
			std::lock_guard<std::mutex> lk(mutex);
			requested.erase(b.key);
		}
		commit_one(b);
	}
}

// Built: key, job, priority, seq, baked, bytes() and bake() (BuiltMesh, BuildingRenderer::Built / SuperBuilt). The
// queue holds pointers: a building result is heavy (its shells, then its meshes) and a backlog of thousands is sorted
// every frame after a camera jump. `waiting` gets the bytes of the built results left waiting for the window
// (RenderWorld holds new builds back while they pile up: their shells are CPU memory).
template <typename Built>
using Results = std::vector<std::shared_ptr<Built>>;

// Where a commit_baked call spends its time, ms, summed (render_cost probe): the locked scan, freeing the stale, the
// sort, the admissions, the hand-over of baked results to their instances and the tail (the handed results freed, the
// rest put back under the lock).
struct CommitTimes {
	double scan = 0, freed = 0, sort = 0, admit = 0, hand = 0, tail = 0;
};

template <typename Built, typename Commit>
void commit_baked(JobPool &pool, std::mutex &mutex, Results<Built> &done, std::unordered_map<int64_t, uint64_t> &requested,
		BakingSet &baking, UploadBudget &upload, const std::chrono::steady_clock::time_point &until, int64_t &waiting,
		Commit &&commit_one, CommitTimes *times = nullptr) {
	Results<Built> local, later, stale;
	waiting = 0;
	using clock = std::chrono::steady_clock;
	auto mark = clock::now();
	auto lap = [&](double CommitTimes::*part) {
		const auto now = clock::now();
		if (times) times->*part += std::chrono::duration<double, std::milli>(now - mark).count();
		mark = now;
	};
	{
		std::lock_guard<std::mutex> lk(mutex);
		local.swap(done);
		for (std::shared_ptr<Built> &b : local) {
			if (b->baked) { // back from its bake: its upload is queued for the next call into the RenderingServer
				auto it = baking.find(b->key);
				if (it != baking.end() && it->second.seq == b->seq) baking.erase(it);
			}
			auto it = requested.find(b->key);
			if (it == requested.end() || it->second != b->seq) stale.push_back(std::move(b)); // newer on its way, or gone
		}
	}
	lap(&CommitTimes::scan);
	stale.clear(); // freed outside the lock
	lap(&CommitTimes::freed);
	local.erase(std::remove(local.begin(), local.end(), nullptr), local.end());
	std::sort(local.begin(), local.end(), [](const std::shared_ptr<Built> &a, const std::shared_ptr<Built> &b) {
		return a->priority < b->priority;
	});
	lap(&CommitTimes::sort);
	for (std::shared_ptr<Built> &b : local) {
		if (!b->baked) {
			const int64_t bytes = b->bytes();
			if (baking.count(b->key) || !upload.take(bytes)) {
				waiting += bytes;
				later.push_back(std::move(b));
				continue;
			}
			baking[b->key] = Baking{ b->seq, bytes };
			pool.submit(b->job.bake(), b->priority, [job = std::move(b), &mutex, &done]() {
				job->bake();
				std::lock_guard<std::mutex> lk(mutex);
				done.push_back(job);
			});
			lap(&CommitTimes::admit);
		} else if (std::chrono::steady_clock::now() < until) {
			{
				std::lock_guard<std::mutex> lk(mutex);
				requested.erase(b->key);
			}
			commit_one(*b);
			lap(&CommitTimes::hand);
		} else {
			later.push_back(std::move(b));
		}
	}
	local.clear(); // the handed results (their meshes went to the instances)
	{
		std::lock_guard<std::mutex> lk(mutex);
		for (std::shared_ptr<Built> &b : later) done.push_back(std::move(b));
	}
	lap(&CommitTimes::tail);
}

} // namespace city
