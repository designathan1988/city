// Priority job pool: see core/job_pool.h.
#include "core/job_pool.h"

#include "core/memory.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <memory>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <sched.h>
#endif

namespace city {

namespace {
// The logical processors this process may run on: its affinity, not the machine's count. A process confined to a few (a test
// under the orchestrator, tools/tasks/pool.py) starts workers for those, never one a core of the machine.
unsigned usable_processors() {
#ifdef _WIN32
	DWORD_PTR mine = 0, system = 0;
	if (GetProcessAffinityMask(GetCurrentProcess(), &mine, &system) && mine) return unsigned(__builtin_popcountll(mine));
#else
	cpu_set_t set;
	CPU_ZERO(&set);
	if (sched_getaffinity(0, sizeof(set), &set) == 0 && CPU_COUNT(&set) > 0) return unsigned(CPU_COUNT(&set));
#endif
	return std::thread::hardware_concurrency();
}

thread_local bool in_pool = false;
// The keyed job running on this thread (running_current).
thread_local JobPool *running_pool = nullptr;
thread_local JobKey running_key;
thread_local uint64_t running_seq = 0;
} // namespace

JobPool::JobPool(unsigned threads) {
	const unsigned hw = usable_processors();
	const unsigned n = threads ? threads : std::max(2u, hw > 1 ? hw - 1 : 2u);
	threads_.reserve(n);
	workers_.reserve(n);
	idle_.reserve(n);
	for (unsigned i = 0; i < n; i++) workers_.push_back(std::make_unique<Worker>());
	for (unsigned i = 0; i < n; i++) threads_.emplace_back([this, i]() { loop(*workers_[i]); });
	// Every worker waits on the stack before the pool takes work: a job is never found by a thread that is only starting, so it
	// goes to the worker a submit woke or to one that has just finished.
	std::unique_lock<std::mutex> lk(m_);
	idle_cv_.wait(lk, [&]() { return started_ == n; });
}

void JobPool::wake_one() {
	if (idle_.empty()) return; // every worker is at work: the first to finish looks at the queue before it waits
	Worker *w = idle_.back();
	idle_.pop_back();
	w->cv.notify_one();
}

void JobPool::wake_all() {
	for (Worker *w : idle_) w->cv.notify_one();
	idle_.clear();
}

JobPool::~JobPool() {
	{
		std::lock_guard<std::mutex> lk(m_);
		stop_ = true;
		wake_all();
	}
	for (std::thread &t : threads_)
		if (t.joinable()) t.join();
}

uint64_t JobPool::submit(JobKey key, float priority, std::function<void()> work) {
	std::function<void()> superseded; // the older queued closure of the key: destroyed after the lock is let go
	uint64_t seq;
	{
		std::lock_guard<std::mutex> lk(m_);
		seq = ++seq_;
		Slot &slot = slots_[key];
		if (slot.work) {
			superseded = std::move(slot.work);
			replaced_++;
		}
		slot.seq = seq;
		slot.work = std::move(work);
		newest_[key] = seq;
		q_.push(Item{ priority, seq, key });
		wake_one();
	}
	return seq;
}

bool JobPool::current(JobKey key, uint64_t seq) {
	std::lock_guard<std::mutex> lk(m_);
	auto it = newest_.find(key);
	return it != newest_.end() && it->second == seq;
}

bool JobPool::running_current() {
	if (running_pool != this) return true;
	return current(running_key, running_seq);
}

bool JobPool::running(JobKey &key) {
	if (!running_pool) return false;
	key = running_key;
	return true;
}

const char *job_lane_name(JobLane lane) {
	switch (lane) {
		case JobLane::ROADS: return "roads";
		case JobLane::ROADS_FAR: return "roads_far";
		case JobLane::SEAMS: return "seams";
		case JobLane::STRUCTURES: return "structures";
		case JobLane::PROPS: return "props";
		case JobLane::BUILDING: return "building";
		case JobLane::BUILDING_SUPER: return "building_super";
		case JobLane::TREES_NEAR: return "trees_near";
		case JobLane::TREES_FAR: return "trees_far";
		case JobLane::COVER: return "cover";
		case JobLane::ZONES: return "zones";
		case JobLane::CONGESTION: return "congestion";
		case JobLane::FOG: return "fog";
		case JobLane::CANYON: return "canyon";
		case JobLane::SHORE: return "shore";
		case JobLane::ECOLOGY: return "ecology";
		case JobLane::RUINS: return "ruins";
		case JobLane::WATER: return "water";
		case JobLane::SIGNATURES: return "signatures";
		case JobLane::PROBE_HOLD: return "probe_hold";
		case JobLane::PROBE_KEY: return "probe_key";
		case JobLane::PROBE_OTHER: return "probe_other";
	}
	return "";
}

void JobPool::parallel_for(size_t n, const std::function<void(size_t)> &fn) {
	if (n == 0) return;
	if (n == 1 || in_pool || threads_.empty()) {
		for (size_t i = 0; i < n; i++) fn(i);
		return;
	}
	// A helper that starts after the caller closed the batch returns without touching `fn` (the caller's frame).
	struct Batch {
		std::atomic<size_t> next{ 0 };
		std::mutex m;
		std::condition_variable cv;
		size_t running = 0;
		bool closed = false;
		const std::function<void(size_t)> *fn = nullptr;
		size_t n = 0;
	};
	auto batch = std::make_shared<Batch>();
	batch->fn = &fn;
	batch->n = n;
	const size_t helpers = std::min(n, threads_.size() + 1) - 1;
	{
		std::lock_guard<std::mutex> lk(m_);
		for (size_t k = 0; k < helpers; k++)
			helpers_.push_back([batch]() {
				{
					std::lock_guard<std::mutex> lk(batch->m);
					if (batch->closed) return;
					batch->running++;
				}
				for (size_t i = batch->next++; i < batch->n; i = batch->next++) (*batch->fn)(i);
				std::lock_guard<std::mutex> lk(batch->m);
				if (--batch->running == 0) batch->cv.notify_all();
			});
		for (size_t k = 0; k < helpers; k++) wake_one(); // a worker a helper, never the whole pool
	}
	for (size_t i = batch->next++; i < n; i = batch->next++) fn(i);
	std::unique_lock<std::mutex> lk(batch->m);
	batch->closed = true;
	batch->cv.wait(lk, [&]() { return batch->running == 0; });
}

size_t JobPool::tracked() {
	std::lock_guard<std::mutex> lk(m_);
	return slots_.size();
}

void JobPool::pause() {
	std::unique_lock<std::mutex> lk(m_);
	paused_ = true;
	idle_cv_.wait(lk, [&]() { return busy_ == 0; });
}

void JobPool::resume() {
	{
		std::lock_guard<std::mutex> lk(m_);
		paused_ = false;
		wake_all();
	}
}

void JobPool::wait_idle() {
	std::unique_lock<std::mutex> lk(m_);
	idle_cv_.wait(lk, [&]() { return idle(); });
}

void JobPool::loop(Worker &me) {
	in_pool = true;
	// This worker ran a keyed job since it last gave its freed memory back: its heap holds what the job built and freed. Only then
	// has it anything to return (core/memory.h); a parallel_for helper, woken at the simulation's tick, leaves nothing behind.
	bool owes = false;
	// It ran something already: it waits above the workers that never did (a thread that starts late must not stand over the one at work).
	bool worked = false;
	const auto delay = std::chrono::milliseconds(std::max(1L, freed_memory_wait_ms()));
	std::unique_lock<std::mutex> lk(m_); // held but while a job runs or memory is given back: a worker that finishes is on the idle stack before anybody can submit again
	++started_; // it waits on the stack before this lock is let go (nothing is queued yet: the constructor has not returned)
	idle_cv_.notify_all();
	for (;;) {
		while (!ready()) {
			if (worked) idle_.push_back(&me);
			else idle_.insert(idle_.begin(), &me);
			const bool timed_out = owes ? me.cv.wait_for(lk, delay) == std::cv_status::timeout : (me.cv.wait(lk), false);
			const auto at = std::find(idle_.begin(), idle_.end(), &me); // whoever woke it took it off; a timeout or a spurious wake did not
			if (at != idle_.end()) idle_.erase(at);
			if (timed_out && !ready()) { // idle for longer than the allocator keeps freed memory: nothing came to use it again
				lk.unlock();
				release_freed_memory();
				owes = false;
				lk.lock();
			}
		}
		if (stop_ && q_.empty() && helpers_.empty()) return;
		std::function<void()> work;
		bool keyed = false;
		JobKey key;
		uint64_t seq = 0;
		if (!helpers_.empty()) {
			work = std::move(helpers_.front());
			helpers_.pop_front();
		} else {
			const Item item = q_.top();
			q_.pop();
			auto it = slots_.find(item.key);
			if (it == slots_.end() || it->second.seq != item.seq) { // superseded while queued: its work went with the newer submit
				if (idle()) idle_cv_.notify_all();
				continue;
			}
			work = std::move(it->second.work);
			slots_.erase(it); // its newest job runs: an older heap entry finds no slot of its seq and is dropped
			keyed = true;
			key = item.key;
			seq = item.seq;
		}
		++busy_;
		worked = true;
		lk.unlock();
		if (keyed) {
			running_pool = this;
			running_key = key;
			running_seq = seq;
			owes = true;
		}
		work();
		running_pool = nullptr;
		work = nullptr; // the closure's captures go before the pool's lock is taken
		lk.lock();
		if (keyed) {
			auto it = newest_.find(key);
			if (it != newest_.end() && it->second == seq) newest_.erase(it);
		}
		--busy_;
		if (busy_ == 0) idle_cv_.notify_all(); // wait_idle and pause wake on it
	}
}

} // namespace city
