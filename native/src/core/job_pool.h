// Priority job pool (roadcraft-novo jobs/pool.ts): a fixed set of worker threads, jobs ordered by priority, a job
// of the same key replacing a queued older one, and a blocking parallel_for for the lot cache. parallel_for helpers
// live outside the keyed queue (no key to collide with or be superseded by) and the caller works too, so it finishes
// even when every worker is parked on a lock the caller holds (mesh jobs wait for map_mutex during a command).
#pragma once

#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

namespace city {

class JobPool {
public:
	explicit JobPool(unsigned threads = 0);
	~JobPool();
	void submit(int64_t key, float priority, std::function<void()> work); // lower priority runs first
	void parallel_for(size_t n, const std::function<void(size_t)> &fn); // blocks until done
	void wait_idle(); // blocks until the queue is empty and no worker is running

private:
	struct Item {
		float priority = 0;
		uint64_t seq = 0;
		int64_t key = 0;
		std::function<void()> work;
	};
	struct Later {
		bool operator()(const Item &a, const Item &b) const {
			if (a.priority != b.priority) return a.priority > b.priority;
			return a.seq > b.seq;
		}
	};
	void loop();
	bool idle() const { return q_.empty() && helpers_.empty() && busy_ == 0; }
	std::vector<std::thread> threads_;
	std::mutex m_;
	std::condition_variable cv_, idle_cv_;
	std::priority_queue<Item, std::vector<Item>, Later> q_;
	std::deque<std::function<void()>> helpers_; // parallel_for helpers, taken before any keyed job
	size_t busy_ = 0;
	std::unordered_map<int64_t, uint64_t> latest_;
	uint64_t seq_ = 0;
	bool stop_ = false;
};

} // namespace city
