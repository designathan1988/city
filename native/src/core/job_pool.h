// Priority job pool: a fixed set of worker threads, jobs ordered by priority, a job
// of the same key replacing a queued older one, and a blocking parallel_for for the lot cache. A key is a lane and an id: two
// users of the pool cannot replace each other's jobs, whatever their ids, because each has a lane of its own. parallel_for helpers
// live outside the keyed queue (no key to collide with or be superseded by) and the caller works too, so it finishes
// even when every worker is parked on a lock the caller holds (mesh jobs wait for map_mutex during a command).
#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace city {

// One lane for each user of the pool. A new user adds its own: never shares one, never does arithmetic on an id to stay clear of another.
enum class JobLane : uint8_t { ROADS, ROADS_FAR, VERGES, STRUCTURES, PROPS, BUILDING, BUILDING_SUPER, TREES_NEAR, TREES_FAR, COVER, ZONES, FOG, CANYON, SHORE, RUINS };

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
	void submit(JobKey key, float priority, std::function<void()> work); // lower priority runs first
	void cancel(JobKey key); // remove queued captures now; running work stops at its next checkpoint
	void cancel_all(); // keyed jobs only; parallel_for helpers must complete their batch
	static bool cancelled(); // the current keyed worker's cancellation checkpoint; false for helpers/callers
	void parallel_for(size_t n, const std::function<void(size_t)> &fn); // blocks until done
	void wait_idle(); // blocks until the queue is empty and no worker is running
	bool drained(); // nonblocking retirement query; caller has already stopped submitting to this pool
	size_t tracked(); // keys with a job still queued: none once the pool is idle (a key is forgotten when its newest job is taken)
	uint64_t replaced() const { return replaced_.load(); } // queued jobs a newer one of their key took the place of

private:
	struct Item {
		float priority = 0;
		uint64_t seq = 0;
		JobKey key;
		std::function<void()> work;
		std::shared_ptr<std::atomic<bool>> cancelled;
	};
	struct Later {
		bool operator()(const Item &a, const Item &b) const {
			if (a.priority != b.priority) return a.priority > b.priority;
			return a.seq > b.seq;
		}
	};
	void loop();
	void swap_items(size_t a, size_t b);
	void sift_up(size_t at);
	void sift_down(size_t at);
	Item remove_at(size_t at); // caller holds m_; destroy the returned captures after unlocking
	bool idle() const { return q_.empty() && helpers_.empty() && busy_ == 0; }
	std::vector<std::thread> threads_;
	std::mutex m_;
	std::condition_variable cv_, idle_cv_;
	std::vector<Item> q_;
	std::unordered_map<JobKey, size_t, JobKeyHash> queued_;
	std::deque<std::function<void()>> helpers_; // parallel_for helpers, taken before any keyed job
	size_t busy_ = 0;
	std::unordered_map<JobKey, std::shared_ptr<std::atomic<bool>>, JobKeyHash> current_;
	std::atomic<uint64_t> replaced_{ 0 };
	uint64_t seq_ = 0;
	bool stop_ = false;
};

} // namespace city
