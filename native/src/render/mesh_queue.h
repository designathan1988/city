// The job queue of a renderer whose chunk results are one surface each (roads, furniture): a job fills the chunk's
// builder while it holds the map shared (submit_mesh_job), its bake and hand-over go through chunk_jobs.h commit_baked.
#pragma once

#include "render/chunk_jobs.h"
#include "render/job_request.h"

namespace city {

// What a chunk's mesh holds, kept once the builder is gone (the probes count it).
struct MeshCount {
	int64_t triangles = 0, vertices = 0, bytes = 0;
};

// Optional producer-owned CPU data with exactly the same lifetime and sequence as the mesh result.
struct MeshPayload {
	virtual ~MeshPayload() = default;
};

// A road or furniture chunk's result: the builder its job filled, then the mesh baked from it.
struct BuiltMesh {
	int64_t key = 0;
	JobKey job;
	float priority = 0;
	uint64_t seq = 0;
	MeshBuilder mb;
	OwnedMesh mesh;
	MeshCount count;
	std::unique_ptr<MeshPayload> payload;
	bool baked = false;
	int64_t bytes() const { return mb.bytes(); }
	void bake() {
		mesh = mb.bake();
		count = { mb.idx.size() / 3, mb.v.size(), mb.bytes() };
		mb = MeshBuilder();
		baked = true;
	}
};

// `build(mb)` fills the chunk's builder from the map.
template <typename Build>
void submit_mesh_job(const CityState &s, JobPool &pool, JobKey job, int64_t key, float priority, std::mutex &mutex,
		Results<BuiltMesh> &done, std::unordered_map<int64_t, uint64_t> &requested, uint64_t &seq_counter,
		Build &&build) {
	uint64_t seq;
	{
		std::lock_guard<std::mutex> lk(mutex);
		seq = ++seq_counter;
		requested[key] = seq;
	}
	pool.submit(job, priority, [&s, &mutex, &done, key, job, seq, priority, build]() {
		auto b = std::make_shared<BuiltMesh>();
		b->key = key;
		b->job = job;
		b->seq = seq;
		b->priority = priority;
		{
			std::shared_lock<std::shared_mutex> map(s.map_mutex);
			if (JobPool::cancelled()) return;
			build(*b);
		}
		if (JobPool::cancelled()) return;
		std::lock_guard<std::mutex> lk(mutex);
		done.push_back(std::move(b));
	});
}

// The queue of a renderer whose chunk results are one surface each (roads, furniture): its jobs, the bakes it admits
// and their commit. The renderer fills a chunk's builder (submit_job) and hands the baked mesh to its instance
// (commit_one).
class MeshQueue {
public:
	void invalidate(JobPool *pool, JobKey job) { cancel_request(pool, job, mutex_, requested_); }
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
	void submit_job(const CityState &s, JobPool &pool, JobKey job, int64_t key, float priority, Build &&build) {
		submit_result(s, pool, job, key, priority, [build = std::forward<Build>(build)](BuiltMesh &result) { build(result.mb); });
	}
	template <typename Build>
	void submit_result(const CityState &s, JobPool &pool, JobKey job, int64_t key, float priority, Build &&build) {
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
	Results<BuiltMesh> done_;
	std::unordered_map<int64_t, uint64_t> requested_;
	BakingSet baking_;
	int64_t waiting_ = 0;
	uint64_t seq_ = 0;
};

} // namespace city
