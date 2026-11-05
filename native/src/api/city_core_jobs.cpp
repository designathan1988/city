// The job pool for the tests: its rule, seen on a pool of its own.
#include "api/city_core.h"

#include "core/job_pool.h"

#include <atomic>
#include <mutex>

using namespace godot;

namespace city {

// The pool's rule on a pool of one worker, held busy while the jobs queue: a key's newest job replaces a queued one, and only that.
Dictionary CityCore::job_pool_probe() const {
	constexpr int64_t CHUNK_ID = 7;
	std::mutex gate;
	std::atomic<int> older{ 0 }, newer{ 0 }, other_lane{ 0 };
	JobPool pool(1);
	gate.lock();
	pool.submit(JobKey{ JobLane::FOG, 0 }, 0.f, [&gate]() { std::lock_guard<std::mutex> hold(gate); });
	pool.submit(JobKey{ JobLane::BUILDING, CHUNK_ID }, 1.f, [&older]() { older++; });
	pool.submit(JobKey{ JobLane::COVER, CHUNK_ID }, 1.f, [&other_lane]() { other_lane++; });
	pool.submit(JobKey{ JobLane::BUILDING, CHUNK_ID }, 1.f, [&newer]() { newer++; });
	gate.unlock();
	pool.wait_idle();
	Dictionary d;
	d["older"] = older.load();
	d["newer"] = newer.load();
	d["other_lane"] = other_lane.load();
	d["replaced"] = int64_t(pool.replaced());
	d["keys_left"] = int64_t(pool.tracked()); // the pool forgets a key once its job ran: its bookkeeping does not grow with the map
	d["game_keys"] = int64_t(state_->jobs->tracked()); // the same of the game's pool now: the keys with a job queued
	return d;
}

} // namespace city
