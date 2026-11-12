// Retire request identity before cancelling captures: their finalizers may publish under the same queue mutex.
#pragma once
#include "core/job_pool.h"
#include <cstdint>
#include <mutex>
#include <unordered_map>

namespace city {
// Main-thread desired-state transition. Does not destroy the displayed representation or consume build credits.
inline void cancel_request(JobPool *pool, JobKey job, std::mutex &mutex, std::unordered_map<int64_t, uint64_t> &requested) {
	{ std::lock_guard<std::mutex> lock(mutex); requested.erase(job.id); }
	// JobPool destroys removed closures outside its own lock; never hold the result lock across this call.
	if (pool) { pool->cancel(job); pool->cancel(job.bake()); }
}
} // namespace city
