// The per-chunk job queues. A job builds a chunk's surfaces on a worker (submit_mesh_job: while it holds the map
// shared). Converting them into meshes (bake) runs on a worker too, but the RenderingServer runs a worker's upload at
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

// Build keys stay within +-2^56 (their high bits all repeat the sign), so flipping bit 62 gives a bake a key of its
// own, never a build's; a chunk has one bake at a time, so no bake is ever replaced in the pool.
constexpr int64_t BAKE_KEY_BIT = int64_t(1) << 62;

// A road or furniture chunk's result: the builder its job filled, then the mesh baked from it.
struct BuiltMesh {
	int64_t key = 0, job = 0;
	float priority = 0;
	uint64_t seq = 0;
	MeshBuilder mb;
	OwnedMesh mesh;
	bool baked = false;
	int64_t bytes() const { return mb.bytes(); }
	void bake() {
		mesh = mb.bake();
		mb = MeshBuilder();
		baked = true;
	}
};

// `build(mb)` fills the chunk's builder from the map.
template <typename Build>
void submit_mesh_job(const CityState &s, JobPool &pool, int64_t job, int64_t key, float priority, std::mutex &mutex,
		std::vector<BuiltMesh> &done, std::unordered_map<int64_t, uint64_t> &requested, uint64_t &seq_counter, Build &&build) {
	uint64_t seq;
	{
		std::lock_guard<std::mutex> lk(mutex);
		seq = ++seq_counter;
		requested[key] = seq;
	}
	pool.submit(job, priority, [&s, &mutex, &done, key, job, seq, priority, build]() {
		BuiltMesh b;
		b.key = key;
		b.job = job;
		b.seq = seq;
		b.priority = priority;
		{
			std::shared_lock<std::shared_mutex> map(s.map_mutex);
			build(b.mb);
		}
		std::lock_guard<std::mutex> lk(mutex);
		done.push_back(std::move(b));
	});
}

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

// Built: key, job, priority, seq, baked, bytes() and bake() (BuiltMesh, BuildingRenderer::Built / SuperBuilt).
// `waiting` gets the bytes of the built results left waiting for the window (RenderWorld holds new builds back while
// they pile up: their shells are CPU memory).
template <typename Built, typename Commit>
void commit_baked(JobPool &pool, std::mutex &mutex, std::vector<Built> &done, std::unordered_map<int64_t, uint64_t> &requested,
		BakingSet &baking, UploadBudget &upload, const std::chrono::steady_clock::time_point &until, int64_t &waiting,
		Commit &&commit_one) {
	std::vector<Built> local, later;
	waiting = 0;
	{
		std::lock_guard<std::mutex> lk(mutex);
		local.swap(done);
	}
	std::sort(local.begin(), local.end(), [](const Built &a, const Built &b) { return a.priority < b.priority; });
	for (Built &b : local) {
		if (b.baked) { // back from its bake: its upload is queued for the next call into the RenderingServer
			auto it = baking.find(b.key);
			if (it != baking.end() && it->second.seq == b.seq) baking.erase(it);
		}
		if (!fresh(mutex, requested, b.key, b.seq)) continue; // a newer build is on its way, or the chunk left
		if (!b.baked) {
			const int64_t bytes = b.bytes();
			if (baking.count(b.key) || !upload.take(bytes)) {
				waiting += bytes;
				later.push_back(std::move(b));
				continue;
			}
			baking[b.key] = Baking{ b.seq, bytes };
			// std::function needs a copyable callable: the move-only result rides in a shared_ptr
			auto job = std::make_shared<Built>(std::move(b));
			pool.submit(job->job ^ BAKE_KEY_BIT, job->priority, [job, &mutex, &done]() {
				job->bake();
				std::lock_guard<std::mutex> lk(mutex);
				done.push_back(std::move(*job));
			});
		} else if (std::chrono::steady_clock::now() < until) {
			{
				std::lock_guard<std::mutex> lk(mutex);
				requested.erase(b.key);
			}
			commit_one(b);
		} else {
			later.push_back(std::move(b));
		}
	}
	std::lock_guard<std::mutex> lk(mutex);
	for (Built &b : later) done.push_back(std::move(b));
}

// The queue of a renderer whose chunk results are one surface each (roads, furniture): its jobs, the bakes it admits
// and their commit. The renderer fills a chunk's builder (submit_job) and hands the baked mesh to its instance
// (commit_one).
class MeshQueue {
public:
	int pending() const {
		std::lock_guard<std::mutex> lk(mutex_);
		return int(requested_.size() + done_.size());
	}
	int64_t baking() const { return baking_bytes(baking_); } // bytes of its bakes still on a worker
	int64_t waiting() const { return waiting_; } // bytes of its built results waiting to bake (last commit)
	void commit(JobPool &pool, UploadBudget &upload, const std::chrono::steady_clock::time_point &until) {
		commit_baked(pool, mutex_, done_, requested_, baking_, upload, until, waiting_, [this](BuiltMesh &b) { commit_one(b); });
	}

protected:
	virtual ~MeshQueue() = default;
	virtual void commit_one(BuiltMesh &b) = 0;
	template <typename Build>
	void submit_job(const CityState &s, JobPool &pool, int64_t job, int64_t key, float priority, Build &&build) {
		submit_mesh_job(s, pool, job, key, priority, mutex_, done_, requested_, seq_, std::forward<Build>(build));
	}
	void forget(int64_t key) { // the chunk left: a result still on its way is dropped
		std::lock_guard<std::mutex> lk(mutex_);
		requested_.erase(key);
	}
	void clear_queue() {
		{
			std::lock_guard<std::mutex> lk(mutex_);
			done_.clear();
			requested_.clear();
		}
		baking_.clear();
		waiting_ = 0;
	}

private:
	mutable std::mutex mutex_;
	std::vector<BuiltMesh> done_;
	std::unordered_map<int64_t, uint64_t> requested_;
	BakingSet baking_;
	int64_t waiting_ = 0;
	uint64_t seq_ = 0;
};

} // namespace city
