// Priority job pool: a fixed set of worker threads, jobs ordered by priority, a job
// of the same key replacing a queued older one, and a blocking parallel_for for the lot cache. A key is a lane and an id: two
// users of the pool cannot replace each other's jobs, whatever their ids, because each has a lane of its own. parallel_for helpers
// live outside the keyed queue (no key to collide with or be superseded by) and the caller works too, so it finishes
// even when every worker is parked on a lock the caller holds (mesh jobs wait for map_mutex during a command).
// The heap holds only (priority, seq, key); a key's queued work lives in its slot, so a newer submit destroys the older
// closure at once (outside the pool's lock) instead of keeping it until the heap reaches it. A running job asks current()
// at its natural boundaries and returns early once a newer job of its key was submitted.
#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

namespace city {

// One lane for each user of the pool. A new user adds its own: never shares one, never does arithmetic on an id to stay clear of another.
enum class JobLane : uint8_t { ROADS, ROADS_FAR, VERGES, STRUCTURES, PROPS, BUILDING, BUILDING_SUPER, TREES_NEAR, TREES_FAR, COVER, ZONES, FOG, CANYON, SHORE, RUINS, WATER, SIGNATURES, PROBE_HOLD, PROBE_KEY, PROBE_OTHER }; // the PROBE_ lanes: the pool's own probe (city_core_jobs.cpp)

// What a job is the latest of. `baked`: the second job of a result, the upload's conversion of what the first built.
struct JobKey {
	JobLane lane = JobLane::ROADS;
	int64_t id = 0;
	bool baked = false;
	bool operator==(const JobKey &o) const { return lane == o.lane && id == o.id && baked == o.baked; }
	JobKey bake() const { return JobKey{ lane, id, true }; }
};

struct JobKeyHash {
	size_t operator()(const JobKey &k) const {
		return std::hash<int64_t>()(k.id) ^ (size_t(k.lane) << 1 | size_t(k.baked)) * 0x9e3779b97f4a7c15ull;
	}
};

class JobPool {
public:
	explicit JobPool(unsigned threads = 0);
	~JobPool();
	// Lower priority runs first. Returns the job's seq (what current() compares).
	uint64_t submit(JobKey key, float priority, std::function<void()> work);
	void parallel_for(size_t n, const std::function<void(size_t)> &fn); // blocks until done
	void wait_idle(); // blocks until the queue is empty and no worker is running
	// Hands out no more work and returns once none runs, until resume(): what the caller changes then, no job sees half done (the
	// city taken in whole, catalogue and tables with it, which some jobs read outside the map lock). parallel_for still finishes,
	// on the caller alone; wait_idle must not be asked in between.
	void pause();
	void resume();
	size_t tracked(); // keys with a job still queued: none once the pool is idle (a key is forgotten when its newest job is taken)
	uint64_t replaced() const { return replaced_.load(); } // queued jobs a newer one of their key took the place of
	// The job `seq` of `key` is still the newest submitted for its key (a running job polls it to stop early).
	bool current(JobKey key, uint64_t seq);
	// The same for the job running on the calling thread (true outside a keyed job).
	bool running_current();

private:
	struct Item {
		float priority = 0;
		uint64_t seq = 0;
		JobKey key;
	};
	struct Later {
		bool operator()(const Item &a, const Item &b) const {
			if (a.priority != b.priority) return a.priority > b.priority;
			return a.seq > b.seq;
		}
	};
	// A key's newest queued job: its seq and its work (the slot goes when a worker takes it).
	struct Slot {
		uint64_t seq = 0;
		std::function<void()> work;
	};
	void loop();
	bool idle() const { return q_.empty() && helpers_.empty() && busy_ == 0; }
	std::vector<std::thread> threads_;
	std::mutex m_;
	std::condition_variable cv_, idle_cv_;
	std::priority_queue<Item, std::vector<Item>, Later> q_;
	std::deque<std::function<void()>> helpers_; // parallel_for helpers, taken before any keyed job
	size_t busy_ = 0;
	bool paused_ = false;
	std::unordered_map<JobKey, Slot, JobKeyHash> slots_;
	// Each key's newest submitted seq while that job is queued or running (current()): gone when it finishes.
	std::unordered_map<JobKey, uint64_t, JobKeyHash> newest_;
	std::atomic<uint64_t> replaced_{ 0 };
	uint64_t seq_ = 0;
	bool stop_ = false;
};

} // namespace city
