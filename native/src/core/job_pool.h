// Priority job pool: a set of worker threads, jobs ordered by priority, a job
// of the same key replacing a queued older one, and a blocking parallel_for for the lot cache. A key is a lane and an id: two
// users of the pool cannot replace each other's jobs, whatever their ids, because each has a lane of its own. parallel_for helpers
// live outside the keyed queue (no key to collide with or be superseded by) and the caller works too, so it finishes
// even when every worker is parked on a lock the caller holds (mesh jobs wait for map_mutex during a command).
// The heap holds only (priority, seq, key); a key's queued work lives in its slot, so a newer submit destroys the older
// closure at once (outside the pool's lock) instead of keeping it until the heap reaches it. A running job asks current()
// at its natural boundaries and returns early once a newer job of its key was submitted.
// The memory its threads hold follows the work that runs at once, never the machine's core count: every thread has a heap of
// its own (core/memory.cpp), so work goes to the worker that worked last (the idle ones wait in a stack, each on a condition
// of its own; a worker's thread starts the first time its place is called). What a job built is freed later by another thread
// (the frame's, after an upload), and the allocator gives it back only when the heap's own thread looks: so a worker at rest
// whose heap still holds pages looks at it a `linger` after its own work, and again when the pool is told that built results
// were freed (freed()), never more than once a linger; once its heap holds nothing it waits for work alone.
#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

namespace city {

// One lane for each user of the pool. A new user adds its own: never shares one, never does arithmetic on an id to stay clear of another.
enum class JobLane : uint8_t { ROADS, ROADS_FAR, VERGES, STRUCTURES, PROPS, BUILDING, BUILDING_SUPER, TREES_NEAR, TREES_FAR, COVER, ZONES, FOG, CANYON, SHORE, RUINS, WATER, SIGNATURES };
const char *job_lane_name(JobLane lane); // what the probes call a lane

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

// The least time between two looks of a resting worker at its heap: longer than any pause inside a stream of work (the
// simulation's tick is 50 ms, a frame 16 ms, so nothing is collected at their pace), short against the time a view stays.
constexpr std::chrono::milliseconds JOB_LINGER{ 2000 };

class JobPool {
public:
	explicit JobPool(unsigned threads = 0, std::chrono::milliseconds linger = JOB_LINGER);
	~JobPool();
	// Lower priority runs first. Returns the job's seq (what current() compares).
	uint64_t submit(JobKey key, float priority, std::function<void()> work);
	void parallel_for(size_t n, const std::function<void(size_t)> &fn); // blocks until done
	void wait_idle(); // blocks until the queue is empty and no worker is running
	// Told by whoever freed what jobs built (the frame, after it dropped or replaced results): the resting workers whose heaps
	// hold pages look at them and give back what was freed there.
	void freed();
	size_t tracked(); // keys with a job still queued: none once the pool is idle (a key is forgotten when its newest job is taken)
	uint64_t replaced() const { return replaced_.load(); } // queued jobs a newer one of their key took the place of
	// The job `seq` of `key` is still the newest submitted for its key (a running job polls it to stop early).
	bool current(JobKey key, uint64_t seq);
	// The same for the job running on the calling thread (true outside a keyed job).
	bool running_current();
	// The keyed job the calling thread runs, of any pool: false outside one (sim/state/map_hold.h names what a thread built by it).
	static bool running(JobKey &key);

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
	// A worker's place: its thread while it lives, and the condition it waits on while it stands in idle_.
	struct Worker {
		std::thread thread;
		std::condition_variable cv;
		bool live = false; // its thread was started (or is about to be, by whoever took the place): it runs until the pool ends
		bool holds = false; // its heap may hold pages of what it built: set by any work, cleared once a look finds none
	};
	void loop(Worker &me);
	bool idle() const { return q_.empty() && helpers_.empty() && busy_ == 0; }
	bool ready() const { return stop_ || !q_.empty() || !helpers_.empty(); } // a worker has something to take
	// Up to `count` workers for new work (m_ held): the idle ones are woken, the one that worked last first; for the rest a
	// place with no thread is taken, and returned for start() to run once the lock is let go.
	std::vector<Worker *> call(size_t count);
	void start(const std::vector<Worker *> &places); // outside the lock: the thread of each place taken begins
	std::vector<std::unique_ptr<Worker>> workers_;
	std::vector<Worker *> idle_; // the workers waiting: the last to go idle on top
	std::chrono::milliseconds linger_;
	std::mutex m_;
	std::condition_variable idle_cv_;
	std::priority_queue<Item, std::vector<Item>, Later> q_;
	std::deque<std::function<void()>> helpers_; // parallel_for helpers, taken before any keyed job
	size_t busy_ = 0;
	std::unordered_map<JobKey, Slot, JobKeyHash> slots_;
	// Each key's newest submitted seq while that job is queued or running (current()): gone when it finishes.
	std::unordered_map<JobKey, uint64_t, JobKeyHash> newest_;
	std::atomic<uint64_t> replaced_{ 0 };
	uint64_t seq_ = 0;
	uint64_t freed_ = 0; // moves each time the pool is told of a free (freed())
	bool stop_ = false;
};

} // namespace city
